/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include "../include/uni_algo/conv.h"
#include "../include/uni_algo/case.h"
#include "../include/uni_algo/norm.h"
#include "../include/uni_algo/prop.h"
#include "../include/uni_algo/break_grapheme.h"
#include "../include/uni_algo/break_word.h"
#include "../include/uni_algo/ranges.h"
#include "../include/uni_algo/locale.h"
// Extensions
#include "../include/uni_algo/ext/ascii.h"
// Transliterators
#include "../include/uni_algo/ext/translit/macedonian_to_latin_docs.h"
#include "../include/uni_algo/ext/translit/japanese_kana_to_romaji_hepburn.h"

// Additional C++ Standard Library includes that are needed for tests
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <set>
#include <list>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

//#define TEST_MODE_GENERATE_VISUAL_FILES
//#define TEST_MODE_WITHOUT_UNICODE_TEST_FILES

//#ifndef UNI_ALGO_EXPERIMENTAL
//#error "Don't forget to test experimental stuff"
//#endif

// Do not use exceptions so it possible to compile the tests with -fno-exceptions
#define TESTX(x) do {if (!(x)) {std::cout << "TEST FAILED: " << (__FILE__) << ":" << (__LINE__) << '\n'; exit(111);}} while(0)

// Tests have some big functions so disable -Wstack-usage warning for them
#ifdef __GNUC__
#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wstack-usage="
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
#include "test_break.h"
#include "test_prop.h"

#include "test_ascii.h"
#include "test_translit.h"
#include "test_translit_buffer.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

int main7()
{
#ifdef TEST_MODE_GENERATE_VISUAL_FILES
    std::cout << "MODE: TEST_MODE_GENERATE_VISUAL_FILES" << '\n';
    test_visual(); // This generates files for the visual test
    return 0;
#endif

#ifdef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
    std::cout << "MODE: TEST_MODE_WITHOUT_UNICODE_TEST_FILES" << '\n' << '\n';
#endif

    std::cout << "Library  Version: " <<
        uni::version::library::major << '.' <<
        uni::version::library::minor << '.' <<
        uni::version::library::patch << '\n';
    std::cout << "Unicode  Version: " <<
        uni::version::unicode::major << '.' <<
        uni::version::unicode::minor << '.' <<
        uni::version::unicode::update << '\n';

#if defined(UNI_ALGO_DISABLE_SYSTEM_LOCALE)
    std::cout << "System    Locale: DISABLED" << '\n';
#elif defined(UNI_ALGO_STATIC_DATA)
    std::cout << "System    Locale: DISABLED HEADER-ONLY" << '\n';
#else
    if (uni::locale::system().is_empty())
        std::cout << "System    Locale: EMPTY" << '\n';
    else
        std::cout << "System    Locale: " << uni::locale::system().to_string() << '\n';
#endif

    static_assert(uni::version::library::major >= 0 && uni::version::library::major <= 255);
    static_assert(uni::version::library::minor >= 0 && uni::version::library::minor <= 255);
    static_assert(uni::version::library::patch >= 0 && uni::version::library::patch <= 255);

    static_assert(uni::version::unicode::major >= 1 && uni::version::unicode::major <= 255);
    static_assert(uni::version::unicode::minor >= 0 && uni::version::unicode::minor <= 255);
    static_assert(uni::version::unicode::update >= 0 && uni::version::unicode::update <= 255);

    std::cout << "C++      Version: " << test_version_cpp() << '\n';
    std::cout << "Ranges   Version: " << test_version_ranges() << '\n';
    std::cout << "Compiler Version: " << test_version_compiler() << '\n';

    std::cout << '\n';

    test_lenient_utf8to16();
    test_lenient_utf8to32();
    test_lenient_utf16to8();
    test_lenient_utf32to8();
    test_lenient_utf16to32();
    test_lenient_utf32to16();

    test_strict_utf8to16();
    test_strict_utf8to32();
    test_strict_utf16to8();
    test_strict_utf32to8();
    test_strict_utf16to32();
    test_strict_utf32to16();

    test2_lenient_utf8to16();
    test2_lenient_utf8to32();
    test2_lenient_utf16to8();
    test2_lenient_utf32to8();
    test2_lenient_utf16to32();
    test2_lenient_utf32to16();

    test2_strict_utf8to16();
    test2_strict_utf8to32();
    test2_strict_utf16to8();
    test2_strict_utf32to8();
    test2_strict_utf16to32();
    test2_strict_utf32to16();

    test_valid_utf8();
    test_valid_utf16();
    test_valid_utf32();
    test2_valid_utf8();
    test2_valid_utf16();
    test2_valid_utf32();

    test_overflow();
    test_alter_value();

    std::cout << "DONE: Conversion Functions" << '\n';

    test_lenient_iter_utf8to16();
    test_lenient_iter_utf16to8();
    test_lenient_iter_rev_utf8to16();
    test_lenient_iter_rev_utf16to8();
    test_strict_iter_utf8to16();
    test_strict_iter_utf16to8();
    test_strict_iter_rev_utf8to16();
    test_strict_iter_rev_utf16to8();

    std::cout << "DONE: Conversion Ranges" << '\n';

    test_short_func_conv();
    test_short_func_case();
    test_short_func_norm();

    test_alloc_func_conv();
    test_alloc_func_case();
    test_alloc_func_norm();

    std::cout << "DONE: Functions" << '\n';

    test_ranges();
    test_ranges_to();
    test_ranges_ctad();
    test_ranges_static_assert();

    std::cout << "DONE: Ranges" << '\n';

    test_locale();

    std::cout << "DONE: Locale" << '\n';

    test_case_compare_collate();
    test_case_search();
    test_case_search_ex();
    test_case_ill_formed();
    test_case_full_case();
    test_case_upper_lower_fold();
    test_case_final_sigma();
    test_case_sort_key();
    test_case_like();
    test_case_locale_lt();
    test_case_locale_tr_az();
    test_case_locale_el();
    test_case_title();
    test_case_title_locale();

    std::cout << "DONE: Case Functions" << '\n';

    test_translit_buffer();
    test_translit_macedonian_to_latin_docs();
    test_translit_japanese_kana_to_romaji_hepburn();

    std::cout << "DONE: Transliterators" << '\n';

    test_ascii_prop();
    test_ascii_upper_lower();
    test_ascii_search();
    test_ascii_trim();
    test_ascii_valid();
    test_ascii_collate();
    test_ascii_short_func();
    test_ascii_alloc_func();

    std::cout << "DONE: ASCII Extension" << '\n';

    test_prop();
    test_prop_case();
    test_prop_norm();

    std::cout << "DONE: Code Point Properties" << '\n';

#ifndef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
    test_break_grapheme();
    test_break_word();
#else
    std::cout << "SKIP: Break Grapheme and Word with test files" << '\n';
#endif
    test_break_word_corner_cases();
    test_break_word_prop();
    test_break_bidi();

    std::cout << "DONE: Break Grapheme and Word" << '\n';

    std::cout << "WAIT: Conversion and Ranges Extra" << '\n';

    test_conv_and_iter_conv();
    test_conv_and_iter_iter();

    std::cout << "DONE: Conversion and Ranges Extra" << '\n';

#ifndef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
    std::cout << "WAIT: Normalization" << '\n';
    test_norm();
#else
    std::cout << "SKIP: Normalization with test files" << '\n';
#endif
    test_norm_detect();
    test_norm_stream_safe();
    test_norm_unaccent();

    std::cout << "DONE: Normalization" << '\n';

    std::cout << "ALL TESTS PASSED!" << '\n';

    return 0;
}
