#include <iostream>
#include <cstring>
#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

int main() {
    std::string nickname;
    std::cout << "Enter your nickname: ";
    std::cin >> nickname;
    std::cin.ignore(); // Clear the newline character from the input buffer

    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;

    if (-1 == SocketFD) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

    if (0 > Res) {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    } else if (0 == Res) {
        perror("char string (second parameter does not contain a valid IP address");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    std::string message;
    std::cout << "Enter your message: ";
    std::getline(std::cin, message);

    // Combine the nickname, destination nickname, and message in the format "nickname,destinationNickname,message"
    std::string fullMessage = nickname + "," + message;
    n = write(SocketFD, fullMessage.c_str(), fullMessage.length());

    close(SocketFD);
    return 0;
}
