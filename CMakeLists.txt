﻿cmake_minimum_required(VERSION 3.5)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(sandcraft VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/camera.cpp
    src/camera.hpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)
add_subdirectory(deps/spdlog)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glad
    glfw
    spdlog
)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
)

add_dependencies(${PROJECT_NAME} copy_assets)