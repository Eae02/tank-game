#include "audio.h"
#include "audiolib.h"
#include "../world/entities/playerentity.h"
#include "../utils/utils.h"

namespace TankGame
{
	ALCdevice* theALDevice;
	ALCcontext* theALContext;
	
	bool CheckAudioInitResult(bool result, const char* message)
	{
		if (result)
			return true;
		GetLogStream() << LOG_ERROR << "Error initializaing audio: " << message << ".\n";
		CloseOpenAL();
		InstallNoOpAudioFunctions();
		return false;
	}
	
	void InitOpenAL()
	{
		if (!LoadAudioFunctions())
		{
			InstallNoOpAudioFunctions();
			return;
		}
		
		if (!CheckAudioInitResult(theALDevice = alcOpenDevice(nullptr), "alcOpenDevice failed"))
			return;
		
		if (!CheckAudioInitResult(theALContext = alcCreateContext(theALDevice, nullptr), "alcCreateContext failed"))
			return;
		
		CheckAudioInitResult(alcMakeContextCurrent(theALContext), "alcMakeContextCurrent failed");
	}
	
	void CloseOpenAL()
	{
		if (theALContext && alcDestroyContext)
		{
			alcDestroyContext(theALContext);
			theALContext = nullptr;
		}
		
		if (theALDevice && alcCloseDevice)
		{
			alcCloseDevice(theALDevice);
			theALDevice = nullptr;
		}
	}
	
	void UpdateListener(const PlayerEntity& playerEntity)
	{
		glm::vec2 velocity = playerEntity.GetVelocity();
		alListener3f(AL_VELOCITY, velocity.x, velocity.y, 0);
		
		glm::vec2 pos = playerEntity.GetTransform().GetPosition();
		alListener3f(AL_POSITION, pos.x, pos.y, 0);
		
		const float ori[6] = 
		{
			playerEntity.GetTransform().GetForward().x, playerEntity.GetTransform().GetForward().y, 0,
			0, 0, 1,
		};
		alListenerfv(AL_ORIENTATION, ori);
	}
	
	void SetMasterVolume(float volume)
	{
		alListenerf(AL_GAIN, volume);
	}
}
