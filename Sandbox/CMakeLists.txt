# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.

cmake_minimum_required(VERSION 3.22)

include(dep_tracer)
include(target_destination)
set(TARGET_NAME LustSandbox)

file(GLOB_RECURSE SANDBOX_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE SANDBOX_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_library(${TARGET_NAME} ${BUILD_TYPE} ${SANDBOX_HDRS} ${SANDBOX_SRCS})
target_link_libraries(${TARGET_NAME} PUBLIC Lust)
target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Sandbox>
	$<INSTALL_INTERFACE:./Lust/Sandbox>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
)

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_SANDBOX_API=__declspec\(dllexport\))
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_SANDBOX_API=__declspec\(dllimport\))
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR BUILD_AS_WINDOWED_APP)
		target_compile_definitions(${TARGET_NAME} PUBLIC LUST_CORE_WIN32_ENTRY)
	endif()
else()
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_SANDBOX_API)
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


# TODO: Adicione testes e instale destinos, se necessário.
