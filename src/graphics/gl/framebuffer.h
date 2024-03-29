#pragma once

#include "glresource.h"

namespace TankGame
{
	void DeleteFramebuffer(GLuint id);
	
	class Framebuffer : public GLResource<DeleteFramebuffer>
	{
	public:
		Framebuffer();
		
		static void Save();
		static void Bind(const Framebuffer& framebuffer, int vpX, int vpY, int vpWidth, int vpHeight);
		static void Restore();
		
		static void SetDefaultViewport(int vpX, int vpY, int vpWidth, int vpHeight);
		
		inline static GLuint GetCurrentFramebufferID()
		{ return s_currentFramebuffer.m_framebufferID; }
		
	private:
		struct StackEntry
		{
			GLuint m_framebufferID;
			int m_viewportX;
			int m_viewportY;
			int m_viewportWidth;
			int m_viewportHeight;
			
			inline StackEntry()
			    : StackEntry(0, 0, 0, 0, 0) { }
			
			inline StackEntry(GLuint framebufferID, int viewportX, int viewportY, int viewportWidth, int viewportHeight)
			    : m_framebufferID(framebufferID), m_viewportX(viewportX), m_viewportY(viewportY),
			      m_viewportWidth(viewportWidth), m_viewportHeight(viewportHeight) { }
		};
		
		static StackEntry s_currentFramebuffer;
		
		static std::vector<StackEntry> s_framebufferStack;
	};
}
