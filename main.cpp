//#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
//#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

int main() {
    std::vector<std::string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const std::string& word : msg) {
        std::cout << word << " ";
    }
    std::cout << std::endl;

    // Initialize winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);
    
    if (wsOk != 0) {
        std::cerr << "Can't initialize winsock! Quitting!" << std::endl;
        return 0;
    }

    // Create socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "Can't create a server socket! Quitting!" << std::endl;
        return 0;
    }

    // Bind the socket to an ip address and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000); // host to network short
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton ....

    bind(listening, (sockaddr*) &hint, sizeof(hint)); // The bind function needs the address of the hint and its size, and for it to be cast that way.

    // Tell winsock the socket is for listening
    listen(listening, SOMAXCONN);

    // Wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client); // Cause we need to fill this in

    SOCKET clientSocket = accept(listening, (sockaddr*) &client, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Can't create a client socket! Quitting!" << std::endl;
        return 0;
    }

    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on

    ZeroMemory(host, NI_MAXHOST);       // same as memset(host, 0, NI_MAXHOST); Use memset if want portable.
    ZeroMemory(service, NI_MAXSERV);    // same as memset(service, 0, NI_MAXHOST);

    // Doing DNS Lookup on the client information, put information in host & service. If we can't get that, display something.
    if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        std::cout << host << " connected on port " << service << std::endl;
    }
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on port " << 
            ntohs(client.sin_port) << std::endl;
             
    }

    // Close listening socket
    closesocket(listening);

    // While loop: accept and echo message back to client
    char buf[4096];

    while (true) {
        ZeroMemory(buf, 4096);
        
        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Error in recv(). Quitting!" << std::endl;
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "Client disconnected " << std::endl;
            break;
        }

        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);

    }

    // Close the socket
    closesocket(clientSocket);
    // Clean up winsock
    WSACleanup();
    
    return 0;
}