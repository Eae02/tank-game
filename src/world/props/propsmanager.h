#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "propclass.h"
#include "../../graphics/gl/texture2d.h"

namespace TankGame
{
	class PropsManager
	{
	public:
		std::shared_ptr<Texture2D> GetTexture(const std::string& path);
		
		const PropClass* GetPropClassByName(const std::string& name) const;
		
		void LoadPropClasses(const fs::path& directoryPath);
		
		inline static void SetInstance(std::unique_ptr<PropsManager>&& instance)
		{ s_instance = std::move(instance); }
		inline static PropsManager& GetInstance()
		{ return *s_instance; }
		
		bool RenderPropClassSeletor(const char* label, const PropClass** propClass) const;
		
	private:
		static std::unique_ptr<PropsManager> s_instance;
		
		std::unordered_map<std::string, std::weak_ptr<Texture2D>> m_textures;
		
		std::vector<std::unique_ptr<PropClass>> m_propClasses;
	};
}
