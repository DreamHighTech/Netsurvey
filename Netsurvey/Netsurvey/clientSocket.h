#pragma once

#ifdef	_WINDOWS
#include <winsock2.h>
#else	// _WINDOWS
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef	unsigned int	SOCKET;
typedef struct sockaddr SOCKADDR;
#define	SD_BOTH			SHUT_RDWR
#define	SOCKET_ERROR    (-1)
#define INVALID_SOCKET	-1
#endif	// _WINDOWS

class clientSocket
{
public:
	clientSocket();
	~clientSocket();

	SOCKET CreateSocket(bool tcp, SOCKET sockobj); // create Socket
	bool CreateTCPClient(sockaddr_in* addr); // client for connect
};

