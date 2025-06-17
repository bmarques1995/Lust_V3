set(TARGET_NAME LustShaderManager)

set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
endif()

set(SPV_REFLECTION_HASH "refs/heads/vulkan-sdk-1.4.304")
set(SPV_REFLECTION_URL "https://raw.githubusercontent.com/KhronosGroup/SPIRV-Reflect")
set(SPV_REFLECTION_FILES "spirv_reflect.h"
						 "spirv_reflect.c"
						 "spirv_reflect.cpp")

download_sources_from_git_repo(COMMIT_VALUE ${SPV_REFLECTION_HASH} SOURCE_BASE_URL ${SPV_REFLECTION_URL} SOURCE_INPUTS ${SPV_REFLECTION_FILES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/spv_reflect)

file(GLOB_RECURSE SHADER_MNG_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE SHADER_MNG_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
	trace_library(NAME "dxcompiler" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/dxc.ps1" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME jsoncpp INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/jsoncpp.ps1")
	trace_dependency(NAME flatbuffers INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/flatbuffers.ps1")
	trace_dependency(NAME Spirv-Headers INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spirv_headers.ps1")
	trace_dependency(NAME SPIRV-Tools INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spirv_tools.ps1")
endif()

add_library(${TARGET_NAME} ${LIB_TYPE} ${SHADER_MNG_HDRS} ${SHADER_MNG_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderManager>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Reflector>
	$<INSTALL_INTERFACE:./>
	$<BUILD_INTERFACE:${CMAKE_PREFIX_PATH}/include/dxc>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/API/VK>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/spv_reflect>
)
if(WIN32)
	target_include_directories(${TARGET_NAME} PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/API/D3D12>
	)
	add_custom_target(copy_d3d12 ALL
			COMMAND ${CMAKE_COMMAND} -E copy
			"${PROJECT_SOURCE_DIR}/D3D12Core/build/native/bin/x64/D3D12Core.dll"
			${CMAKE_PREFIX_PATH}/bin/D3D12/D3D12Core.dll
			COMMENT "Copying D3D12Core dll into binary directory")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_custom_target(copy_d3d12_sdk ALL
				COMMAND ${CMAKE_COMMAND} -E copy
				"${PROJECT_SOURCE_DIR}/D3D12Core/build/native/bin/x64/d3d12SDKLayers.dll"
				${CMAKE_PREFIX_PATH}/bin/D3D12/d3d12SDKLayers.dll
				COMMENT "Copying d3d12SDKLayers dll into binary directory")
		add_dependencies(${TARGET_NAME} copy_d3d12_sdk)
	endif()
	add_dependencies(${TARGET_NAME} copy_d3d12)

	find_library(D3D12_FOUND NAMES d3d12 REQUIRED)
	find_library(DXGI_FOUND NAMES dxgi REQUIRED)
	find_library(DXGUID_FOUND NAMES dxguid REQUIRED)

	target_link_libraries(${TARGET_NAME} PRIVATE d3d12 dxgi dxguid)

endif()

if(WIN32)
	target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_PREFIX_PATH}/lib/dxcompiler.lib)
elseif(LINUX)
	target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_PREFIX_PATH}/lib/dxcompiler.so)
endif()
target_link_libraries(${TARGET_NAME} PUBLIC LustUtils jsoncpp_lib flatbuffers::flatbuffers SPIRV-Headers::SPIRV-Headers)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${TARGET_NAME} PUBLIC SHADER_MNG_DEBUG_MODE)
else()
	target_compile_definitions(${TARGET_NAME} PUBLIC SHADER_MNG_RELEASE_MODE)
endif()

target_compile_definitions(${TARGET_NAME} PRIVATE SPIRV_REFLECT_USE_SYSTEM_SPIRV_H)

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_SHADER_MNG_BUILD_DLL)
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_SHADER_MNG_CONSUME_DLL)
endif()

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_SHADER_MNG_USES_WINDOWS)
endif()

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
	TARGET_NAME ${TARGET_NAME}
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "Lust" 
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderManager/
	HEADER_OUTPUT ./include/Lust/ShaderManager)

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