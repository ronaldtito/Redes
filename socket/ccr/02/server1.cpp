#include <iostream>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <map>
#include <netinet/in.h>
#include <unistd.h>

std::map<std::string, int> clientSockets;

void clientHandler(int clientSocket, const std::string& clientNickname) {
    char buffer[256];
    ssize_t n;

    while (true) {
        n = read(clientSocket, buffer, sizeof(buffer));
        if (n <= 0) {
            std::cerr << clientNickname << " disconnected" << std::endl;
            close(clientSocket);
            clientSockets.erase(clientNickname);
            break;
        }

        buffer[n] = '\0';
        std::string message(buffer);

        // Parse the message to extract destination nickname
        size_t commaPos = message.find(',');
        if (commaPos != std::string::npos) {
            std::string destinationNickname = message.substr(0, commaPos);
            std::string messageBody = message.substr(commaPos + 1);

            auto it = clientSockets.find(destinationNickname);
            if (it != clientSockets.end()) {
                // Send the message to the destination client
                write(it->second, messageBody.c_str(), messageBody.size());
            }
        }
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1100);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    listen(serverSocket, 5);
    std::cout << "Server listening on port 1100" << std::endl;

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        char buffer[256];
        ssize_t n = read(clientSocket, buffer, sizeof(buffer));
        if (n <= 0) {
            std::cerr << "Invalid client request" << std::endl;
            close(clientSocket);
        } else {
            buffer[n] = '\0';
            std::string clientInfo(buffer);

            // Extract client nickname
            size_t commaPos = clientInfo.find(',');
            if (commaPos != std::string::npos) {
                std::string clientNickname = clientInfo.substr(0, commaPos);
                clientSockets[clientNickname] = clientSocket;
                std::cout << "Client " << clientNickname << " connected" << std::endl;

                std::thread(clientHandler, clientSocket, clientNickname).detach();
            }
        }
    }

    close(serverSocket);
    return 0;
}
