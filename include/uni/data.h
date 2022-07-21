/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <cstddef>
#include <type_traits>

#include <uni/config.h>

#ifndef UNI_ALGO_DISABLE_CASE
#include <uni/impl/case_data_extern.h>
#include <uni/impl/case_data.h>
#endif

#ifndef UNI_ALGO_DISABLE_NORM
#include <uni/impl/norm_data_extern.h>
#include <uni/impl/norm_data.h>
#endif

#ifndef UNI_ALGO_DISABLE_BREAK_GRAPHEME
#include <uni/impl/break_grapheme_data_extern.h>
#include <uni/impl/break_grapheme_data.h>
#endif

#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#include <uni/impl/break_word_data_extern.h>
#include <uni/impl/break_word_data.h>
#endif
