// T1.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable: 4127)      // Conditional expression is a constant
#pragma comment(lib, "ws2_32.lib") 

#define	 DATA_BUFSIZE 5
#define  MAX_ASYN_IO_BUFF_COUNT 2

/**
@name : 重叠结构，可用于异步IO
*/
struct AsynIoResultImp: public OVERLAPPED
{
	AsynIoResultImp()
	{
		connSocket = INVALID_SOCKET;
		IniBuf();
	}

	// 测试用就不考虑释放了
	void IniBuf()
	{
		for (size_t i = 0; i < MAX_ASYN_IO_BUFF_COUNT; ++i)
		{
			buffList[i].len = DATA_BUFSIZE;
			buffList[i].buf = new  char[DATA_BUFSIZE];
		}
		buffCount = MAX_ASYN_IO_BUFF_COUNT;
		recvBytes = 0;
	}

	SOCKET			  connSocket;
	WSABUF			  buffList[MAX_ASYN_IO_BUFF_COUNT];
	DWORD             buffCount;
	DWORD			  recvBytes;
};


void  main(int argc, char **argv)
{
	WSADATA wsd;
	struct addrinfo *result = NULL, *ptr = NULL, hints = { 0 };
	DWORD Flags;
	int err, rc;

	AsynIoResultImp io;

	// Load Winsock
	rc = WSAStartup(MAKEWORD(2, 2), &wsd);
	if (rc != 0) 
	{
		fprintf(stderr, "Unable to load Winsock: %d\n", rc);
		return;
	}

	// Initialize the hints to retrieve the server address for IPv4
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

 	rc = getaddrinfo("192.168.10.42", "3014", &hints, &result);
 	if (rc != 0) 
 	{
 		fprintf(stderr, "getaddrinfo failed: %d\n", rc);
 		return;
 	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
	{
		io.connSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (io.connSocket == INVALID_SOCKET)
		{
			fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			return;
		}

		rc = connect(io.connSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (rc == SOCKET_ERROR) 
		{
			if (WSAECONNREFUSED == (err = WSAGetLastError())) 
			{
				closesocket(io.connSocket);
				io.connSocket = INVALID_SOCKET;
				continue;
			}
			fprintf(stderr, "connect failed: %d\n", err);
			freeaddrinfo(result);
			closesocket(io.connSocket);
			return;
		}
		break;
	}
	if (io.connSocket == INVALID_SOCKET)
	{
		fprintf(stderr, "Unable to establish connection with the server!\n");
		freeaddrinfo(result);
		return;
	}

	printf("Client connected...\n");

	// Create an event handle and setup an overlapped structure.
	io.hEvent = WSACreateEvent();
	if (io.hEvent == NULL) {
		fprintf(stderr, "WSACreateEvent failed: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(io.connSocket);
		return;
	}

	char data[DATA_BUFSIZE+1] = { 0 };

	// Call WSARecv until the peer closes the connection
	// or until an error occurs
	while (1) 
	{
		io.IniBuf();
		Flags = 0;
		rc = WSARecv(io.connSocket, (LPWSABUF)io.buffList, io.buffCount, &io.recvBytes, &Flags, &io, NULL);
		if ((rc == SOCKET_ERROR) && (WSA_IO_PENDING != (err = WSAGetLastError()))) 
		{
			fprintf(stderr, "WSARecv failed: %d\n", err);
			break;
		}

		rc = WSAWaitForMultipleEvents(1, &io.hEvent, TRUE, INFINITE, TRUE);
		if (rc == WSA_WAIT_FAILED) 
		{
			fprintf(stderr, "WSAWaitForMultipleEvents failed: %d\n", WSAGetLastError());
			break;
		}

		rc = WSAGetOverlappedResult(io.connSocket, &io, &io.recvBytes, FALSE, &Flags);
		if (rc == FALSE) 
		{
			fprintf(stderr, "WSARecv operation failed: %d\n", WSAGetLastError());
			break;
		}

		for (size_t i = 0; i < MAX_ASYN_IO_BUFF_COUNT; ++i)
		{
			size_t len = min(DATA_BUFSIZE, io.recvBytes - i*DATA_BUFSIZE);
			memmove(data, io.buffList[i].buf, len);
			data[len] = '\0';
			printf("Read %d bytes %s\n", io.recvBytes, data);
			if (len != DATA_BUFSIZE) { break; }
		}
	
		WSAResetEvent(io.hEvent);

		// If 0 bytes are received, the connection was closed
		if (io.recvBytes == 0)
			break;
	}

	WSACloseEvent(io.hEvent);
	closesocket(io.connSocket);
	freeaddrinfo(result);

	WSACleanup();

	return;
}



