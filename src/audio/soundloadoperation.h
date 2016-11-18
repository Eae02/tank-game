#pragma once

#include "soundsmanager.h"
#include "../iasyncoperation.h"
#include "../utils/memory/stackobject.h"

#include <functional>

namespace TankGame
{
	class SoundLoadOperation : public IASyncOperation
	{
	public:
		using DoneCallback = void(*)(SoundsManager&&);
		
		SoundLoadOperation(fs::path jsonPath, DoneCallback doneCallback);
		
		virtual void DoWork() override;
		virtual void ProcessResult() override;
		
	private:
		fs::path m_jsonPath;
		DoneCallback m_doneCallback;
		
		StackObject<SoundsManager> m_result;
	};
}
