set(TARGET_NAME Tests)

file(GLOB_RECURSE TESTS_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE TESTS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

if(WIN32)
    trace_dependency(NAME GTest INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/gtest.ps1")
    trace_dependency(NAME glm INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/glm.ps1")
    trace_dependency(NAME Eigen3 INSTALL_SCRIPT "${PROJECT_SOURCE_DIR}/installers/eigen.ps1" LIMIT_SEARCH_PATHS)
endif()

enable_testing()
add_executable(${TARGET_NAME} ${TESTS_SRCS})
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_PREFIX_PATH}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(${TARGET_NAME} PRIVATE GTest::gtest_main GTest::gtest GTest::gmock_main GTest::gmock glm::glm-header-only Eigen3::Eigen Lust)
set_cxx_project_standards(${TARGET_NAME} 20 FALSE)
add_test(
    ${TARGET_NAME}
    ${TARGET_NAME}
)

