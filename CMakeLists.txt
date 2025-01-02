# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# set C compiler binary location
set(CMAKE_C_COMPILER "/usr/bin/gcc")
# Set the project name
project(CeeLightning-Server)

# Define the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/lib
)

# Add source files
set(SOURCES
    config.c
    ceeServer.c
    utils.c
    lib/router.c
    lib/routes.c
    lib/file_helper.c
    lib/http_helper.c
    lib/json_helper.c
    lib/db_helper.c
)

# Specify the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Add the executable target
add_executable(server.out ${SOURCES})
