# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project
project(server VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(${PROJECT_NAME}
    driver.cpp
    request.cpp
    server_helper.cpp
    server_threadpool.cpp
    atomic_writer.cpp
)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Display project information
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Source Directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Project Binary Directory: ${PROJECT_BINARY_DIR}")
