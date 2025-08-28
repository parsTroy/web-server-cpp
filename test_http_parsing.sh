#!/bin/bash

echo "Testing HTTP Request Parsing"
echo "============================"
echo

# Start the server in the background
echo "Starting HTTP server..."
./build/web_server > server.log 2>&1 &
SERVER_PID=$!

# Wait for server to start
sleep 2

echo "Server started with PID: $SERVER_PID"
echo

# Test 1: Valid GET request
echo "Test 1: Valid GET request to /index.html"
curl -s http://localhost:8080/index.html | grep -E "(Method|Path|Version)"
echo

# Test 2: Valid GET request to different path
echo "Test 2: Valid GET request to /api/users"
curl -s http://localhost:8080/api/users | grep -E "(Method|Path|Version)"
echo

# Test 3: Invalid POST request (should return 400)
echo "Test 3: Invalid POST request (should return 400)"
echo -e "POST /test HTTP/1.1\r\nHost: localhost:8080\r\n\r\n" | nc -w 1 localhost 8080
echo

# Test 4: Malformed request (should return 400)
echo "Test 4: Malformed request (should return 400)"
echo -e "INVALID REQUEST\r\n\r\n" | nc -w 1 localhost 8080
echo

# Test 5: Empty request (should return 400)
echo "Test 5: Empty request (should return 400)"
echo "" | nc -w 1 localhost 8080
echo

echo "All tests completed!"
echo

# Stop the server
echo "Stopping server..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo "Server stopped."
echo
echo "Server log:"
cat server.log
rm server.log
