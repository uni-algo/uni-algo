/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <cstddef>
#include <type_traits>

#include "cpp_uni_config.h"

#ifndef UNI_ALGO_DISABLE_CASE
#include "impl/impl_case_data_extern.h"
#include "impl/impl_case_data.h"
#endif

#ifndef UNI_ALGO_DISABLE_NORM
#include "impl/impl_norm_data_extern.h"
#include "impl/impl_norm_data.h"
#endif

#ifndef UNI_ALGO_DISABLE_BREAK_GRAPHEME
#include "impl/impl_break_grapheme_data_extern.h"
#include "impl/impl_break_grapheme_data.h"
#endif

#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#include "impl/impl_break_word_data_extern.h"
#include "impl/impl_break_word_data.h"
#endif

