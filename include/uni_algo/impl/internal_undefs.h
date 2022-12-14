/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See LICENSE.txt */

// IMPORTANT: This file must never be used by a wrapper

// Internal undefs, must be used together with internal_defines.h

#undef uaix_always_inline
#undef uaix_always_inline_tmpl
#undef uaix_likely
#undef uaix_unlikely
#undef uaix_static
#undef uaix_inline
#undef uaix_const
#undef uaix_const_data
#undef uaix_array
#undef uaix_array2
#undef uaix_array_brace_ellison
#undef uaix_data_array
#undef uaix_data_array2

#ifdef __cplusplus
#  if defined(__GNUC__) || defined(__clang__)
#    pragma GCC diagnostic pop
#  endif
#endif
