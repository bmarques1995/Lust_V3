cmake_minimum_required(VERSION 3.22)

set(EXAMPLE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Internal variable")

if(WIN32)
	add_d3d12_version_file(${CMAKE_CURRENT_SOURCE_DIR}/D3D12ContextExport.cpp)
endif()

add_subdirectory("Layers")
add_subdirectory("FlappyClone")