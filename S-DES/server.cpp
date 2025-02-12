#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <csignal>

constexpr int MAX = 80;
constexpr int PORT = 8080;
int server_fd;

void handleClient(int connfd) {
    char buffer[MAX];
    ssize_t bytesRead;

    while (true) {
        std::memset(buffer, 0, MAX);

        // Read the message from client
        bytesRead = read(connfd, buffer, sizeof(buffer) - 1);
        if (bytesRead < 0) {
            std::cerr << "Read error\n";
            break;
        } else if (bytesRead == 0) {
            std::cout << "Client disconnected.\n";
            break;
        }

        // Display client's message
        std::cout << "From client: " << buffer << "\nTo client: ";
        std::memset(buffer, 0, MAX);

        // Get server response
        std::cin.getline(buffer, MAX);

        // Send response to client
        if (write(connfd, buffer, std::strlen(buffer)) < 0) {
            std::cerr << "Write error\n";
            break;
        }

        // Exit if the server sends "exit"
        if (std::strncmp("exit", buffer, 4) == 0) {
            std::cout << "Server Exit...\n";
            break;
        }
    }
}

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received. Shutting down...\n";
    close(server_fd);
    exit(signum);
}

int main() {
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);

    // Register signal handler for graceful shutdown
    signal(SIGINT, signalHandler);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket successfully created.\n";

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::memset(&servaddr, 0, sizeof(servaddr));

    // Assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&servaddr), sizeof(servaddr)) != 0) {
        perror("Socket bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket successfully binded.\n";

    // Listen for connections
    if (listen(server_fd, 5) != 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening...\n";

    // Accept and handle clients in a loop
    while (true) {
        int client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&cliaddr), &cliaddr_len);
        if (client_fd < 0) {
            perror("Server accept failed");
            continue;
        }
        std::cout << "Server accepted the client.\n";

        // Handle client communication
        handleClient(client_fd);

        // Close client socket
        close(client_fd);
    }

    // Close server socket
    close(server_fd);

    return 0;
}

//This is the server code for the S-DES encryption and decryption. The server listens for incoming connections from clients and handles the encryption and decryption of messages using the S-DES algorithm. The server uses a fixed 10-bit key for simplicity and generates two subkeys using the key. The server implements the S-DES encryption and decryption functions, as well as helper functions for permutation, left shift, XOR, S-Box substitution, and F-function. The server reads plaintext or ciphertext messages from clients, encrypts or decrypts them using S-DES, and sends the result back to the clients. The server also includes functions to convert characters to binary strings and vice versa, as well as functions to read a specific number of bytes from a socket. The server uses a socket to communicate with clients and handles client connections in a loop, accepting and handling multiple clients concurrently. The server also includes signal handling to gracefully shut down the server when an interrupt signal is received.

// #include <iostream>
// #include <bitset>
// #include <string>
// #include <utility>
// #include <cstring>
// #include <unistd.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <vector>

// using namespace std;

// constexpr int MAX = 80;
// constexpr int PORT = 8080;

// // Fixed 10-bit key for simplicity
// string key = "1010000010";

// // Permutation functions
// int P10[10] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
// int P8[8] = {6, 3, 7, 4, 8, 5, 10, 9};
// int P4[4] = {2, 4, 3, 1};
// int IP[8] = {2, 6, 3, 1, 4, 8, 5, 7};
// int IP_inv[8] = {4, 1, 3, 5, 7, 2, 8, 6};
// int EP[8] = {4, 1, 2, 3, 2, 3, 4, 1};
// int S0[4][4] = {{1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 3, 2}};
// int S1[4][4] = {{0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 0}, {2, 1, 0, 3}};

// // Permutation helper function
// string permute(const string &input, const int *table, int size) {
//     string output = "";
//     for (int i = 0; i < size; i++) {
//         output += input[table[i] - 1];
//     }
//     return output;
// }

// // Left shift
// string leftShift(const string &key, int shifts) {
//     return key.substr(shifts) + key.substr(0, shifts);
// }

// // Generate two subkeys
// pair<string, string> generateKeys(const string &key) {
//     string permutedKey = permute(key, P10, 10);
//     string left = permutedKey.substr(0, 5);
//     string right = permutedKey.substr(5, 5);
//     left = leftShift(left, 1);
//     right = leftShift(right, 1);
//     string K1 = permute(left + right, P8, 8);

//     left = leftShift(left, 2);
//     right = leftShift(right, 2);
//     string K2 = permute(left + right, P8, 8);

//     return {K1, K2};
// }

// // XOR function
// string XOR(const string &a, const string &b) {
//     string result = "";
//     for (size_t i = 0; i < a.size(); i++) {
//         result += (a[i] == b[i]) ? '0' : '1';
//     }
//     return result;
// }

// // S-Box substitution
// string SBox(const string &input, int sbox[4][4]) {
//     int row = (input[0] - '0') * 2 + (input[3] - '0');
//     int col = (input[1] - '0') * 2 + (input[2] - '0');
//     return bitset<2>(sbox[row][col]).to_string();
// }

// // F-function
// string f(const string &right, const string &key) {
//     string expanded = permute(right, EP, 8);
//     string xored = XOR(expanded, key);
//     string left = xored.substr(0, 4);
//     string rightPart = xored.substr(4, 4);
//     string sboxOutput = SBox(left, S0) + SBox(rightPart, S1);
//     return permute(sboxOutput, P4, 4);
// }

