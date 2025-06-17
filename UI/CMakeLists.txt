set(TARGET_NAME LustUI)

set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
endif()

file(GLOB_RECURSE VMA_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "vma/*.hpp")
file(GLOB_RECURSE VMA_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "vma/*.cpp")
file(GLOB_RECURSE UI_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE UI_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
	trace_library(NAME "dxcompiler" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/dxc.ps1" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME jsoncpp INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/jsoncpp.ps1")
	trace_dependency(NAME VulkanHeaders INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_headers.ps1")
	trace_dependency(NAME VulkanLoader INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_loader.ps1")
	trace_dependency(NAME VulkanMemoryAllocator INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vma.ps1")
	trace_dependency(NAME D3D12MemoryAllocator INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/d3d12ma.ps1")
endif()

add_library(LustVMA ${LIB_TYPE} ${VMA_HDRS} ${VMA_SRCS})
target_link_libraries(LustVMA PUBLIC GPUOpen::VulkanMemoryAllocator Vulkan::Headers Vulkan::Loader)
target_include_directories(LustVMA PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vma>
	$<INSTALL_INTERFACE:./>
)

set_cxx_project_standards(LustVMA 20 FALSE)
target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vma.cmake.in
	TARGET_NAME LustVMA
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "Lust" 
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/vma/
	HEADER_OUTPUT ./include/LustVMA)

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(LustVMA PRIVATE LUST_VMA_BUILD_DLL)
	target_compile_definitions(LustVMA INTERFACE LUST_VMA_CONSUME_DLL)
endif()

add_library(${TARGET_NAME} ${LIB_TYPE} ${UI_HDRS} ${UI_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/UI>
	$<INSTALL_INTERFACE:./>
	$<BUILD_INTERFACE:${CMAKE_PREFIX_PATH}/include/dxc>
	PRIVATE
	#$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/API/VK>
)
target_link_libraries(${TARGET_NAME} PRIVATE Vulkan::Headers Vulkan::Loader LustVMA)
if(WIN32)
	find_library(D3D12_FOUND NAMES d3d12 REQUIRED)
	find_library(DXGI_FOUND NAMES dxgi REQUIRED)
	find_library(DXGUID_FOUND NAMES dxguid REQUIRED)

	target_link_libraries(${TARGET_NAME} PRIVATE d3d12 dxgi dxguid GPUOpen::D3D12MemoryAllocator)
endif()

if(WIN32)
	target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_PREFIX_PATH}/lib/dxcompiler.lib)
elseif(LINUX)
	target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_PREFIX_PATH}/lib/dxcompiler.so)
endif()
target_link_libraries(${TARGET_NAME} PUBLIC LustShaderManager)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${TARGET_NAME} PUBLIC UI_DEBUG_MODE)
else()
	target_compile_definitions(${TARGET_NAME} PUBLIC UI_RELEASE_MODE)
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_UI_BUILD_DLL)
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_UI_CONSUME_DLL)
endif()

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_UI_USES_WINDOWS)
endif()

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
	TARGET_NAME ${TARGET_NAME}
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "RenderV2" 
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/UI/
	HEADER_OUTPUT ./include/Lust/UI)

if(BUILD_TESTS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(TARGET_TEST_NAME ${TARGET_NAME}Test)
	if(WIN32)
		target_add_test(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
						SUB_DIR "tests" 
						TARGET_NAME ${TARGET_TEST_NAME}
						GTEST_INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/gtest.ps1"
						EXTRA_LINKED_LIBS ${TARGET_NAME}
						EXTRA_INCLUDE_DIRS "${CMAKE_PREFIX_PATH}/include" "${CMAKE_CURRENT_SOURCE_DIR}/tests")
	endif()
endif()