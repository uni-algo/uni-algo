# Changelog
Only the most important stuff is listed here.<br>
See commit history between tags for details.

## [v1.2.0](https://github.com/uni-algo/uni-algo/tree/v1.2.0) (2023-12-11)
- Fixed lifetime issue with ranges in some cases (ABI change) ([#35](https://github.com/uni-algo/uni-algo/pull/35)). Thanks [@unixod](https://github.com/unixod).
- Fixed compilation with old versions of Clang compiler ([#34](https://github.com/uni-algo/uni-algo/pull/34)). Thanks [@psiha](https://github.com/psiha).

## [v1.1.0](https://github.com/uni-algo/uni-algo/tree/v1.1.0) (2023-09-12)
- Updated to Unicode 15.1.0

## [v1.0.0](https://github.com/uni-algo/uni-algo/tree/v1.0.0) (2023-07-07)
- Stabilized API ([doc/API_API_STABILITY.md](doc/API_API_STABILITY.md))
- Added more documentation (see `doc` directory)
- Added support for `una::error` to normalization detection functions
- Reduced the size of normalization Unicode data by 80 KB
- Removed normalization functions that work with iterators (they are experimental for now)
- Removed config define `UNI_ALGO_DISABLE_FULL_CASE` (the define is still available for testing purposes but not supported anymore)

## [v0.8.2](https://github.com/uni-algo/uni-algo/tree/v0.8.2) (2023-06-21)
- Fixed normalization detection functions ([#28](https://github.com/uni-algo/uni-algo/issues/28))

## [v0.8.1](https://github.com/uni-algo/uni-algo/tree/v0.8.1) (2023-05-15)
- Fixed constexpr UTF conversion in Clang ([#27](https://github.com/uni-algo/uni-algo/issues/27))

## [v0.8.0](https://github.com/uni-algo/uni-algo/tree/v0.8.0) (2023-05-07)
- Added scripts and script extensions support ([UAX #24: Unicode Script Property](https://www.unicode.org/reports/tr24))
- Added cursor support for text segmentation ([doc/CURSOR.md](doc/CURSOR.md))
- Optimized UTF conversion of ASCII strings
- Changed versions in namespace `una::version` to classes ([#18](https://github.com/uni-algo/uni-algo/issues/18))
- File `uni_algo/version.h` is not included by other files anymore
- Improved `una::error` class added `una::error::code`
- Renamed `search` functions to `find` ([#23](https://github.com/uni-algo/uni-algo/issues/23))
- Renamed `una::search` class to `una::found` ([#23](https://github.com/uni-algo/uni-algo/issues/23))
- Renamed config defines:
  - `UNI_ALGO_DISABLE_SHRINK_TO_FIT` to `UNI_ALGO_NO_SHRINK_TO_FIT`
  - `UNI_ALGO_DISABLE_BREAK_GRAPHEME` to `UNI_ALGO_DISABLE_SEGMENT_GRAPHEME`
  - `UNI_ALGO_DISABLE_BREAK_WORD` to `UNI_ALGO_DISABLE_SEGMENT_WORD`

## [v0.7.1](https://github.com/uni-algo/uni-algo/tree/v0.7.1) (2023-02-27)
- Fixed bug when normalization and UTF-16 views are used with input ranges ([#22](https://github.com/uni-algo/uni-algo/issues/22)). Thanks [@tocic](https://github.com/tocic).

## [v0.7.0](https://github.com/uni-algo/uni-algo/tree/v0.7.0) (2023-02-07)
- Renamed main namespace from `uni::` to `una::`
- Splitted .h files with ranges and functions: ([#15](https://github.com/uni-algo/uni-algo/issues/15))
  - Added files: `uni_algo/ranges_norm.h` and `uni_algo/ranges_conv.h`
  - Renamed file `uni_algo/break_grapheme.h` to `uni_algo/ranges_grapheme.h`
  - Renamed file `uni_algo/break_word.h` to `uni_algo/ranges_word.h`
- The library is full constexpr now (requeries C++20 or higher and header-only enabled)
- Added safe layer (bounds checks enabled in debug disabled in release)
- In C++23 std::string::resize_and_overwrite is now used for better performance
- Added ASCII extension
- Removed function uni::codepoint::to_decompose_hangul_u32

## [v0.6.0](https://github.com/uni-algo/uni-algo/tree/v0.6.0) (2022-11-07)
- Added uni::is_valid_utf8/16/32 functions for strings
- Added custom allocator support for template functions
- Added custom allocator support for output ranges
- Added header-only support (CMake option UNI_ALGO_HEADER_ONLY=ON)
- Added shared library support (CMake option BUILD_SHARED_LIBS=ON)
- Renamed file uni_algo/convert.h to uni_algo/conv.h
- Lowered requirements for word/grapheme breaks to bidirectional range
- Extensions now use unx namespace instead of uni

## [v0.5.0](https://github.com/uni-algo/uni-algo/tree/v0.5.0) (2022-10-01)
- Updated to Unicode 15.0.0
- Added single include version
- Renamed files from cpp_uni_* to uni_algo/*
- Renamed most functions, utf8/utf16 now at the end of a function
- Added better locale support
- Added word properties
- Added reverse grapheme/word algorithms
- Added alternative syntax for code point general category
- Removed function uni::codepoint::is_same_category
- Redesigned transliterators more than two times faster now

## [v0.4.0](https://github.com/uni-algo/uni-algo/tree/v0.4.0) (2022-08-30)
- Added code point basic properties
- Added code point case properties
- Added code point case tranformations
- Added code point normalization properties
- Added code point normalization tranformations
- Added ranges to_utf8_reserve and to_utf16_reserve

## [v0.3.0](https://github.com/uni-algo/uni-algo/tree/v0.3.0) (2022-08-07)
- Added support for char8_t strings
- Switched from iterators to ranges
- Switched to ranges in transliterators
- Added utf8, utf16, basic views
- Added to_utf8, to_utf16 ranges
- Added normalization views
- Added break: grapheme, word, word_only views
- Added internal translit view
- Removed utf8, utf16, output, translit, func iterators
- Removed normalization iterators
- Removed break: grapheme, word iterators
- Renamed function is_pos() to has_pos() in class error

## [v0.2.0](https://github.com/uni-algo/uni-algo/tree/v0.2.0) (2022-07-14)
- Updated to Unicode 14.0.0

## [v0.1.0](https://github.com/uni-algo/uni-algo/tree/v0.1.0) (2022-07-07)
- Initial commit
