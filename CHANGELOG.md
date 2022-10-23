# Changelog
Only the most important stuff is listed here.<br>
See commit history between tags for details.

## v0.6.0 (TBD)
- Added custom allocator support for template functions and output ranges
- Added uni::is_valid_utf8/16/32 functions for strings
- Added header-only support (CMake option UNI_ALGO_HEADER_ONLY=ON)
- Added shared library support (CMake option BUILD_SHARED_LIBS=ON)

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
