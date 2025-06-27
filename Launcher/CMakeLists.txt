cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME LustLauncher)

if(WIN32)
	add_d3d12_version_file(${CMAKE_CURRENT_SOURCE_DIR}/src/D3D12ContextExport.cpp)
endif()

set(SAMPLE_IMAGES
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/nanao.png"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/sample.png"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/david.jpg"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/rpg_pack.png"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/yor.png"
)

download_remote_files(SOURCE_INPUTS ${SAMPLE_IMAGES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures FORCE_DOWNLOAD)

file(GLOB_RECURSE LAUNCHER_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE LAUNCHER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

add_executable(${TARGET_NAME} ${LAUNCHER_HDRS} ${LAUNCHER_SRCS})
target_link_libraries(${TARGET_NAME} PUBLIC LustSandbox)
if(BUILD_SHARED_LIBS AND WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_SHARED_LIBS)
endif()

if(BUILD_AS_WINDOWED_APP)
	set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
