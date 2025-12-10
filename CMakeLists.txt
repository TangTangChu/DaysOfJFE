cmake_minimum_required(VERSION 3.20)
project(DaysOfJFE)

set(CMAKE_CXX_STANDARD 17)

# 设置库路径
set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(SKIA_DIR "${LIBS_DIR}/skia")

# 查找系统库
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_check_modules(YAMLCPP REQUIRED yaml-cpp)
find_package(OpenGL REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(Freetype REQUIRED)

# 创建资源软链接
add_custom_target(create_asset_symlink ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink 
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
)

# 收集源代码
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(DaysOfJFE ${SOURCES})
add_dependencies(DaysOfJFE create_asset_symlink)

# 配置头文件路径
target_include_directories(DaysOfJFE PRIVATE
    include
    ${SDL2_INCLUDE_DIRS}
    ${YAMLCPP_INCLUDE_DIRS}
    ${SKIA_DIR}
    ${SKIA_DIR}/include
    ${SKIA_DIR}/include/core
    ${SKIA_DIR}/include/gpu
    ${SKIA_DIR}/include/gpu/ganesh
    ${SKIA_DIR}/include/gpu/ganesh/gl
    ${SKIA_DIR}/include/utils
    ${SKIA_DIR}/include/effects
    ${SKIA_DIR}/include/codec
    ${SKIA_DIR}/include/config
    ${SKIA_DIR}/modules/skcms
)

# 链接库
target_link_libraries(DaysOfJFE PRIVATE
    ${SKIA_DIR}/lib/libskia.a
    ${SDL2_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${YAMLCPP_LIBRARIES}
    OpenGL::GL
    Fontconfig::Fontconfig
    Freetype::Freetype
    pthread
    dl
    z
)
