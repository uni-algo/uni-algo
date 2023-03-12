/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Include all modules to check them with Clang-Tidy
//
// Command line: clang-tidy test/clang-tidy/main.cpp -header-filter=.* -extra-arg=-std=c++17 --
//
// REMINDER: https://stackoverflow.com/questions/52710180/how-to-use-and-configure-clang-tidy-on-windows

#include "../../include/uni_algo/version.h"
#include "../../include/uni_algo/conv.h"
#include "../../include/uni_algo/locale.h"
#include "../../include/uni_algo/case.h"
#include "../../include/uni_algo/norm.h"
#include "../../include/uni_algo/prop.h"
#include "../../include/uni_algo/script.h"
#include "../../include/uni_algo/ranges.h"
#include "../../include/uni_algo/ranges_conv.h"
#include "../../include/uni_algo/ranges_norm.h"
#include "../../include/uni_algo/ranges_grapheme.h"
#include "../../include/uni_algo/ranges_word.h"
// Extensions
#include "../../include/uni_algo/ext/ascii.h"
// Transliterators
#include "../../include/uni_algo/ext/translit/macedonian_to_latin_docs.h"
#include "../../include/uni_algo/ext/translit/japanese_kana_to_romaji_hepburn.h"

int main()
{
    return 0;
}
