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

add_library(${TARGET_NAME} ${BUILD_TYPE} ${LUST_HDRS} ${LUST_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Core>
	$<INSTALL_INTERFACE:./Lust/Core>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Events>
	$<INSTALL_INTERFACE:./Lust/Events>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
)
target_link_libraries(${TARGET_NAME} PUBLIC LustUtils)

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
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/
	HEADER_OUTPUT ./include/Lust/Core ./include/Lust/Events)

# TODO: Adicione testes e instale destinos, se necessário.
