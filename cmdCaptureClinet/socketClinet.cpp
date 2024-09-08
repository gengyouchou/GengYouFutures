#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 30666
#define buffer_size 4096

void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	// Get the console handle
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int main()
{
	WSADATA wsaData;
	SOCKET sock = INVALID_SOCKET;
	struct sockaddr_in serv_addr;
	char buffer[buffer_size] = {0};

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed" << std::endl;
		return 1;
	}

	// Create socket descriptor
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cerr << "Socket creation failed" << std::endl;
		WSACleanup();
		return 1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert address to binary format
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		std::cerr << "Invalid address / Address not supported" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// Connect to the server
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
	{
		std::cerr << "Connection failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// Receive initial connection message
	int valread = recv(sock, buffer, buffer_size, 0);
	if (valread > 0)
	{
		buffer[valread] = '\0';
		std::cout << "Message from server: " << buffer << std::endl;
	}

	std::string message = "fk"; // Hardcoded message
	while (true)
	{
		send(sock, message.c_str(), message.length(), 0); // Send message to the server
		valread = recv(sock, buffer, buffer_size, 0);	  // Read the server's response

		if (valread > 0)
		{
			buffer[valread] = '\0';
			std::cout << "Message from server: " << buffer << std::endl;

			Sleep(300);	   // Delay for 300 milliseconds
			system("cls"); // Clear console
		}
		else
		{
			std::cerr << "recv failed" << std::endl;
			break;
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}
