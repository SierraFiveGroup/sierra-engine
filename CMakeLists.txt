cmake_minimum_required(VERSION 3.20)
project(engine LANGUAGES C CXX)

# ----------------------------------------
# Global build settings
# ----------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug or Release" FORCE)
endif()

include(FetchContent)

# ----------------------------------------
# Platform detection
# ----------------------------------------
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    set(PLATFORM_MAC TRUE)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
endif()

# ============================================================
#                 Collect Engine Sources
# ============================================================
file(GLOB_RECURSE SOURCES src/*.cpp)

# Create executable
add_executable(engine ${SOURCES})

# ============================================================
#                      GLFW (Hybrid)
# ============================================================
set(BUILD_GLFW_FROM_SOURCE FALSE)

if(PLATFORM_LINUX)
    find_package(PkgConfig QUIET)

    if(PkgConfig_FOUND)
        pkg_check_modules(GLFW3 QUIET glfw3)

        if(GLFW3_FOUND)
            message(STATUS "Using system GLFW from pkg-config")

            # pkg-config variables:
            #   GLFW3_INCLUDE_DIRS
            #   GLFW3_LIBRARY_DIRS
            #   GLFW3_LIBRARIES

            # Add include dirs and libs manually
            target_include_directories(engine BEFORE PRIVATE ${GLFW3_INCLUDE_DIRS})
            link_directories(${GLFW3_LIBRARY_DIRS})

            set(GLFW_TARGET_NAME ${GLFW3_LIBRARIES})   # typically "glfw" or "glfw3")
        else()
            message(STATUS "System GLFW not found → Fetching GLFW from source.")
            set(BUILD_GLFW_FROM_SOURCE TRUE)
        endif()
    else()
        message(STATUS "pkg-config not found → Fetching GLFW from source.")
        set(BUILD_GLFW_FROM_SOURCE TRUE)
    endif()

else()
    # Windows/macOS always use source build
    set(BUILD_GLFW_FROM_SOURCE TRUE)
endif()

if(BUILD_GLFW_FROM_SOURCE)
    message(STATUS "Building GLFW from source...")

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        master
    )

    # Disable OpenGL to avoid glad conflict
    set(GLFW_USE_OPENGL OFF CACHE BOOL "" FORCE)
    set(GLFW_USE_EGL OFF CACHE BOOL "" FORCE)
    set(GLFW_USE_OSMESA OFF CACHE BOOL "" FORCE)
    set(GLFW_CLIENT_LIBRARY None CACHE STRING "" FORCE)

    # Disable examples/tests/docs
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(glfw)

    set(GLFW_TARGET_NAME glfw)
endif()

# ============================================================
#                      Assimp (Hybrid)
# ============================================================
set(BUILD_ASSIMP_FROM_SOURCE FALSE)

if(PLATFORM_LINUX)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(ASSIMP QUIET assimp)
    endif()

    if(ASSIMP_FOUND)
        message(STATUS "Using system Assimp (pkg-config)")

        # pkg-config variables:
        #   ASSIMP_INCLUDE_DIRS
        #   ASSIMP_LIBRARY_DIRS
        #   ASSIMP_LIBRARIES

        # Inject headers for the engine target
        target_include_directories(engine BEFORE PRIVATE ${ASSIMP_INCLUDE_DIRS})

        # Add library search paths
        link_directories(${ASSIMP_LIBRARY_DIRS})

        # Use the raw library names (ex: "assimp")
        set(ASSIMP_TARGET ${ASSIMP_LIBRARIES})
    else()
        message(STATUS "System Assimp not found → Fetching Assimp from source.")
        set(BUILD_ASSIMP_FROM_SOURCE TRUE)
    endif()

else()
    # Windows & macOS always build from source
    set(BUILD_ASSIMP_FROM_SOURCE TRUE)
endif()

if(BUILD_ASSIMP_FROM_SOURCE)
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG        v5.3.1
    )
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(assimp)

    # FetchContent builds a target literally named "assimp"
    set(ASSIMP_TARGET assimp)
endif()

# ============================================================
#                    OpenAL Soft (Hybrid)
# ============================================================
set(BUILD_OPENAL_FROM_SOURCE FALSE)

if(PLATFORM_LINUX)
    find_package(OpenAL QUIET)
    if(OpenAL_FOUND)
        message(STATUS "Using system OpenAL")
        set(OPENAL_TARGET OpenAL::OpenAL)
    else()
        message(STATUS "OpenAL not found → Fetching from source.")
        set(BUILD_OPENAL_FROM_SOURCE TRUE)
    endif()
else()
    set(BUILD_OPENAL_FROM_SOURCE TRUE)
endif()

if(BUILD_OPENAL_FROM_SOURCE)
    FetchContent_Declare(
        openal
        GIT_REPOSITORY https://github.com/kcat/openal-soft.git
        GIT_TAG        master
    )
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(openal)
    set(OPENAL_TARGET OpenAL)
endif()

# ============================================================
#                        Freetype (Hybrid)
# ============================================================
set(USE_FREETYPE TRUE)    # Set to FALSE if you want to disable it globally

if(USE_FREETYPE)

    if(PLATFORM_LINUX)
        # --- Linux: prefer system Freetype ---
        find_package(Freetype QUIET)

        if(Freetype_FOUND)
            message(STATUS "Using system Freetype")
            set(FREETYPE_TARGET Freetype::Freetype)
        else()
            message(STATUS "System Freetype not found → Building from source")

            FetchContent_Declare(
                freetype
                GIT_REPOSITORY https://github.com/freetype/freetype.git
                GIT_TAG        VER-2-13-2
            )
            set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
            set(FT_DISABLE_PNG  ON CACHE BOOL "" FORCE)
            set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
            set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

            FetchContent_MakeAvailable(freetype)
            set(FREETYPE_TARGET freetype)
        endif()

    elseif(PLATFORM_WINDOWS)
        # --- Windows: DO NOT build freetype from source ---
        # Try Vulkan SDK's prebuilt Freetype first
        message(STATUS "Checking for Freetype in Vulkan SDK...")

        find_path(FREETYPE_INCLUDE_DIR freetype/config/ftheader.h
            HINTS "$ENV{VULKAN_SDK}/Include"
        )

        find_library(FREETYPE_LIBRARY freetype
            HINTS "$ENV{VULKAN_SDK}/Lib"
        )

        if(FREETYPE_INCLUDE_DIR AND FREETYPE_LIBRARY)
            message(STATUS "Using Freetype from Vulkan SDK")
            target_include_directories(engine PRIVATE ${FREETYPE_INCLUDE_DIR})
            set(FREETYPE_TARGET ${FREETYPE_LIBRARY})
        else()
            # No Freetype on Windows → Disable it
            message(WARNING "Freetype not found on Windows → Disabling Freetype")
            set(USE_FREETYPE FALSE)
        endif()

    else()
        # macOS or unknown → disable by default
        message(WARNING "Freetype unsupported on this platform → Disabling")
        set(USE_FREETYPE FALSE)
    endif()

endif()

# ============================================================
#                        Vulkan
# ============================================================
find_package(Vulkan REQUIRED)

# ============================================================
#                        GLAD (local)
# ============================================================
file(GLOB_RECURSE GLAD_SOURCES glad/*.c)
add_library(glad STATIC ${GLAD_SOURCES})
target_include_directories(glad PUBLIC glad)

# ============================================================
#                    Include Directories
# ============================================================
target_include_directories(engine PRIVATE
    src
    stb
    drlibs
    ${Vulkan_INCLUDE_DIRS}
)

# ============================================================
#                         Build Flags
# ============================================================
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(engine PRIVATE DEBUG)
    target_compile_options(engine PRIVATE -O0 -g)
else()
    target_compile_options(engine PRIVATE -O3)
endif()

if(WIN32)
    target_compile_definitions(engine PRIVATE ENGINE_ENABLE_STACKTRACE=0)
endif()

# ============================================================
#                     Linking Libraries
# ============================================================
target_link_libraries(engine PRIVATE
    ${GLFW_TARGET_NAME}
    ${ASSIMP_TARGET}
    ${OPENAL_TARGET}
    Vulkan::Vulkan
    glad
    stdc++exp
)

if(USE_FREETYPE)
    target_link_libraries(engine PRIVATE ${FREETYPE_TARGET})
endif()

# Extra dependencies for Windows/macOS
if(WIN32)
    target_link_libraries(engine PRIVATE ws2_32 winmm)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)

    target_link_libraries(engine PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using GLFW target: ${GLFW_TARGET_NAME}")
message(STATUS "Using Assimp target: ${ASSIMP_TARGET}")
message(STATUS "Using OpenAL target: ${OPENAL_TARGET}")
message(STATUS "Using Freetype target: ${FREETYPE_TARGET}")