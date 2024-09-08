#include <chrono> // For time delay
#include <iostream>
#include <string>
#include <thread> // For std::this_thread::sleep_for
#include <winsock2.h>

#include <windows.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 30666
#define buffer_size 4096

void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
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

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert address to binary format
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		std::cerr << "Invalid address / Address not supported" << std::endl;
		WSACleanup();
		return 1;
	}

	// Continuous loop to attempt connection to the server
	while (true)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			std::cerr << "Socket creation failed" << std::endl;
			WSACleanup();
			return 1;
		}

		std::cout << "Attempting to connect to server..." << std::endl;
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		{
			std::cerr << "Connection failed, retrying in 5 seconds..." << std::endl;
			closesocket(sock);
			std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait before retrying
			continue;											  // Retry connection
		}

		std::cout << "Connected to server!" << std::endl;

		// Receive initial message from server
		int valread = recv(sock, buffer, buffer_size, 0);
		if (valread > 0)
		{
			buffer[valread] = '\0';
			std::cout << "Message from server: " << buffer << std::endl;
		}

		// Start communication loop with server
		std::string message = "Hello, server!";
		while (true)
		{
			send(sock, message.c_str(), message.length(), 0); // Send message to server
			valread = recv(sock, buffer, buffer_size, 0);	  // Receive message from server

			if (valread > 0)
			{
				buffer[valread] = '\0';
				std::cout << "Message from server: " << buffer << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
				system("cls"); // Clear screen
			}
			else if (valread == 0)
			{
				std::cerr << "Server disconnected. Reconnecting..." << std::endl;
				break; // Exit inner loop to reconnect
			}
			else
			{
				std::cerr << "recv failed" << std::endl;
				break;
			}
		}

		closesocket(sock); // Close socket and retry connection
	}

	WSACleanup();
	return 0;
}
