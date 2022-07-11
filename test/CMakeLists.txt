cmake_minimum_required(VERSION 3.5)

project(uni-algo-test)

# TODO: FIXME:
# This file is used by GitHub Actions and may not work for other tasks.

add_executable(${PROJECT_NAME})

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11))
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
else()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror;-Wall;-Wextra>")

target_compile_definitions(${PROJECT_NAME} PRIVATE "NDEBUG" "TEST_MODE_WITHOUT_UNICODE_TEST_FILES")

#target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../src")

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

target_sources(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/../src/cpp_uni_data.cpp"
    "${PROJECT_SOURCE_DIR}/main.cpp")
