# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
cmake_minimum_required(VERSION 3.22)

include(dep_tracer)
include(target_destination)
set(TARGET_NAME Lust)

file(GLOB_RECURSE LUST_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE LUST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
	trace_file(NAME "d3d12core" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/d3d12core.ps1" LOCATION "${PROJECT_SOURCE_DIR}" EXTENSION "zip")
	trace_dependency(NAME sdl3 INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/sdl3.ps1")
	trace_dependency(NAME jsoncpp INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/jsoncpp.ps1")
	trace_dependency(NAME VulkanHeaders INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_headers.ps1")
	trace_dependency(NAME VulkanLoader INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_loader.ps1")
	trace_dependency(NAME Spirv-Headers INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spirv_headers.ps1")
	trace_dependency(NAME SPIRV-Tools INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spirv_tools.ps1")
	
	trace_installable_file(NAME "VkLayer_khronos_shader_object" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_extensions.ps1" LOCATION "${CMAKE_PREFIX_PATH}/bin" EXTENSION "dll")
	trace_installable_file(NAME "VkLayer_khronos_shader_object" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_extensions.ps1" LOCATION "${CMAKE_PREFIX_PATH}/bin" EXTENSION "json")
	trace_installable_file(NAME "VkLayer_khronos_synchronization2" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_extensions.ps1" LOCATION "${CMAKE_PREFIX_PATH}/bin" EXTENSION "dll")
	trace_installable_file(NAME "VkLayer_khronos_synchronization2" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_extensions.ps1" LOCATION "${CMAKE_PREFIX_PATH}/bin" EXTENSION "json")
	trace_installable_file(NAME "VkLayer_khronos_validation" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_validation.ps1" LOCATION "${CMAKE_PREFIX_PATH}/bin" EXTENSION "dll")
	trace_installable_file(NAME "VkLayer_khronos_validation" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_validation.ps1" LOCATION "${CMAKE_PREFIX_PATH}/bin" EXTENSION "json")

	if(REGISTER_VULKAN_LAYERS)

		file(GLOB LAYER_FILES "${CMAKE_INSTALL_PREFIX}/bin/VkLayer_khronos_*.json")

		list(JOIN LAYER_FILES "|" LAYER_FILES_STR)

		execute_process(COMMAND powershell -File "${PROJECT_SOURCE_DIR}/installers/vulkan_regedit.ps1" ${LAYER_FILES_STR})
	endif()

endif()

add_library(${TARGET_NAME} ${BUILD_TYPE} ${LUST_HDRS} ${LUST_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Core>
	$<INSTALL_INTERFACE:./Lust/Core>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Events>
	$<INSTALL_INTERFACE:./Lust/Events>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Window>
	$<INSTALL_INTERFACE:./Lust/Window>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics>
	$<INSTALL_INTERFACE:./Lust/Graphics>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Window>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Graphics/VK>
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_DEBUG_MODE)
else()
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_RELEASE_MODE)
endif()

if(WIN32)
	target_include_directories(${TARGET_NAME} PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Graphics/D3D12>
	)
	target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Platform/Graphics/D3D12>)
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
	
	target_compile_definitions(${TARGET_NAME} PUBLIC WIN32_LEAN_AND_MEAN LUST_USES_WINDOWS NOMINMAX VK_USE_PLATFORM_WIN32_KHR)

	find_library(D3D12_FOUND NAMES d3d12 REQUIRED)
	find_library(DXGI_FOUND NAMES dxgi REQUIRED)
	find_library(DXGUID_FOUND NAMES dxguid REQUIRED)

	target_link_libraries(${TARGET_NAME} PRIVATE d3d12 dxgi dxguid)
endif()
target_link_libraries(${TARGET_NAME} PUBLIC LustUtils SDL3::SDL3 jsoncpp_lib Vulkan::Headers Vulkan::Loader )

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_BUILD_DLL)
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_CONSUME_DLL)
endif()

target_precompile_headers(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch/pch.hpp)

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
	TARGET_NAME ${TARGET_NAME}
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "Lust"
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/ ${CMAKE_CURRENT_SOURCE_DIR}/src/Window/
	HEADER_OUTPUT ./include/Lust/Core ./include/Lust/Events ./include/Lust/Window)

# TODO: Adicione testes e instale destinos, se necessário.
