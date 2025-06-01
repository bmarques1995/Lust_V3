cmake_minimum_required(VERSION 3.22)

set(TARGET_NAME FlappyClone)

set(SAMPLE_IMAGES
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/LustShip.png"
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/Triangle.png"
)

if(USE_REMOTE)
	set(SAMPLE_IMAGES
		"https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/LustShip.png"
		"https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/textures/Triangle.png"
	)
else()
	set(SAMPLE_IMAGES
		"http://localhost:8000/images/LustShip.png"
		"http://localhost:8000/images/Triangle.png"
	)
endif()

download_remote_files(SOURCE_INPUTS ${SAMPLE_IMAGES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Examples/FlappyClone/assets/textures)

set(SAMPLE_FONTS
    "https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/fonts/OpenSans-Regular.ttf"
)

if(USE_REMOTE)
	set(SAMPLE_FONTS
		"https://media.githubusercontent.com/media/bmarques1995/Render/master/TargetView/assets/fonts/OpenSans-Regular.ttf"
	)
else()
	set(SAMPLE_IMAGES
		"http://localhost:8000/fonts/OpenSans-Regular.ttf"
	)
endif()

download_remote_files(SOURCE_INPUTS ${SAMPLE_FONTS} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Examples/FlappyClone/assets/fonts)

file(GLOB_RECURSE LAUNCHER_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.hpp")
file(GLOB_RECURSE LAUNCHER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

set(SOURCE_LIST ${LAUNCHER_SRCS})
if(WIN32)
	set(SOURCE_LIST ${SOURCE_LIST} ${EXAMPLE_ROOT_DIR}/D3D12ContextExport.cpp)
endif()

add_executable(${TARGET_NAME} ${LAUNCHER_HDRS} ${SOURCE_LIST})
target_link_libraries(${TARGET_NAME} PUBLIC LustSandbox)
if(BUILD_SHARED_LIBS AND WIN32)
	target_compile_definitions(${TARGET_NAME} PUBLIC LUST_SHARED_LIBS)
endif()

if(BUILD_AS_WINDOWED_APP)
	set_target_properties(${TARGET_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

set_cxx_project_standards(${TARGET_NAME} 20 FALSE)

add_custom_target(copy_flappy_assets ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/Examples"
			${CMAKE_PREFIX_PATH}/bin/Examples
			COMMENT "Copying D3D12Core dll into binary directory")
add_dependencies(${TARGET_NAME} copy_flappy_assets)