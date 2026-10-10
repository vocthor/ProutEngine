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
target_link_libraries(imgui PUBLIC glfw)


##### SOURCES #####
option(PROUT_BUILD_EDITOR "Build the editor executable (ImGui panels, viewport)" ON)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/editor/*.cpp")
list(REMOVE_ITEM CORE_SOURCES ${EDITOR_SOURCES})

set(PROUT_WARNINGS
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

# Engine core + game layer: shared by the game and the editor
add_library(prout_core STATIC ${CORE_SOURCES})
target_include_directories(prout_core PUBLIC include lib/imgui)
target_link_libraries(prout_core PUBLIC glm glfw glad stb_image assimp imgui)
target_compile_options(prout_core PRIVATE ${PROUT_WARNINGS})
set_property(TARGET prout_core PROPERTY CXX_STANDARD 23)

# Game: no editor code
add_executable(ProutGame app/game/main.cpp)
target_link_libraries(ProutGame PRIVATE prout_core)
target_compile_options(ProutGame PRIVATE ${PROUT_WARNINGS})
set_property(TARGET ProutGame PROPERTY CXX_STANDARD 23)

# Editor: game + EditorLayer
if(PROUT_BUILD_EDITOR)
    add_library(prout_editor STATIC ${EDITOR_SOURCES})
    target_link_libraries(prout_editor PUBLIC prout_core)
    target_compile_options(prout_editor PRIVATE ${PROUT_WARNINGS})
    set_property(TARGET prout_editor PROPERTY CXX_STANDARD 23)

    add_executable(ProutEditor app/editor/main.cpp)
    target_link_libraries(ProutEditor PRIVATE prout_editor)
    target_compile_options(ProutEditor PRIVATE ${PROUT_WARNINGS})
    set_property(TARGET ProutEditor PROPERTY CXX_STANDARD 23)
endif()
