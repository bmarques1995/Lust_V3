cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME LustD3D12SampleFix)

file(GLOB_RECURSE LAUNCHER_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.hpp")
file(GLOB_RECURSE LAUNCHER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

set(SOURCE_LIST ${LAUNCHER_SRCS})
if(WIN32)
	set(SOURCE_LIST ${SOURCE_LIST} ${EXAMPLE_ROOT_DIR}/D3D12ContextExport.cpp)
endif()

add_executable(${TARGET_NAME} ${LAUNCHER_HDRS} ${SOURCE_LIST})
target_link_libraries(${TARGET_NAME} PUBLIC LustSandbox d3d12.lib dxgi.lib dxguid.lib)
if(BUILD_SHARED_LIBS AND WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_SHARED_LIBS)
endif()

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
