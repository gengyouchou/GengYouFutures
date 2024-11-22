#include <windows.h>
#include <iostream>
#include <string>

int main()
{
    const char *pipeName = R"(\.\pipe\FuturesPipe)"; // Named pipe name

    // Create the named pipe
    HANDLE hPipe = CreateNamedPipe(
        pipeName,                                              // Pipe name
        PIPE_ACCESS_DUPLEX,                                    // Duplex access permission
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // Message mode
        1,                                                     // Maximum instances
        1024,                                                  // Output buffer size
        1024,                                                  // Input buffer size
        0,                                                     // Default wait time
        nullptr);                                              // Default security attributes

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to create pipe, error code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Waiting for Python client to connect..." << std::endl;

    // Wait for the Python client to connect
    BOOL connected = ConnectNamedPipe(hPipe, nullptr);
    if (!connected)
    {
        std::cerr << "Connection failed, error code: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    std::cout << "Connection successful, starting to send data..." << std::endl;

    // Simulate futures data
    std::string data = R"({"time": "2024-11-17 10:05", "price": 1234.56, "volume": 100, "symbol": "FUTURE1"})";

    // Write data to the pipe
    DWORD bytesWritten;
    BOOL writeResult = WriteFile(hPipe, data.c_str(), data.size(), &bytesWritten, nullptr);
    if (writeResult)
    {
        std::cout << "Successfully wrote data: " << data << std::endl;
    }
    else
    {
        std::cerr << "Failed to write data, error code: " << GetLastError() << std::endl;
    }

    // Close the pipe
    CloseHandle(hPipe);
    return 0;
}
