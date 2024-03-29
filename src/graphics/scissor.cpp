#include "scissor.h"

namespace TankGame
{
	struct ScissorRect
	{
		GLint m_x;
		GLint m_y;
		GLsizei m_width;
		GLsizei m_height;
		
		ScissorRect(GLint x, GLint y, GLsizei width, GLsizei height)
		    : m_x(x), m_y(y), m_width(width), m_height(height) { }
	};
	
	static std::vector<ScissorRect> scissorRectStack;
	
	void PopScissorRect()
	{
		if (scissorRectStack.empty())
		    return;
		
		scissorRectStack.pop_back();
		
		if (scissorRectStack.empty())
		{
			glDisable(GL_SCISSOR_TEST);
		}
		else
		{
			ScissorRect scissor = scissorRectStack.back();
			glScissor(scissor.m_x, scissor.m_y, scissor.m_width, scissor.m_height);
		}
	}
	
	void PushScissorRect(GLint x, GLint y, GLsizei w, GLsizei h)
	{
		if (scissorRectStack.empty())
		{
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			const ScissorRect& currentRect = scissorRectStack.back();
			
			x = glm::max(x, currentRect.m_x);
			y = glm::max(y, currentRect.m_y);
			
			w = glm::max(glm::min(x + w, currentRect.m_x + currentRect.m_width) - x, 0);
			h = glm::max(glm::min(y + h, currentRect.m_y + currentRect.m_height) - y, 0);
		}
		
		scissorRectStack.emplace_back(x, y, w, h);
		glScissor(x, y, w, h);
	}
	
	void PushScissorRect(const Rectangle& rectangle)
	{
		PushScissorRect(static_cast<int>(rectangle.x), static_cast<int>(rectangle.y),
		                static_cast<int>(std::ceil(rectangle.FarX())) - static_cast<int>(rectangle.x),
		                static_cast<int>(std::ceil(rectangle.FarY())) - static_cast<int>(rectangle.y));
	}
}
