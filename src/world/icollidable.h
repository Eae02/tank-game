#pragma once

#include "../colliderinfo.h"
#include "../utils/abstract.h"

namespace TankGame
{
	enum class CollidableTypes
	{
		Object,
		Npc,
		Player
	};
	
	class ICollidable : public Abstract
	{
	public:
		virtual ColliderInfo GetColliderInfo() const = 0;
		virtual CollidableTypes GetCollidableType() const = 0;
		virtual bool IsStaticCollider() const = 0;
		
		inline static bool IsObject(const ICollidable& collidable)
		{
			return collidable.GetCollidableType() == CollidableTypes::Object;
		}
	};
}
