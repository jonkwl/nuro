cmake_minimum_required(VERSION 3.10)

project(nuro CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(assimp CONFIG REQUIRED) 
find_package(EnTT CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_subdirectory(nuro-core)
add_subdirectory(nuro-editor)