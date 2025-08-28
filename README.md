# HTTP Web Server

A multithreaded HTTP web server implemented in C++ that listens on localhost:8080 and serves HTTP responses with request parsing capabilities.

## Features

- **HTTP Request Parsing**: Parses HTTP request lines to extract method, path, and version
- **HTTP Response Generation**: Sends valid HTTP responses with proper headers and content
- **TCP Socket Handling**: Full TCP socket implementation with proper error handling
- **Multithreaded**: Handles multiple client connections concurrently using std::thread
- **Cross-Platform**: Works on Windows, macOS, and Linux
- **Signal Handling**: Graceful shutdown with Ctrl+C (SIGINT) and SIGTERM
- **Client Management**: Automatically cleans up disconnected client threads
- **Error Handling**: Returns proper HTTP error responses for invalid requests

## Acceptance Criteria Met

**Server runs on port 8080** - Configured to listen on localhost:8080  
**HTTP Request Parsing** - Successfully parses method, path, and version from request lines  
**HTTP Response Generation** - Sends valid HTTP responses with proper headers and content  
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

### Option 1: Using the HTTP Test Script

```bash
# Run the comprehensive HTTP test script
./test_http_response.sh
```

This script will:
1. Start the server
2. Test valid GET requests and verify response format
3. Test different paths
4. Test invalid requests (POST, malformed)
5. Verify proper HTTP responses
6. Stop the server and show logs

### Option 2: Using curl

```bash
# Test valid GET request
curl http://localhost:8080/

# Test different path
curl http://localhost:8080/index.html

# Test with verbose output to see headers
curl -v http://localhost:8080/test
```

### Option 3: Using netcat (nc)

```bash
# Test custom HTTP request
echo -e "GET /custom HTTP/1.1\r\nHost: localhost:8080\r\n\r\n" | nc localhost 8080

# Test invalid request
echo -e "POST /test HTTP/1.1\r\n\r\n" | nc localhost 8080
```

## Server Behavior

- **HTTP Request Parsing**: Parses incoming HTTP requests to extract method, path, and version
- **HTTP Response Generation**: Generates appropriate HTTP responses with proper headers and content
- **Connection Handling**: Accepts new connections and spawns a thread for each client
- **Concurrent Clients**: Can handle multiple clients simultaneously
- **Graceful Shutdown**: Responds to SIGINT (Ctrl+C) and SIGTERM signals
- **Resource Cleanup**: Automatically closes client connections and cleans up threads

## HTTP Response Format

For valid GET requests, the server responds with:

```
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 12

Hello World!
```

For invalid requests, the server responds with:

```
HTTP/1.1 400 Bad Request
Content-Type: text/plain
Content-Length: 25

400 Bad Request
```

## Architecture

```
TCPServer Class
├── start() - Initialize socket, bind, and listen
├── run() - Main server loop accepting connections
├── handleClient() - Handle individual client communication
└── stop() - Graceful shutdown and cleanup

HTTPRequest Class
├── parse() - Parse HTTP request line into method, path, and version
├── generateResponse() - Generate appropriate HTTP response
└── Validation methods for request components

Client Handling
├── Each client gets its own thread
├── HTTP request parsing and response generation
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
- Additional HTTP methods (POST, PUT, DELETE)
- HTTP header parsing and handling
- Static file serving
- Keep-alive connections
- Connection pooling
- Performance benchmarking
- Configuration file support
- Route handling and middleware support

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
