# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME LustUtils)

file(GLOB_RECURSE UTILS_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE UTILS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
	trace_dependency(NAME jsoncpp INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/jsoncpp.ps1" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME spdlog INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spdlog.ps1")
	trace_dependency(NAME ZLIB INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/zlib.ps1" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME PNG INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/libpng.ps1")
	trace_dependency(NAME libjpeg-turbo INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/libjpeg_turbo.ps1")
endif()

add_library(${TARGET_NAME} ${BUILD_TYPE} ${UTILS_HDRS} ${UTILS_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Utils>
	$<INSTALL_INTERFACE:./>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog libjpeg-turbo::jpeg PNG::PNG ZLIB::ZLIB jsoncpp_lib)

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

set(TARGET_TEST_NAME ${TARGET_NAME}Test)



if(BUILD_TESTS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	
	file(GLOB_RECURSE TESTS_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/*.hpp")
	file(GLOB_RECURSE TESTS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/*.cpp")

	enable_testing()
	if(WIN32)
		trace_dependency(NAME GTest INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/gtest.ps1")
	endif()
	add_executable(${TARGET_TEST_NAME} ${TESTS_SRCS})
	target_include_directories(${TARGET_TEST_NAME} PRIVATE "${CMAKE_PREFIX_PATH}/include" "${CMAKE_CURRENT_SOURCE_DIR}/tests")
	target_link_libraries(${TARGET_TEST_NAME} PRIVATE GTest::gtest_main GTest::gtest GTest::gmock_main GTest::gmock ${TARGET_NAME})
	set_cxx_project_standards(${TARGET_TEST_NAME} 20 FALSE)
	add_test(
		${TARGET_TEST_NAME}
		${TARGET_TEST_NAME}
	)

endif()