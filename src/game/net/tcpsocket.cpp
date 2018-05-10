#include "tcpsocket.h"


namespace TankGame
{
	TCPSocket TCPSocket::CreateServer(uint16_t port)
	{
		int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0)
			return TCPSocket(-1);
		
		sockaddr_in address = { };
		address.sin_family = AF_INET;
		address.sin_port = htons(port);
		address.sin_addr.s_addr = INADDR_ANY;
		if (bind(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0)
			return TCPSocket(-1);
		
		listen(fd, 10);
		
		return TCPSocket(fd);
	}
	
	void TCPSocket::MakeNonBlocking()
	{
		fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK);
	}
}
