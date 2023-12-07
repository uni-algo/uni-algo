cmake_minimum_required(VERSION 3.12...3.28)

# NOTE: This file is used by GitHub Actions workflows edit with care

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/default-build-type.cmake")

project(uni-algo-test LANGUAGES CXX)

option(DOWNLOAD_UNICODE_TEST_FILES "Download Unicode test files and test with them" OFF)
option(BUILD_UNI_ALGO "Build the library before testing otherwise build together with tests (manual use)" OFF)
option(TEST_CONSTEXPR "Test in constexpr mode (implies header-only and requeries C++20 or higher)" OFF)
option(TEST_HEADER_ONLY "Test with header-only library" OFF)
option(TEST_SINGLE_INCLUDE "Test with single include library (copy uni_algo_ext.h to test folder)" OFF)
option(TEST_STANDALONE "Test that every file can be compiled as standalone include" OFF)
option(TEST_EXPERIMENTAL "Test experimental stuff" OFF)

if(TEST_CONSTEXPR AND DOWNLOAD_UNICODE_TEST_FILES)
    message(FATAL_ERROR "TEST_CONSTEXPR and DOWNLOAD_UNICODE_TEST_FILES must not be used together")
endif()

if(TEST_SINGLE_INCLUDE AND BUILD_UNI_ALGO)
    message(FATAL_ERROR "TEST_SINGLE_INCLUDE and BUILD_UNI_ALGO must not be used together")
endif()

add_executable(${PROJECT_NAME})

# Use C++17 for old compilers or when it forced with -DCMAKE_CXX_STANDARD=17 otherwise C++20
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.29))
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
elseif(NOT DEFINED CMAKE_CXX_STANDARD)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
endif()

# Compile warnings

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8;/W4;/WX")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        "-Werror"
        "-Wall;-Wextra;-Wpedantic"
        "-Wconversion;-Wsign-conversion;-Wsign-compare"
        "-Wnull-dereference;-Wcast-align;-Wcast-qual"
        "-Wshadow;-Wmissing-braces")

    # Hidden option GCC extra strict (may produce false positives especially with constexpr tests)
    if(TEST_GCC_EXTRA_STRICT AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wstrict-overflow=2;-Warray-bounds=2;-Wstack-usage=3000")
    endif()
endif()

# Increase compile constexpr limit

if(TEST_CONSTEXPR)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE       "/constexpr:steps33554432") # Default is 100000
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE     "-fconstexpr-steps=33554432") # Default is unknown
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE "-fconstexpr-ops-limit=33554432") # Default is 33554432
    endif()
endif()

# Compile definitions

if(NOT DOWNLOAD_UNICODE_TEST_FILES)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_MODE_WITHOUT_UNICODE_TEST_FILES)
endif()

if(TEST_CONSTEXPR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_MODE_CONSTEXPR)
endif()

if(TEST_SINGLE_INCLUDE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE TEST_MODE_SINGLE_INCLUDE)
endif()

if(TEST_EXPERIMENTAL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNI_ALGO_EXPERIMENTAL)
endif()

# NOTE: By default tests compile together with uni-algo library (manual use) without building it first
# but if needed to test with building the library or with header-only mode for example
# then pass to CMake: -DBUILD_UNI_ALGO=ON -DUNI_ALGO_HEADER_ONLY=ON
# For header-only mode -DTEST_HEADER_ONLY can be used instead now.
if(NOT BUILD_UNI_ALGO)
    # Single include is always header-only so skip
    if(NOT TEST_SINGLE_INCLUDE)
        # Manual use must work even without include directory compile option
        #target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../include/uni_algo")
        if(TEST_HEADER_ONLY OR TEST_CONSTEXPR)
            target_compile_definitions(${PROJECT_NAME} PRIVATE UNI_ALGO_STATIC_DATA)
        else()
            target_sources(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../src/data.cpp")
        endif()
    endif()

    # For better visual switch between BUILD_UNI_ALGO=ON/OFF in cmake-gui
    # unset (hide) all uni-algo library options (see main CMakeLists.txt for them)
    unset(BUILD_SHARED_LIBS CACHE)
    unset(UNI_ALGO_HEADER_ONLY CACHE)
    unset(UNI_ALGO_INSTALL CACHE)
else()
    if(TEST_HEADER_ONLY OR TEST_CONSTEXPR)
        set(UNI_ALGO_HEADER_ONLY ON CACHE BOOL "" FORCE)
    else()
        set(UNI_ALGO_HEADER_ONLY OFF CACHE BOOL "" FORCE)
    endif()

    add_subdirectory("${PROJECT_SOURCE_DIR}/.." build_uni_algo)
    target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)
endif()

target_sources(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/main.cpp")

if(DOWNLOAD_UNICODE_TEST_FILES)
    set(UNICODE_TEST_FILES_VERSION "15.1.0")

    # if Unicode test files are not already downloaded
    if(NOT UNICODE_TEST_FILES_VERSION STREQUAL UNICODE_TEST_FILES_CACHE)
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

        message(STATUS "Download Unicode test files version: " ${UNICODE_TEST_FILES_VERSION})

        func_download("https://www.unicode.org/Public/${UNICODE_TEST_FILES_VERSION}/ucd/NormalizationTest.txt")
        func_download("https://www.unicode.org/Public/${UNICODE_TEST_FILES_VERSION}/ucd/auxiliary/GraphemeBreakTest.txt")
        func_download("https://www.unicode.org/Public/${UNICODE_TEST_FILES_VERSION}/ucd/auxiliary/WordBreakTest.txt")

        set(UNICODE_TEST_FILES_CACHE ${UNICODE_TEST_FILES_VERSION} CACHE INTERNAL "")
    endif()
endif() # DOWNLOAD_UNICODE_TEST_FILES

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# On Windows add path with shared library to environment PATH
if(WIN32 AND BUILD_UNI_ALGO AND BUILD_SHARED_LIBS AND NOT UNI_ALGO_HEADER_ONLY)
    set_tests_properties(${PROJECT_NAME} PROPERTIES ENVIRONMENT "PATH=$<TARGET_FILE_DIR:uni-algo::uni-algo>\;%PATH%")
endif()

# Test that every file can be compiled as standalone include
if(TEST_STANDALONE)
    include("${PROJECT_SOURCE_DIR}/standalone/standalone.cmake")
endif()
