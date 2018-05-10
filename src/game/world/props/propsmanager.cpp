#include "propsmanager.h"
#include "propclassloadoperation.h"
#include "../../utils/filesystem.h"
#include "../../utils/utils.h"

#include <iostream>
#include <algorithm>
#include <imgui.h>

namespace TankGame
{
	std::unique_ptr<PropsManager> PropsManager::s_instance;
	
	const Texture2D* PropsManager::GetTexture(const std::string& path)
	{
		auto texturePos = m_textures.find(path);
		bool found = texturePos != m_textures.end();
		
		if (found)
			return texturePos->second.get();
		
		GetLogStream() << LOG_WARNING << "Loading prop texture lazily (path: '" << path << "'\n";
		
		std::unique_ptr<Texture2D> texture = std::make_unique<Texture2D>(Texture2D::FromFile(path));
		texture->SetWrapMode(GL_CLAMP_TO_EDGE);
		
		const Texture2D* result = texture.get();
		
		m_textures.emplace(path, std::move(texture));
		
		return result;
	}
	
	std::unique_ptr<IASyncOperation> PropsManager::LoadPropClasses(const fs::path& directoryPath)
	{
		return std::make_unique<PropClassLoadOperation>(*this, directoryPath);
	}
	
	bool PropsManager::RenderPropClassSeletor(const char* label, const PropClass** propClass) const
	{
		auto pos = std::find_if(m_propClasses.begin(), m_propClasses.end(), [&] (const auto& entry)
		{
			return entry.get() == *propClass;
		});
		
		if (pos == m_propClasses.end())
			throw std::runtime_error("Invalid prop class.");
		
		int currentIndex = pos - m_propClasses.begin();
		
		bool changed = ImGui::Combo(label, &currentIndex, [] (void* data, int i, const char** out)
		{
			*out = reinterpret_cast<const PropsManager*>(data)->m_propClasses[i]->GetName().c_str();
			return true;
		}, const_cast<PropsManager*>(this), m_propClasses.size(), 4);
		
		if (changed)
			*propClass = m_propClasses[currentIndex].get();
		
		return changed;
	}
	
	void PropsManager::SortPropClasses()
	{
		std::sort(m_propClasses.begin(), m_propClasses.end(), [] (const auto& a, const auto& b)
		{
			return std::less<std::string>()(a->GetName(), b->GetName());
		});
	}
	
	const PropClass* PropsManager::GetPropClassByName(const std::string& name) const
	{
		auto pos = std::find_if(m_propClasses.begin(), m_propClasses.end(), [&] (const auto& propClass)
		{
			return propClass->GetName() == name;
		});
		
		if (pos == m_propClasses.end())
			return nullptr;
		return pos->get();
	}
}
