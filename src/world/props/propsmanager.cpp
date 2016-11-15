#include "propsmanager.h"
#include "../../utils/filesystem.h"
#include "../../utils/utils.h"

#include <iostream>
#include <algorithm>
#include <imgui.h>

namespace TankGame
{
	std::unique_ptr<PropsManager> PropsManager::s_instance;
	
	std::shared_ptr<Texture2D> PropsManager::GetTexture(const std::string& path)
	{
		auto texturePos = m_textures.find(path);
		bool found = texturePos != m_textures.end();
		
		if (found)
		{
			std::shared_ptr<Texture2D> sharedPtr = texturePos->second.lock();
			if (sharedPtr != nullptr)
				return sharedPtr;
		}
		
		auto sharedPtr = std::make_shared<Texture2D>(Texture2D::FromFile(path));
		
		sharedPtr->SetWrapMode(GL_CLAMP_TO_EDGE);
		
		if (found)
			texturePos->second = sharedPtr;
		else
			m_textures.emplace(path, sharedPtr);
		
		return sharedPtr;
	}
	
	void PropsManager::LoadPropClasses(const fs::path& directoryPath)
	{
		for (const fs::directory_entry& entry : fs::recursive_directory_iterator(directoryPath))
		{
			if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".json")
			{
				try
				{
					m_propClasses.emplace_back(std::make_unique<PropClass>(PropClass::FromJSON(entry.path(), *this)));
				}
				catch (const std::exception& error)
				{
					GetLogStream() << LOG_ERROR << "Error loading prop from " << entry.path() << ": "
					               << error.what() << ".\n" << std::endl;
				}
			}
		}
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
