cmake_minimum_required(VERSION 3.12)
project(cutlass)

find_package(glfw3 3.2 REQUIRED)
find_library(COCOA_LIBRARY Cocoa)
find_library(OPENGL_LIBRARY OpenGL)
find_library(IOKIT_LIBRARY IOKit)
find_library(COREVIDEO_LIBRARY CoreVideo)

set(CMAKE_CXX_STANDARD 17)
set(SRC_FILES src/main.cpp src/glad.c src/shader.cpp src/stb_image.cpp src/cutlass.cpp)

add_executable(cutlass ${SRC_FILES})
add_subdirectory(include/glm)

target_include_directories(cutlass PRIVATE include)
target_link_libraries(cutlass glfw ${COCOA_LIBRARY} ${OPENGL_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})

file(COPY src/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY src/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})