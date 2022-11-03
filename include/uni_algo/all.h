/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// This file includes all modules that are enabled in uni_algo/config.h

#ifndef UNI_ALGO_ALL_H_UAIH
#define UNI_ALGO_ALL_H_UAIH

#include "conv.h"
#include "ranges.h"

#ifndef UNI_ALGO_DISABLE_CASE
#include "case.h"
#endif

#ifndef UNI_ALGO_DISABLE_NORM
#include "norm.h"
#endif

#ifndef UNI_ALGO_DISABLE_PROP
#include "prop.h"
#endif

#ifndef UNI_ALGO_DISABLE_BREAK_GRAPHEME
#include "break_grapheme.h"
#endif

#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#include "break_word.h"
#endif

#endif // UNI_ALGO_ALL_H_UAIH
