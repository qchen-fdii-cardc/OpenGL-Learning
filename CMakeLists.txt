cmake_minimum_required(VERSION 3.10)
project(HelloWorldGLEW C)

set(CMAKE_C_STANDARD 99)

# Set OpenGL policy and preference
cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE GLVND)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Freetype REQUIRED)

# Add executables
add_executable(HelloWorldGLEW helloworld.c)
add_executable(HelloWorldCN helloworld_cn.c)

# Include directories for both executables
target_include_directories(HelloWorldGLEW PRIVATE 
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

target_include_directories(HelloWorldCN PRIVATE 
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

# Link libraries for both executables
target_link_libraries(HelloWorldGLEW
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw
    ${FREETYPE_LIBRARIES}
)

target_link_libraries(HelloWorldCN
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw
    ${FREETYPE_LIBRARIES}
) 