# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.

cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME LustSandbox)

set(SAMPLE_IMAGES
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/nanao.png"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/sample.png"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/david.jpg"
	"https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/rpg_pack.png"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/yor.png"
)

download_remote_files(SOURCE_INPUTS ${SAMPLE_IMAGES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures FORCE_DOWNLOAD)

file(GLOB_RECURSE SANDBOX_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE SANDBOX_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_library(${TARGET_NAME} ${BUILD_TYPE} ${SANDBOX_HDRS} ${SANDBOX_SRCS})
target_link_libraries(${TARGET_NAME} PUBLIC Lust)
target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Sandbox>
	$<INSTALL_INTERFACE:./>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
)

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_SANDBOX_BUILD_DLL)
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_SANDBOX_CONSUME_DLL)
	if(BUILD_AS_WINDOWED_APP)
		target_compile_definitions(${TARGET_NAME} PUBLIC LUST_CORE_WIN32_ENTRY)
	endif()
endif()

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_SANDBOX_WINDOWS)
else()
	message(FATAL_ERROR "Platform not implemented")
endif()

target_precompile_headers(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch/pch.hpp)

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
	TARGET_NAME ${TARGET_NAME}
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "Lust"
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Sandbox/
	HEADER_OUTPUT ./include/Lust/Sandbox)

add_custom_target(copy_sandbox_assets ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/assets"
			${CMAKE_PREFIX_PATH}/bin/assets
			COMMENT "Copying D3D12Core dll into binary directory")
add_dependencies(${TARGET_NAME} copy_sandbox_assets)

# TODO: Adicione testes e instale destinos, se necessário.
