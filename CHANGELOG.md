# Changelog
Only the most important stuff is listed here.<br>
See commit history between tags for details.

## [v0.7.1](https://github.com/uni-algo/uni-algo/tree/v0.7.1) (27.02.2023)
- Fixed bug when normalization and UTF-16 views are used with input ranges ([#22](https://github.com/uni-algo/uni-algo/issues/22)). Thanks [@tocic](https://github.com/tocic).

## [v0.7.0](https://github.com/uni-algo/uni-algo/tree/v0.7.0) (07.02.2023)
- Renamed main namespace from `uni::` to `una::`
- Splitted .h files with ranges and functions:
  - Added files: `uni_algo/ranges_norm.h` and `uni_algo/ranges_conv.h`
  - Renamed file `uni_algo/break_grapheme.h` to `uni_algo/ranges_grapheme.h`
  - Renamed file `uni_algo/break_word.h` to `uni_algo/ranges_word.h`
- The library is full constexpr now (requeries C++20 or higher and header-only enabled)
- Added safe layer (bounds checks enabled in debug disabled in release)
- In C++23 std::string::resize_and_overwrite is now used for better performance
- Added ASCII extension
- Removed function uni::codepoint::to_decompose_hangul_u32

## [v0.6.0](https://github.com/uni-algo/uni-algo/tree/v0.6.0) (07.11.2022)
- Added uni::is_valid_utf8/16/32 functions for strings
- Added custom allocator support for template functions
- Added custom allocator support for output ranges
- Added header-only support (CMake option UNI_ALGO_HEADER_ONLY=ON)
- Added shared library support (CMake option BUILD_SHARED_LIBS=ON)
- Renamed file uni_algo/convert.h to uni_algo/conv.h
- Lowered requirements for word/grapheme breaks to bidirectional range
- Extensions now use unx namespace instead of uni

## [v0.5.0](https://github.com/uni-algo/uni-algo/tree/v0.5.0) (01.10.2022)
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

## [v0.4.0](https://github.com/uni-algo/uni-algo/tree/v0.4.0) (30.08.2022)
- Added code point basic properties
- Added code point case properties
- Added code point case tranformations
- Added code point normalization properties
- Added code point normalization tranformations
- Added ranges to_utf8_reserve and to_utf16_reserve

## [v0.3.0](https://github.com/uni-algo/uni-algo/tree/v0.3.0) (07.08.2022)
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

## [v0.2.0](https://github.com/uni-algo/uni-algo/tree/v0.2.0) (14.07.2022)
- Updated to Unicode 14.0.0

## [v0.1.0](https://github.com/uni-algo/uni-algo/tree/v0.1.0) (07.07.2022)
- Initial commit
