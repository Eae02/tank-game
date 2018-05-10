#pragma once

#include <vector>
#include <memory>
#include <future>
#include <mutex>

#include "iasyncoperation.h"

namespace TankGame
{
	class ASyncWorkList
	{
	public:
		void SubmitWork(std::unique_ptr<IASyncOperation>&& work);
		
		void BeginProcessing();
		void ProcessResults();
		
		bool ProcessSingleResult();
		
		bool IsDone();
		
	private:
		std::vector<std::unique_ptr<IASyncOperation>> m_work;
		std::mutex m_workMutex;
		
		std::vector<std::unique_ptr<IASyncOperation>> m_completedWork;
		std::mutex m_completedWorkMutex;
	};
}
