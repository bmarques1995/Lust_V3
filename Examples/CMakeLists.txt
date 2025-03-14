cmake_minimum_required(VERSION 3.22)

set(EXAMPLE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Internal variable")

if(WIN32)
	file(READ "${PROJECT_SOURCE_DIR}/d3d12sdk_version.txt" SDK_VERSION)
	string(REGEX REPLACE "[^0-9]" "" SANITIZED_SDK_VERSION "${SDK_VERSION}")
	file(READ ${CMAKE_CURRENT_SOURCE_DIR}/D3D12ContextExport.cxx.in SOURCE_CONTENTS)
	string(REGEX REPLACE SDK_VERSION "${SANITIZED_SDK_VERSION}" OUTPUT_SOURCE ${SOURCE_CONTENTS})
	file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/D3D12ContextExport.cpp ${OUTPUT_SOURCE})
endif()

add_subdirectory("Layers")
add_subdirectory("FlappyClone")