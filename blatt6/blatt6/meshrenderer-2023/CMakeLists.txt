cmake_minimum_required(VERSION 3.10)
project(meshrenderer)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
    src
    /opt/homebrew/include
    /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenGL.framework/Headers
)

link_directories(/opt/homebrew/lib)

add_executable(meshrenderer
    src/main.cpp
    src/context.cpp
    src/mesh.cpp
    src/shader.cpp
)

target_link_libraries(meshrenderer
    "-framework OpenGL"
    glfw
    GLEW
)
add_definitions(-DGL_SILENCE_DEPRECATION)
target_compile_definitions(meshrenderer PRIVATE
    GL_GLEXT_PROTOTYPES
)
target_compile_definitions(meshrenderer PRIVATE
    GL_SILENCE_DEPRECATION
)