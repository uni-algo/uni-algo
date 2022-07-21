/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See LICENSE.txt */

// Must be included to define and check internal types in C wrapper.
// Must never be included by a C++ wrapper.

#ifndef __cplusplus
// This part is only for C

// typedef something type_char8 // Can be whatever
// typedef something type_char16 // Can be 16-bit or more signed/unsigned
// typedef something type_char32 // Can be 32-bit or more signed/unsigned

// type_codept is used for code point calculations must be 32-bit or more unsigned only
// typedef something type_codept

// Use this in a wrapper to initialize position variables instead of -1
#define impl_npos ((size_t)-1)
//static const size_t impl_npos = (size_t)-1; // Test. Low-level must compile even if it's defined like this

// Use this in a wrapper for consistency instead of NULL or 0 or whatever
// C Standard: 6.3.2.3 Pointers
// An integer constant expression with the value 0, or such an expression cast to type void *,
// is called a null pointer constant.
#define impl_nullptr ((void*)0)
//static void* impl_nullptr = (void*)0; // Test. Low-level must compile even if it's defined like this
// Fun fact ((void*)0) technically is not null pointer constant in C but it's just a flaw in C standard
// https://stackoverflow.com/questions/26477209/is-void0-a-null-pointer-constant

// By doing sizeof(short) it's guaranteed that these static asserts work with any CHAR_BIT
// because short is always 16-bit or more on all platforms but for example int or long have
// different sizeof on different platforms and cannot be used for the static asserts.

// type_char16 can be 16-bit or more signed/unsigned
typedef int static_assert_char16[sizeof(type_char16) >= sizeof(short) ? 1 : -1];

// type_char32 can be 32-bit or more signed/unsigned
typedef int static_assert_char32[sizeof(type_char32) >= sizeof(short) * 2 ? 1 : -1];

// type_codept can be 32-bit or more unsigned only
typedef int static_assert_codept[sizeof(type_codept) >= sizeof(short) * 2 && (type_codept)-1 > 0 ? 1 : -1];

// Define C pointer based internal iterators
typedef const type_char8*  it_in_utf8;
typedef const type_char16* it_in_utf16;
typedef const type_char32* it_in_utf32;
typedef const type_char8*  it_end_utf8;
typedef const type_char16* it_end_utf16;
typedef const type_char32* it_end_utf32;
typedef type_char8*        it_out_utf8;
typedef type_char16*       it_out_utf16;
typedef type_char32*       it_out_utf32;

#define UNI_ALGO_IMPL_NAMESPACE_BEGIN
#define UNI_ALGO_IMPL_NAMESPACE_END
#endif

/* This file is not used in C++ wrapper but it's here for a reference.
 * In C++ the types must be defined by a wrapper.
 * Note that all the types must be unsigned in C++ because the implementation
 * knows nothing about iterator types and signed casts might alter values.
 * Note that in C++ the types (except type_codept) are used only to
 * supress warnings by casts but the casts still can alter values.
 * See: test/test_extra.h -> test_alter_value()
 * This means the types must be checked with static asserts like this: */
#if 0
#ifdef __cplusplus
namespace uni { namespace detail {
using type_codept = char32_t;
using type_char8  = unsigned char;
using type_char16 = char16_t;
using type_char32 = char32_t;
const size_t impl_npos = static_cast<size_t>(-1); // the same as const size_t impl_npos = std::numeric_limits<size_t>::max();
const std::nullptr_t impl_nullptr = nullptr;
static_assert(std::is_unsigned<type_codept>::value && sizeof(type_codept) >= sizeof(char32_t));
static_assert(std::is_unsigned<type_char8>::value);
static_assert(std::is_unsigned<type_char16>::value && sizeof(type_char16) >= sizeof(char16_t));
static_assert(std::is_unsigned<type_char32>::value && sizeof(type_char32) >= sizeof(char32_t));
// This is almost the same as this in C:
//typedef unsigned int type_codept
//typedef unsigned char type_char8
//typedef unsigned short type_char16
//typedef unsigned int type_char32
//static const size_t impl_npos = (size_t)-1;
//static void* impl_nullptr = (void*)0;
//typedef int static_assert_char8[(type_char8)-1 > 0 ? 1 : -1];
//typedef int static_assert_char16[sizeof(type_char16) >= sizeof(short) && (type_codept)-1 > 0 ? 1 : -1];
//typedef int static_assert_char32[sizeof(type_char32) >= sizeof(short) * 2 && (type_codept)-1 > 0 ? 1 : -1];
//typedef int static_assert_codept[sizeof(type_codept) >= sizeof(short) * 2 && (type_codept)-1 > 0 ? 1 : -1];
}} // namespace uni::detail
#define UNI_ALGO_IMPL_NAMESPACE_BEGIN namespace uni { namespace detail {
#define UNI_ALGO_IMPL_NAMESPACE_END }}
#endif
#endif // 0
