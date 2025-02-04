# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
cmake_minimum_required(VERSION 3.22)

include(dep_tracer)
include(target_destination)
set(TARGET_NAME LustUtils)

file(GLOB_RECURSE UTILS_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE UTILS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
	trace_dependency(NAME spdlog INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spdlog.ps1")
endif()

add_library(${TARGET_NAME} ${BUILD_TYPE} ${UTILS_HDRS} ${UTILS_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Utils>
	$<INSTALL_INTERFACE:./Lust/Utils>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_UTILS_BUILD_DLL)
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_UTILS_CONSUME_DLL)
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR BUILD_AS_WINDOWED_APP)
		target_compile_definitions(${TARGET_NAME} PUBLIC LUST_UTILS_WINDOWED)
	endif()
endif()

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_UTILS_WINDOWS)
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
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/
	HEADER_OUTPUT ./include/Lust/Utils)