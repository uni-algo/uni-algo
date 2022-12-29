cmake_minimum_required(VERSION 3.12...3.20)

# NOTE: This file is used by GitHub Actions workflows edit with care

# OPTIONS:
# DOWNLOAD_UNICODE_TEST_FILES - Download/redownload Unicode test files before testing
# BUILD_UNI_ALGO - Build the library before testing otherwise build together with tests (manual use)
# TEST_CONSTEXPR - Test in constexpr mode (implies header-only and requeries C++20 or higher)
# TEST_HEADER_ONLY - Test with header-only library
# TEST_SINGLE_INCLUDE - Test with single include library (copy uni_algo.h to test folder first)

project(uni-algo-test LANGUAGES CXX)

if(DOWNLOAD_UNICODE_TEST_FILES AND TEST_CONSTEXPR)
    message(FATAL_ERROR "DOWNLOAD_UNICODE_TEST_FILES and TEST_CONSTEXPR must not be used together")
endif()

add_executable(${PROJECT_NAME})

# Use C++17 for old compilers or when it forced with CMAKE_CXX_STANDARD=17 otherwise C++20
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.29))
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
elseif(NOT DEFINED CMAKE_CXX_STANDARD)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8;/W4;/WX")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        "-Werror"
        "-Wall;-Wextra;-Wpedantic"
        "-Wconversion;-Wsign-conversion;-Wsign-compare"
        "-Wnull-dereference;-Wcast-align;-Wcast-qual"
        "-Wshadow;-Wmissing-braces")
endif()

if(NOT DOWNLOAD_UNICODE_TEST_FILES)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_MODE_WITHOUT_UNICODE_TEST_FILES)
endif()

if(TEST_CONSTEXPR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_MODE_CONSTEXPR)
endif()

if(TEST_SINGLE_INCLUDE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_MODE_SINGLE_INCLUDE)
endif()

# NOTE: By default tests compile together with uni-algo library (manual use) without building it first
# but if needed to test with building the library or with header-only mode for example
# then pass to CMake: -DBUILD_UNI_ALGO=ON -DUNI_ALGO_HEADER_ONLY=ON
# For header-only mode -DTEST_HEADER_ONLY can be used instead now.
if(NOT BUILD_UNI_ALGO)
    # Manual use should work even without include directory compile option
    #target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../include/uni_algo")

    if(TEST_HEADER_ONLY OR TEST_CONSTEXPR)
        target_compile_definitions(${PROJECT_NAME} PRIVATE UNI_ALGO_STATIC_DATA)
    elseif(NOT TEST_SINGLE_INCLUDE)
        target_sources(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../src/data.cpp")
    endif()
else()
    if(TEST_HEADER_ONLY OR TEST_CONSTEXPR)
        set(UNI_ALGO_HEADER_ONLY ON CACHE INTERNAL "")
    endif()

    add_subdirectory("${PROJECT_SOURCE_DIR}/.." build_uni_algo)
    target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)
endif()

target_sources(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/main.cpp")

if(DOWNLOAD_UNICODE_TEST_FILES)
    # https://stackoverflow.com/questions/61255773/cmake-file-download-ignores-errors
    function(func_download func_arg)
        get_filename_component(file_name ${func_arg} NAME)
        message(STATUS "Downloading... " ${file_name})
        file(DOWNLOAD ${func_arg} ${PROJECT_BINARY_DIR}/${file_name} STATUS download_status)
        list(GET download_status 0 status_code)
        if(NOT ${status_code} EQUAL 0)
            message(FATAL_ERROR "Error while downloading: " ${file_name})
        endif()
    endfunction()

    func_download("https://www.unicode.org/Public/15.0.0/ucd/NormalizationTest.txt")
    func_download("https://www.unicode.org/Public/15.0.0/ucd/auxiliary/GraphemeBreakTest.txt")
    func_download("https://www.unicode.org/Public/15.0.0/ucd/auxiliary/WordBreakTest.txt")
endif() # DOWNLOAD_UNICODE_TEST_FILES

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
