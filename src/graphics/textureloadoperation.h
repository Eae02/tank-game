#pragma once

#include "gl/texture2d.h"
#include "../iasyncoperation.h"
#include "../utils/filesystem.h"

namespace TankGame
{
	class TextureLoadOperation : public IASyncOperation
	{
	public:
		using DoneCallback = void(*)(Texture2D&&);
		
		TextureLoadOperation(std::string path, DoneCallback doneCallback);
		TextureLoadOperation(const fs::path& path, DoneCallback doneCallback);
		
		virtual void DoWork() final override;
		virtual void ProcessResult() final override;
		
		Texture2D CreateTexture() const;
		
		inline const std::string& GetPath() const
		{ return m_path; }
		
		static void STBIDataDeleter(uint8_t* data);
		
	private:
		DoneCallback m_doneCallback;
		std::string m_path;
		
		std::unique_ptr<uint8_t, void(*)(uint8_t*)> m_data;
		
		int m_width = 0;
		int m_height = 0;
		
		int m_numMipmaps = 1;
		int m_numComponents = 0;
	};
}
