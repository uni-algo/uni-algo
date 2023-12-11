[![Version](https://img.shields.io/badge/Version-1.2.0-blue)](https://github.com/uni-algo/uni-algo/releases/tag/v1.2.0)
[![Unicode](https://img.shields.io/badge/Unicode-15.1.0-blue)](https://www.unicode.org/versions/Unicode15.1.0/)
[![Language](https://img.shields.io/badge/C%2B%2B-17/20/23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/License-Unlicense/MIT-darkorchid.svg)](https://opensource.org/licenses/MIT)
[![Linux](https://github.com/uni-algo/uni-algo/actions/workflows/linux.yml/badge.svg?branch=main)](https://github.com/uni-algo/uni-algo/actions/workflows/linux.yml)
[![Windows](https://github.com/uni-algo/uni-algo/actions/workflows/windows.yml/badge.svg?branch=main)](https://github.com/uni-algo/uni-algo/actions/workflows/windows.yml)
[![macOS](https://github.com/uni-algo/uni-algo/actions/workflows/macos.yml/badge.svg?branch=main)](https://github.com/uni-algo/uni-algo/actions/workflows/macos.yml)

## Table of Contents

- [Introduction](#introduction)
- [Design](#design)
- [Usage](#usage)
- [Examples](#examples)
  - [Conversion Functions](#conversion-functions)
  - [Case Functions](#case-functions)
  - [Normalization Functions](#normalization-functions)
  - [Code Point Properties](#code-point-properties)
  - [Locale](#locale)
  - [Basic Ranges](#basic-ranges)
  - [Conversion Ranges](#conversion-ranges)
  - [Grapheme/Word Ranges](#graphemeword-ranges)
  - [Normalization Ranges](#normalization-ranges)
- [Ranges vs Functions](#ranges-vs-functions)
- [Unicode Algorithms](#unicode-algorithms)
- [Performance](#performance)
- [C Language](#c-language)
- [SQLite Extension](#sqlite-extension)
- [License](#license)

## Introduction

There are plenty of Unicode libraries for C/C++ out there that implement random
Unicode algorithms, but many of them don't handle ill-formed UTF sequences at all.<br>
In the best-case scenario, you'll get an exception/error; in the worst-case, undefined behavior.
The biggest problem is that in 99% cases everything will be fine.
This is inappropriate for security reasons.<br>
This library handles such problems (there are not only ill-formed sequences actually)
properly and always according to The Unicode Standard.

In C/C++, unlike some other programming languages, there is no safe type for UTF-8/UTF-16
that guarantees that the data will be well-formed; this makes the problem even worse.
The library doesn't introduce such a type either because the library doesn't work with
types/strings/files/streams, it works with the data inside them and makes it safe when it's needed.

Check this article if you want more information about ill-formed sequences: https://hsivonen.fi/broken-utf-8<br>
It is a bit outdated because ICU (International Components for Unicode) now uses W3C conformant
implementation too, but the information in the article is very useful.<br>
This library does use W3C conformant implementation too.

## Design

The design of the library follows these principles:<br>
Security. Performance. Usability. Portability.

Security means:
- None of the functions can ever produce an ill-formed UTF-8/16/32 sequence, even if the input was some random bytes;
this also means that the data can never cause an undefined behavior.
- All standards are followed exactly as written,
which also means that all the Unicode algorithms are conformant with The Unicode Standard.
- Test files provided by The Unicode Standard are used with many other tests.
- The library never uses dangerous things like pointer casts etc.
- The library has two levels: a low level of pure algorithms where very few things are allowed,
and a wrapper level of very easy-to-use functions and ranges.
- The library does not use raw pointers or C arrays, and uses safe iterators and arrays with bounds checks through
[the safe layer](https://github.com/uni-algo/uni-algo/blob/main/doc/SAFE_LAYER.md) in debug, with the ability to enable it in release.
- GCC with `-Wall` and many extra warnings is used in the development and Clang-Tidy is used as a static analyzer.

Performance means:
- The library is designed for the performance from the beginning.
- The performance is at least the same as other implementations.
- Most of the optimization techniques that are mentioned in The Unicode Standard are used.
- The library uses low-level optimizations when it's possible.

Usability means:
- The only dependency is the C++ standard library.
- Most of the functions can be used in a single line. See examples below.
- The library doesn't introduce new types for strings it uses: `std::string`, `std::u16string` etc.
- Ranges and views are compatible with the C++ standard library.
- The library doesn't use exceptions or RTTI, so it can be compiled with `-fno-exceptions` and `-fno-rtti`.
- The library itself is header-only, but Unicode data (one .cpp file) must be compiled.
Pure header-only version is also available.
- You can disable modules or even a part of a module by using configuration defines
that can drastically reduce the size of Unicode data that must be compiled.

Portability means:
- The library works even if all available types are 64-bit with `sizeof == 1`
and/or fixed width types are unavailable and/or `CHAR_BIT` is not 8.
- The exact same behavior on all platforms, the library doesn't use `std::locale`
and things related to it because most of that stuff is broken
(see [P2020R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2020r0.pdf)).

## Usage

<details><summary><b>Single include</b></summary><p>

See this repository: https://github.com/uni-algo/uni-algo-single-include

</p></details>

<details><summary><b>vcpkg</b></summary><p>

In vcpkg classic mode you can install the library with `vcpkg install uni-algo` and then handle it the way you prefer.

In vcpkg manifest mode you can add the library to your project's vcpkg.json file as a dependency:

```json
{
  "name": "your-app-name",
  "version": "0.1.0",
  "dependencies": [
    {
      "name": "uni-algo"
    }
  ]
}
```

Then add to your project's CMakeLists.txt:

```cmake
find_package(uni-algo CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)
```

And then build your project with the vcpkg toolchain:

```
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=path_to_vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

For reference: https://learn.microsoft.com/en-us/vcpkg/examples/manifest-mode-cmake

</p></details>

<details><summary><b>CMake add_subdirectory</b></summary><p>

Add to your CMakeLists.txt:

```cmake
# CMake option example (enable header-only version)
# set(UNI_ALGO_HEADER_ONLY ON CACHE INTERNAL "")
# Note that header-only makes the library constexpr

add_subdirectory(repo) # The directory where you extracted this repository

# Config define example (disable normalization)
# target_compile_definitions(uni-algo PUBLIC UNI_ALGO_DISABLE_NORM)
# Note that you can ignore config defines if header-only is enabled
# because Unicode data that is not used won't be compiled anyway.

target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)
```
</p></details>

<details><summary><b>CMake find_package</b></summary><p>

Build and install the library first.

Steps for building and installing in release mode with a single-configuration generator, like the Unix Makefiles one:

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build
```

Steps for building and installing in release mode with a multi-configuration generator, like the Visual Studio ones:

```
cmake -S . -B build
cmake --build build --config Release
cmake --install build --config Release
```

Then add to your CMakeLists.txt:

```cmake
find_package(uni-algo CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)
```
</p></details>

<details><summary><b>CMake FetchContent</b></summary><p>

Add to your CMakeLists.txt:

```cmake
include(FetchContent)

FetchContent_Declare(uni-algo
  URL https://github.com/uni-algo/uni-algo/archive/v1.0.0.tar.gz
  FIND_PACKAGE_ARGS 1.0 CONFIG) # Download only if find_package(uni-algo 1.0 CONFIG)
                                # is unsuccessful (requires CMake 3.24 or higher)

# Or if you have Git installed (but it's always better to use the URL approach above)
# FetchContent_Declare(uni-algo
#   GIT_REPOSITORY https://github.com/uni-algo/uni-algo.git
#   GIT_SHALLOW ON  # Download the branch without its history
#   GIT_TAG v1.0.0) # The version you want to download

# Be aware that FetchContent_MakeAvailable requires CMake 3.14 or higher
FetchContent_MakeAvailable(uni-algo)

target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)
```
</p></details>

<details><summary><b>Manual use</b></summary><p>

Include the header file you want to use from the `include/uni_algo` directory and compile one file `src/data.cpp`.

For the header-only (and constexpr in C++20 or higher) version, add the `UNI_ALGO_STATIC_DATA` define
to your project and ignore the `src/data.cpp` file.

</p></details>

<details><summary><b>Testing</b></summary><p>

Note that test is a CMake project, not a subproject, so you need to do it like this:

Steps for testing in release mode with a single-configuration generator, like the Unix Makefiles one:

```
cmake -S test -B build_test -DCMAKE_BUILD_TYPE=Release -DDOWNLOAD_UNICODE_TEST_FILES=ON
cmake --build build_test
cd build_test
ctest --verbose
```

Steps for testing in release mode with a multi-configuration generator, like the Visual Studio ones:

```
cmake -S test -B build_test -DDOWNLOAD_UNICODE_TEST_FILES=ON
cmake --build build_test --config Release
cd build_test
ctest --verbose --build-config Release
```

For constexpr tests use `-DTEST_CONSTEXPR=ON` without `-DDOWNLOAD_UNICODE_TEST_FILES=ON`,<br>
and the Clang compiler is preferred for better testing because GCC/MSVC are less strict.<br>
For other options see `test/CMakeLists.txt`.
</p></details>

## Examples

For these examples to work, you need to compile them in GCC/Clang with `-std=c++17` (or higher)
or in MSVC with `/utf-8 /std:c++17` (or higher) and a terminal that actually supports UTF-8.<br>
These are the lowest versions the library has been tested on: GCC 7.3, Clang 8.0, MSVS 2017 15.9 (MSVC 19.16).

Note that the terms code point, code unit, grapheme are used in the examples.
You can read about them here:
https://stackoverflow.com/questions/27331819/whats-the-difference-between-a-character-a-code-point-a-glyph-and-a-grapheme

Note that the files are called modules because the word is too good in this case.
It has nothing to do with C++20 modules.

## Conversion Functions
```cpp
// This module doesn't require Unicode data, so it can be used as header-only
#include "uni_algo/conv.h"

// Lenient conversion (cannot fail), "\xD800" is unpaired high surrogate in UTF-16
std::string str8 = una::utf16to8(u"Te\xD800st"); // std::u8string is supported too
assert(str8 == "Te\xEF\xBF\xBDst"); // "\xEF\xBF\xBD" is the replacement character U+FFFD in UTF-8

// Strict conversion
una::error error;
std::string str8 = una::strict::utf16to8(u"Te\xD800st", error);
assert(str8.empty() && error && error.pos() == 2);

// Template functions:

// Lenient conversion (cannot fail), "\xC2" is truncated sequence in UTF-8
std::u16string str16 = una::utf8to16<char, char16_t>("Te\xC2st");
assert(str16 == u"Te\xFFFDst"); // "\xFFFD" is the replacement character U+FFFD in UTF-16

// Strict conversion
una::error error;
std::u16string str16 = una::strict::utf8to16<char, char16_t>("Te\xC2st", error);
assert(str16.empty() && error && error.pos() == 2);

// Note that most of the functions in the library have template versions because the low level
// of the library is type-agnostic; it only requires that the UTF-16 type is enough to store 16-bit
// values and the UTF-32 type is enough to store 32-bit values; aside from that, the low-level doesn't
// care about the types, so with the template functions you can do some strange stuff like this:

std::basic_string<long long> str16 = una::utf8to16<char, long long>("😺");
std::string str8 = una::utf16to8<long long, char>(str16);
assert(str8 == "😺");

// This works perfectly fine; this is allowed and tested. Of course, it is not needed in most
// situations, but it can be used to handle std::wstring and wchar_t properly because the only
// portable way of using std::wstring is to use it only to store UTF-16 internally, and, for example,
// on Linux, convert it to a real UTF-32 std::wstring when it's needed. It can be done like this:

// Works on Linux, static_assert on Windows
std::wstring str32 = una::utf16to32<wchar_t, wchar_t>(L"Test");

// It can be helpful if you work with a library that actually does this
// for example: https://github.com/taglib/taglib
// Aside from such corner cases, you should use non-template functions because they are just shorter.

// It's not like only wchar_t is broken, char has the same problem: you never know what it stores.
// So most of the functions in the library end with utf8/utf16/utf32. These suffixes show what data
// the function works with, the types are irrelevant.

// Note that you need to use template functions if you want to use a custom allocator,
// because non-template functions always use default std::allocator.
```
## Case Functions
```cpp
#include "uni_algo/case.h"

std::cout << una::cases::to_uppercase_utf8("Straße") << '\n';
std::cout << una::cases::to_lowercase_utf8("ДВА") << '\n';
std::cout << una::cases::to_casefold_utf8("Ligature ﬃ") << '\n';
std::cout << una::cases::to_titlecase_utf8("teMPuS eDAX reRuM") << '\n';

// Output:
// STRASSE
// два
// ligature ffi
// Tempus Edax Rerum

// With Greek locale, removes diacritics etc.
std::cout << una::cases::to_uppercase_utf8("Ἀριστοτέλης", una::locale("el")) << '\n';
// With Turkish locale, maps i to İ etc.
std::cout << una::cases::to_uppercase_utf8("istanbul", una::locale("tr")) << '\n';
// With Dutch locale, maps ij to IJ at the start of a word.
std::cout << una::cases::to_titlecase_utf8("ijsland", una::locale("nl")) << '\n';

// Output:
// ΑΡΙΣΤΟΤΕΛΗΣ
// İSTANBUL
// IJsland

assert(una::caseless::compare_utf8("ﬃ", "FFI") == 0);

una::found found = una::caseless::find_utf8("Ligature ﬁ test", "FI");
assert(found && found.pos() == 9 && found.end_pos() == 12);

// The module provides a very simple collation function too.

// Use the Ukrainian alphabet, for example
std::u32string str32 = U"абвгґдеєжзиіїйклмнопрстуфхцчшщьюяАБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯ";

std::vector<std::string> vec8;
for (char32_t c : str32) // Convert the code points to a vector of UTF-8 code units
    vec8.emplace_back(una::utf32to8(std::u32string(1, c)));
std::shuffle(vec8.begin(), vec8.end(), std::mt19937(42)); // Shuffle them just in case

// For example, sort them with the binary comparison first
std::sort(vec8.begin(), vec8.end());

// Output: ЄІЇАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЮЯабвгдежзийклмнопрстуфхцчшщьюяєіїҐґ
// Everything is out of place

// Sort them with una::casesens::utf8_collate
std::sort(vec8.begin(), vec8.end(), [](auto a, auto b) {
    return una::casesens::collate_utf8(a, b) < 0;
});

std::for_each(vec8.begin(), vec8.end(), [](auto s) { std::cout << s; });
std::cout << '\n';

// Output: аАбБвВгГґҐдДеЕєЄжЖзЗиИіІїЇйЙкКлЛмМнНоОпПрРсСтТуУфФхХцЦчЧшШщЩьЬюЮяЯ
// This is the correct order for the Ukrainian alphabet

// Group them too
auto it = std::unique(vec8.begin(), vec8.end(), [](auto a, auto b) {
    return una::caseless::collate_utf8(a, b) == 0;
});
vec8.erase(it, vec8.end());

// Output: абвгґдеєжзиіїйклмнопрстуфхцчшщьюя
```
## Normalization Functions
```cpp
#include "uni_algo/norm.h"

// Normalization Form C (NFC)

// "W" with circumflex == "Ŵ"
assert(una::norm::to_nfc_utf8("W\u0302") == "Ŵ");

assert(una::norm::is_nfc_utf8("Ŵ") == true);
assert(una::norm::is_nfc_utf8("W\u0302") == false);

// Normalization Form D (NFD)

assert(una::norm::to_nfd_utf8("Ŵ") == "W\u0302");

assert(una::norm::is_nfd_utf8("Ŵ") == false);
assert(una::norm::is_nfd_utf8("W\u0302") == true);
```
## Code Point Properties
```cpp
#include "uni_algo/prop.h"

assert(una::codepoint::is_numeric(U'7'));
assert(una::codepoint::is_alphabetic(U'W'));
assert(una::codepoint::prop{U'W'}.Alphabetic()); // Equivalent to the previous one

// Other modules can provide more properties
#include "uni_algo/case.h"

assert(una::codepoint::is_lowercase(U'w'));
assert(una::codepoint::prop_case{U'w'}.Lowercase()); // Equivalent to the previous one

// Code point properties are useful when you want to implement your own Unicode algorithm.
// For example, a function that checks that a UTF-8 string contains only alphabetic characters
// and works properly for all scripts and all Unicode normalization forms looks something like this:
bool is_alphabetic_string_utf8(std::string_view view)
{
    // Note that this algorithm uses UTF-8 range view read below about this
    for (char32_t c : view | una::views::utf8)
    {
        if (c == 0x200D || c == 0x200C) // Ignore ZERO WIDTH JOINER/NON-JOINER
            continue;

        auto prop = una::codepoint::prop{c};

        if (prop.General_Category_M()) // Ignore combining marks
            continue;

        if (!prop.Alphabetic())
            return false;
    }
    return true;
}
// Note that degenerate cases when a string starts with combining marks or contains
// only combining marks are not handled, so the real function will be a bit more complex.
```
## Locale
```cpp
#include "uni_algo/locale.h"
#include "uni_algo/case.h"

// Uppercase a string using system locale
una::cases::to_uppercase_utf8("Test", una::locale::system());

una::cases::to_uppercase_utf8("Test", una::locale{"en-US"}); // Using locale string
una::cases::to_uppercase_utf8("Test", una::locale::language{"en"}); // Using language subtag directly

// Parse locale string (can be ill-formed, the parser will normalize it)
una::locale locale{"EN_latn_US"};

assert(locale.is_empty() == false);
assert(locale == una::locale::language{"en"});
assert(locale == una::locale::script{"Latn"});
assert(locale == una::locale::region{"US"});
assert(locale.to_string() == "en-Latn-US");

// Use locale subtags in switch case
switch (una::locale::system().get_language())
{
    case una::locale::language{"en"}:
        std::cout << "English Language" << '\n';
        break;
    case una::locale::language{"ja"}:
        std::cout << "Japanese Language" << '\n';
        break;
    case una::locale::language{"ar"}:
        std::cout << "Arabic Language" << '\n';
        break;
}

// Note that system locale functions are not available in header-only and single include versions.
```
## Basic Ranges
```cpp
// These modules don't require Unicode data, so they can be used as header-only
#include "uni_algo/ranges.h"
#include "uni_algo/ranges_conv.h"

// Ranges in this library are compatible with C++20 ranges.
// In C++17, the library implements some basic ranges that are similar to C++20 ranges:
una::views::reverse
una::views::transform
una::views::filter
una::views::drop
una::views::take

// In C++20, you can just use std::ranges variants of these ranges,
// but always use una::views::reverse from this library
// because std::views::reverse is not good enought for Unicode.

// You use these ranges the same way you use C++20 ranges
assert((std::string_view{"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ"}
        | una::views::utf8
        | una::views::reverse | una::views::drop(3) | una::views::take(5)
        | una::views::reverse | una::ranges::to_utf8<std::string>())
        == "ⅢⅣⅤⅥⅦ");
```
## Conversion Ranges
```cpp
// This module doesn't require Unicode data, so it can be used as header-only
#include "uni_algo/ranges_conv.h"

// A simple conversion function using ranges looks like this:
std::u16string utf8to16(std::string_view view)
{
    return view | una::views::utf8 | una::ranges::to_utf16<std::u16string>();
}

// The following examples demonstrate how to work directly with a conversion range view

// 3 UTF-8 code points
std::string str8 = "😺😼🙀"; // These emojis use 1 code point

// Make UTF-8 view from std::string
auto view = una::ranges::utf8_view{str8};

// Count code points
std::cout << "Number of code points: " << std::distance(view.begin(), view.end()) << '\n';

// Print code points
for (auto it = view.begin(); it != view.end(); ++it)
{
    std::cout << "Code point: " << std::to_string(*it)
              << " at: " << it.begin() - str8.begin() << '\n';
}

// Skip 2 code points
if (auto it = std::next(view.begin(), 2); it != view.end())
    std::cout << "3rd code point at: " << it.begin() - str8.begin() << '\n';

// Find code point 128572
if (auto it = std::find(view.begin(), view.end(), 128572); it != view.end())
    std::cout << "Code point 128572 at: " << it.begin() - str8.begin() << '\n';

std::cout << "Reverse order:" << '\n';
for (auto it = view.end(); it != view.begin();)
{
    --it;
    std::cout << "Code point: " << std::to_string(*it)
              << " at: " << it.begin() - str8.begin() << '\n';
}

// Output:
// Number of code points: 3
// Code point: 128570 at: 0
// Code point: 128572 at: 4
// Code point: 128576 at: 8
// 3rd code point at: 8
// Code point 128572 at: 4
// Reverse order:
// Code point: 128576 at: 8
// Code point: 128572 at: 4
// Code point: 128570 at: 0
```
## Grapheme/Word Ranges
```cpp
#include "uni_algo/ranges_grapheme.h"
#include "uni_algo/ranges_word.h"

// Grapheme/Word aka text segmentation ranges are similar to conversion ranges,
// but they return subranges in the form of std::string_view.
// They use default grapheme/word boundary rules from The Unicode Standard UAX #29

std::string str8 = "Άλμπερτ Αϊνστάιν";

// Replace it with una::views::grapheme::utf8 for grapheme segmentation
auto view = una::views::word::utf8(str8);

// Count words/graphemes (note that a space/punctuation is a word too according to the rules)
std::cout << "Number of words: " << std::distance(view.begin(), view.end()) << '\n';

// Print positions of the words/graphemes
for (auto it = view.begin(); it != view.end(); ++it)
    std::cout << "Word at: " << it.begin() - str8.begin() << '\n';

// Word/grapheme tokenizer
std::cout << "Words:" << '\n';
// Note that we use the word_only view here to skip all punctuation and spaces
for (std::string_view word : una::views::word_only::utf8(str8))
    std::cout << word << '\n';

// Output:
// Number of words: 3
// Word at: 0
// Word at: 14
// Word at: 15
// Words:
// Άλμπερτ
// Αϊνστάιν

// For example, you need to append or insert a code point or grapheme to a string

// To append a code point to a UTF-8 string, all you need is the Conversion module
std::string str8 = "🏴󠁧󠁢󠁷󠁬󠁳󠁿👨‍👩‍👧🧙‍♀️"; // These emojis use multiple code points
str8 += una::utf32to8(std::u32string{U'😺'}); // This emoji use 1 code point

// But if you need to insert a code point, you need text segmentation ranges too
auto view = una::views::grapheme::utf8(str8);
// Insert a code point after the 2nd grapheme
auto pos = std::next(view.begin(), 2).begin() - str8.begin();
str8.insert(pos, una::utf32to8(std::u32string{U'😼'})); // This emoji uses 1 code point

// Output: 🏴󠁧󠁢󠁷󠁬󠁳󠁿👨‍👩‍👧😼🧙‍♀️😺

// The same way a grapheme can be appended or inserted, and of course, you don't even need
// the Conversion module if your grapheme or code point is already in UTF-8 encoding.
```
## Normalization Ranges
```cpp
// For example, we need to remove accents that mark the stressed vowels in a text.
// The algorithm is simple: NFD -> Remove grave and acute accents (U+0300 and U+0301) -> NFC
// Note that the library has the una::norm::to_unaccent_utf8 function, but it won't produce
// the same result because it removes all accents, not specific ones,
// but it is possible to implement this algorithm using ranges like this.

#include "uni_algo/ranges.h"
#include "uni_algo/ranges_conv.h"
#include "uni_algo/ranges_norm.h"

std::string remove_grave_and_acute_accents(std::string_view str_view)
{
    // Use ranges: UTF-8 -> NFD -> Filter -> NFC -> To UTF-8

    // Note that in C++20 una::views::filter can be replaced with std::views::filter

    return str_view
         | una::views::utf8
         | una::views::norm::nfd
         | una::views::filter([](char32_t c) { return c != 0x0300 && c != 0x0301; })
         | una::views::norm::nfc
         | una::ranges::to_utf8<std::string>();
}

std::cout << remove_grave_and_acute_accents("bótte bòtte") << '\n';
// Output: botte botte
```
## Ranges vs Functions

What is the difference between ranges and functions?

Functions are faster because they are implemented directly at the low level without using
ranges, so they use many optimizations that can be implemented only at the low level.
Ranges, on the other hand, are much more flexible than functions: a function can do one job
in one pass, but ranges can do many jobs in one pass.<br>
This means that for a complex algorithm ranges can outperform functions
because everything will be done in one pass. For example, to normalize a UTF-8 string and
convert it to UTF-16, you need to use 2 functions requiring 2 passes,
but using ranges it can be done in one pass.<br>
Also, this makes ranges especially useful for streams where you have only one pass for everything.

## Unicode Algorithms

**Unicode algorithms that the library does implement:**

The Unicode Standard 3.4 Table 3-1. Named Unicode Algorithms:
- Default Case Conversion (locale-independent and locale-dependent)
- Default Case Folding
- Default Caseless Matching
- Character Segmentation (UAX #29)
- Word Segmentation (UAX #29)
- Normalization* (UAX #15)

And algorithms that are a part of normalization:
- Canonical Ordering
- Canonical Composition
- Hangul Syllable Composition
- Hangul Syllable Decomposition

*The library implements the normalization algorithm in a stream-friendly way,
so it can be used with streams and without memory allocations in all cases.<br>
For more info see: https://www.unicode.org/reports/tr15/#Stream_Safe_Text_Format

**Other algorithms:**

The library implements a very simple collation algorithm
that uses DUCET (Default Unicode Collation Element Table)
to sort code points in the correct order instead of sorting them
in code point order as a simple comparison function does.
Case insensitive version of the algorithm is based on Default Caseless Matching.

And a very simple search algorithm that is based on Default Caseless Matching.

**Unicode algorithms that the library does not implement:**

Unicode Collation Algorithm (UCA) (UTS #10)<br>
A very complex algorithm to sort strings in many different ways with many different rules.

Bidirectional Algorithm (UBA) (UAX #9)<br>
Specifications for the positioning of characters from right to left, such as Arabic or Hebrew.

Line Breaking Algorithm (UAX #14)<br>
Line breaking, also known as word wrapping, is the process of breaking a section of text into lines.

These algorithms are planned for future versions of the library. UCA is long-term plan.

## Performance

ICU (International Components for Unicode) and some functions from WinAPI are used
for the performance comparison because these implementations are highly optimized.<br>
See `perf/result` folder.

## C Language

Low-level functions can be compiled in C mode.
This means you can compile them using any C99 compiler, even Tiny C Compiler will work.
The library doesn't provide an "official" C wrapper because there are many ways to implement
it in C depending on which allocator you are using, which strings etc.<br>
A sample C wrapper that uses malloc and basic types can be found in the examples folder.<br>
You are on your own if you want to use the low-level functions directly.

## SQLite Extension

Undocumented for now.<br>
The SQLite extension is implemented by using the low level directly, so it always uses the SQLite allocator.<br>
See the sqlite folder.

## License

**Public Domain or MIT** - choose whatever you want.

**Public Domain License**

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**MIT License**

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
