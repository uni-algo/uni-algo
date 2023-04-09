/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

// This file must be inlined into a data file by a wrapper
// when define UNI_ALGO_STATIC_DATA is NOT used.

#ifdef UNI_ALGO_STATIC_DATA
#error "This file must not be used with define UNI_ALGO_STATIC_DATA"
#endif

#include "internal_defines.h"

#ifndef UNI_ALGO_DISABLE_CASE
#include "data/extern_case.h"
#include "data/data_case.h"
#endif

#ifndef UNI_ALGO_DISABLE_NORM
#include "data/extern_norm.h"
#include "data/data_norm.h"
#endif

#ifndef UNI_ALGO_DISABLE_PROP
#include "data/extern_prop.h"
#include "data/data_prop.h"
#endif

#ifndef UNI_ALGO_DISABLE_SCRIPT
#include "data/extern_script.h"
#include "data/data_script.h"
#endif

#ifndef UNI_ALGO_DISABLE_SEGMENT_GRAPHEME
#include "data/extern_segment_grapheme.h"
#include "data/data_segment_grapheme.h"
#endif

#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
#include "data/extern_segment_word.h"
#include "data/data_segment_word.h"
#endif

#include "internal_undefs.h"
