set(TARGET_NAME LustShaderManager)

set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
endif()

file(GLOB_RECURSE SHADER_MNG_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE SHADER_MNG_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
	trace_library(NAME "dxcompiler" INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/dxc.ps1" LIMIT_SEARCH_PATHS)
	trace_dependency(NAME jsoncpp INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/jsoncpp.ps1")
endif()

add_library(${TARGET_NAME} ${LIB_TYPE} ${SHADER_MNG_HDRS} ${SHADER_MNG_SRCS})
target_include_directories(${TARGET_NAME} PUBLIC 

	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderManager>
	$<INSTALL_INTERFACE:./>
	$<BUILD_INTERFACE:${CMAKE_PREFIX_PATH}/include/dxc>
)
if(WIN32)
	target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_PREFIX_PATH}/lib/dxcompiler.lib)
elseif(LINUX)
	target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_PREFIX_PATH}/lib/dxcompiler.so)
endif()
target_link_libraries(${TARGET_NAME} PUBLIC LustUtils jsoncpp_lib)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${TARGET_NAME} PUBLIC SHADER_MNG_DEBUG_MODE)
else()
	target_compile_definitions(${TARGET_NAME} PUBLIC SHADER_MNG_RELEASE_MODE)
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(${TARGET_NAME} PRIVATE LUST_SHADER_MNG_BUILD_DLL)
	target_compile_definitions(${TARGET_NAME} INTERFACE LUST_SHADER_MNG_CONSUME_DLL)
endif()

if(WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_SHADER_MNG_USES_WINDOWS)
endif()

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in
	TARGET_NAME ${TARGET_NAME}
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "RenderV2" 
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderManager/
	HEADER_OUTPUT ./include/Lust/ShaderManager)