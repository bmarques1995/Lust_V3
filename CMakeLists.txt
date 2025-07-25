﻿# CMakeList.txt: arquivo de projeto de CMake de nível superior, fazer configuração global
# e inclua subprojetos aqui.
#
cmake_minimum_required (VERSION 3.22)

project("Lust" VERSION 0.0.1)

option(BUILD_SHARED_LIBS "Build all libs as .dll/.so" OFF)
option(BUILD_AS_WINDOWED_APP "Build the app as Win32 application" ON)
option(REGISTER_VULKAN_LAYERS "Set vulkan layers location on regedit" OFF)
option(BUILD_EXAMPLES "Build example program" ON)
option(BUILD_TESTS "Build tests" ON)
option(GEN_DOCS "Generate documentation" ON)
option(DISABLE_BUILDS "Disable builds" OFF)

set(BUILD_TYPE STATIC CACHE STRING "Set if the lib will be static or shared")
if(BUILD_SHARED_LIBS)
	set(BUILD_TYPE SHARED)
endif()

# Habilite a Recarga Dinâmica para compiladores MSVC, se houver suporte.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_helpers;${PROJECT_SOURCE_DIR}/cmake_extra")

include(dep_tracer)
include(target_destination)
include(source_downloader)
include(d3d12_exporter)

if (MSVC)
    add_compile_options(/ZI)
endif ()

file(GLOB_RECURSE VMA_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "vma/*.hpp")
file(GLOB_RECURSE VMA_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "vma/*.cpp")

if(WIN32)
    trace_dependency(NAME Qt6 COMPONENTS Core Widgets INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/qtbase.py" USE_VSTOOLS LIMIT_SEARCH_PATHS)
	trace_dependency(NAME Qt6 COMPONENTS Svg INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/qtsvg.py" USE_VSTOOLS LIMIT_SEARCH_PATHS)
	trace_dependency(NAME Qt6 COMPONENTS Linguist INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/qttools.py" USE_VSTOOLS LIMIT_SEARCH_PATHS)
	trace_dependency(NAME VulkanHeaders INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_headers.py")
	trace_dependency(NAME VulkanLoader INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vulkan_loader.py")
	trace_dependency(NAME VulkanMemoryAllocator INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/vma.py")
endif()

add_library(LustVMA ${LIB_TYPE} ${VMA_HDRS} ${VMA_SRCS})
target_link_libraries(LustVMA PUBLIC GPUOpen::VulkanMemoryAllocator Vulkan::Headers Vulkan::Loader)
target_include_directories(LustVMA PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vma>
	$<INSTALL_INTERFACE:./>
)

set_cxx_project_standards(LustVMA 20 FALSE)
target_installation_behaviour(
	CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vma/cmake/vma.cmake.in
	TARGET_NAME LustVMA
	VERSION ${PROJECT_VERSION}
	PROJECT_NAME ${PROJECT_NAME}
	NAMESPACE "Lust" 
	HEADER_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/vma/
	HEADER_OUTPUT ./include/LustVMA)

if(WIN32 AND BUILD_SHARED_LIBS)
	target_compile_definitions(LustVMA PRIVATE LUST_VMA_BUILD_DLL)
	target_compile_definitions(LustVMA INTERFACE LUST_VMA_CONSUME_DLL)
endif()

if(NOT DISABLE_BUILDS)
    # Inclua subprojetos.
    add_subdirectory("Utils")
    add_subdirectory("ShaderManager")
    add_subdirectory("Lust")
    add_subdirectory("Sandbox")
    add_subdirectory("Editor")
    add_subdirectory("Launcher")
    if(BUILD_EXAMPLES)
	    add_subdirectory("Examples")
    endif()
    add_subdirectory("MathTests")
endif()

if(GEN_DOCS)
	find_package(Doxygen REQUIRED)

    set(DOXY_AWESOME_COMMIT_HASH "568f56cde6ac78b6dfcc14acd380b2e745c301ea")
    set(DOXY_AWESOME_SOURCE_URL "https://raw.githubusercontent.com/jothepro/doxygen-awesome-css")
    set(DOXY_AWESOME_FILES  "doxygen-awesome.css"
                            "doxygen-awesome-sidebar-only-darkmode-toggle.css"
                            "doxygen-awesome-sidebar-only.css"
                            "doxygen-custom/custom.css"
                            "doxygen-custom/custom-alternative.css"
                            "doxygen-awesome-darkmode-toggle.js"
                            "doxygen-awesome-fragment-copy-button.js"
                            "doxygen-awesome-paragraph-link.js"
                            "doxygen-custom/toggle-alternative-theme.js"
                            "doxygen-awesome-interactive-toc.js"
                            "doxygen-awesome-tabs.js")

    # https://raw.githubusercontent.com/jothepro/doxygen-awesome-css/568f56cde6ac78b6dfcc14acd380b2e745c301ea/doxygen-awesome.css
    download_sources_from_git_repo(COMMIT_VALUE ${DOXY_AWESOME_COMMIT_HASH} SOURCE_BASE_URL ${DOXY_AWESOME_SOURCE_URL} SOURCE_INPUTS ${DOXY_AWESOME_FILES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs_style)

    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_OUT ${PROJECT_SOURCE_DIR}/docs)

    # Ensure the docs directory exists
    file(MAKE_DIRECTORY ${DOXYGEN_OUT})

    add_custom_target(doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )

endif()
