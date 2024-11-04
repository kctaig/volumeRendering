cmake_minimum_required(VERSION 3.10)

project(
    volumeRendering
    CXX
)
# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Make the C++ standard required
set(CMAKE_CXX_EXTENSIONS OFF)         # Disable compiler-specific extensions


file(GLOB_RECURSE NLOHMANN_FILES "3dparty/nlohmann/*.hpp")

# 设置当前源目录
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(source)

