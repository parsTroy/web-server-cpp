#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SOCKET_TYPE = SOCKET;
    #define CLOSE_SOCKET closesocket
    #define SOCKET_ERROR_CODE WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    using SOCKET_TYPE = int;
    #define CLOSE_SOCKET close
    #define SOCKET_ERROR_CODE errno
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

class TestClient {
private:
    SOCKET_TYPE clientSocket;
    const std::string serverIP;
    const int serverPort;
    
public:
    TestClient(const std::string& ip, int port) 
        : serverIP(ip), serverPort(port), clientSocket(INVALID_SOCKET) {
        #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
        #endif
    }
    
    ~TestClient() {
        disconnect();
        #ifdef _WIN32
        WSACleanup();
        #endif
    }
    
    bool connect() {
        // Create socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create socket. Error: " << SOCKET_ERROR_CODE << std::endl;
            return false;
        }
        
        // Set up server address
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        
        if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
            std::cerr << "Invalid address: " << serverIP << std::endl;
            CLOSE_SOCKET(clientSocket);
            return false;
        }
        
        // Connect to server
        if (::connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), 
                     sizeof(serverAddr)) < 0) {
            std::cerr << "Connection failed. Error: " << SOCKET_ERROR_CODE << std::endl;
            CLOSE_SOCKET(clientSocket);
            return false;
        }
        
        std::cout << "Connected to server at " << serverIP << ":" << serverPort << std::endl;
        return true;
    }
    
    void disconnect() {
        if (clientSocket != INVALID_SOCKET) {
            CLOSE_SOCKET(clientSocket);
            clientSocket = INVALID_SOCKET;
            std::cout << "Disconnected from server" << std::endl;
        }
    }
    
    bool sendMessage(const std::string& message) {
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Not connected to server" << std::endl;
            return false;
        }
        
        int bytesSent = send(clientSocket, message.c_str(), message.length(), 0);
        if (bytesSent < 0) {
            std::cerr << "Failed to send message. Error: " << SOCKET_ERROR_CODE << std::endl;
            return false;
        }
        
        std::cout << "Sent: " << message << std::endl;
        return true;
    }
    
    std::string receiveMessage() {
        if (clientSocket == INVALID_SOCKET) {
            return "";
        }
        
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived <= 0) {
            return "";
        }
        
        buffer[bytesReceived] = '\0';
        std::string response(buffer);
        std::cout << "Received: " << response << std::endl;
        return response;
    }
    
    void runInteractive() {
        std::string input;
        std::cout << "Enter messages to send (type 'quit' to exit):" << std::endl;
        
        while (std::getline(std::cin, input)) {
            if (input == "quit") {
                break;
            }
            
            if (!sendMessage(input)) {
                break;
            }
            
            // Wait a bit for server response
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            std::string response = receiveMessage();
            if (response.empty()) {
                std::cout << "No response received from server" << std::endl;
                break;
            }
        }
    }
};

int main() {
    std::cout << "TCP Echo Client Test" << std::endl;
    
    try {
        TestClient client("127.0.0.1", 8080);
        
        if (!client.connect()) {
            std::cerr << "Failed to connect to server" << std::endl;
            return 1;
        }
        
        // Test basic echo functionality
        std::cout << "\n=== Testing Basic Echo ===" << std::endl;
        
        std::vector<std::string> testMessages = {
            "Hello, Server!",
            "This is a test message",
            "12345",
            "Special chars: !@#$%^&*()",
            "Unicode: 🚀🌟🎮"
        };
        
        for (const auto& msg : testMessages) {
            if (!client.sendMessage(msg)) {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            std::string response = client.receiveMessage();
            if (response == msg) {
                std::cout << "✓ Echo test passed for: " << msg << std::endl;
            } else {
                std::cout << "✗ Echo test failed for: " << msg << std::endl;
                std::cout << "  Expected: " << msg << std::endl;
                std::cout << "  Got: " << response << std::endl;
            }
        }
        
        std::cout << "\n=== Interactive Mode ===" << std::endl;
        client.runInteractive();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
