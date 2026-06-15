#include "ConnectionChecker.h"
#include "Engine/Engine.h"
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
#include <WinSock2.h>
#include <Ws2tcpip.h>
#endif
#include "DungeonsGameInstance.h"
#include "online/OnlineCommon.h"


UConnectionChecker::UConnectionChecker()
{	
}

UConnectionChecker::~UConnectionChecker()
{
}

void UConnectionChecker::Init()
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	//Winsock Initialisation
	WSADATA wsaData;
	int result;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		FString fstring = FString::Printf(TEXT("WSAStartup failed: %d"), result);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, fstring);
	}
#endif	
}


EOnlineServerConnectionStatus::Type UConnectionChecker::CheckConnection()
{
#if PLATFORM_WINDOWS || PLATFORM_XBOXONE
	EOnlineServerConnectionStatus::Type hasConnection = EOnlineServerConnectionStatus::ConnectionDropped;
	ADDRINFOW hints;
	ADDRINFOW *result = NULL;
	ADDRINFOW *ptr = NULL;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int* sockets;
	int SocketCount = 0, NSockets = 0;

	fd_set write, except;

	FD_ZERO(&write);
	FD_ZERO(&except);

	DWORD dwRetval;

	dwRetval = GetAddrInfoW(ConnectionCheckURL, TEXT("443"), &hints, &result);

	if (dwRetval != 0 || result == nullptr)
	{
		return hasConnection;
	}

	//Compute number of sockets
	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{
		if (ptr->ai_family == AF_INET || ptr->ai_family == AF_INET6)
		{
			//Only count ipv4 & ipv4 addresses
			NSockets++;
		}
	}

	if (!NSockets)
	{
		return hasConnection;
	}
	else
	{
		sockets = new int[NSockets];
	}

	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
	{

		if (ptr->ai_family == AF_INET || ptr->ai_family == AF_INET6)
		{
			sockets[SocketCount] = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			unsigned long mode = 1;
			//Set as non-blocking
			int s = ioctlsocket(sockets[SocketCount], FIONBIO, &mode);

			int iResult = connect(sockets[SocketCount], ptr->ai_addr, ptr->ai_addrlen);

			if (iResult != 0 && WSAGetLastError() == WSAEWOULDBLOCK)
			{
				FD_SET(sockets[SocketCount], &write);
				FD_SET(sockets[SocketCount], &except);
			}
		}
		SocketCount++;
	}

	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	int sel = select(NULL, NULL, &write, &except, &tv);

	if (sel <= 0)
	{
		WSASetLastError(WSAETIMEDOUT);
		//Close all sockets

		FString fstring = FString::Printf(TEXT("Fail select: %d"), WSAGetLastError());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, fstring);
		hasConnection = EOnlineServerConnectionStatus::ConnectionDropped;
	}
	else
	{
		for (int i = 0; i < NSockets; i++)
		{
			if (FD_ISSET(sockets[i], &write))
			{
				//Close sockets
				hasConnection = EOnlineServerConnectionStatus::Connected;
				break;
			}
		}
	}

	for (int i = 0; i < NSockets; i++)
	{
		closesocket(sockets[i]);
	}

	delete[] sockets;
	FreeAddrInfoW(result);
	return hasConnection;
#endif
}