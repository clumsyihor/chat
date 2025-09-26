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

	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (clientSock == INVALID_SOCKET) {
		std::cerr << "Error initialization socket #" << WSAGetLastError() << std::endl;
		closesocket(clientSock);
		WSACleanup();
		return 1;
	} else std::cout << "Server socket initialization is OK" << std::endl;

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

	erStat = connect(clientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		std::cerr << "Connection to Server is FAILED. Error # " << WSAGetLastError() << std::endl;
		closesocket(clientSock);
		WSACleanup();
		return 1;
	}
	else std::cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << std::endl;

	std::vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);							// Buffers for sending and receiving data
	short packet_size = 0;

	while (true) {

		std::cout << "Your (Client) message to Server: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		// Check whether client like to stop chatting 
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(clientSock, SD_BOTH);
			closesocket(clientSock);
			WSACleanup();
			return 0;
		}

		packet_size = send(clientSock, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Server. Error # " << WSAGetLastError() << std::endl;
			closesocket(clientSock);
			WSACleanup();
			return 1;
		}

		packet_size = recv(clientSock, servBuff.data(), servBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't receive message from Server. Error # " << WSAGetLastError() << std::endl;
			closesocket(clientSock);
			WSACleanup();
			return 1;
		}
		else
			std::cout << "Server message: " << servBuff.data() << std::endl;

	}

	closesocket(clientSock);
	WSACleanup();

	return 0;
}