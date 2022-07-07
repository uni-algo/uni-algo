/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// This file includes all modules that are enabled in cpp_uni_config.h

#ifndef CPP_UNI_ALL_H_UAIX
#define CPP_UNI_ALL_H_UAIX

#include "cpp_uni_convert.h"
#include "cpp_uni_iterator.h"

#ifndef UNI_ALGO_IMPL_DISABLE_CASE
#include "cpp_uni_case.h"
#endif

#ifndef UNI_ALGO_IMPL_DISABLE_NORM
#include "cpp_uni_norm.h"
#endif

#ifndef UNI_ALGO_IMPL_DISABLE_BREAK_GRAPHEME
#include "cpp_uni_break_grapheme.h"
#endif

#ifndef UNI_ALGO_IMPL_DISABLE_BREAK_WORD
#include "cpp_uni_break_word.h"
#endif

#endif // CPP_UNI_ALL_H_UAIX
