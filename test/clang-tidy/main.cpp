/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Include all modules to check them with Clang-Tidy
//
// Command line: clang-tidy test/clang-tidy/main.cpp -header-filter=.* -extra-arg=-std=c++17 --
//
// REMINDER: https://stackoverflow.com/questions/52710180/how-to-use-and-configure-clang-tidy-on-windows

#include "../../src/cpp_uni_convert.h"
#include "../../src/cpp_uni_case.h"
#include "../../src/cpp_uni_norm.h"
#include "../../src/cpp_uni_iterator.h"
#include "../../src/cpp_uni_break_grapheme.h"
#include "../../src/cpp_uni_break_word.h"

// Include all transliterators

#include "../../src/ext/translit/macedonian_to_latin_docs.h"
#include "../../src/ext/translit/japanese_kana_to_romaji_hepburn.h"

int main()
{
    return 0;
}
