/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See LICENSE.txt */

// Warning! This file must never be used by a wrapper.

// Internal undefs, must be used together with impl_defines.h

#undef uaix_always_inline
#undef uaix_always_inline_tmpl
#undef uaix_likely
#undef uaix_unlikely
#undef uaix_static
#undef uaix_inline
#undef uaix_const
#undef uaix_const_data

#ifdef __cplusplus
#  if defined(__GNUC__) || defined(__clang__)
#    pragma GCC diagnostic pop
#  endif
#endif
