#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

int main() {
	const short BUFF_SIZE = 1024;

	WSADATA wsData;

	int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

	if (erStat != 0) {
		std::cerr << "Error WinSock version initialization #";
		std::cerr << WSAGetLastError();
		return 1;
	}
	else std::cout << "WinSock initiliazation is OK!" << std::endl;

	SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSock == INVALID_SOCKET) {
		std::cerr << "Error initialization socket #" << WSAGetLastError() << std::endl;
		closesocket(serverSock);
		WSACleanup();
		return 1;
	}
	else std::cout << "Server socket initialization is OK" << std::endl;

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
	if (erStat <= 0) {
		std::cerr << "Error in IP translation to special numeric format" << std::endl;
		return 1;
	}

	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(1234);

	erStat = bind(serverSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		std::cerr << "Error Socket binding to server info. Error # " << WSAGetLastError() << std::endl;
		closesocket(serverSock);
		WSACleanup();
		return 1;
	}
	else std::cout << "Binding socket to Server info is OK" << std::endl;

	erStat = listen(serverSock, SOMAXCONN);

	if (erStat != 0) {
		std::cerr << "Can't start to listen to. Error # " << WSAGetLastError() << std::endl;
		closesocket(serverSock);
		WSACleanup();
		return 1;
	}
	else std::cout << "Listening..." << std::endl;

	sockaddr_in clientInfo;

	ZeroMemory(&clientInfo, sizeof(clientInfo));

	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConn = accept(serverSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (ClientConn == INVALID_SOCKET) {
		std::cerr << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << std::endl;
		closesocket(serverSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else std::cout << "Connection to a client established successfully" << std::endl;

	std::vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);							// Creation of buffers for sending and receiving data
	short packet_size = 0;												// The size of sending / receiving packet in bytes

	while (true) {
		packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);					// Receiving packet from client. Program is waiting (system pause) until receive
		std::cout << "Client's message: " << servBuff.data() << std::endl;

		std::cout << "Your (host) message: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		// Check whether server would like to stop chatting 
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(ClientConn, SD_BOTH);
			closesocket(serverSock);
			closesocket(ClientConn);
			WSACleanup();
			return 0;
		}

		packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << std::endl;
			closesocket(serverSock);
			closesocket(ClientConn);
			WSACleanup();
			return 1;
		}

	}

	closesocket(serverSock);
	closesocket(ClientConn);
	WSACleanup();

	return 0;
}