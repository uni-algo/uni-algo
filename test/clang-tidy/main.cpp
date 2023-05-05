/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Command line for Clang-Tidy and Cppcheck:
//
// clang-tidy test/clang-tidy/main.cpp -header-filter=.* -extra-arg=-std=c++17 --
//
// cppcheck test/clang-tidy/main.cpp --error-exitcode=1 --max-configs=1 --std=c++17
//          --enable=warning,performance,portability,information,unusedFunction,missingInclude
//          --suppress=missingIncludeSystem --suppress=toomanyconfigs
//
// NOTE: Replace c++17 with c++20 to check C++20 or higher specific things
//
// REMINDER: https://stackoverflow.com/questions/52710180/how-to-use-and-configure-clang-tidy-on-windows

// NOTE: No point to use the define to include data files
// it just makes the test slower and achieves nothing
//#define UNI_ALGO_STATIC_DATA

// Enable experimental stuff
#ifndef UNI_ALGO_EXPERIMENTAL
#define UNI_ALGO_EXPERIMENTAL
#endif

// Include all modules to check them with Clang-Tidy or Cppcheck

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
