# CMakeList.txt: arquivo de projeto de CMake de nível superior, fazer configuração global
# e inclua subprojetos aqui.
#
cmake_minimum_required (VERSION 3.22)

project("Lust" VERSION 0.0.1)

option(BUILD_SHARED_LIBS "Build all libs as .dll/.so" OFF)
option(BUILD_AS_WINDOWED_APP "Build the app as Win32 application" OFF)
option(REGISTER_VULKAN_LAYERS "Set vulkan layers location on regedit" OFF)
option(BUILD_EXAMPLES "Build example program" ON)
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

if (MSVC)
  add_compile_options(/ZI)
endif ()

if(NOT DISABLE_BUILDS)
    # Inclua subprojetos.
    add_subdirectory("Utils")
    add_subdirectory("ShaderManager")
    add_subdirectory("Lust")
    add_subdirectory("Sandbox")
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
