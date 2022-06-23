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
	void AddConsoleCommands(Console& console, const std::shared_ptr<Game>& game)
	{
		console.AddCommand("heal", [=] (const std::string* argv, size_t argc)
		{
			if (game->gameManager->GetLevel() == nullptr)
			{
				GetLogStream() << LOG_ERROR << "No level loaded.\n";
				return;
			}
			
			PlayerEntity& player = game->gameManager->GetLevel()->GetPlayerEntity();
			
			if (argc == 0)
				player.SetHp(player.GetMaxHp());
			else
				player.SetHp(player.GetHp() + std::stof(argv[0]));
		});
		
		console.AddCommand("checkpoint", [=] (const std::string* argv, size_t argc)
		{
			if (game->gameManager->GetLevel() == nullptr)
				GetLogStream() << LOG_ERROR << "No level loaded.\n";
			else if (!game->gameManager->GetLevel()->TryJumpToCheckpoint(std::stoi(argv[0])))
				GetLogStream() << LOG_ERROR << "Checkpoint not found.\n";
		});
		
		console.AddCommand("setprogress", [=] (const std::string* argv, size_t argc)
		{
			if (!fs::exists(Level::GetLevelsPath() / argv[0]))
				GetLogStream() << LOG_ERROR << "Level not found: '" + argv[0] + "'.\n";
			else
				Progress::GetInstance().UpdateLevelProgress(argv[0], std::stoi(argv[1]));
		}, 2);
		
		console.AddCommand("powerup", [=] (const std::string* argv, size_t argc)
		{
			if (game->gameManager->GetLevel() == nullptr)
			{
				GetLogStream() << LOG_ERROR << "No level loaded.\n";
				return;
			}
			
			int powerUpID = std::stoi(argv[0]);
			if (powerUpID < 0 || powerUpID >= POWER_UP_COUNT)
				GetLogStream() << LOG_ERROR << "Invalid power up id: " + argv[0] + ".\n";
			else
				game->gameManager->GetLevel()->GetPlayerEntity().GivePowerUp(static_cast<PowerUps>(powerUpID));
		}, 1);
		
		console.AddCommand("ammo", [=] (const std::string* argv, size_t argc)
		{
			if (game->gameManager->GetLevel() == nullptr)
			{
				GetLogStream() << LOG_ERROR << "No level loaded.\n";
				return;
			}
			
			int weaponID = std::stoi(argv[0]);
			if (weaponID < 0 || weaponID >= SPECIAL_WEAPONS_COUNT)
			{
				GetLogStream() << LOG_ERROR << "Invalid weapon id: " + argv[0] + ".\n";
				return;
			}
			
			game->gameManager->GetLevel()->GetPlayerEntity().GetWeaponState().GiveAmmo(
				static_cast<SpecialWeapons>(weaponID), std::stoi(argv[1]));
		}, 2);
		
		console.AddCommand("edit", [=] (const std::string* argv, size_t argc)
		{
			if (!game->EditLevel(argv[0]))
				GetLogStream() << LOG_ERROR << "Failed to load level: '" << argv[0] << "'.\n";
		}, 1);
		
		console.AddCommand("level", [=] (const std::string* argv, size_t argc)
		{
			if (!game->LoadLevel(argv[0]))
				GetLogStream() << LOG_ERROR << "Failed to load level: '" << argv[0] << "'.\n";
		}, 1);
		
#ifndef __EMSCRIPTEN__
		console.AddCommand("newlevel", [=] (const std::string* argv, size_t argc)
		{
			int width = std::stoi(argv[1]);
			int height = std::stoi(argv[2]);
			
			if (width < 10 || height < 10)
			{
				GetLogStream() << LOG_ERROR << "Level size must be at least 10x10.\n";
				return;
			}
			
			fs::path path = Level::GetLevelsPath() / argv[0];
			if (fs::exists(path))
			{
				GetLogStream() << LOG_ERROR << "The level aready exists.\n";
				return;
			}
			
			std::ofstream stream(path.string(), std::ios::binary);
			WriteEmptyWorld(argv[0], width, height, stream);
		}, 3);
		
		console.AddCommand("makemi", [=] (const std::string* argv, size_t argc)
		{
			std::optional<Level> level = Level::FromName(argv[0], GameWorld::Types::ScreenShot);
			if (!level)
			{
				GetLogStream() << LOG_ERROR << "Level not found: " << argv[0] << "\n";
			}
			else
			{
				std::ofstream stream(Level::GetLevelsPath() / (argv[0] + ".mi"), std::ios::binary);
				LevelMenuInfo::WriteMenuInfo(*level, argv[1], stream);
			}
		}, 2);
#endif
		
		console.AddCommand("men", [=] (const std::string* argv, size_t argc)
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
				GetLogStream() << LOG_ERROR << "Invalid argument, should be 'true' or 'false'.\n";
			else
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
