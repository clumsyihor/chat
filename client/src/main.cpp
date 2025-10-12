#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

class Network {
public:
	Network() {
		int erStat = WSAStartup(MAKEWORD(2, 2), &m_wsData);

		if (erStat != 0) {
			std::cerr << "Error WinSock version initialization #";
			std::cerr << WSAGetLastError();
			return;
		}
		else std::cout << "WinSock initiliazation is OK!" << std::endl;

		m_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (m_socket == INVALID_SOCKET) {
			std::cerr << "Error initialization socket #" << WSAGetLastError() << std::endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}
		else std::cout << "Client socket initialization is OK" << std::endl;
	}

	~Network() {
		m_stop = true;
		WSACleanup();
		closesocket(m_socket);
	}

	bool Connect(const std::string& ip, const int& port) {
		in_addr ip_to_num;
		int erStat = inet_pton(AF_INET, ip.c_str(), &ip_to_num);
		if (erStat <= 0) {
			std::cerr << "Error in IP translation to special numeric format" << std::endl;
			return false;
		}

		sockaddr_in servInfo;

		ZeroMemory(&servInfo, sizeof(servInfo));

		servInfo.sin_family = AF_INET;
		servInfo.sin_addr = ip_to_num;
		servInfo.sin_port = htons(port);

		erStat = connect(m_socket, (sockaddr*)&servInfo, sizeof(servInfo));

		if (erStat != 0) {
			std::cerr << "Connection to Server is FAILED. Error # " << WSAGetLastError() << std::endl;
			return false;
		}
		else {
			m_stop = false;
			return true;
		}
	}

	void Handler() {
		std::vector<char> data_buffer(MAX_BUFFER_SIZE);
		int data_size = 0;

		while (!m_stop) {
			data_size = recv(m_socket, data_buffer.data(), data_buffer.size(), 0);

			if (data_size > 0) {
				std::string message(data_buffer.data(), data_size);
				std::cout << "Server message: " << message << std::endl;
			}
			else if (data_size == 0) {
				std::cout << "Server disconnected." << std::endl;
				closesocket(m_socket);
				break;
			}
			else {
				std::cerr << "Recv failed. Error # " << WSAGetLastError() << std::endl;
				closesocket(m_socket);
				break;
			}
		}
	}


	bool Send(const char* data, size_t size) {
		int data_size = send(m_socket, data, size, 0);

		if (data_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Server. Error # " << WSAGetLastError() << std::endl;
			return false;
		}

		return true;
	}
private:
	WSADATA m_wsData;
	SOCKET m_socket;
	bool m_stop = true;

	const int MAX_BUFFER_SIZE = 2048;
};

int main() {
	Network net;
	net.Connect("127.0.0.1", 1234);
	std::thread handler_thread(&Network::Handler, &net);

	while (true) {
		std::string str;
		std::getline(std::cin, str);
		net.Send(str.c_str(), str.size());
	}

	handler_thread.join();
	return 0;
}