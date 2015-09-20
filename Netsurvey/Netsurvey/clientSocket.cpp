#include "stdafx.h"
#include "clientSocket.h"

#include <time.h>
#include <errno.h>

#ifdef _WINDOWS
#include <Ws2tcpip.h>
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")

#define net_errno (WSAGetLastError())
#define SEND(fd,buf,len) send((fd),(buf),(len),0)
#define CLOSE_SOCKET(sk) shutdown((sk), SD_BOTH); closesocket((sk))
#ifdef EWOULDBLOCK
#undef EWOULDBLOCK
#undef EINPROGRESS
#undef ECONNRESET
#undef EMSGSIZE
#endif
#define EWOULDBLOCK	WSAEWOULDBLOCK
#define EINPROGRESS	WSAEINPROGRESS
#define	ECONNRESET	WSAECONNRESET
#define	EMSGSIZE	WSAEMSGSIZE
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#define net_errno errno
#ifdef _ANDROID
#define SEND(fd,buf,len) send((fd),(buf),(len), MSG_NOSIGNAL)
#else
#define SEND(fd,buf,len) send((fd),(buf),(len), 0)
#endif
#define CLOSE_SOCKET(sk) shutdown((sk), 2); close((sk))
#endif


clientSocket::clientSocket()
{
}


clientSocket::~clientSocket()
{
}

SOCKET clientSocket::CreateSocket(bool tcp, SOCKET sockobj)
{
	SOCKET sock = sockobj;

	// create socket
	if (sock == INVALID_SOCKET)
	{
		if (tcp)
		{
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		else
		{
			// Create a Socket that is bound to a specific service provide
			sock = socket(AF_INET, SOCK_DGRAM, 0);
		}

		if (INVALID_SOCKET == sock)
		{
			return INVALID_SOCKET;
		}
	}

	// set socket to non block
#ifdef _WINDOWS
	/*unsigned long val = 1;
	if (ioctlsocket(sock, FIONBIO, &val) != 0)
	{
		CLOSE_SOCKET(sock);
		return INVALID_SOCKET;
	}

	struct linger ling;
	ling.l_onoff = 1;
	ling.l_linger = 0;

	setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));*/

#else // _WINDOWS
	int f_old;
	f_old = fcntl(sock, F_GETFL, 0);
	if (f_old < 0)
	{
		CLOSE_SOCKET(sock);
		return INVALID_SOCKET;
	}
	f_old |= O_NONBLOCK;
	if (fcntl(sock, F_SETFL, f_old) < 0)
	{
		CLOSE_SOCKET(sock);
		return INVALID_SOCKET;
	}
#endif // _WINDOWS

	return sock;
}

bool clientSocket::CreateTCPClient(sockaddr_in* addr) // client for connect
{
	// Create a Socket that is bound to a specific service provide
	SOCKET sock = CreateSocket(true, INVALID_SOCKET);

	if (INVALID_SOCKET == sock)
		return false;

	// try to connect - if fail, server not ready
	if (SOCKET_ERROR == connect(sock, (struct sockaddr*)addr, sizeof(struct sockaddr)))
	{
		if (net_errno != EINPROGRESS && net_errno != EWOULDBLOCK)
		{
			CLOSE_SOCKET(sock);
			return false;
		}
	}

	CLOSE_SOCKET(sock);
	return true;
}
