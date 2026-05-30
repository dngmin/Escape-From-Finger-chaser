cmake_minimum_required(VERSION 3.12)
project(effc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list (APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
find_package(SFML 3 REQUIRED COMPONENTS Graphics Network)

add_executable(${PROJECT_NAME}
    src/cpp/main.cpp

    src/cpp/Player.cpp
    src/cpp/Chaser.cpp
    src/cpp/PlayerState.cpp
    src/cpp/ChaserState.cpp
    src/cpp/ConfigLoader.cpp
    src/cpp/SocketHandler.cpp
    src/cpp/Entity.cpp
    src/cpp/Utils.cpp

    include/Player.hpp
    include/Chaser.hpp
    include/PlayerState.hpp
    include/ChaserState.hpp
    include/ConfigLoader.hpp
    include/SocketHandler.hpp
    include/Entity.hpp
    include/Utils.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Network)