cmake_minimum_required(VERSION 3.12)
project(engine)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug/Release)" FORCE)
endif()

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect source files
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE SOURCES ${SOURCES} glad/glad.c)


find_package(OpenAL REQUIRED)

# Add executable
add_executable(engine ${SOURCES})

#add_library(engine SHARED ${SOURCES})
#set_target_properties(engine PROPERTIES OUTPUT_NAME "SierraEngine")

# Add compile options based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# -g flag for debugging symbols
target_compile_options(engine PRIVATE -g -DDEBUG -Wno-int-to-pointer-cast -Wno-cpp -O0 -DGLFW_INCLUDE_NONE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
# Optimization flag for release builds. You can change -O3 to other optimization levels if desired.
target_compile_options(engine PRIVATE -O3 -DGLFW_INCLUDE_NONE)
endif()


# Include directories (assumes headers are in PATH-visible locations)
include_directories(/usr/include) # Just in case :)
include_directories($ENV{VULKAN_SDK}/Include)
include_directories($ENV{GLFW_INCLUDE_DIR})  # Optional, if GLFW headers are not in a standard include path
include_directories(src)
include_directories(glad)
include_directories(stb/stb_image)
include_directories(stb/stb_truetype)
include_directories(stb)
include_directories(drlibs)
include_directories(/usr/include/freetype2)

# Link libraries (assuming they are in the system linker path)
target_link_libraries(engine PRIVATE glfw OpenAL::OpenAL freetype vulkan stdc++exp)

# Output some informational messages
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
