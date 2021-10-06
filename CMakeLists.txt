cmake_minimum_required(VERSION 3.0.0)

project(Mandelbrot)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(GLEW_PATH ${CMAKE_SOURCE_DIR}/dependencies/glew-2.1.0)

add_compile_definitions(GLEW_STATIC)

add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/glfw)
find_package(OpenGL REQUIRED)

include_directories(${GLEW_PATH}/include dependencies/glm)

add_executable(${PROJECT_NAME} main/Application.cpp)

target_link_libraries(${PROJECT_NAME} ${GLEW_PATH}/lib/Release/x64/glew32s.lib)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)