# CMakeList.txt: arquivo de projeto de CMake de nível superior, fazer configuração global
# e inclua subprojetos aqui.
#
cmake_minimum_required (VERSION 3.22)

project("Lust" VERSION 0.0.1)

option(BUILD_SHARED_LIBS "Build all libs as .dll/.so" OFF)
option(BUILD_AS_WINDOWED_APP "Build the app as Win32 application" OFF)
option(REGISTER_VULKAN_LAYERS "Set vulkan layers location on regedit" OFF)

set(BUILD_TYPE STATIC CACHE STRING "Set if the lib will be static or shared")
if(BUILD_SHARED_LIBS)
	set(BUILD_TYPE SHARED)
endif()

# Habilite a Recarga Dinâmica para compiladores MSVC, se houver suporte.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_helpers")

include(dep_tracer)
include(target_destination)
include(source_downloader)

# Inclua subprojetos.
add_subdirectory("Utils")
add_subdirectory("ShaderManager")
add_subdirectory("Lust")
add_subdirectory("Sandbox")
add_subdirectory("Launcher")
