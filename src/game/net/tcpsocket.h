#pragma once

#include <cstdint>
#include <algorithm>
#include <chrono>

#include "netplatform.h"

namespace TankGame
{
	class TCPSocket
	{
	public:
		explicit TCPSocket(int fd)
			: m_fd(fd) { }
		
		inline ~TCPSocket() noexcept
		{
			closesocket(m_fd);
		}
		
		TCPSocket(const TCPSocket& other) = delete;
		
		inline TCPSocket(TCPSocket&& other) noexcept
			: m_fd(other.m_fd)
		{
			other.m_fd = -1;
		}
		
		inline TCPSocket& operator=(TCPSocket other) noexcept
		{
			std::swap(m_fd, other.m_fd);
			return *this;
		}
		
		static TCPSocket CreateServer(uint16_t port);
		
		void MakeNonBlocking();
		
		inline void ClearReceiveTimeout()
		{
			setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, nullptr, 0);
		}
		
		inline void SetReceiveTimeout(std::chrono::microseconds time)
		{
			const timeval timeout = { 0, time.count() };
			setsockopt(m_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		}
		
		inline void SetSendTimeout(std::chrono::microseconds time)
		{
			const timeval timeout = { 0, time.count() };
			setsockopt(m_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
		}
		
		inline int64_t Read(void* buffer, size_t size)
		{
			return read(m_fd, buffer, size);
		}
		
		inline void Write(const void* buffer, size_t size)
		{
			write(m_fd, buffer, size);
		}
		
		inline TCPSocket Accept()
		{
			return TCPSocket(accept(m_fd, nullptr, nullptr));
		}
		
		inline bool OK() const
		{
			return m_fd != -1;
		}
		
	private:
		int m_fd;
	};
}
