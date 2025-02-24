cmake_minimum_required(VERSION 3.10)

project(nuro CXX)

set(CMAKE_CXX_STANDARD 17)

#
# engine packages
#

find_package(assimp CONFIG REQUIRED) 
find_package(EnTT CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

#
# editor packages
#

# find_package(imgui CONFIG REQUIRED)      # imported from local directory
# find_package(implot CONFIG REQUIRED)     # imported from local directory
# find_package(imguizmo CONFIG REQUIRED)   # imported from local directory

add_subdirectory(engine-core)
add_subdirectory(engine-editor)