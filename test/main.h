/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <uni/convert.h>
#include <uni/case.h>
#include <uni/norm.h>
#include <uni/iterator.h>
#include <uni/break_grapheme.h>
#include <uni/break_word.h>
#include <uni/ranges.h>
#include <uni/data.h>

#include <stdexcept>
#include <iostream>

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

#include "test_lenient.h"
#include "test_strict.h"
#include "test_lenient2.h"
#include "test_strict2.h"
#include "test_extra.h"
#include "test_visual.h"

#include "test_convert_and_iter.h"

#include "test_case.h"
#include "test_norm.h"
#include "test_norm_unaccent.h"
#include "test_break.h"

#include "test_lenient_iter.h"
#include "test_lenient_iter_rev.h"
#include "test_strict_iter.h"
#include "test_strict_iter_rev.h"

#include "test_translit.h"

#include "test_ranges.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

int main7()
{
#ifdef TEST_MODE_GENERATE_VISUAL_FILES
    std::cout << "Test mode: TEST_MODE_GENERATE_VISUAL_FILES" << '\n';
    test_visual(); // This generates files for the visual test
    return 0;
#endif

#ifdef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
    std::cout << "Test mode: TEST_MODE_WITHOUT_UNICODE_TEST_FILES" << '\n';
#endif

    std::cout << "Library version: " <<
        uni::version::library::major << '.' <<
        uni::version::library::minor << '.' <<
        uni::version::library::patch << '\n';
    std::cout << "Unicode version: " <<
        uni::version::unicode::major << '.' <<
        uni::version::unicode::minor << '.' <<
        uni::version::unicode::update << '\n' << '\n';

    static_assert(uni::version::library::major >= 0 && uni::version::library::major <= 255);
    static_assert(uni::version::library::minor >= 0 && uni::version::library::minor <= 255);
    static_assert(uni::version::library::patch >= 0 && uni::version::library::patch <= 255);

    static_assert(uni::version::unicode::major >= 1 && uni::version::unicode::major <= 255);
    static_assert(uni::version::unicode::minor >= 0 && uni::version::unicode::minor <= 255);
    static_assert(uni::version::unicode::update >= 0 && uni::version::unicode::update <= 255);

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

    test_overflow();
    test_alter_value();

    std::cout << "DONE: Convert module" << '\n';

    test_lenient_iter_utf8to16();
    test_lenient_iter_utf16to8();
    test_lenient_iter_rev_utf8to16();
    test_lenient_iter_rev_utf16to8();
    test_strict_iter_utf8to16();
    test_strict_iter_utf16to8();
    test_strict_iter_rev_utf8to16();
    test_strict_iter_rev_utf16to8();

    std::cout << "DONE: Iterator module" << '\n';

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

    std::cout << "DONE: Case module" << '\n';

#ifndef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
    test_break_grapheme();
    test_break_word();
    test_break_word_corner_cases();

    std::cout << "DONE: Break grapheme and word modules" << '\n';
#else
    std::cout << "SKIPPED: Break grapheme and word modules" << '\n';
#endif

    test_translit_macedonian_to_latin_docs();
    test_translit_japanese_kana_to_romaji_hepburn();

    std::cout << "DONE: Transliterators" << '\n';

    std::cout << "DOING: Convert and iterator module" << '\n';

    test_convert_and_iter_convert();
    test_convert_and_iter_iter();

    std::cout << "DONE: Convert and iterator module" << '\n';

#ifndef TEST_MODE_WITHOUT_UNICODE_TEST_FILES
    std::cout << "DOING: Normalization module" << '\n';

    test_norm();
    test_norm_detect();
    test_norm_stream_safe();
    test_norm_unaccent();

    std::cout << "DONE: Normalization module" << '\n';
#else
    std::cout << "SKIPPED: Normalization module" << '\n';
#endif

    test_ranges();
    test_ranges_ctad();
    test_ranges_static_assert();

    std::cout << "ALL TESTS PASSED!" << '\n';

    return 0;
}
