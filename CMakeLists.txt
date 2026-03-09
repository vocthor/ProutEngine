cmake_minimum_required(VERSION 3.31.6)
project(ProutEngine VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenGL REQUIRED)

# Ajouter les librairies externes d'abord
add_subdirectory(lib/glfw-3.4)
add_subdirectory(lib/glad)
add_subdirectory(lib/stb_image)
add_subdirectory(lib/glm)
add_subdirectory(lib/assimp)

# Configuration de la cible du projet
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("${CMAKE_PROJECT_NAME}")

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC include)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw glad stb_image assimp)

# Compiler options appliquées SEULEMENT au projet (PRIVATE)
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