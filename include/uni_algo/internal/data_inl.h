/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// IMPORTANT: This file doesn't have include guards and must be inlined into .cpp file

#include "../config.h"
#include "safe_layer.h"

#ifdef UNI_ALGO_STATIC_DATA
#error "data.cpp file must not be compiled when header-only is enabled"
#else

#include "../impl/impl_data.h"

#ifndef UNI_ALGO_DISABLE_SYSTEM_LOCALE
#include "locale_inl.h"
#endif // UNI_ALGO_DISABLE_SYSTEM_LOCALE

#endif // UNI_ALGO_STATIC_DATA
