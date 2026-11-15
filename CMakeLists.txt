cmake_minimum_required(VERSION 3.16)
project(blackhole LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# lokalne naglowki Xinerama (jak nie ma libxinerama-dev w systemie)
set(X11_EXTRA_INC "${CMAKE_SOURCE_DIR}/external/x11_extra")
set(X11_Xinerama_INCLUDE_PATH "${X11_EXTRA_INC}" CACHE PATH "Xinerama include" FORCE)

# GLFW - jak nie ma w systemie, sciagamy
find_package(glfw3 3.3 QUIET)
if(NOT glfw3_FOUND)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.10
  )
  FetchContent_MakeAvailable(glfw)
endif()

# glm - naglowki
find_package(glm QUIET)
if(NOT glm_FOUND)
  FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
  )
  FetchContent_MakeAvailable(glm)
endif()

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_executable(blackhole
  src/main.cpp
  src/shader.cpp
  src/camera.cpp
)

target_include_directories(blackhole PRIVATE src)
target_link_libraries(blackhole PRIVATE glad glfw glm::glm)

# na Linuxie OpenGL z systemu
if(UNIX AND NOT APPLE)
  target_link_libraries(blackhole PRIVATE dl)
endif()

# sciezka do shaderow przy odpaleniu z katalogu build/
target_compile_definitions(blackhole PRIVATE
  SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders"
)
