#include "soundloadoperation.h"

namespace TankGame
{
	SoundLoadOperation::SoundLoadOperation(fs::path jsonPath, SoundLoadOperation::DoneCallback doneCallback)
	    : m_jsonPath(std::move(jsonPath)), m_doneCallback(doneCallback) { }
	
	void SoundLoadOperation::DoWork()
	{
		m_result.Construct(m_jsonPath);
	}
	
	void SoundLoadOperation::ProcessResult()
	{
		m_doneCallback(std::move(*m_result));
		m_result.Destroy();
	}
}
