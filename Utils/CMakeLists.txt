# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME LustUtils)

file(GLOB_RECURSE UTILS_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE UTILS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
	trace_dependency(NAME Qt6 COMPONENTS Widgets INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/qtbase.py" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME jsoncpp INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/jsoncpp.py" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME spdlog INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/spdlog.py")
	trace_dependency(NAME Eigen3 INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/eigen.py" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME ZLIB INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/zlib.py" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME PNG INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/libpng.py")
	trace_dependency(NAME libjpeg-turbo INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/libjpeg_turbo.py")
endif()

# Check if OpenMP is available
find_package(OpenMP REQUIRED COMPONENTS C CXX)

add_library(${TARGET_NAME} ${BUILD_TYPE} ${UTILS_HDRS} ${UTILS_SRCS})

# Enable OpenMP if found
if(OpenMP_CXX_FOUND)
  target_compile_options(${TARGET_NAME} PUBLIC ${OpenMP_CXX_FLAGS})
  target_link_libraries(${TARGET_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()

# Enable Eigen's threading support
target_compile_definitions(${TARGET_NAME} PUBLIC EIGEN_USE_THREADS)

target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Utils>
	$<INSTALL_INTERFACE:./>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
)
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog libjpeg-turbo::jpeg PNG::PNG ZLIB::ZLIB jsoncpp_lib Eigen3::Eigen Qt6::Widgets)

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



if(BUILD_TESTS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(TARGET_TEST_NAME ${TARGET_NAME}Test)
	if(WIN32)
		target_add_test(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
						SUB_DIR "tests" 
						TARGET_NAME ${TARGET_TEST_NAME}
						GTEST_INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/gtest.py"
						EXTRA_LINKED_LIBS ${TARGET_NAME} d3d12
						EXTRA_INCLUDE_DIRS "${CMAKE_PREFIX_PATH}/include" "${CMAKE_CURRENT_SOURCE_DIR}/tests")
	endif()
endif()