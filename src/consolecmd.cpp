#include "consolecmd.h"
#include "gamemanager.h"
#include "console.h"
#include "world/entities/playerentity.h"
#include "world/serialization/serializeworld.h"
#include "graphics/gl/dsawrapper.h"
#include "settings.h"
#include "progress.h"

#include <fstream>
#include <string>

namespace TankGame
{
	Level CommandCallbackLevelFromName(const std::string& name, GameWorld::Types worldType)
	{
		try
		{
			return Level::FromName(name, worldType);
		}
		catch (const std::exception& ex)
		{
			throw Console::CommandException(ex.what());
		}
	}
	
	void AddConsoleCommands(Console& console, const std::shared_ptr<Game>& game)
	{
		console.AddCommand("heal", [=] (const std::string* argv, size_t argc)
		{
			if (game->gameManager->GetLevel() == nullptr)
				throw std::runtime_error("No level loaded.");
			
			PlayerEntity& player = game->gameManager->GetLevel()->GetPlayerEntity();
			
			if (argc == 0)
				player.SetHp(player.GetMaxHp());
			else
				player.SetHp(player.GetHp() + std::stof(argv[0]));
		});
		
		console.AddCommand("checkpoint", [=] (const std::string* argv, size_t argc)
		{
			if (game->gameManager->GetLevel() == nullptr)
				throw Console::CommandException("No level loaded.");
			if (!game->gameManager->GetLevel()->TryJumpToCheckpoint(std::stoi(argv[0])))
				throw Console::CommandException("Checkpoint not found.");
		});
		
		console.AddCommand("setprogress", [=] (const std::string* argv, size_t argc)
		{
			if (!fs::exists(Level::GetLevelsPath() / argv[0]))
				throw Console::CommandException("Level not found: '" + argv[0] + "'.");
			Progress::GetInstance().UpdateLevelProgress(argv[0], std::stoi(argv[1]));
		}, 2);
		
		console.AddCommand("powerup", [=] (const std::string* argv, size_t argc)
		{
			if (game->gameManager->GetLevel() == nullptr)
				throw Console::CommandException("No level loaded.");
			
			int powerUpID = std::stoi(argv[0]);
			if (powerUpID < 0 || powerUpID >= POWER_UP_COUNT)
				throw Console::CommandException("Invalid power up id: " + argv[0] + ".");
			
			game->gameManager->GetLevel()->GetPlayerEntity().GivePowerUp(static_cast<PowerUps>(powerUpID));
		}, 1);
		
		console.AddCommand("ammo", [=] (const std::string* argv, size_t argc)
		{
			if (game->gameManager->GetLevel() == nullptr)
				throw Console::CommandException("No level loaded.");
			
			int weaponID = std::stoi(argv[0]);
			if (weaponID < 0 || weaponID >= SPECIAL_WEAPONS_COUNT)
				throw Console::CommandException("Invalid weapon id: " + argv[0] + ".");
			
			game->gameManager->GetLevel()->GetPlayerEntity().GetWeaponState().GiveAmmo(static_cast<SpecialWeapons>(weaponID), std::stoi(argv[1]));
		}, 2);
		
		console.AddCommand("edit", [=] (const std::string* argv, size_t argc) { game->EditLevel(argv[0]); }, 1);
		
		console.AddCommand("level", [=] (const std::string* argv, size_t argc) { game->LoadLevel(argv[0]); }, 1);
		
		console.AddCommand("newlevel", [=] (const std::string* argv, size_t argc)
		{
			int width = std::stoi(argv[1]);
			int height = std::stoi(argv[2]);
			
			if (width < 10 || height < 10)
				throw Console::CommandException("Level size must be at least 10x10.");
			
			fs::path path = Level::GetLevelsPath() / argv[0];
			if (fs::exists(path))
				throw Console::CommandException("The level aready exists.");
			
			std::ofstream stream(path.string(), std::ios::binary);
			WriteEmptyWorld(argv[0], width, height, stream);
		}, 3);
		
		console.AddCommand("makemi", [=] (const std::string* argv, size_t argc)
		{
			Level level = CommandCallbackLevelFromName(argv[0], GameWorld::Types::ScreenShot);
			std::ofstream stream(Level::GetLevelsPath() / (argv[0] + ".mi"), std::ios::binary);
			LevelMenuInfo::WriteMenuInfo(level, argv[1], stream);
		}, 2);
		
		console.AddCommand("menu", [=] (const std::string* argv, size_t argc)
		{
			if (game->menuManager->Visible())
				return;
			
			game->menuManager->ShowMainMenu();
			
			game->gameManager->ExitLevel();
			if (game->editor != nullptr)
				game->editor->Close();
		});
		
		console.AddCommand("qf", [=] (const std::string* argv, size_t argc)
		{
			if (argv[0] != "true" && argv[0] != "false")
				throw Console::CommandException("Invalid argument, should be 'true' or 'false'.");
			Settings::instance.SetQueueFrames(argv[0] == "true");
		}, 1);
		
		console.AddCommand("glinfo", [] (const std::string* argv, size_t argc)
		{
			GetLogStream() <<
				" Renderer: " << glGetString(GL_RENDERER) << "\n"
				" Vendor:   " << glGetString(GL_VENDOR) << "\n"
				" Version:  " << glGetString(GL_VERSION) << "\n"
				" UB Align: " << GetUniformBufferOffsetAlignment() << "\n"
				" DSA=" << !DSAWrapper::IsActive() << " BS=" << hasBufferStorage << " TS=" << hasTextureStorage << "\n";
		});
	}
}
