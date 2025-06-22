# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME Lust)

set(IMGUI_COMMIT_HASH "11b3a7c8ca23201294464c7f368614a9106af2a1")
set(IMGUI_SOURCE_URL "https://raw.githubusercontent.com/ocornut/imgui")

set(IMGUI_PUBLIC_FILES "imgui.h"
					   "imconfig.h"
					   "imgui.cpp"
					   "imstb_rectpack.h"
					   "imstb_textedit.h"
					   "imstb_truetype.h"
					   "imgui_demo.cpp"
					   "imgui_draw.cpp"
					   "imgui_widgets.cpp"
					   "imgui_tables.cpp" 
					   "imgui_internal.h")

set(IMGUI_BACKEND_FILES "backends/imgui_impl_sdl3.cpp"
						"backends/imgui_impl_sdl3.h"
						"backends/imgui_impl_vulkan.cpp"
						"backends/imgui_impl_vulkan.h")

if(WIN32)
	set(IMGUI_BACKEND_FILES ${IMGUI_BACKEND_FILES}
							"backends/imgui_impl_dx12.cpp"
							"backends/imgui_impl_dx12.h")
endif()

download_sources_from_git_repo(COMMIT_VALUE ${IMGUI_COMMIT_HASH} SOURCE_BASE_URL ${IMGUI_SOURCE_URL} SOURCE_INPUTS ${IMGUI_PUBLIC_FILES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/imgui/public/imgui)
download_sources_from_git_repo(COMMIT_VALUE ${IMGUI_COMMIT_HASH} SOURCE_BASE_URL ${IMGUI_SOURCE_URL} SOURCE_INPUTS ${IMGUI_BACKEND_FILES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/imgui)

file(GLOB_RECURSE LUST_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE LUST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
	trace_file(NAME "d3d12core" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/d3d12core.ps1" LOCATION "${PROJECT_SOURCE_DIR}" EXTENSION "zip")
	trace_dependency(NAME sdl3 INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/sdl3.ps1")
	trace_dependency(NAME entt INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/entt.ps1")
	trace_dependency(NAME VulkanHeaders INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_headers.ps1")
	trace_dependency(NAME VulkanLoader INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_loader.ps1")
	trace_dependency(NAME Spirv-Headers INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spirv_headers.ps1")
	trace_dependency(NAME SPIRV-Tools INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spirv_tools.ps1")
	trace_dependency(NAME D3D12MemoryAllocator INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/d3d12ma.ps1")
	trace_dependency(NAME mimalloc INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/mimalloc.ps1")
	
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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Events>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Window>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Sockets>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Scene>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Graphics>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/MathOperations>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/imgui/public>
	$<INSTALL_INTERFACE:./Lust/third_party/imgui/public>
	$<INSTALL_INTERFACE:./>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Window>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Graphics/VK>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/imgui/public/imgui>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/imgui/backends>
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_DEBUG_MODE)
else()
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_RELEASE_MODE)
endif()

if(WIN32)
	target_include_directories(${TARGET_NAME} PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Graphics/D3D12>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Sockets/Win32>
	)
	target_include_directories(${TARGET_NAME}
		PRIVATE 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Platform/Graphics/D3D12>)
	
	target_compile_definitions(${TARGET_NAME} PUBLIC WIN32_LEAN_AND_MEAN LUST_USES_WINDOWS NOMINMAX VK_USE_PLATFORM_WIN32_KHR)

	find_library(D3D12_FOUND NAMES d3d12 REQUIRED)
	find_library(DXGI_FOUND NAMES dxgi REQUIRED)
	find_library(DXGUID_FOUND NAMES dxguid REQUIRED)

	target_link_libraries(${TARGET_NAME} PRIVATE d3d12 dxgi dxguid ws2_32 GPUOpen::D3D12MemoryAllocator)
endif()
target_link_libraries(${TARGET_NAME} PUBLIC LustUtils SDL3::SDL3 Vulkan::Headers Vulkan::Loader LustShaderManager EnTT::EnTT LustVMA)
if(WIN32)
	target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_PREFIX_PATH}/lib/dxcompiler.lib)
elseif(LINUX)
	target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_PREFIX_PATH}/lib/dxcompiler.so)
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_BUILD_DLL)
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_CONSUME_DLL)
	target_compile_definitions(${TARGET_NAME} PRIVATE IMGUI_API=__declspec\(dllexport\))
	target_compile_definitions(${TARGET_NAME} INTERFACE IMGUI_API=__declspec\(dllimport\))
endif()

target_precompile_headers(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch/pch.hpp)

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
	TARGET_NAME ${TARGET_NAME}
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "Lust"
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ 
				 ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/
				 ${CMAKE_CURRENT_SOURCE_DIR}/src/Window/
				 ${CMAKE_CURRENT_SOURCE_DIR}/src/MathOperations/
				 ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/imgui/public
	HEADER_OUTPUT ./include/Lust/Core
				  ./include/Lust/Events
				  ./include/Lust/Window
				  ./include/Lust/MathOperations
				  ./include/Lust/third_party/imgui/public)

# TODO: Adicione testes e instale destinos, se necessário.

add_custom_target(copy_core_assets ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/assets"
			${CMAKE_PREFIX_PATH}/bin/assets
			COMMENT "Copying D3D12Core dll into binary directory")
add_dependencies(${TARGET_NAME} copy_core_assets)
