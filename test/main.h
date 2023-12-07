/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

//#define TEST_MODE_WITHOUT_UNICODE_TEST_FILES
//#define TEST_MODE_SINGLE_INCLUDE
//#define TEST_MODE_CONSTEXPR

//#define TEST_MODE_GENERATE_VISUAL_FILES

#ifndef TEST_MODE_SINGLE_INCLUDE
#include "../include/uni_algo/version.h"
#include "../include/uni_algo/conv.h"
#include "../include/uni_algo/locale.h"
#include "../include/uni_algo/case.h"
#include "../include/uni_algo/norm.h"
#include "../include/uni_algo/prop.h"
#include "../include/uni_algo/script.h"
#include "../include/uni_algo/ranges.h"
#include "../include/uni_algo/ranges_conv.h"
#include "../include/uni_algo/ranges_norm.h"
#include "../include/uni_algo/ranges_grapheme.h"
#include "../include/uni_algo/ranges_word.h"
// Extensions
#include "../include/uni_algo/ext/ascii.h"
// Transliterators
#include "../include/uni_algo/ext/translit/macedonian_to_latin_docs.h"
#include "../include/uni_algo/ext/translit/japanese_kana_to_romaji_hepburn.h"
#else
#include "uni_algo_ext.h"
#endif

// Additional C++ Standard Library includes that are needed for tests
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <set>
#include <list>
#include <iostream>
#include <fstream>
#include <random>
// TODO: Disable chrono for now because it's completely broken in Clang 14 GitHub runner:
// https://github.com/actions/runner-images/issues/8659
// Should be re-enabled later here and in file: test/test_ascii.h
//#include <chrono>

#ifdef TEST_MODE_CONSTEXPR
#ifndef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
#define TEST_MODE_WITHOUT_UNICODE_TEST_FILES
#endif
#endif

// NOTE:
// Lowest compiler versions for constexpr library:
// GCC 12, Clang 15, MSVC 19.29 (MSVS 2019 16.10)
// because constexpr library requires constexpr std::string
// so __cpp_lib_constexpr_string needs to be defined.
// For reference: https://en.cppreference.com/w/cpp/compiler_support
// P0980R1: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0980r1.pdf

#if defined(TEST_MODE_CONSTEXPR) && !defined(UNI_ALGO_CONSTEXPR)
#error "Test mode constexpr requires constexpr library (C++20 or higher and header-only enabled)"
#undef TEST_MODE_CONSTEXPR
#endif

#ifndef TEST_MODE_CONSTEXPR
// Do not use exceptions so it possible to compile the tests with -fno-exceptions
#define TESTX(x) do {if (!(x)) {std::cout << "TEST FAILED: " << (__FILE__) << ":" << (__LINE__) << '\n'; exit(111);}} while(0)
#define STATIC_TESTX
#define test_constexpr
#else
#define TESTX(x) do {if (!(x)) return false;} while(0)
#define STATIC_TESTX(x) static_assert(x)
#define test_constexpr constexpr
#endif

// Tests have some big functions so disable -Wstack-usage warning for them
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstack-usage="
#endif

// All the following includes should not include other files.
// The tests are designed to include all tests into this one .h file: test/main.h
// and then include it into one .cpp file: test/main.cpp
// to make the compilation of the tests as fast as possible.

#include "test_version.h"

#include "test_lenient.h"
#include "test_strict.h"
#include "test_lenient2.h"
#include "test_strict2.h"
#include "test_valid.h"
#include "test_fast_ascii.h"
#include "test_extra.h"
#include "test_visual.h"

#include "test_lenient_iter.h"
#include "test_lenient_iter_rev.h"
#include "test_strict_iter.h"
#include "test_strict_iter_rev.h"

#include "test_conv_and_iter.h"

#include "test_short_func.h"
#include "test_alloc_func.h"

#include "test_ranges.h"

#include "test_locale.h"
#include "test_case.h"
#include "test_norm.h"
#include "test_norm_unaccent.h"
#include "test_segment.h"
#include "test_segment_cursor.h"
#include "test_prop.h"
#include "test_script.h"

#include "test_ascii.h"
#include "test_translit.h"
#include "test_translit_buffer.h"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

