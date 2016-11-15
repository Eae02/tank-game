#include "scissor.h"
#include "../exceptions/invalidstateexception.h"

#include <stack>

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
	
	static std::stack<ScissorRect> scissorRectStack;
	
	void PopScissorRect()
	{
		if (scissorRectStack.empty())
		    return;
		
		scissorRectStack.pop();
		
		if (scissorRectStack.empty())
		{
			glDisable(GL_SCISSOR_TEST);
		}
		else
		{
			ScissorRect scissor = scissorRectStack.top();
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
			const ScissorRect& currentRect = scissorRectStack.top();
			
			x = glm::max(x, currentRect.m_x);
			y = glm::max(y, currentRect.m_y);
			
			w = glm::min(x + w, currentRect.m_x + currentRect.m_width) - x;
			h = glm::min(y + h, currentRect.m_y + currentRect.m_height) - y;
		}
		
		scissorRectStack.emplace(x, y, w, h);
		glScissor(x, y, w, h);
	}
	
	void PushScissorRect(const Rectangle& rectangle)
	{
		PushScissorRect(static_cast<int>(rectangle.x), static_cast<int>(rectangle.y),
		                static_cast<int>(std::ceil(rectangle.FarX())) - static_cast<int>(rectangle.x),
		                static_cast<int>(std::ceil(rectangle.FarY())) - static_cast<int>(rectangle.y));
	}
}
