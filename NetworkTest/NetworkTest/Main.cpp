#ifndef UNICODE
#define UNICODE 1
#endif

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>   // Needed for _wtoi
#include <iostream>

int main()
{
	SOCKET sock = INVALID_SOCKET;
	WSADATA wsaData = {0};
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) printf("Congratulations, you're a failure.\n");
	else printf("It worked!!!\n");

	char buffer[2];
	recv(sock, buffer, 1, MSG_PEEK);
	buffer[1] = 0;
	std::cout << (int)buffer[0];

	closesocket(sock);

	WSACleanup();
	return 0;
}