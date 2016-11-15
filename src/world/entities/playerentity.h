#pragma once

#include "tankentity.h"
#include "hittable.h"
#include "../lights/spotlightentity.h"
#include "../../graphics/gl/texture2d.h"
#include "../../graphics/spritematerial.h"
#include "../../utils/memory/stackobject.h"

#include <random>

namespace TankGame
{
	class PlayerEntity : public TankEntity
	{
	public:
		PlayerEntity();
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual Hittable* AsHittable() final override
		{ return this; }
		
		virtual Circle GetHitCircle() const;
		
		inline glm::vec2 GetVelocity() const
		{ return m_velocity; }
		
		virtual void SetHp(float hp) override;
		
		virtual const char* GetObjectName() const override;
		
	protected:
		virtual const class SpriteMaterial& GetBaseMaterial(int frame) const override;
		virtual const class SpriteMaterial& GetCannonMaterial() const override;
		
		virtual void OnKilled() override;
		
	private:
		static bool s_areTexturesLoaded;
		static StackObject<Texture2D> s_cannonTexture;
		static StackObject<Texture2D> s_cannonNormalMap;
		static StackObject<SpriteMaterial> s_cannonMaterial;
		
		float m_rotationVelocity = 0;
		glm::vec2 m_velocity;
		
		std::normal_distribution<float> m_dist;
	};
}
