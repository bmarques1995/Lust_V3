cmake_minimum_required(VERSION 3.22)

include(dep_tracer)
include(target_destination)
set(TARGET_NAME LustLauncher)

file(GLOB_RECURSE SANDBOX_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE SANDBOX_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_executable(${TARGET_NAME} ${SANDBOX_HDRS} ${SANDBOX_SRCS})
target_link_libraries(${TARGET_NAME} PUBLIC Lust)

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)