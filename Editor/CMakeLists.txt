# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.

cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME LustEditor)

if(WIN32)
	file(READ "${PROJECT_SOURCE_DIR}/d3d12sdk_version.txt" SDK_VERSION)
	string(REGEX REPLACE "[^0-9]" "" SANITIZED_SDK_VERSION "${SDK_VERSION}")
	file(READ ${PROJECT_SOURCE_DIR}/D3D12ContextExport.cxx.in SOURCE_CONTENTS)
	string(REGEX REPLACE SDK_VERSION "${SANITIZED_SDK_VERSION}" OUTPUT_SOURCE ${SOURCE_CONTENTS})
	file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/src/D3D12ContextExport.cpp ${OUTPUT_SOURCE})
endif()

if(WIN32)
	trace_dependency(NAME Qt6 COMPONENTS Core Widgets Gui INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/qtbase.ps1" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME Qt6 COMPONENTS Svg INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/qtsvg.ps1" LIMIT_SEARCH_PATHS)
endif()

file(GLOB_RECURSE LAUNCHER_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE LAUNCHER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_executable(${TARGET_NAME} ${LAUNCHER_HDRS} ${LAUNCHER_SRCS})
target_link_libraries(${TARGET_NAME} PUBLIC LustSandbox Qt6::Core Qt6::Widgets Qt6::Svg Qt6::Gui)
target_include_directories(${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/Editor>
	$<INSTALL_INTERFACE:./>
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch>
)
if(BUILD_SHARED_LIBS AND WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_SHARED_LIBS)
endif()

if(BUILD_AS_WINDOWED_APP)
	set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR BUILD_AS_WINDOWED_APP)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_EDITOR_WINDOWED)
endif()

target_precompile_headers(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pch/pch.hpp)
set_cxx_project_standards(${TARGET_NAME} 20 FALSE)

add_custom_target(copy_translations ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/translations"
			${CMAKE_PREFIX_PATH}/bin/translations
			COMMENT "Copying D3D12Core dll into binary directory")
add_dependencies(${TARGET_NAME} copy_translations)
