cmake_minimum_required(VERSION 3.12)

# NOTE: This file is used by GitHub Actions workflows edit with care

project(uni-algo-test LANGUAGES CXX)

add_executable(${PROJECT_NAME})

# Use C++17 for old compilers or when it forced with CMAKE_CXX_STANDARD=17 otherwise C++20
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0) OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.29))
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
elseif(NOT DEFINED CMAKE_CXX_STANDARD)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8;/W4>"
    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:"
        "-Wall;-Wextra;-Wconversion;-Wsign-conversion;-Wsign-compare"
        "-Wshadow;-Wmissing-braces>")

if(NOT DOWNLOAD_UNICODE_TEST_FILES)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "TEST_MODE_WITHOUT_UNICODE_TEST_FILES")
endif()

# NOTE: By default tests compile together with uni-algo library (manual use) without building it first
# but if needed to test with building the library or with header-only mode for example
# then pass to CMake: -DBUILD_UNI_ALGO=ON -DUNI_ALGO_HEADER_ONLY=ON
if(NOT BUILD_UNI_ALGO)
    # Manual use should work even without include directory compile option
    #target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../include/uni_algo")
    target_sources(${PROJECT_NAME} PRIVATE
        "${PROJECT_SOURCE_DIR}/../src/data.cpp"
        "${PROJECT_SOURCE_DIR}/main.cpp")
else()
    add_subdirectory("${PROJECT_SOURCE_DIR}/.." build_uni_algo)
    target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)

    target_sources(${PROJECT_NAME} PRIVATE
        "${PROJECT_SOURCE_DIR}/main.cpp")
endif()

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
