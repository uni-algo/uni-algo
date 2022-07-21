cmake_minimum_required(VERSION 3.5)

project(uni-algo-test)

# This file is used by GitHub Actions workflows edit with care.

add_executable(${PROJECT_NAME})

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11))
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
else()
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8;/W4>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-Wextra;-Wconversion;-Wsign-conversion;-Wsign-compare;-Wshadow;-Wmissing-braces>")

if(NOT DOWNLOAD_UNICODE_TEST_FILES)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_MODE_WITHOUT_UNICODE_TEST_FILES")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../include")

target_sources(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/main.cpp")

if(DOWNLOAD_UNICODE_TEST_FILES)
    # https://stackoverflow.com/questions/61255773/cmake-file-download-ignores-errors
    function(FUNC_DOWNLOAD FUNC_ARG)
        get_filename_component(FILE_NAME ${FUNC_ARG} NAME)
        message(STATUS "Downloading... " ${FILE_NAME})
        file(DOWNLOAD ${FUNC_ARG} ${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME} STATUS DOWNLOAD_STATUS)
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        if(NOT ${STATUS_CODE} EQUAL 0)
            message(FATAL_ERROR "Error while downloading: " ${FILE_NAME})
        endif()
    endfunction()

    FUNC_DOWNLOAD("https://unicode.org/Public/14.0.0/ucd/NormalizationTest.txt")
    FUNC_DOWNLOAD("https://unicode.org/Public/14.0.0/ucd/auxiliary/GraphemeBreakTest.txt")
    FUNC_DOWNLOAD("https://unicode.org/Public/14.0.0/ucd/auxiliary/WordBreakTest.txt")
endif() # DOWNLOAD_UNICODE_TEST_FILES

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
