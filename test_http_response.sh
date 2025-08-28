#!/bin/bash

echo "Testing HTTP Response Functionality"
echo "=================================="
echo

# Start the server in the background
echo "Starting HTTP server..."
./build/web_server > server.log 2>&1 &
SERVER_PID=$!

# Wait for server to start
sleep 2

echo "Server started with PID: $SERVER_PID"
echo

# Test 1: Verify exact response format from acceptance criteria
echo "Test 1: Verifying exact response format from acceptance criteria"
echo "Expected: HTTP/1.1 200 OK, Content-Type: text/plain, Content-Length: 12, Hello World!"
echo

response=$(curl -s -D - http://localhost:8080/ 2>/dev/null)
echo "Actual response:"
echo "$response"
echo

# Check if response matches acceptance criteria
if echo "$response" | grep -q "HTTP/1.1 200 OK" && \
   echo "$response" | grep -q "Content-Type: text/plain" && \
   echo "$response" | grep -q "Content-Length: 12" && \
   echo "$response" | grep -q "Hello World!"; then
    echo "✅ PASS: Response matches acceptance criteria exactly!"
else
    echo "❌ FAIL: Response does not match acceptance criteria"
fi
echo

# Test 2: Test different paths
echo "Test 2: Testing different paths"
echo "Testing /index.html..."
curl -s http://localhost:8080/index.html | grep "Hello World!"
echo "Testing /api/users..."
curl -s http://localhost:8080/api/users | grep "Hello World!"
echo

# Test 3: Test invalid request (should return 400)
echo "Test 3: Testing invalid POST request (should return 400)"
echo -e "POST /test HTTP/1.1\r\n\r\n" | nc -w 1 localhost 8080
echo

# Test 4: Test in browser simulation
echo "Test 4: Testing browser-like request"
curl -v http://localhost:8080/ 2>&1 | grep -E "(HTTP|Content-Type|Content-Length|Hello)"
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
