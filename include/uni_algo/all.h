/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// This file includes all modules that are enabled in uni_algo/config.h

#ifndef UNI_ALGO_ALL_H_UAIH
#define UNI_ALGO_ALL_H_UAIH

#include "version.h"

#include "conv.h"
#include "ranges.h"
#include "ranges_conv.h"

#ifndef UNI_ALGO_DISABLE_CASE
#include "case.h"
#endif

#ifndef UNI_ALGO_DISABLE_NORM
#include "norm.h"
#include "ranges_norm.h"
#endif

#ifndef UNI_ALGO_DISABLE_PROP
#include "prop.h"
#endif

#ifndef UNI_ALGO_DISABLE_SCRIPT
#include "script.h"
#endif

#ifndef UNI_ALGO_DISABLE_SEGMENT_GRAPHEME
#include "ranges_grapheme.h"
#endif

#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
#include "ranges_word.h"
#endif

#endif // UNI_ALGO_ALL_H_UAIH
