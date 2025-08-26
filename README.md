# TCP Echo Server

A multithreaded TCP echo server implemented in C++ that listens on localhost:8080 and echoes back any client input.

## Features

- **TCP Socket Handling**: Full TCP socket implementation with proper error handling
- **Multithreaded**: Handles multiple client connections concurrently using std::thread
- **Cross-Platform**: Works on Windows, macOS, and Linux
- **Signal Handling**: Graceful shutdown with Ctrl+C (SIGINT) and SIGTERM
- **Client Management**: Automatically cleans up disconnected client threads

## Acceptance Criteria Met

**Server runs on port 8080** - Configured to listen on localhost:8080  
**Client receives back exactly what it sends** - Full echo functionality implemented  
**Server can handle at least 1 client at a time** - Multithreaded design supports multiple concurrent clients  

## Building the Project

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Make or Ninja build system

### Build Commands

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make
# or on Windows with Visual Studio:
# cmake --build . --config Release
```

This will create two executables:
- `web_server` - The main TCP echo server
- `test_client` - A test client for verifying server functionality

## Running the Server

```bash
# Start the server
./web_server
```

The server will start and display:
```
TCP Echo Server starting...
TCP Echo Server started on port 8080
Waiting for connections...
```

## Testing the Server

### Option 1: Using the Test Client

```bash
# In another terminal, run the test client
./test_client
```

The test client will:
1. Connect to the server
2. Run automated tests with various message types
3. Enter interactive mode for manual testing

### Option 2: Using Telnet

```bash
# Connect using telnet
telnet localhost 8080

# Type any message and press Enter
# The server will echo it back exactly
# Type 'quit' or Ctrl+] then 'quit' to exit
```

### Option 3: Using netcat (nc)

```bash
# Connect using netcat
nc localhost 8080

# Type messages and see them echoed back
# Press Ctrl+C to disconnect
```

## Server Behavior

- **Connection Handling**: Accepts new connections and spawns a thread for each client
- **Echo Functionality**: Receives data from clients and sends it back exactly as received
- **Concurrent Clients**: Can handle multiple clients simultaneously
- **Graceful Shutdown**: Responds to SIGINT (Ctrl+C) and SIGTERM signals
- **Resource Cleanup**: Automatically closes client connections and cleans up threads

## Architecture

```
TCPServer Class
├── start() - Initialize socket, bind, and listen
├── run() - Main server loop accepting connections
├── handleClient() - Handle individual client communication
└── stop() - Graceful shutdown and cleanup

Client Handling
├── Each client gets its own thread
├── Automatic thread cleanup on disconnect
└── Non-blocking main server loop
```

## Error Handling

- Socket creation failures
- Bind/listen errors
- Client connection issues
- Send/receive failures
- Graceful error recovery and logging

## Performance Considerations

- Uses std::thread for client handling
- Efficient socket operations
- Minimal memory allocation during operation
- Proper resource cleanup

## Future Enhancements

This implementation provides a solid foundation for:
- HTTP protocol support
- Static file serving
- Keep-alive connections
- Connection pooling
- Performance benchmarking
- Configuration file support

## Troubleshooting

### Port Already in Use
If you get a "Bind failed" error, the port 8080 might be in use:
```bash
# Check what's using port 8080
lsof -i :8080  # macOS/Linux
netstat -an | findstr :8080  # Windows
```

### Permission Denied
On some systems, binding to ports below 1024 requires root privileges. Port 8080 should work without elevated permissions.

### Build Issues
Ensure you have a C++17 compatible compiler:
```bash
g++ --version  # Should show version 7 or higher
clang++ --version  # Should show version 5 or higher
```

## License

This project is part of a learning exercise for multithreaded network programming in C++.
