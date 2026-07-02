cmake_minimum_required(VERSION 3.31.6)
project(ProutEngine VERSION 0.1.0 LANGUAGES C CXX)


##### LIBS #####
find_package(OpenGL REQUIRED)
add_subdirectory(lib/glfw-3.4)
add_subdirectory(lib/glad)
add_subdirectory(lib/stb_image)
add_subdirectory(lib/glm)
add_subdirectory(lib/assimp)


##### ImGui #####
add_library(imgui
    lib/imgui/imgui.cpp
    # lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
)
target_include_directories(imgui PUBLIC
    lib/imgui
    lib/imgui/backends
)


##### SOURCES #####
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("${CMAKE_PROJECT_NAME}")

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES})
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC include lib/imgui)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw glad stb_image assimp imgui)
target_compile_options("${CMAKE_PROJECT_NAME}" PRIVATE
    -Wall
    -Wextra
    -pedantic
    # -Werror
    -Wconversion-null
    -Wmissing-declarations
    -Woverlength-strings
    -Wpointer-arith
    -Wunused-local-typedefs
    -Wunused-result
    -Wvarargs
    -Wvla
    -Wwrite-strings
)

set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 23)
