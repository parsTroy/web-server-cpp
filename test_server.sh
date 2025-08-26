#!/bin/bash

echo "=== TCP Echo Server Test Script ==="
echo "Building project..."
cd build
make

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Starting server in background..."
./web_server &
SERVER_PID=$!

echo "Server started with PID: $SERVER_PID"
echo "Waiting for server to start..."
sleep 3

echo "Testing server with test client..."
./test_client

echo "Stopping server..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo "Test complete!"
