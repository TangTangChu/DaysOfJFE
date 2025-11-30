cmake_minimum_required(VERSION 3.20)

project(DaysOfJFE LANGUAGES C CXX)

# ---------- C++ standard ----------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------- Options ----------
option(USE_YAML "Enable yaml-cpp" ON)

# Assets root (used by code)
set(ASSETS_ROOT "${CMAKE_SOURCE_DIR}/assets" CACHE PATH "Path to assets folder")

# ---------- Sources ----------
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
)

add_executable(DaysOfJFE ${SRC_FILES})

# ---------- Include dirs ----------
# Always include src, and also all first-level subfolders under src/
target_include_directories(DaysOfJFE PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

file(GLOB SRC_CHILDREN RELATIVE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/*")
foreach(child ${SRC_CHILDREN})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/src/${child}")
        target_include_directories(DaysOfJFE PRIVATE "${CMAKE_SOURCE_DIR}/src/${child}")
    endif()
endforeach()

# ---------- Compile definitions ----------
target_compile_definitions(DaysOfJFE PRIVATE
    ASSETS_ROOT="${ASSETS_ROOT}"
)

if (WIN32)
    target_compile_definitions(DaysOfJFE PRIVATE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif()

# ---------- Dependencies via vcpkg ----------
# GLFW
find_package(glfw3 CONFIG REQUIRED)
if (TARGET glfw)
    set(GLFW_TARGET glfw)
elseif(TARGET glfw3::glfw3)
    set(GLFW_TARGET glfw3::glfw3)
else()
    message(FATAL_ERROR "glfw target not found. Check your vcpkg install of glfw3.")
endif()

# Skia (vcpkg unofficial port)
find_package(unofficial-skia CONFIG REQUIRED)
if (NOT TARGET unofficial::skia::skia)
    message(FATAL_ERROR "unofficial::skia::skia target not found. Check vcpkg unofficial-skia.")
endif()

# yaml-cpp
if (USE_YAML)
    find_package(yaml-cpp CONFIG REQUIRED)
endif()

# OpenGL + Threads (cross-platform)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
# ---------- Link libraries ----------
target_link_libraries(DaysOfJFE PRIVATE
    ${GLFW_TARGET}
    unofficial::skia::skia
    OpenGL::GL
    Threads::Threads
)
target_include_directories(DaysOfJFE PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/dr_libs
)
target_link_libraries(DaysOfJFE PRIVATE
    OpenAL::OpenAL
)
if (USE_YAML)
    target_link_libraries(DaysOfJFE PRIVATE yaml-cpp::yaml-cpp)
    target_compile_definitions(DaysOfJFE PRIVATE USE_YAML=1)
endif()
find_package(glad CONFIG REQUIRED)

target_link_libraries(DaysOfJFE PRIVATE
    glad::glad
)
target_compile_definitions(DaysOfJFE PRIVATE SK_GL=1 SK_GANESH=1)

# Extra system libs on Linux
if (UNIX AND NOT APPLE)
    target_link_libraries(DaysOfJFE PRIVATE dl)
endif()

# ---------- Warnings / flags ----------
if (MSVC)
    target_compile_options(DaysOfJFE PRIVATE
        /W4 /permissive- /utf-8
    )
else()
    target_compile_options(DaysOfJFE PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

# ---------- Friendly status ----------
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "CMAKE_TOOLCHAIN_FILE not set. If you use vcpkg, pass it when configuring.")
endif()
message(STATUS "ASSETS_ROOT = ${ASSETS_ROOT}")
message(STATUS "USE_YAML    = ${USE_YAML}")
