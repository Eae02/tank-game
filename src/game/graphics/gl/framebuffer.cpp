#include "framebuffer.h"
#include "../../exceptions/invalidstateexception.h"

namespace TankGame
{
	Framebuffer::StackEntry Framebuffer::s_currentFramebuffer;
	
	std::stack<Framebuffer::StackEntry> Framebuffer::s_framebufferStack;
	
	Framebuffer::Framebuffer()
	{
		GLuint id;
		glCreateFramebuffers(1, &id);
		SetID(id);
	}
	
	void Framebuffer::Save()
	{
		s_framebufferStack.push(s_currentFramebuffer);
	}
	
	void Framebuffer::Bind(const Framebuffer& framebuffer, int vpX, int vpY, int vpWidth, int vpHeight)
	{
		if (s_currentFramebuffer.m_framebufferID == framebuffer.GetID())
			return;
		
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.GetID());
		glViewport(vpX, vpY, vpWidth, vpHeight);
		
		s_currentFramebuffer = { framebuffer.GetID(), vpX, vpY, vpWidth, vpHeight };
	}
	
	void Framebuffer::Restore()
	{
		if (s_framebufferStack.empty())
			return;
		
		s_currentFramebuffer = s_framebufferStack.top();
		s_framebufferStack.pop();
		
		glBindFramebuffer(GL_FRAMEBUFFER, s_currentFramebuffer.m_framebufferID);
		glViewport(s_currentFramebuffer.m_viewportX, s_currentFramebuffer.m_viewportY,
		           s_currentFramebuffer.m_viewportWidth, s_currentFramebuffer.m_viewportHeight);
	}
	
	void Framebuffer::SetDefaultViewport(int vpX, int vpY, int vpWidth, int vpHeight)
	{
		if (!s_framebufferStack.empty())
			throw InvalidStateException("The default framebuffer must be bound when SetDefaultViewport() is called.");
		
		s_currentFramebuffer.m_viewportX = vpX;
		s_currentFramebuffer.m_viewportY = vpY;
		s_currentFramebuffer.m_viewportWidth = vpWidth;
		s_currentFramebuffer.m_viewportHeight = vpHeight;
	}
	
	void DeleteFramebuffer(GLuint id)
	{
		glDeleteFramebuffers(1, &id);
	}
}