int main7()
{
#ifdef TEST_MODE_GENERATE_VISUAL_FILES
    std::cout << "MODE: TEST_MODE_GENERATE_VISUAL_FILES" << '\n';
    test_visual(); // This generates files for the visual test
    return 0;
#endif

#ifdef TEST_MODE_SINGLE_INCLUDE
    std::cout << "MODE: TEST_MODE_SINGLE_INCLUDE" << '\n';
#endif

#if defined(TEST_MODE_CONSTEXPR)
    std::cout << "MODE: TEST_MODE_CONSTEXPR" << '\n' << '\n';
#elif defined(TEST_MODE_WITHOUT_UNICODE_TEST_FILES)
    std::cout << "MODE: TEST_MODE_WITHOUT_UNICODE_TEST_FILES" << '\n' << '\n';
#endif

    std::cout << "Library  Version: " <<
        una::version::library.major() << '.' <<
        una::version::library.minor() << '.' <<
        una::version::library.patch() << '\n';
    std::cout << "Unicode  Version: " <<
        una::version::unicode.major() << '.' <<
        una::version::unicode.minor() << '.' <<
        una::version::unicode.update() << '\n';

#if defined(UNI_ALGO_DISABLE_SYSTEM_LOCALE)
    std::cout << "System    Locale: DISABLED" << '\n';
#elif defined(UNI_ALGO_STATIC_DATA)
    std::cout << "System    Locale: DISABLED HEADER-ONLY" << '\n';
#else
    if (una::locale::system().is_empty())
        std::cout << "System    Locale: EMPTY" << '\n';
    else
        std::cout << "System    Locale: " << una::locale::system().to_string() << '\n';
#endif

    static_assert(una::version::library.major() >= 0 && una::version::library.major() <= 255);
    static_assert(una::version::library.minor() >= 0 && una::version::library.minor() <= 255);
    static_assert(una::version::library.patch() >= 0 && una::version::library.patch() <= 255);

    static_assert(una::version::unicode.major() >= 1 && una::version::unicode.major() <= 255);
    static_assert(una::version::unicode.minor() >= 0 && una::version::unicode.minor() <= 255);
    static_assert(una::version::unicode.update() >= 0 && una::version::unicode.update() <= 255);

    std::cout << "C++      Version: " << test_version_cpp() << '\n';
    std::cout << "Ranges   Version: " << test_version_ranges() << '\n';
    std::cout << "Compiler Version: " << test_version_compiler() << '\n';

    std::cout << '\n';

    STATIC_TESTX(test_lenient_utf8to16());
    STATIC_TESTX(test_lenient_utf8to32());
    STATIC_TESTX(test_lenient_utf16to8());
    STATIC_TESTX(test_lenient_utf32to8());
    STATIC_TESTX(test_lenient_utf16to32());
    STATIC_TESTX(test_lenient_utf32to16());

    STATIC_TESTX(test_strict_utf8to16());
    STATIC_TESTX(test_strict_utf8to32());
    STATIC_TESTX(test_strict_utf16to8());
    STATIC_TESTX(test_strict_utf32to8());
    STATIC_TESTX(test_strict_utf16to32());
    STATIC_TESTX(test_strict_utf32to16());

    STATIC_TESTX(test2_lenient_utf8to16());
    STATIC_TESTX(test2_lenient_utf8to32());
    STATIC_TESTX(test2_lenient_utf16to8());
    STATIC_TESTX(test2_lenient_utf32to8());
    STATIC_TESTX(test2_lenient_utf16to32());
    STATIC_TESTX(test2_lenient_utf32to16());

    STATIC_TESTX(test2_strict_utf8to16());
    STATIC_TESTX(test2_strict_utf8to32());
    STATIC_TESTX(test2_strict_utf16to8());
    STATIC_TESTX(test2_strict_utf32to8());
    STATIC_TESTX(test2_strict_utf16to32());
    STATIC_TESTX(test2_strict_utf32to16());

    STATIC_TESTX(test_valid_utf8());
    STATIC_TESTX(test_valid_utf16());
    STATIC_TESTX(test_valid_utf32());
    STATIC_TESTX(test2_valid_utf8());
    STATIC_TESTX(test2_valid_utf16());
    STATIC_TESTX(test2_valid_utf32());

    STATIC_TESTX(test_fast_ascii_utf8to16());
    STATIC_TESTX(test_fast_ascii_utf8to32());

    STATIC_TESTX(test_overflow());
    STATIC_TESTX(test_alter_value());

    std::cout << "DONE: Conversion Functions" << '\n';

    STATIC_TESTX(test_lenient_iter_utf8to16());
    STATIC_TESTX(test_lenient_iter_utf16to8());
    STATIC_TESTX(test_lenient_iter_rev_utf8to16());
    STATIC_TESTX(test_lenient_iter_rev_utf16to8());
    STATIC_TESTX(test_strict_iter_utf8to16());
    STATIC_TESTX(test_strict_iter_utf16to8());
    STATIC_TESTX(test_strict_iter_rev_utf8to16());
    STATIC_TESTX(test_strict_iter_rev_utf16to8());

    std::cout << "DONE: Conversion Ranges" << '\n';

    STATIC_TESTX(test_short_func_conv());
    STATIC_TESTX(test_short_func_case());
    STATIC_TESTX(test_short_func_norm());

    STATIC_TESTX(test_alloc_func_conv());
    STATIC_TESTX(test_alloc_func_case());
    STATIC_TESTX(test_alloc_func_norm());

    std::cout << "DONE: Functions" << '\n';

    STATIC_TESTX(test_ranges());
    STATIC_TESTX(test_ranges_to());
    STATIC_TESTX(test_ranges_ctad());
    STATIC_TESTX(test_ranges_static_assert());
    STATIC_TESTX(test_ranges_cache());

    std::cout << "DONE: Ranges" << '\n';

    STATIC_TESTX(test_locale());

    std::cout << "DONE: Locale" << '\n';

    STATIC_TESTX(test_case_compare_collate());
    STATIC_TESTX(test_case_find());
    STATIC_TESTX(test_case_find_ex());
    STATIC_TESTX(test_case_ill_formed());
    STATIC_TESTX(test_case_full_case());
    STATIC_TESTX(test_case_upper_lower_fold());
    STATIC_TESTX(test_case_final_sigma());
    STATIC_TESTX(test_case_sort_key());
    STATIC_TESTX(test_case_like());
    STATIC_TESTX(test_case_locale_lt());
    STATIC_TESTX(test_case_locale_tr_az());
    STATIC_TESTX(test_case_locale_el());
    STATIC_TESTX(test_case_title());
    STATIC_TESTX(test_case_title_locale());

    std::cout << "DONE: Case Functions" << '\n';

    STATIC_TESTX(test_prop());
    STATIC_TESTX(test_prop_case());
    STATIC_TESTX(test_prop_norm());

    std::cout << "DONE: Code Point Properties" << '\n';

    STATIC_TESTX(test_script());
    STATIC_TESTX(test_script_ext());

    std::cout << "DONE: Scripts" << '\n';

    STATIC_TESTX(test_ascii_prop());
    STATIC_TESTX(test_ascii_upper_lower());
    STATIC_TESTX(test_ascii_find());
    STATIC_TESTX(test_ascii_trim());
    STATIC_TESTX(test_ascii_valid());
    STATIC_TESTX(test_ascii_collate());
    STATIC_TESTX(test_ascii_short_func());
    STATIC_TESTX(test_ascii_alloc_func());

    std::cout << "DONE: Extension: ASCII" << '\n';

    STATIC_TESTX(test_translit_buffer());
    STATIC_TESTX(test_translit_macedonian_to_latin_docs());
    STATIC_TESTX(test_translit_japanese_kana_to_romaji_hepburn());

    std::cout << "DONE: Extension: Transliterators" << '\n';

#ifndef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
    test_segment_grapheme();
    test_segment_word();
#else
    std::cout << "SKIP: Text Segmentation with test files" << '\n';
#endif
    STATIC_TESTX(test_segment_word_corner_cases());
    STATIC_TESTX(test_segment_word_prop());
    STATIC_TESTX(test_segment_bidi());
    STATIC_TESTX(test_segment_cursor_grapheme());
    STATIC_TESTX(test_segment_cursor_word());
    STATIC_TESTX(test_segment_cursor_word_only());
    STATIC_TESTX(test2_segment_cursor_grapheme());
    STATIC_TESTX(test2_segment_cursor_word());
    STATIC_TESTX(test2_segment_cursor_word_only());

    std::cout << "DONE: Text Segmentation" << '\n';

    // Skip constexpr test because takes too long and may exceed constexpr ops limit
#ifndef TEST_MODE_CONSTEXPR
    std::cout << "WAIT: Conversion and Ranges Extra" << '\n';

    STATIC_TESTX(test_conv_and_iter_conv());
    STATIC_TESTX(test_conv_and_iter_iter());

    std::cout << "DONE: Conversion and Ranges Extra" << '\n';
#else
    std::cout << "SKIP: Conversion and Ranges Extra" << '\n';
#endif

#ifndef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
    std::cout << "WAIT: Normalization" << '\n';
    test_norm();
#else
    std::cout << "SKIP: Normalization with test files" << '\n';
#endif
    STATIC_TESTX(test_norm_detect());
    STATIC_TESTX(test_norm_detect_error());
    std::cout << "DONE: Detecting Normalization Forms" << '\n';
    STATIC_TESTX(test_norm_stream_safe());
    std::cout << "DONE: Normalization Stream-Safe Text Format" << '\n';
    STATIC_TESTX(test_norm_unaccent());

    std::cout << "DONE: Normalization" << '\n';

#ifdef TEST_MODE_CONSTEXPR
    std::cout << "ALL CONSTEXPR TESTS PASSED!" << '\n';
#else
    std::cout << "ALL TESTS PASSED!" << '\n';
#endif

    return 0;
}
