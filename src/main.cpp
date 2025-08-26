#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <csignal>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SOCKET_TYPE = SOCKET;
    using SOCKET_SIZE_TYPE = int;
    #define CLOSE_SOCKET closesocket
    #define SOCKET_ERROR_CODE WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    using SOCKET_TYPE = int;
    using SOCKET_SIZE_TYPE = socklen_t;
    #define CLOSE_SOCKET close
    #define SOCKET_ERROR_CODE errno
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

class TCPServer {
private:
    SOCKET_TYPE serverSocket;
    std::atomic<bool> running;
    std::vector<std::thread> clientThreads;
    const int port;
    
public:
    TCPServer(int port) : port(port), running(false) {
        #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
        #endif
    }
    
    ~TCPServer() {
        stop();
        #ifdef _WIN32
        WSACleanup();
        #endif
    }
    
    bool start() {
        // Create socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create socket. Error: " << SOCKET_ERROR_CODE << std::endl;
            return false;
        }
        
        // Set socket options
        int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, 
                       reinterpret_cast<char*>(&opt), sizeof(opt)) < 0) {
            std::cerr << "setsockopt failed. Error: " << SOCKET_ERROR_CODE << std::endl;
            CLOSE_SOCKET(serverSocket);
            return false;
        }
        
        // Bind socket
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);
        
        if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), 
                 sizeof(serverAddr)) < 0) {
            std::cerr << "Bind failed. Error: " << SOCKET_ERROR_CODE << std::endl;
            CLOSE_SOCKET(serverSocket);
            return false;
        }
        
        // Listen for connections
        if (listen(serverSocket, 5) < 0) {
            std::cerr << "Listen failed. Error: " << SOCKET_ERROR_CODE << std::endl;
            CLOSE_SOCKET(serverSocket);
            return false;
        }
        
        running = true;
        std::cout << "TCP Echo Server started on port " << port << std::endl;
        std::cout << "Waiting for connections..." << std::endl;
        
        return true;
    }
    
    void run() {
        while (running) {
            struct sockaddr_in clientAddr;
            SOCKET_SIZE_TYPE clientAddrLen = sizeof(clientAddr);
            
            SOCKET_TYPE clientSocket = accept(serverSocket, 
                                            reinterpret_cast<struct sockaddr*>(&clientAddr), 
                                            &clientAddrLen);
            
            if (clientSocket == INVALID_SOCKET) {
                if (running) {
                    std::cerr << "Accept failed. Error: " << SOCKET_ERROR_CODE << std::endl;
                }
                continue;
            }
            
            // Get client IP address
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cout << "New connection from " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
            
            // Handle client in a separate thread
            clientThreads.emplace_back(&TCPServer::handleClient, this, clientSocket);
            
            // Clean up finished threads
            clientThreads.erase(
                std::remove_if(clientThreads.begin(), clientThreads.end(),
                    [](std::thread& t) {
                        if (t.joinable()) {
                            t.join();
                            return true;
                        }
                        return false;
                    }),
                clientThreads.end()
            );
        }
    }
    
    void stop() {
        running = false;
        
        // Close server socket
        if (serverSocket != INVALID_SOCKET) {
            CLOSE_SOCKET(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
        
        // Wait for all client threads to finish
        for (auto& thread : clientThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        clientThreads.clear();
        
        std::cout << "Server stopped." << std::endl;
    }
    
private:
    void handleClient(SOCKET_TYPE clientSocket) {
        char buffer[1024];
        
        while (running) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            
            if (bytesReceived <= 0) {
                break; // Client disconnected or error
            }
            
            // Null-terminate the received data
            buffer[bytesReceived] = '\0';
            
            // Echo the data back to the client
            int bytesSent = send(clientSocket, buffer, bytesReceived, 0);
            if (bytesSent < 0) {
                std::cerr << "Failed to send data to client. Error: " << SOCKET_ERROR_CODE << std::endl;
                break;
            }
            
            std::cout << "Echoed " << bytesReceived << " bytes to client" << std::endl;
        }
        
        CLOSE_SOCKET(clientSocket);
        std::cout << "Client disconnected" << std::endl;
    }
};

std::atomic<bool> shouldStop(false);

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down..." << std::endl;
    shouldStop = true;
}

int main() {
    std::cout << "TCP Echo Server starting..." << std::endl;
    
    // Set up signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        TCPServer server(8080);
        
        if (!server.start()) {
            std::cerr << "Failed to start server" << std::endl;
            return 1;
        }
        
        // Run server in a separate thread so we can handle signals
        std::thread serverThread([&server]() {
            server.run();
        });
        
        // Wait for stop signal
        while (!shouldStop) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Stop server
        server.stop();
        
        if (serverThread.joinable()) {
            serverThread.join();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Server shutdown complete." << std::endl;
    return 0;
}
