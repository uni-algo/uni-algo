# Set default build type to Release
# NOTE: Must be before project call to make it possible to set it to empty (old default)
get_property(GEN_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT GEN_IS_MULTI_CONFIG)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
    # Drop-down list for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ";Debug;Release;MinSizeRel;RelWithDebInfo")
endif()

# REFERENCE:
# https://www.kitware.com/cmake-and-the-default-build-type/
# https://gitlab.kitware.com/cmake/cmake/-/issues/19401#note_589718
# https://gitlab.kitware.com/cmake/cmake/-/issues/17944#note_404504
# https://gitlab.kitware.com/cmake/cmake/-/issues/20921#note_793410