// // Encryption/Decryption function
// string sdes(const string &input, const string &K1, const string &K2, bool decrypt) {
//     string permutedInput = permute(input, IP, 8);
//     string left = permutedInput.substr(0, 4);
//     string right = permutedInput.substr(4, 4);
//     string key1 = decrypt ? K2 : K1;
//     string key2 = decrypt ? K1 : K2;

//     string temp = f(right, key1);
//     string leftXOR = XOR(left, temp);
//     swap(leftXOR, right);

//     temp = f(right, key2);
//     string result = XOR(leftXOR, temp) + right;

//     return permute(result, IP_inv, 8);
// }

// // Helper function to convert a character to an 8-bit binary string
// string charToBinaryString(char c) {
//     return bitset<8>(c).to_string();
// }

// // Helper function to convert an 8-bit binary string to a character
// char binaryStringToChar(const string &s) {
//     return static_cast<char>(bitset<8>(s).to_ulong());
// }

// // Encrypt a plaintext string
// std::string sdes_encrypt(const std::string &plaintext) {
//     auto keys = generateKeys(key);
//     std::string ciphertext = "";
//     for (char c : plaintext) {
//         std::string binaryChar = charToBinaryString(c);
//         std::string encryptedChar = sdes(binaryChar, keys.first, keys.second, false);
//         ciphertext += binaryStringToChar(encryptedChar);
//     }
//     return ciphertext;
// }

// // Decrypt a ciphertext string
// std::string sdes_decrypt(const std::string &ciphertext) {
//     auto keys = generateKeys(key);
//     std::string plaintext = "";
//     for (char c : ciphertext) {
//         std::string binaryChar = charToBinaryString(c);
//         std::string decryptedChar = sdes(binaryChar, keys.first, keys.second, true);
//         plaintext += binaryStringToChar(decryptedChar);
//     }
//     return plaintext;
// }

// // Function to read a specific number of bytes from a socket
// ssize_t readNBytes(int socket, char* buffer, size_t n) {
//     size_t totalRead = 0;
//     while (totalRead < n) {
//         ssize_t bytesRead = read(socket, buffer + totalRead, n - totalRead);
//         if (bytesRead <= 0) {
//             return bytesRead; // Error or connection closed
//         }
//         totalRead += bytesRead;
//     }
//     return totalRead;
// }

// void handleClient(int connfd) {
//     char buffer[MAX];
//     ssize_t bytesRead;

//     while (true) {
//         std::memset(buffer, 0, MAX);

//         // Read the message from client
//         bytesRead = read(connfd, buffer, sizeof(buffer) - 1);
//         if (bytesRead < 0) {
//             std::cerr << "Read error\n";
//             break;
//         } else if (bytesRead == 0) {
//             std::cout << "Client disconnected.\n";
//             break;
//         }

//         std::string command(buffer);
//         std::string response;

//         if (command.find("encrypt(") == 0 && command.back() == ')') {
//             std::string plaintext = command.substr(8, command.size() - 9);
//             response = sdes_encrypt(plaintext);
//         } else if (command.find("decrypt(") == 0 && command.back() == ')') {
//             std::string ciphertext = command.substr(8, command.size() - 9);
//             response = sdes_decrypt(ciphertext);
//         } else {
//             response = "Invalid command";
//         }

//         // Send response to client
//         if (write(connfd, response.c_str(), response.size()) < 0) {
//             std::cerr << "Write error\n";
//             break;
//         }
//     }
// }

// int main() {
//     int server_fd, client_fd;
//     socklen_t len;
//     struct sockaddr_in servaddr, cliaddr;

//     // Create socket
//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1) {
//         std::cerr << "Socket creation failed...\n";
//         exit(EXIT_FAILURE);
//     }
//     std::cout << "Socket successfully created.\n";

//     std::memset(&servaddr, 0, sizeof(servaddr));

//     // Assign IP and PORT
//     servaddr.sin_family = AF_INET;
//     servaddr.sin_addr.s_addr = INADDR_ANY;
//     servaddr.sin_port = htons(PORT);
    
//     // Bind socket
//     if (::bind(server_fd, reinterpret_cast<struct sockaddr*>(&servaddr), sizeof(servaddr)) != 0) {
//         std::cerr << "Socket bind failed...\n";
//         close(server_fd);
//         exit(EXIT_FAILURE);
//     }
//     std::cout << "Socket successfully binded.\n";


//     // Listen for connections
//     if (listen(server_fd, 5) != 0) {
//         std::cerr << "Listen failed...\n";
//         close(server_fd);
//         exit(EXIT_FAILURE);
//     }
//     std::cout << "Server listening...\n";

//     len = sizeof(cliaddr);

//     // Accept the data packet from client and verification
//     client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&cliaddr), &len);
//     if (client_fd < 0) {
//         std::cerr << "Server accept failed...\n";
//         close(server_fd);
//         exit(EXIT_FAILURE);
//     }
//     std::cout << "Server accepted the client...\n";

//     // Function for chatting between client and server
//     handleClient(client_fd);

//     // After chatting close the socket
//     close(server_fd);
//     return 0;
// }
