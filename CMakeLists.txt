cmake_minimum_required(VERSION 3.12)
project(effc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list (APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
find_package(SFML 3 REQUIRED COMPONENTS Graphics Network)

add_executable(${PROJECT_NAME} src/cpp/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Network)