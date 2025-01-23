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
	trace_dependency(NAME sdl3 INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/sdl3.ps1")
	trace_dependency(NAME jsoncpp INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/jsoncpp.ps1")
endif()

add_library(${TARGET_NAME} ${BUILD_TYPE} ${LUST_HDRS} ${LUST_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Core>
	$<INSTALL_INTERFACE:./Lust/Core>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Events>
	$<INSTALL_INTERFACE:./Lust/Events>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Window>
	$<INSTALL_INTERFACE:./Lust/Window>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Window>
)
target_link_libraries(${TARGET_NAME} PUBLIC LustUtils SDL3::SDL3 jsoncpp_lib)

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_API=__declspec\(dllexport\))
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_API=__declspec\(dllimport\))
else()
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_API)
endif()

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_CORE_WINDOWS)
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
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/ ${CMAKE_CURRENT_SOURCE_DIR}/src/Window/
	HEADER_OUTPUT ./include/Lust/Core ./include/Lust/Events ./include/Lust/Window)

# TODO: Adicione testes e instale destinos, se necessário.
