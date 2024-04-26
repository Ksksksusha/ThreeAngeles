cmake_minimum_required(VERSION 3.8)

project(Triangles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

aux_source_directory(./geometry/src GEOMETRY)

aux_source_directory(./vulkan/src VULKAN)

add_executable(triangles main.cpp ${GEOMETRY} ${VULKAN})

find_package(GTest REQUIRED)

enable_testing()

find_package(Vulkan REQUIRED)

find_package(glfw3 3.3 REQUIRED)

find_program(glslc NAMES glslc HINTS Vulkan::glslc REQUIRED)

function(add_spirv_shader TARGET_NAME INPUT_FILE)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.spv
        COMMAND ${glslc} ${INPUT_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.spv
        MAIN_DEPENDENCY ${INPUT_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_custom_target(${TARGET_NAME} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.spv)

    set(${TARGET_NAME}_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.spv PARENT_SCOPE)
endfunction()

add_spirv_shader(vert shaders/shader.vert)

add_spirv_shader(frag shaders/shader.frag)

add_custom_target(shaders ALL DEPENDS vert frag)

include_directories(${Vulkan_INCLUDE_DIRS})

target_link_libraries(triangles glfw ${Vulkan_LIBRARIES})

target_include_directories(triangles PRIVATE ${Vulkan_INCLUDE_DIRS})

target_include_directories(triangles PRIVATE ${PROJECT_SOURCE_DIR}/geometry/inc)

target_include_directories(triangles PRIVATE ${PROJECT_SOURCE_DIR}/vulkan/inc)