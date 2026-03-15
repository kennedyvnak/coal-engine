cmake_minimum_required(VERSION 3.24.3)
project(coal_engine
        VERSION 1.0.0
        DESCRIPTION "A raw c++ game engine"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SUPPRESS_REGENERATION true)

option(BUILD_EDITOR "Build editor" ON)

#========================== ENGINE ==========================#
file(GLOB_RECURSE SOURCE_FILES coal_engine/*.cpp coal_engine/*.h)
file(GLOB_RECURSE RESOURCE_FILES res/**)

if(BUILD_EDITOR)
    message("Building Engine Editor")
    file(GLOB_RECURSE EDITOR_SOURCE_FILES coal_engine_editor/*.cpp coal_engine_editor/*.h)
    add_executable(${PROJECT_NAME} coal_engine_editor/engine_editor.cpp ${SOURCE_FILES} ${RESOURCE_FILES} ${EDITOR_SOURCE_FILES})
else()
    message("Building Engine Player")
    file(GLOB_RECURSE PLAYER_SOURCE_FILES coal_engine_player/*.cpp coal_engine_player/*.h)
    add_executable(${PROJECT_NAME} coal_engine_player/engine_player.cpp ${SOURCE_FILES} ${RESOURCE_FILES} ${PLAYER_SOURCE_FILES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "CoalEngine")

target_precompile_headers(${PROJECT_NAME} PRIVATE coal_engine/enpch.h)

#========================== INCLUDES ==========================#
include_directories(coal_engine)
include_directories(coal_engine_editor)
include_directories(coal_engine_player)
include_directories(vendor)
include_directories(vendor/glew/include)
include_directories(vendor/glfw/include)
include_directories(vendor/glm)
include_directories(vendor/imgui)
include_directories(vendor/stb_image)

#========================== STB IMAGE ==========================#
add_library(stb_image STATIC vendor/stb_image/stb_image.cpp vendor/stb_image/stb_image.h)
target_link_libraries(${PROJECT_NAME} stb_image)

#========================== GLEW ==========================#
add_subdirectory(vendor/glew/build/cmake)
target_link_libraries(${PROJECT_NAME} glew_s)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework CoreVideo")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} X11)
    target_link_libraries(${PROJECT_NAME} GL)
    target_link_libraries(${PROJECT_NAME} dl)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

#========================== GLFW ==========================#
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

#========================== IMGUI ==========================#
set(IMGUI_PATH vendor/imgui)
file(GLOB IMGUI_FILES
    ${IMGUI_PATH}/*.cpp ${IMGUI_PATH}/*.h
    ${IMGUI_PATH}/backends/imgui_impl_glfw.h
    ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
    ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_FILES})
target_link_libraries(${PROJECT_NAME} imgui)

#========================== DEFINITIONS ==========================#
add_definitions(-DGLFW_INCLUDE_NONE -DGLEW_STATIC)

if(BUILD_EDITOR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_EDITOR=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENGINE_PLAYER=1)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE "ENGINE_DEBUG=$<CONFIG:Debug>")
target_compile_definitions(${PROJECT_NAME} PRIVATE "ENGINE_RELEASE=$<CONFIG:Release>")

#========================== COMMANDS ==========================#
add_custom_command(
    TARGET  ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/res/
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/res/)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/config/)
