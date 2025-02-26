cmake_minimum_required(VERSION 3.5.0)
project(ProutEngine VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenGL REQUIRED)


file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("${CMAKE_PROJECT_NAME}")

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC include)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw glad stb_image)

add_subdirectory(lib/glfw-3.4)
add_subdirectory(lib/glad)
add_subdirectory(lib/stb_image)
add_subdirectory(lib/glm)

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 23)