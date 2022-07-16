[![Linux](https://github.com/uni-algo/uni-algo/workflows/Linux/badge.svg)](https://github.com/uni-algo/uni-algo/actions?query=workflow%3ALinux)
[![Windows](https://github.com/uni-algo/uni-algo/workflows/Windows/badge.svg)](https://github.com/uni-algo/uni-algo/actions?query=workflow%3AWindows)
[![macOS](https://github.com/uni-algo/uni-algo/workflows/macOS/badge.svg)](https://github.com/uni-algo/uni-algo/actions?query=workflow%3AmacOS)

## Table of Contents

- [Introduction](#introduction)
- [Design](#design)
- [Usage](#usage)
- [Examples](#examples)
  - [Convert Module](#convert-module)
  - [Case Module](#case-module)
  - [Iterator Module](#iterator-module)
  - [Break Module](#break-module)
  - [Normalization Module](#normalization-module)
  - [Iterator Chains](#iterator-chains)
- [Unicode Algorithms](#unicode-algorithms)
- [Performance](#performance)
- [C Language](#c-language)
- [SQLite Extension](#sqlite-extension)
- [License](#license)

## Introduction

There are plenty of Unicode libraries for C/C++ out there that implement random
Unicode algorithms but many of them doesn't handle ill-formed UTF sequences at all.<br />
In the best case scenario you'll get an exception/error in the worst undefined behavior.<br />
The biggest problem that in 99% cases everything will be fine.
This is inappropriate for security reasons.<br />
This library handles such problems (there are not only ill-formed sequences actually)
properly and always according to Unicode Standard.

In C/C++ there isn't exist a safe type for UTF-8/UTF-16 that guarantees that a data will be
well-formed like in some other programming languages this makes the problem even worse.
The library doesn't introduce such type either because the library doesn't work with
types/strings/files/streams it works with the data inside them and makes it safe when it needed.

Check this article if you want more information about ill-formed sequences: https://hsivonen.fi/broken-utf-8<br />
It is a bit outdated because ICU (International Components for Unicode) now uses W3C conformant
implementation too but the information in the article is very useful.<br />
This library does use W3C conformant implementation too.

## Design

The design of the library follows these principles:<br />
Security. Performance. Usability. Portability.

Security means:
- None of the functions can ever produce ill-formed UTF-8/16/32 sequence even if an input was some random bytes,
this also means that a data can never cause an undefined behavior.
- All standards are followed exactly as written,
this also means all Unicode algorithms are conformant with Unicode Standard.
- Test files provided by Unicode are used with many other tests.
- The library never uses dangerous things like pointer casts etc.
- The library uses two levels: the low-level where very few things are allowed the level is pure algorithms
and the wrapper-level where template into template into a very easy to use function or iterator.
- GCC with -Wall and many extra warnings is used in the development and Clang-Tidy is used as a static analyzer.

Performance means:
- The library is designed for the performance from the start.
- The performance is at least the same as other implementations.
- Most of optimization techniques that are mentioned in Unicode Standard are used.
- The library uses low-level optimizations when it's possible.

Usability means:
- Most functions can be used in a single line. See examples below.
- The library doesn't introduce new types for strings it uses std::string, std::u16string etc.
- Iterators are compatible with C++ Standard Library algorithms.
- The library doesn't use exceptions because there are no exceptional situations in
Unicode algorithms. Unicode Standard always describes what need to be done if an issue occurs.
Of course C++ Standard Library still can throw if -fno-exceptions isn't used.
- The only dependency is C++ Standard Library.
- The library itself is header-only but Unicode data (one .cpp file) must be compiled.
- You can disable modules or even a part of a module by using configuration defines
that can drastically reduce the size of Unicode data that must be compiled.

Portability means:
- The library works even if all available types are 64-bit with sizeof == 1
and/or fixed width types are unavailable and/or CHAR_BIT is not 8.<br />
Note that this is partially tested so if you find any issues please report them.

## Usage

<details><summary><b>CMake add_subdirectory</b></summary><p>

Add to your CMakeLists.txt

```cmake
add_subdirectory(lib) # the directory where you extracted this repository

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

Then add to your CMakeLists.txt

```cmake
find_package(uni-algo)

target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)
```
</p></details>

<details><summary><b>CMake FetchContent</b></summary><p>

Add to your CMakeLists.txt

```cmake
include(FetchContent)

# Note that you need Git installed for this to work.
FetchContent_Declare(uni-algo
  GIT_REPOSITORY https://github.com/uni-algo/uni-algo.git
  GIT_TAG v0.2.0) # the version you want to download

# Or you can use it like this without Git.
# FetchContent_Declare(uni-algo
#   URL https://github.com/uni-algo/uni-algo/archive/refs/tags/v0.2.0.zip)

FetchContent_MakeAvailable(uni-algo)

target_link_libraries(${PROJECT_NAME} PRIVATE uni-algo::uni-algo)
```
</p></details>

<details><summary><b>Manual usage</b></summary><p>

Include a header file you want to use from src directory and compile one file `src/cpp_uni_data.cpp`

</p></details>

<details><summary><b>Test</b></summary><p>

Note that test is a CMake project not a subproject so you need to do it like this:

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
</p></details>

## Examples

For these examples to work you need to compile them in GCC/Clang with -std=c++17 (or higher)
or in MSVC with /utf-8 /std:c++17 (or higher) and a terminal that actually supports UTF-8.<br />
These are the lowest versions the library tested: GCC 7.3, Clang 8.0.1, MSVS 2017 15.9 (MSVC 19.16)

Note that terms: code point, code unit, grapheme are used in the examples.
You can read about them here:
https://stackoverflow.com/questions/27331819/whats-the-difference-between-a-character-a-code-point-a-glyph-and-a-grapheme

Note that the files are called modules because the word is too good in this case.
It has nothing to do with C++20 modules.

## Convert module
```cpp
// The module doesn't require Unicode data so it can be used as header-only
#include "cpp_uni_convert.h"

// Lenient conversion (cannot fail) "\xD800" is unpaired high surrogate in UTF-16
std::string str8 = uni::utf16to8(u"Te\xD800st");
assert(str8 == "Te\xEF\xBF\xBDst"); // "\xEF\xBF\xBD" is replacement character U+FFFD in UTF-8

// Strict conversion
uni::error error;
std::string str8 = uni::strict::utf16to8(u"Te\xD800st", error);
assert(str8.empty() && error && error.pos() == 2);

// Template functions:

// Lenient conversion (cannot fail) "\xC2" is truncated sequence in UTF-8
std::u16string str16 = uni::utf8to16<char, char16_t>("Te\xC2st");
assert(str16 == u"Te\xFFFDst"); // "\xFFFD" is replacement character U+FFFD in UTF-16

// Strict conversion
uni::error error;
std::u16string str16 = uni::strict::utf8to16<char, char16_t>("Te\xC2st", error);
assert(str16.empty() && error && error.pos() == 2);

// Note that most of the functions in the library have template versions because the low-level
// of the library is type-agnostic it only requires that UTF-16 type is enough to store 16-bit values
// and UTF-32 is enough to store 32-bit aside from that the low-level doesn't care about the types,
// so with the template functions you can do some strange stuff like this:

std::basic_string<long long> str16 = uni::utf8to16<char, long long>("😺");
std::string str8 = uni::utf16to8<long long, char>(str16);
assert(str8 == "😺");

// This works perfectly fine this is allowed and tested. Of course it is not needed in most
// situations but it can be used to handle std::wstring and wchar_t properly because the only
// portable way of using std::wstring is to use it only to store UTF-16 internally and for example
// on Linux convert it to the real UTF-32 std::wstring when it's needed. It can be done like this:

// Works on Linux, static_assert on Windows
std::wstring str32 = uni::utf16to32<wchar_t, wchar_t>(L"Test");

// It can be helpful if you work with a library that actually does this
// for example: https://github.com/taglib/taglib
// Aside from such corner cases you should use non-template functions because they are just shorter.

// It's not like only wchar_t is broken char has the same problem you never know what it stores
// so most of functions in the library start with utf8/utf16/utf32 it shows with what data
// a function works with, types are irrelevant.

// Note that there are no short functions for std::u8string at the moment
// but template functions will work perfectly fine with it.
```
## Case module
```cpp
#include "cpp_uni_case.h" // and compile "cpp_uni_data.cpp"

std::cout << uni::cases::utf8_upper("Straße") << '\n';
std::cout << uni::cases::utf8_lower("ДВА") << '\n';
std::cout << uni::cases::utf8_fold("Ligature ﬃ") << '\n';
std::cout << uni::cases::utf8_title("word WoRd wOrD") << '\n';

// Output:
// STRASSE
// два
// ligature ffi
// Word Word Word

// With Greek locale, removes diacritics etc.
std::cout << uni::cases::utf8_upper("Ἀριστοτέλης", uni::locale("el")) << '\n';
// With Turkish locale, maps i to İ etc.
std::cout << uni::cases::utf8_upper("istanbul", uni::locale("tr")) << '\n';
// With Dutch locale, maps ij to IJ at the start of a word
std::cout << uni::cases::utf8_title("ijsland", uni::locale("nl")) << '\n';

// Output:
// ΑΡΙΣΤΟΤΕΛΗΣ
// İSTANBUL
// IJsland

assert(uni::caseless::utf8_compare("ﬃ", "FFI") == 0);

uni::search found = uni::caseless::utf8_search("Ligature ﬁ test", "FI");
assert(found && found.pos() == 9 && found.end_pos() == 12);

// The module provides a very simple collation function too

// Use the Makedonian alphabet for example
std::u32string str32 = U"абвгдѓежзѕијклљмнњопрстќуфхцчџшАБВГДЃЕЖЗЅИЈКЛЉМНЊОПРСТЌУФХЦЧЏШ";

std::vector<std::string> vec8;
for (char32_t c : str32) // Convert the code points to a vector of UTF-8 code units
    vec8.emplace_back(uni::utf32to8(std::u32string(1, c)));
std::shuffle(vec8.begin(), vec8.end(), std::mt19937(42)); // Shuffle them just in case

// For example sort them with the binary comparison first
std::sort(vec8.begin(), vec8.end());

// Output: ЃЅЈЉЊЌЏАБВГДЕЖЗИКЛМНОПРСТУФХЦЧШабвгдежзиклмнопрстуфхцчшѓѕјљњќџ
// Everything is out of place

// Sort them with uni::casesens::utf8_collate
std::sort(vec8.begin(), vec8.end(), [](auto a, auto b) {
    return uni::casesens::utf8_collate(a, b) < 0;
});

std::for_each(vec8.begin(), vec8.end(), [](auto s) { std::cout << s; });
std::cout << '\n';

// Output: аАбБвВгГдДѓЃеЕжЖзЗѕЅиИјЈкКлЛљЉмМнНњЊоОпПрРсСтТќЌуУфФхХцЦчЧџЏшШ
// This is the correct order for the Makedonian alphabet

// Group them too
auto it = std::unique(vec8.begin(), vec8.end(), [](auto a, auto b) {
    return uni::caseless::utf8_collate(a, b) == 0;
});
vec8.erase(it, vec8.end());

// Output: абвгдѓежзѕијклљмнњопрстќуфхцчџш
```
## Iterator module
```cpp
// Be aware that this module will be redesigned in favour of std::ranges

// The module doesn't require Unicode data so it can be used as header-only
#include "cpp_uni_iterator.h"

// 3 UTF-8 code points, these emojis use 1 code point
std::string str8 = "😺😼🙀";

// Make UTF-8 iterator adaptors from std::string iterators
uni::iter::utf8 begin{str8.cbegin(), str8.cend()};
uni::iter::utf8 end{str8.cend(), str8.cend()};

// Count code points
std::cout << "Number of code points: " << std::distance(begin, end) << '\n';

// Print code points
for (auto it = begin; it != end; ++it)
    std::cout << "Code point: " << std::to_string(*it) << " at: " << it - begin << '\n';

// Skip 2 code points
if (auto it = std::next(begin, 2); it != end)
    std::cout << "3rd code point at: " << it - begin << '\n';

// Find code point 128572
if (auto it = std::find(begin, end, 128572); it != end)
    std::cout << "Code point 128572 at: " << it - begin << '\n';

std::cout << "Reverse order:" << '\n';
{
    uni::iter::utf8 begin{str8.cbegin(), str8.cend(), str8.cend()};
    uni::iter::utf8 end{str8.cbegin(), str8.cbegin()};

    for (auto it = begin; it != end;)
    {
        --it;
        std::cout << "Code point: " << std::to_string(*it) << " at: " << it - end << '\n';
    }
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
## Break module
```cpp
// Be aware that this module will be redesigned in favour of std::ranges

#include "cpp_uni_break.h" // and compile "cpp_uni_data.cpp"

// Breaks are similar to iterators but they don't return anything
// so operator* isn't defined and operator- must be used to determine a break point.
// Breaks use default grapheme/word boundary rules as described in Unicode Standard UAX #29

std::string str8 = "Алберт Ајнштајн";

// Replace it with uni::breaks::grapheme::utf8 for grapheme breaks
uni::breaks::word::utf8 begin{str8.cbegin(), str8.cend()};
uni::breaks::word::utf8 end{str8.cend(), str8.cend()};

// Count words/graphemes (note that a space/punctuation is a word too according to rules)
std::cout << "Number of words: " << std::distance(begin, end) << '\n';

// Print positions of the words/graphemes
for (auto it = begin; it != end; ++it)
    std::cout << "Word at: " << it - begin << '\n';

// Word/grapheme tokenizer
std::cout << "Words:" << '\n';
for (auto it = begin, prev = it; it != end; prev = it)
{
    ++it;
    // If you use word breaks you can use this helper function to determine that there is
    // an actual word and not a space or something on the left side of the boundary.
    if (it.word_on_left())
        std::cout << str8.substr(prev - begin, it - prev) << '\n';
}

// Output:
// Number of words: 3
// Word at: 0
// Word at: 14
// Word at: 15
// Words:
// Алберт
// Ајнштајн

// For example you need to append or insert a code point or grapheme to a string.

// For append a code point to an UTF-8 string all you need is Convert module.
std::string str8 = "🏴󠁧󠁢󠁷󠁬󠁳󠁿👨‍👩‍👧🧙‍♀️"; // These emojis use multiple code points
str8 += uni::utf32to8(std::u32string{U'😺'}); // This emoji use 1 code point

// But if you need to insert a code point you need Break module too.
uni::breaks::grapheme::utf8 it{str8.cbegin(), str8.cend()};
// Insert a code point after 2nd grapheme. This emoji use 1 code point.
str8.insert(std::next(it, 2) - it, uni::utf32to8(std::u32string{U'😼'}));

// The same way a grapheme can be appended or inserted and of course you don't even need
// Convert module if your grapheme or code point are already in UTF-8 encoding.

// Output: 🏴󠁧󠁢󠁷󠁬󠁳󠁿👨‍👩‍👧😼🧙‍♀️😺
```
## Normalization module
```cpp
#include "cpp_uni_norm.h" // and compile "cpp_uni_data.cpp"

// "Z" with acute == "Ź"
assert(uni::norm::utf8_nfc("\x5A\xCC\x81") == "\xC5\xB9");

assert(uni::norm::utf8_is_nfc("\x5A\xCC\x81") == false);
assert(uni::norm::utf8_is_nfc("\xC5\xB9") == true);

// Note that the normalization algorithm in the library supports streams
// so you can use the same function to normalize a file with 0 allocations

std::ifstream input{"random_file.txt", std::ios::binary};
std::ofstream output{"random_file_normalized.txt", std::ios::binary};

std::istreambuf_iterator<char> it{input.rdbuf()};
std::istreambuf_iterator<char> end;

std::ostreambuf_iterator<char> out{output.rdbuf()};

uni::norm::utf8_nfc(it, end, out);

// The function always produces well-formed normalized UTF-8 text in Stream-Safe Text Format
// even if an input was a binary file, of course in such degenerate case the output will be
// some random UTF-8 code points with a bunch of U+FFFD (replacement characters).
// It just means it is safe to use the function with everything.

// Note that the same result can be achieved with iterator chains.
```
## Iterator chains
```cpp
// Be aware that this will be redesigned in favour of std::ranges

// For example we need to remove accents that mark the stressed vowels in a text.
// The algorithm is simple: NFD -> Remove grave and acute accents (U+0300 and U+0301) -> NFC
// Note that the library has uni::norm::utf8_unaccent function but it won't produce
// the same result because it removes all accents not specific ones,
// but it is possible to implement this algorithm using multiple iterators in a chain like this.

#include "cpp_uni_iterator.h"
#include "cpp_uni_norm.h"
// Note that we include Normalization module because some modules
// provide not only functions but iterators too

std::string remove_grave_and_acute_accents(std::string_view str)
{
    // Chain iterators: UTF-8 -> NFD -> Filter -> NFC -> UTF-8 Output
    // Note that sentinels are used here instead of end iterators it makes the code cleaner

    uni::iter::utf8 begin{str.cbegin(), str.cend()};

    uni::iter::norm::nfd nfd_begin{begin, uni::sentinel};

    struct func { bool operator()(char32_t c) { return c != 0x0300 && c != 0x0301; } };
    uni::iter::func::filter func_begin{func{}, nfd_begin, uni::sentinel};

    // Or you can just use a lambda function:
    //auto func = [](char32_t c) { return c != 0x0300 && c != 0x0301; };
    //uni::iter::func::filter func_begin{func, nfd_begin, uni::sentinel};

    uni::iter::norm::nfc nfc_begin{func_begin, uni::sentinel};

    std::string result;
    uni::iter::output::utf8 out{std::back_inserter(result)};

    for (auto it = nfc_begin; it != uni::sentinel; ++it)
        out = *it;

    return result;

    // This syntax is planned in the future
    // It will be compatible with C++20 std::ranges::views
    // so uni::views::filter can be replaced with std::views::filter
    std::string result = str
         | uni::views::utf8
         | uni::views::norm::nfd
         | uni::views::filter([](char32_t c) { return c != 0x0300 && c != 0x0301; })
         | uni::views::norm::nfc
         | uni::ranges::to_utf8<std::string>();
}

std::cout << remove_grave_and_acute_accents("bótte bòtte") << '\n';
// Output: botte botte
```
What is the difference between iterators and functions?

Functions are faster because they implemented directly on the low-level without using
the iterators so they use many optimizations that can be implemented only on the low-level.
Iterators on the other hand are much more flexibly than functions, a function can do one job
in one pass but iterators can do many jobs in one pass.<br />
This means that for a complex algorithm iterator chains can outperform function chains
because everything will be done in one pass, for example to normalize an UTF-8 string and
convert it to UTF-16 you need to use 2 functions it will be 2 passes,
but using iterators it can be done in one pass.<br />
Also this makes iterators are especially useful for streams where you have only one pass for everything.

## Unicode Algorithms

**Unicode algorithms that the library does implement:**

Unicode Standard 3.4: Table 3-1. Named Unicode Algorithms:
- Default Case Conversion (locale-independent and locale-dependant)
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

*The library implements the normalization algorithm in stream-friendly way
so it can be used with streams and without memory allocations in all cases.<br />
For more info see: https://www.unicode.org/reports/tr15/#Stream_Safe_Text_Format

**Other algorithms:**

The library implements a very simple collation algorithm
that uses DUCET (Default Unicode Collation Element Table)
to sort code points in the correct order instead of sort them
in code point order as a simple comparison function does.
Case insensitive version of the algorithm is based on Default Caseless Matching.

And a very simple search algorithm that is based on Default Caseless Matching.

**Unicode algorithms that the library does not implement:**

Unicode Collation Algorithm (UCA) (UTS #10)<br />
A very complex algorithm to sort strings in many different ways with many different rules.

Bidirectional Algorithm (UBA) (UAX #9)<br />
Specifications for the positioning of characters from right to left, such as Arabic or Hebrew.

Line Breaking Algorithm (UAX #14)

If you need these algorithms you should probably just use
ICU (International Components for Unicode) library.

## Performance

ICU (International Components for Unicode) and some functions from WinAPI are used
for the performance comparison because these implementations are highly optimized.<br />
See perf/result folder.

## C Language

Low-level functions can be compiled in C mode.
This means you can compile them using any C99 compiler even Tiny C Compiler will work.
The library doesn't provide an "official" C wrapper because there are many ways to implement
it in C depending on what allocator you are using, what strings etc.<br />
Sample C wrapper that uses malloc and basic types can be found in examples folder.<br />
You are on your own if you want to use the low-level functions directly.

## SQLite Extension

Undocumented for now.<br />
SQLite extension implemented by using the low-level directly so it always uses SQLite allocator.<br />
See sqlite folder.

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
