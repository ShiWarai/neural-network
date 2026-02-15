cmake_minimum_required(VERSION 3.10)
project(NeuralNetwork LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(neural_network src/main.cpp)
target_include_directories(neural_network PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# std::filesystem требует линковки с stdc++fs в GCC 8
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
	target_link_libraries(neural_network PRIVATE stdc++fs)
endif()
