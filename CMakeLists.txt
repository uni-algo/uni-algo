cmake_minimum_required(VERSION 3.12)

project("uni-algo" VERSION 0.5.0 LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libs" OFF)
option(UNI_ALGO_HEADER_ONLY "Header-only" OFF)

if(NOT UNI_ALGO_HEADER_ONLY)
    add_library(${PROJECT_NAME})
else()
    add_library(${PROJECT_NAME} INTERFACE)
endif()
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(GNUInstallDirs)

if(NOT UNI_ALGO_HEADER_ONLY)
    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)
    endif()

    target_sources(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/src/data.cpp)
else()
    target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

    target_compile_definitions(${PROJECT_NAME} INTERFACE UNI_ALGO_STATIC_DATA)
endif()

if(BUILD_SHARED_LIBS AND NOT UNI_ALGO_HEADER_ONLY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNI_ALGO_DLL_EXPORT INTERFACE UNI_ALGO_DLL_IMPORT)
    # Set hidden visibility for the shared library (doesn't matter on Windows)
    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${PROJECT_NAME} PROPERTIES VISIBILITY_INLINES_HIDDEN ON)
    # Make soname/output name include major.minor version instead of only major
    # because ABI breakage is allowed in minor versions for now.
    if(WIN32)
        set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME
            ${PROJECT_NAME}-v${PROJECT_VERSION_MAJOR}-${PROJECT_VERSION_MINOR})
        # Remove "lib" prefix in MinGW
        #set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" IMPORT_PREFIX "")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
    endif()
endif()

# -------
# INSTALL
# -------

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}-config-version.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMinorVersion)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION
    "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake")

install(EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake")

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake")

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/uni_algo"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.h")
