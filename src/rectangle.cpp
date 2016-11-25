#include "rectangle.h"

#include <algorithm>
#include <cmath>

namespace TankGame
{
	bool Rectangle::Contains(float px, float py) const
	{
		return px >= x && px <= FarX() && py >= y && py <= FarY();
	}
	
	bool Rectangle::Contains(glm::vec2 point) const
	{
		return point.x >= x && point.x <= FarX() && point.y >= y && point.y <= FarY();
	}
	
	bool Rectangle::Contains(const Rectangle& other) const
	{
		return x < other.x && y < other.y && FarX() > other.FarX() && FarY() > other.FarY();
	}
	
	bool Rectangle::Intersects(const Rectangle& other) const
	{
		return !(other.x >= FarX() ||
				other.FarX() <= x ||
				other.y >= FarY() ||
				other.FarY() <= y);
	}
	
	std::array<Rectangle, 4> Rectangle::Divide() const
	{
		std::array<Rectangle, 4> result;
		
		float divW = w / 2.0f;
		float divH = h / 2.0f;
		
		for (unsigned int i = 0; i < 4; i++)
		{
			result[i].x = x + (i % 2) * divW;
			result[i].y = y + (i / 2) * divH;
			
			result[i].w = divW;
			result[i].h = divH;
		}
		
		return result;
	}
	
	Rectangle Rectangle::CreateCentered(float x, float y, float w, float h)
	{
		return Rectangle(x - (w / 2.0f), y - (h / 2.0f), w, h);
	}
	
	Rectangle Rectangle::Mix(const Rectangle& a, const Rectangle& b, float x)
	{
		return { glm::mix(a.x, b.x, x), glm::mix(a.y, b.y, x), glm::mix(a.w, b.w, x), glm::mix(a.h, b.h, x) };
	}
	
	void Rectangle::Inflate(float amount)
	{
		x -= amount;
		y -= amount;
		
		w += amount * 2;
		h += amount * 2;
	}
	
	Rectangle Rectangle::FromMinMax(glm::vec2 min, glm::vec2 max)
	{
		return Rectangle(
				std::min(min.x, max.x),
				std::min(min.y, max.y),
				std::abs(max.x - min.x),
				std::abs(max.y - min.y)
		);
	}
}
