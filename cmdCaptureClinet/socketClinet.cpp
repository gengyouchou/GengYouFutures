#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 30666
#define buffer_size  4096

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    // 获取控制台的句柄
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int main() {
	WSADATA wsaData;
	SOCKET sock = INVALID_SOCKET;
	struct sockaddr_in serv_addr;
	char buffer[buffer_size] = { 0 };

	// 初始化 Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		return 1;
	}

	// 创建套接字文件描述符
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		std::cerr << "Socket creation failed" << std::endl;
		WSACleanup();
		return 1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// 将地址转换成二进制形式
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		std::cerr << "Invalid address / Address not supported" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// 连接服务器
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
		std::cerr << "Connection failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// 接收初始连接消息
	int valread = recv(sock, buffer, buffer_size, 0);
	if (valread > 0) {
		buffer[valread] = '\0';
		std::cout << "Message from server: " << buffer << std::endl;
	}

	std::string message="fk";
	while (true) {
		/*
		std::cout << "Enter message: ";
		std::getline(std::cin, message);

		if (message == "exit") {
			break;
		}

		send(sock, message.c_str(), message.length(), 0); // 发送消息给服务器
		valread = recv(sock, buffer, 1024, 0); // 读取服务器返回的消息

		if (valread > 0) {
			buffer[valread] = '\0';
			std::cout << "Message from server: " << buffer << std::endl;
		}
		else {
			std::cerr << "recv failed" << std::endl;
			break;
		}
		*/
		send(sock, message.c_str(), message.length(), 0); // 发送消息给服务器
		valread = recv(sock, buffer, buffer_size, 0); // 读取服务器返回的消息
		if (valread > 0) {
			buffer[valread] = '\0';
			std::cout << "Message from server: " << buffer << std::endl;
			
			Sleep(300);
			system("cls");	
		}
		else {
			std::cerr << "recv failed" << std::endl;
			break;
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}