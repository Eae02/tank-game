#include "almanager.h"
#include "../world/entities/playerentity.h"
#include "../utils/utils.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <stdexcept>

namespace TankGame
{
	ALCdevice* theALDevice;
	ALCcontext* theALContext;
	
	void InitOpenAL()
	{
		theALDevice = alcOpenDevice(nullptr);
		if (theALDevice == nullptr)
			Panic("Error opening OpenAL device.");
		
		theALContext = alcCreateContext(theALDevice, nullptr);
		if (theALContext == nullptr)
			Panic("Error creating OpenAL context.");
		alcMakeContextCurrent(theALContext);
	}
	
	void CloseOpenAL()
	{
		alcDestroyContext(theALContext);
		alcCloseDevice(theALDevice);
	}
	
	void UpdateListener(const PlayerEntity& playerEntity)
	{
		glm::vec2 velocity = playerEntity.GetVelocity();
		alListener3f(AL_VELOCITY, velocity.x, velocity.y, 0);
		
		glm::vec2 forward = playerEntity.GetTransform().GetForward();
		alListener3f(AL_DIRECTION, forward.x, forward.y, 0);
	}
	
	void SetMasterVolume(float volume)
	{
		alListenerf(AL_GAIN, volume);
	}
}
