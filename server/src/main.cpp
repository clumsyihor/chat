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
		else std::cout << "Server socket initialization is OK" << std::endl;
	}

	~Network() {
		m_stop = true;
		WSACleanup();
		closesocket(m_socket);
	}

	bool SetupSocket(const std::string& ip, const int& port) {
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

		erStat = bind(m_socket, (sockaddr*)&servInfo, sizeof(servInfo));

		if (erStat != 0) {
			std::cerr << "Error Socket binding to server info. Error # " << WSAGetLastError() << std::endl;
			return false;
		}
		else std::cout << "Binding socket to Server info is OK" << std::endl;

		m_stop = false;

		return true;
	}

	void StartListen() {
		int erStat = listen(m_socket, SOMAXCONN);

		if (erStat != 0) {
			std::cerr << "Can't start to listen to. Error # " << WSAGetLastError() << std::endl;
			return;
		}
		else std::cout << "Listening..." << std::endl;

		sockaddr_in clientInfo;

		while (!m_stop) {
			ZeroMemory(&clientInfo, sizeof(clientInfo));

			int clientInfo_size = sizeof(clientInfo);

			SOCKET new_connection = accept(m_socket, (sockaddr*)&clientInfo, &clientInfo_size);

			if (new_connection == INVALID_SOCKET) {
				std::cerr << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << std::endl;
				closesocket(new_connection);
			}
			else std::cout << "Connection to a client established successfully" << std::endl;

			AddClient(new_connection);
		}
	}

	void AddClient(SOCKET socket) {
		m_clients.push_back(socket);
		std::thread client_handler_thread(&Network::ClientHandler, this, socket);
		client_handler_thread.detach();
	}

	void ClientHandler(SOCKET socket) {
		std::vector<char> data_buffer(MAX_BUFFER_SIZE);
		int data_size = 0;

		while (!m_stop) {
			data_size = recv(socket, data_buffer.data(), data_buffer.size(), 0);

			if (data_size > 0) {
				std::string message(data_buffer.data(), data_size);
				std::cout << "Client message: " << message << std::endl;

				message = "[] " + message;

				for (auto& client : m_clients) {
					if (client == socket) continue;
					int sent = send(client, message.c_str(), message.size(), 0);

					if (sent == SOCKET_ERROR) {
						std::cout << "Can't send message to Client. Error # "
							<< WSAGetLastError() << std::endl;
					}
				}
			}
			else if (data_size == 0) {
				std::cout << "Client disconnected." << std::endl;
				closesocket(socket);
				break;
			}
			else {
				std::cout << "Recv failed. Error # " << WSAGetLastError() << std::endl;
				closesocket(socket);
				break;
			}
		}
	}

private:
	WSADATA m_wsData;
	SOCKET m_socket;
	bool m_stop = true;

	const int MAX_BUFFER_SIZE = 2048;
	std::vector<SOCKET> m_clients;
};

int main() {
	Network net;
	net.SetupSocket("127.0.0.1", 1234);
	net.StartListen();
	
	return 0;
}