#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

constexpr int MAX = 80;
constexpr int PORT = 8080;

void chat(int sockfd) {
    char buff[MAX];
    while (true) {
        std::memset(buff, 0, sizeof(buff));
        std::cout << "Enter the string: ";
        std::cin.getline(buff, MAX);

        // Exit if the client inputs "exit"
        if (std::strncmp("exit", buff, 4) == 0) {
            std::cout << "Client Exit...\n";
            break;
        }

        // Write to server
        ssize_t bytes_written = write(sockfd, buff, strlen(buff));
        if (bytes_written < 0) {
            perror("Error writing to socket");
            break;
        }

        std::memset(buff, 0, sizeof(buff));

        // Read from server
        ssize_t bytes_read = read(sockfd, buff, sizeof(buff) - 1);
        if (bytes_read < 0) {
            perror("Error reading from socket");
            break;
        }

        std::cout << "From Server: " << buff << std::endl;

        // Exit if the server sends "exit"
        if (std::strncmp("exit", buff, 4) == 0) {
            std::cout << "Server initiated exit. Client Exit...\n";
            break;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket successfully created.\n";

    std::memset(&servaddr, 0, sizeof(servaddr));

    // Assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&servaddr), sizeof(servaddr)) != 0) {
        perror("Connection with the server failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Connected to the server.\n";

    // Function for chat
    chat(sockfd);

    // Close the socket
    close(sockfd);
    return 0;
}

//Below this is the code for S-DES Client communication

// #include <iostream>
// #include <cstring>
// #include <unistd.h>
// #include <arpa/inet.h>

// constexpr int MAX = 80;
// constexpr int PORT = 8080;

// using namespace std;

// void chat(int sockfd) {
//     char buff[MAX];
//     while (true) {
//         memset(buff, 0, sizeof(buff));
//         cout << "Enter command (encrypt(plaintext) or decrypt(ciphertext)): ";
//         cin.getline(buff, MAX);

//         // Send command to server
//         ssize_t bytes_written = write(sockfd, buff, strlen(buff));
//         if (bytes_written < 0) {
//             cerr << "Error writing to socket\n";
//             break;
//         }

//         memset(buff, 0, sizeof(buff));

//         // Read response from server
//         ssize_t bytes_read = read(sockfd, buff, sizeof(buff) - 1);
//         if (bytes_read < 0) {
//             cerr << "Error reading from socket\n";
//             break;
//         }

//         cout << "From Server: " << buff << endl;

//         if (strncmp(buff, "exit", 4) == 0) {
//             cout << "Client Exit...\n";
//             break;
//         }
//     }
// }

// int main() {
//     int sockfd;
//     struct sockaddr_in servaddr;

//     // Create socket
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd == -1) {
//         cerr << "Socket creation failed...\n";
//         exit(EXIT_FAILURE);
//     }
//     cout << "Socket successfully created.\n";

//     memset(&servaddr, 0, sizeof(servaddr));

//     // Assign IP and PORT
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_port = htons(PORT);

//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
//         cerr << "Invalid address/ Address not supported\n";
//         exit(EXIT_FAILURE);
//     }

//     // Connect to the server
//     if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&servaddr), sizeof(servaddr)) != 0) {
//         cerr << "Connection with the server failed...\n";
//         exit(EXIT_FAILURE);
//     }
//     cout << "Connected to the server.\n";

//     // Function for chat
//     chat(sockfd);

//     // Close the socket
//     close(sockfd);
//     return 0;
// }
