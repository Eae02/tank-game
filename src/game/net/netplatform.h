#pragma once

#ifdef _WIN32

#include <Winsock2.h>

#else

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>

#define closesocket close

#endif
