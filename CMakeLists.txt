cmake_minimum_required(VERSION 3.10)
project(WebServerCPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Find threading library
find_package(Threads REQUIRED)

add_executable(web_server
    src/main.cpp
)

# Link against threading library
target_link_libraries(web_server Threads::Threads)

# Platform-specific settings
if(WIN32)
    target_link_libraries(web_server ws2_32)
endif()

# Test client executable
add_executable(test_client
    src/test_client.cpp
)

target_link_libraries(test_client Threads::Threads)

if(WIN32)
    target_link_libraries(test_client ws2_32)
endif()
