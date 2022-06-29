#pragma once

#include <future>
#include <mutex>

namespace TankGame
{
#ifdef __EMSCRIPTEN__
	static constexpr std::launch LOADING_LAUNCH_POLICY = std::launch::deferred;
#else
	static constexpr std::launch LOADING_LAUNCH_POLICY = std::launch::async;
#endif
	
	class ASyncWorkList
	{
	public:
		template <typename T, typename C>
		void Add(std::future<T> future, C onCompleted)
		{
			m_items.push_back(std::make_unique<Job<T, C>>(std::move(future), std::move(onCompleted)));
		}
		
		void Poll()
		{
			if (!m_items.empty() && m_items.back()->Poll())
				m_items.pop_back();
		}
		
		bool IsDone()
		{
			return m_items.empty();
		}
		
	private:
		struct JobBase
		{
			virtual ~JobBase() { }
			virtual bool Poll() = 0;
		};
		
		template <typename T, typename C>
		struct Job : JobBase
		{
			std::future<T> future;
			C onCompleted;
			
			Job(std::future<T> _future, C _onCompleted)
				: future(std::move(_future)), onCompleted(std::move(_onCompleted)) { }
			
			bool Poll() override
			{
				if (future.wait_for(std::chrono::seconds(0)) == std::future_status::timeout)
					return false;
				onCompleted(future.get());
				return true;
			}
		};
		
		std::vector<std::unique_ptr<JobBase>> m_items;
	};
}
