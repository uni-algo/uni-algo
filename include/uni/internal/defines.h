/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See LICENSE.txt */

// Warning! This file must never be used by a wrapper.

// Internal defines, must be used together with impl_undefs.h

// The internal defines are lowercased because they make too much noise in the code if uppercased.

// Always inlining is important in the low-level.
// There will be 20-70% performance loss without it.
#ifndef UNI_ALGO_TEST_DISABLE_ALWAYS_INLINE_GLOBAL
#  if defined(__GNUC__) || defined(__clang__)
#    define uaix_always_inline      __attribute__((__always_inline__)) inline
#    define uaix_always_inline_tmpl __attribute__((__always_inline__)) inline
#  elif defined(_MSC_VER)
#    define uaix_always_inline      __forceinline
#    define uaix_always_inline_tmpl __forceinline
#  else
#    define uaix_always_inline      inline
#    define uaix_always_inline_tmpl inline
#  endif
#else // UNI_ALGO_TEST_DISABLE_ALWAYS_INLINE_GLOBAL
#  ifndef UNI_ALGO_TEST_DISABLE_ALWAYS_INLINE_GLOBAL_EX
#    define uaix_always_inline inline // Functions still need to use inline to prevent ODR
#    define uaix_always_inline_tmpl // Do not inline templates (this includes iterator functions)
#  else // UNI_ALGO_TEST_DISABLE_ALWAYS_INLINE_GLOBAL_EX
/* Use this to force to compile the smallest possible code.
 * It makes everything template so only very small functions will be inlined by default
 * (with inline that prevents ODR GCC inlines medium functions too).
 * This also compiles the slowest possible code.
 * Compare the size with always inline version to make sure always inline is not much bigger.
 * Use nm to check what a compiler actually does. */
#    define uaix_always_inline template<typename = void>
#    define uaix_always_inline_tmpl
#  endif
#endif

// Must not be used without performance testing it can degrade performance in many cases
#if defined(__GNUC__) || defined(__clang__)
#  define uaix_likely(x)   __builtin_expect(!!(x),1)
#  define uaix_unlikely(x) __builtin_expect(!!(x),0)
#else
#  define uaix_likely(x)   (!!(x))
#  define uaix_unlikely(x) (!!(x))
#endif

// C implementation must be static because it will be included in .c file
// Also add maybe unused to everything in C because a wrapper doesn't need to implement everything
#ifdef __cplusplus
#  define uaix_static
#else
#  if defined(__GNUC__) || defined(__clang__)
#    define uaix_static static __attribute__((__unused__))
#  else
#    define uaix_static static
#  endif
#endif

// If an internal (non-template) function should not be always inlined
// then this must be used instead of uaix_static. This prevents C++ ODR violation.
// Note that it must be used for small functions only because we test with -Winline too.
#ifdef __cplusplus
#  define uaix_inline inline // template<typename = void> alternative if -Winline will be too annoying
#else
#  define uaix_inline uaix_static
#endif

// All variables that are outside functions must use uaix_const
// and all variables that are inside functions must just use const if needed.
#ifdef __cplusplus
#  if (!defined(_MSVC_LANG) && __cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#    define uaix_const inline constexpr
#  else
#    define uaix_const constexpr
#  endif
#else
#  define uaix_const uaix_static const
#endif

// The define can be used to switch Unicode data to static (constexpr in C++) mode
// uaix_const_data is only used by generated Unicode data files
#ifdef UNI_ALGO_STATIC_DATA
#  ifdef __cplusplus
#    define uaix_const_data uaix_const
#  else
#    define uaix_const_data uaix_static const
#  endif
#else
#  define uaix_const_data const
#endif

// -Wsign-conversion is the only warning that cannot be suppressed in C++ by using other tools
// because it's almost impossible to suppress it inside templates.
// Note that because of this the warning isn't even enabled in GCC
// by -Wconversion in C++ but it is enabled by -Wconversion in Clang even in C++.
// Note that in C mode it isn't a problem so it need to be suppressed only in C++ mode.
// Suppress -Wold-style-cast here too, low-level uses C casts to suppress conversion warnings
// so we need to suppress the warning about suppressing warnings.
// Note that even though these warnings are supressed it doesn't mean stupid things with
// casts are allowed that means the low-level must use casts only to suppress warnings.
#ifdef __cplusplus
#  if defined(__GNUC__) || defined(__clang__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsign-conversion"
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#  endif
#endif
