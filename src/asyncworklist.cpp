#include "asyncworklist.h"
#include "utils/utils.h"

#include <iostream>
#include <chrono>

namespace TankGame
{
	void ASyncWorkList::SubmitWork(std::unique_ptr<IASyncOperation>&& work)
	{
		std::lock_guard<std::mutex> lock(m_workMutex);
		m_work.emplace_back(std::move(work));
	}
	
	void ASyncWorkList::BeginProcessing()
	{
		std::async(std::launch::async, [this]
		{
			std::lock_guard<std::mutex> lock(m_workMutex);
			
			while (!m_work.empty())
			{
				m_work.back()->DoWork();
				
				std::lock_guard<std::mutex> completedWorkLock(m_completedWorkMutex);
				m_completedWork.emplace_back(std::move(m_work.back()));
				m_work.pop_back();
			}
		});
	}
	
	void ASyncWorkList::ProcessResults()
	{
		while (ProcessSingleResult()) { }
	}
	
	bool ASyncWorkList::ProcessSingleResult()
	{
		std::unique_lock<std::mutex> lock(m_completedWorkMutex);
		if (m_completedWork.empty())
			return false;
		
		std::unique_ptr<IASyncOperation> work = std::move(m_completedWork.back());
		m_completedWork.pop_back();
		
		lock.unlock();
		
		work->ProcessResult();
	}
	
	bool ASyncWorkList::IsDone()
	{
		std::unique_lock<std::mutex> workLock(m_workMutex, std::try_to_lock);
		if (!workLock.owns_lock())
			return false;
		
		std::unique_lock<std::mutex> completedWorkLock(m_completedWorkMutex, std::try_to_lock);
		if (!completedWorkLock.owns_lock())
			return false;
		
		return m_work.empty() && m_completedWork.empty();
	}
}
