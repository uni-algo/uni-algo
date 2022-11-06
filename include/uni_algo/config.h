/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_CONFIG_H_UAIH
#define UNI_ALGO_CONFIG_H_UAIH

// UNI_ALGO_CPP_LIB_VERSION
// Describes the library version number in XYYYZZZ format such that:
// (VERSION % 1000) is the patch version 0..255,
// (VERSION / 1000 % 1000) is the minor version 0..255,
// (VERSION / 1000000) is the major version 0..255.

// UNI_ALGO_UNICODE_VERSION
// Describes Unicode version number in XYYYZZZ format such that:
// (VERSION % 1000) is the update version 0..255,
// (VERSION / 1000 % 1000) is the minor version 0..255,
// (VERSION / 1000000) is the major version 1..255.

// Note that uni::version namespace can be used to get these values.

// Note that you can just add the following defines to your project
// instead of uncommenting the defines here.

//#define UNI_ALGO_DISABLE_CASE
// Disable Case module.
// Reduces the size of the library by ~200 KB.

//#define UNI_ALGO_DISABLE_NORM
// Disable Normalization module.
// Reduces the size of the library by ~300 KB.

//#define UNI_ALGO_DISABLE_PROP
// Disable Code Point Properties module.
// Reduces the size of the library by ~35 KB.
// Note that if this module is disabled unaccent functions
// in Normalization module will be disabled too.

//#define UNI_ALGO_DISABLE_BREAK_GRAPHEME
// Disable Break Grapheme module.
// Reduces the size of the library by ~25 KB.

//#define UNI_ALGO_DISABLE_BREAK_WORD
// Disable Break Word module.
// Reduces the size of the library by ~35 KB.
// Note that if Break Word module is disabled title case functions
// in Case module will be disabled too because it is needed for them.

//#define UNI_ALGO_DISABLE_COLLATE
// Disable collation functions (part of Case module).
// Reduces the size of Case module by ~100 KB.

//#define UNI_ALGO_DISABLE_NFKC_NFKD
// Disable NFKC and NFKD normalization forms (part of Normalization module).
// These forms are rarely used and can be disabled.
// Reduces the size of Normalization module by ~100 KB.

//#define UNI_ALGO_DISABLE_SYSTEM_LOCALE
// Disable system locale facilities: uni::locale::system() function etc.

//#define UNI_ALGO_DISABLE_SHRINK_TO_FIT
// Most of functions do shrink_to_fit() call at the of a function by default
// but if you use a custom allocator or want to maximize the performance
// it might be better to disable it and do the call manually only when needed.

//#define UNI_ALGO_DISABLE_CPP_ITERATORS
// With this define pointers will be used instead of C++ iterators internally.
// The only reason to use the define is to maximize performance in debug builds,
// for example MSVC debug iterators are very slow.
// Note that the define only affects some functions.
// The define does not affect behaviour.

//#define UNI_ALGO_DISABLE_FULL_CASE
// Note that this define can be deprecated in the future.
// Disable full case mapping and use simple case mapping instead.
// The define is only usefull if you need compatibility with legacy implementations
// that don't expect that an UTF-16 string can grow in size.
// Note that UTF-8 string still can grow in size even with simple case mapping.
// Can be used to achive the maximum performance when you are 100% sure that you
// will be using the library with languages that don't need full case mapping.
// In other words the define must be avoided at all cost.
// The define affects only Case module.

//#define UNI_ALGO_STATIC_DATA
// Note that with CMake use UNI_ALGO_HEADER_ONLY=ON option instead.
// The define can be used to build header-only version of the library.

//#define UNI_ALGO_DLL_EXPORT
//#define UNI_ALGO_DLL_IMPORT
// Note that with CMake use BUILD_SHARED_LIBS=ON option instead.
// The defines can be used to build shared library
// export must be defined when building and import when using.

// DO NOT CHANGE ANYTHING BELOW THIS LINE

#if (__cplusplus < 201703L && !defined(_MSVC_LANG)) || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
#error "C++17 or better is required"
#endif

// All the comments below are not for users of the library

namespace uni::detail {
// Never change these types except for tests.
using type_codept = char32_t;
using type_char8  = unsigned char;
using type_char16 = char16_t;
using type_char32 = char32_t;
inline constexpr size_t impl_npos = static_cast<size_t>(-1);
inline constexpr std::nullptr_t impl_nullptr = nullptr;
// The types can be changed for tests without violating these static asserts.
// Note that there will be many conversion warnings because the wrapper is not designed
// to silent them in such mode but still all tests must pass even if all types are unsigned long long.
// With this we almost guarantee that it will work when all types will be actually long long.
static_assert(std::is_unsigned<type_codept>::value && sizeof(type_codept) >= sizeof(char32_t));
static_assert(std::is_unsigned<type_char8>::value);
static_assert(std::is_unsigned<type_char16>::value && sizeof(type_char16) >= sizeof(char16_t));
static_assert(std::is_unsigned<type_char32>::value && sizeof(type_char32) >= sizeof(char32_t));
// Note that the the library supports CHAR_BIT more than 8-bit perfectly fine.
// Such platforms are rare so it is just mentioned here.
//static_assert(std::numeric_limits<unsigned char>::max() >= 255); // Always true
}

// Define namespace that low-level will use
#define UNI_ALGO_IMPL_NAMESPACE_BEGIN namespace uni::detail {
#define UNI_ALGO_IMPL_NAMESPACE_END }

// Define dllexport/dllimport for shared library
#if defined(UNI_ALGO_DLL_EXPORT) && !defined(UNI_ALGO_STATIC_DATA)
#  if defined(_WIN32)
#    define UNI_ALGO_DLL __declspec(dllexport)
#  elif defined(__GNUC__) || defined(__clang__)
#    define UNI_ALGO_DLL __attribute__((visibility("default")))
#  endif
#elif defined(UNI_ALGO_DLL_IMPORT) && !defined(UNI_ALGO_STATIC_DATA)
#  if defined(_WIN32)
#    define UNI_ALGO_DLL __declspec(dllimport)
#  elif defined(__GNUC__) || defined(__clang__)
#    define UNI_ALGO_DLL __attribute__((visibility("default")))
#  endif
#endif
#ifndef UNI_ALGO_DLL
#define UNI_ALGO_DLL
#endif

// TODO: Disable constexpr for now it doesn't work properly in some cases
#define uaiw_constexpr //constexpr

//#define UNI_ALGO_EXPERIMENTAL // Enable experimental stuff for tests
//#define UNI_ALGO_LOG_CPP_ITER // Enable logging for UTF-8/16 iterators

// Can be enabled for testing and debugging aid and should be disabled in stable releases.
// Never ever rely on asserts! Use the define together with -D_GLIBCXX_DEBUG but do not rely on this either.
//#define UNI_ALGO_TEST_ASSERT
#ifdef UNI_ALGO_TEST_ASSERT
#include <cassert>
#define uaix_assert assert
#endif

// Other test defines
//#define UNI_ALGO_TEST_DISABLE_ALWAYS_INLINE_GLOBAL
//#define UNI_ALGO_TEST_DISABLE_ALWAYS_INLINE_GLOBAL_EX
//#include <stdexcept>
//#define UNI_ALGO_TEST_CPP_THROW_ON_ILL_FORMED
//#define UNI_ALGO_TEST_CPP_THROW_ASSERT

#endif // UNI_ALGO_CONFIG_H_UAIH
