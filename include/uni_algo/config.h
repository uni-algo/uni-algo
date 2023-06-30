/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef UNI_ALGO_CONFIG_H_UAIH
#define UNI_ALGO_CONFIG_H_UAIH

// Note that you can just add the following defines to your project
// instead of uncommenting the defines here.

//#define UNI_ALGO_DISABLE_CASE
// Disable Case module.
// Reduces Unicode data size by ~200 KB.

//#define UNI_ALGO_DISABLE_NORM
// Disable Normalization module.
// Reduces Unicode data size by ~200 KB.

//#define UNI_ALGO_DISABLE_PROP
// Disable Code Point Properties module.
// Reduces Unicode data size by ~35 KB.
// Note that if this module is disabled unaccent functions
// in Normalization module will be disabled too.

//#define UNI_ALGO_DISABLE_SCRIPT
// Disable Script module.
// Reduces Unicode data size by ~70 KB.

//#define UNI_ALGO_DISABLE_SEGMENT_GRAPHEME
// Disable Grapheme segmentation module.
// Reduces Unicode data size by ~25 KB.

//#define UNI_ALGO_DISABLE_SEGMENT_WORD
// Disable Word segmentation module.
// Reduces Unicode data size by ~35 KB.
// Note that if Word segmentation module is disabled title case functions
// in Case module will be disabled too because it is needed for them.

//#define UNI_ALGO_DISABLE_COLLATE
// Disable collation functions (part of Case module).
// Reduces Unicode data size by ~100 KB.

//#define UNI_ALGO_DISABLE_NFKC_NFKD
// Disable NFKC and NFKD normalization forms (part of Normalization module).
// These forms are rarely used and can be disabled.
// Reduces Unicode data size by ~100 KB.

// ----------------------- ALL DEFINES BELOW THIS LINE DO NOT CHANGE UNICODE DATA SIZE ----------------------

//#define UNI_ALGO_ENABLE_SAFE_LAYER
// Force to enable safe layer even in release (enabled in debug by default).

//#define UNI_ALGO_DISABLE_SYSTEM_LOCALE
// Disable system locale facilities: una::locale::system() function etc.

//#define UNI_ALGO_NO_SHRINK_TO_FIT
// Most of functions do shrink_to_fit() call at the end of a function by default
// but if you use a custom allocator or want to maximize the performance of the library
// it might be better to disable it and do the call manually only when needed.
// Note that ranges (una::ranges::to_utf8/to_utf16 and such) never do shring_to_fit() call.

//#define UNI_ALGO_STATIC_DATA
// Note that with CMake use UNI_ALGO_HEADER_ONLY=ON option instead.
// The define can be used for header-only version of the library.
// Also even when the library was built and installed with
// CMake option UNI_ALGO_HEADER_ONLY=OFF the define can be used to
// force header-only version, but treat it as a workaround in this case.

//#define UNI_ALGO_DLL_EXPORT
//#define UNI_ALGO_DLL_IMPORT
// Note that with CMake use BUILD_SHARED_LIBS=ON option instead.
// The defines can be used to build shared library
// export must be defined when building and import when using.

// --------------------------------- DO NOT CHANGE ANYTHING BELOW THIS LINE ---------------------------------

// All the comments below are not for users of the library

// This file must include only these 2 files
// Low-level requires <cstddef> for size_t and such, static asserts here require <type_traits>
// Note that most low-level modules also need type_array to be defined it's located in safe layer
// that need to be included after config.h (this file) for such low-level modules.
// Note that one extra include <version> below may be used in C++20 for proper constexpr detection.
#include <cstddef> // size_t, nullptr_t
#include <type_traits> // std::is_unsigned

#if (__cplusplus < 201703L && !defined(_MSVC_LANG)) || (defined(_MSVC_LANG) && _MSVC_LANG < 201703L)
#error "C++17 or higher is required"
#endif

namespace una::detail {
// Never change these types except for tests.
using type_codept = char32_t;
using type_char8  = unsigned char;
using type_char16 = char16_t;
using type_char32 = char32_t;
//using type_array = [std::array like]; // Defined in internal/safe_layer.h
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
} // namespace una::detail

// Define namespace that low-level will use
#define UNI_ALGO_IMPL_NAMESPACE_BEGIN namespace una::detail {
#define UNI_ALGO_IMPL_NAMESPACE_END }

// Enable safe layer if it is forced with this define and in debug
#ifndef UNI_ALGO_ENABLE_SAFE_LAYER
#ifndef NDEBUG
#define UNI_ALGO_ENABLE_SAFE_LAYER
#endif
#endif

// Using any of the followings defines will disable a part of safe layer.
// Use them only for perf testing to make sure safe layer doesn't mess things up.
//#define UNI_ALGO_FORCE_CPP_ITERATORS // Force to use C++ iterators in low-level
//#define UNI_ALGO_FORCE_C_POINTERS // Force to use C pointers in low-level
//#define UNI_ALGO_FORCE_C_ARRAYS // Force to use C arrays in low-level

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

// C++20 or higher and header-only version is required for constexpr library
#if defined(UNI_ALGO_STATIC_DATA) && (__cplusplus >= 202002L || _MSVC_LANG >= 202002L)
// NOTE: This include is needed for __cpp_lib_constexpr_string below
#include <version>
#if (__cpp_constexpr >= 201907L) && defined(__cpp_lib_constexpr_string) \
    && !(defined(__clang__) && defined(__GLIBCXX__)) // constexpr standard lib is broken in Clang with libstdc++
#define UNI_ALGO_CONSTEXPR
#define uaiw_constexpr constexpr
#endif
#endif
#ifndef UNI_ALGO_CONSTEXPR
#define uaiw_constexpr
#endif

//#define UNI_ALGO_EXPERIMENTAL // Enable experimental stuff for tests
//#define UNI_ALGO_LOG_CPP_ITER // Enable logging for UTF-8/16 iterators

// Disable full case mapping and use simple case mapping instead in Case module
//#define UNI_ALGO_DISABLE_FULL_CASE

// Can be enabled for testing and debugging aid and must be disabled in stable releases.
// The define must only be used by the low-level and it must never rely on asserts.
//#define UNI_ALGO_TEST_ASSERT
#ifdef UNI_ALGO_TEST_ASSERT
#include <cassert>
#define uaix_assert assert
#endif

// Other test defines
//#define UNI_ALGO_TEST_DISABLE_ALWAYS_INLINE_GLOBAL
//#define UNI_ALGO_TEST_DISABLE_ALWAYS_INLINE_GLOBAL_EX

#endif // UNI_ALGO_CONFIG_H_UAIH
