/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_CASE_H_UAIH
#define UNI_ALGO_CASE_H_UAIH

#ifdef UNI_ALGO_DISABLE_CASE
#error "Case module is disabled via define UNI_ALGO_DISABLE_CASE"
#endif

#include <type_traits>
#include <string>
#include <string_view>
#include <cassert>

#include "config.h"
#include "version.h"
#include "internal/safe_layer.h"
#include "internal/search.h"

// Clang-Tidy thinks that locale.h form C is included here
// NOLINTNEXTLINE(modernize-deprecated-headers, hicpp-deprecated-headers)
#include "locale.h"

#include "impl/impl_case.h"
#ifndef UNI_ALGO_DISABLE_FULL_CASE
#include "impl/impl_case_locale.h"
#endif

namespace uni {

namespace detail {

template<typename Dst, typename Alloc, typename Src, size_t SizeX,
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    size_t(*FnMap)(typename Src::const_iterator, typename Src::const_iterator, typename Dst::iterator, int, char32_t)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    size_t(*FnMap)(typename Src::const_pointer, typename Src::const_pointer, typename Dst::pointer, int, char32_t)>
#else // Safe layer
    size_t(*FnMap)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, safe::out<typename Dst::pointer>, int, char32_t)>
#endif
Dst t_map(const Alloc& alloc, Src src, int mode, char32_t loc = 0)
{
    Dst dst{alloc};

    std::size_t length = src.size();

    if (length)
    {
        if (length > dst.max_size() / SizeX) // Overflow protection
        {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || (_HAS_EXCEPTIONS != 0)
            throw std::bad_alloc();
#else
            std::abort();
#endif
        }

        dst.resize(length * SizeX);
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
        dst.resize(FnMap(src.cbegin(), src.cend(), dst.begin(), mode, loc));
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
        dst.resize(FnMap(src.data(), src.data() + src.size(), dst.data(), mode, loc));
#else // Safe layer
        dst.resize(FnMap(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, safe::out{dst.data(), dst.size()}, mode, loc));
#endif

#ifndef UNI_ALGO_DISABLE_SHRINK_TO_FIT
        dst.shrink_to_fit();
#endif
    }

    return dst;
}

} // namespace detail

namespace cases {

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_lowercase_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_map_utf8, detail::impl_case_map_loc_utf8>(alloc, source,
            detail::impl_case_map_mode_lowercase);
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_lowercase_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_map_utf16, detail::impl_case_map_loc_utf16>(alloc, source,
            detail::impl_case_map_mode_lowercase);
}
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_uppercase_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_map_utf8, detail::impl_case_map_loc_utf8>(alloc, source,
            detail::impl_case_map_mode_uppercase);
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_uppercase_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_map_utf16, detail::impl_case_map_loc_utf16>(alloc, source,
            detail::impl_case_map_mode_uppercase);
}
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_casefold_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_map_utf8, detail::impl_case_map_loc_utf8>(alloc, source,
            detail::impl_case_map_mode_casefold);
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_casefold_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_map_utf16, detail::impl_case_map_loc_utf16>(alloc, source,
            detail::impl_case_map_mode_casefold);
}

inline std::string to_lowercase_utf8(std::string_view source)
{
    return to_lowercase_utf8<char>(source);
}
inline std::string to_uppercase_utf8(std::string_view source)
{
    return to_uppercase_utf8<char>(source);
}
inline std::string to_casefold_utf8(std::string_view source)
{
    return to_casefold_utf8<char>(source);
}
inline std::u16string to_lowercase_utf16(std::u16string_view source)
{
    return to_lowercase_utf16<char16_t>(source);
}
inline std::u16string to_uppercase_utf16(std::u16string_view source)
{
    return to_uppercase_utf16<char16_t>(source);
}
inline std::u16string to_casefold_utf16(std::u16string_view source)
{
    return to_casefold_utf16<char16_t>(source);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring to_lowercase_utf16(std::wstring_view source)
{
    return to_lowercase_utf16<wchar_t>(source);
}
inline std::wstring to_uppercase_utf16(std::wstring_view source)
{
    return to_uppercase_utf16<wchar_t>(source);
}
inline std::wstring to_casefold_utf16(std::wstring_view source)
{
    return to_casefold_utf16<wchar_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifndef UNI_ALGO_DISABLE_FULL_CASE
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_lowercase_utf8(std::basic_string_view<UTF8> source, const uni::locale& locale, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_map_utf8, detail::impl_case_map_locale_utf8>(alloc, source,
            detail::impl_case_map_mode_lowercase, static_cast<char32_t>(locale.get_language()));
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_lowercase_utf16(std::basic_string_view<UTF16> source, const uni::locale& locale, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_map_utf16, detail::impl_case_map_locale_utf16>(alloc, source,
            detail::impl_case_map_mode_lowercase, static_cast<char32_t>(locale.get_language()));
}
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_uppercase_utf8(std::basic_string_view<UTF8> source, const uni::locale& locale, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_map_utf8, detail::impl_case_map_locale_utf8>(alloc, source,
            detail::impl_case_map_mode_uppercase, static_cast<char32_t>(locale.get_language()));
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_uppercase_utf16(std::basic_string_view<UTF16> source, const uni::locale& locale, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_map_utf16, detail::impl_case_map_locale_utf16>(alloc, source,
            detail::impl_case_map_mode_uppercase, static_cast<char32_t>(locale.get_language()));
}
inline std::string to_lowercase_utf8(std::string_view source, const uni::locale& locale)
{
    return to_lowercase_utf8<char>(source, locale);
}
inline std::string to_uppercase_utf8(std::string_view source, const uni::locale& locale)
{
    return to_uppercase_utf8<char>(source, locale);
}
inline std::u16string to_lowercase_utf16(std::u16string_view source, const uni::locale& locale)
{
    return to_lowercase_utf16<char16_t>(source, locale);
}
inline std::u16string to_uppercase_utf16(std::u16string_view source, const uni::locale& locale)
{
    return to_uppercase_utf16<char16_t>(source, locale);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring to_lowercase_utf16(std::wstring_view source, const uni::locale& locale)
{
    return to_lowercase_utf16<wchar_t>(source, locale);
}
inline std::wstring to_uppercase_utf16(std::wstring_view source, const uni::locale& locale)
{
    return to_uppercase_utf16<wchar_t>(source, locale);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#endif // UNI_ALGO_DISABLE_FULL_CASE

#ifndef UNI_ALGO_DISABLE_BREAK_WORD
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_titlecase_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_map_utf8, detail::impl_case_map_loc_utf8>(alloc, source,
            detail::impl_case_map_mode_titlecase);
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_titlecase_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_map_utf16, detail::impl_case_map_loc_utf16>(alloc, source,
            detail::impl_case_map_mode_titlecase);
}
inline std::string to_titlecase_utf8(std::string_view source)
{
    return to_titlecase_utf8<char>(source);
}
inline std::u16string to_titlecase_utf16(std::u16string_view source)
{
    return to_titlecase_utf16<char16_t>(source);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring to_titlecase_utf16(std::wstring_view source)
{
    return to_titlecase_utf16<wchar_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#ifndef UNI_ALGO_DISABLE_FULL_CASE
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_titlecase_utf8(std::basic_string_view<UTF8> source, const uni::locale& locale, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_map_utf8, detail::impl_case_map_locale_utf8>(alloc, source,
            detail::impl_case_map_mode_titlecase, static_cast<char32_t>(locale.get_language()));
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_titlecase_utf16(std::basic_string_view<UTF16> source, const uni::locale& locale, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_map_utf16, detail::impl_case_map_locale_utf16>(alloc, source,
            detail::impl_case_map_mode_titlecase, static_cast<char32_t>(locale.get_language()));
}
inline std::string to_titlecase_utf8(std::string_view source, const uni::locale& locale)
{
    return to_titlecase_utf8<char>(source, locale);
}
inline std::u16string to_titlecase_utf16(std::u16string_view source, const uni::locale& locale)
{
    return to_titlecase_utf16<char16_t>(source, locale);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring to_titlecase_utf16(std::wstring_view source, const uni::locale& locale)
{
    return to_titlecase_utf16<wchar_t>(source, locale);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD

} // namespace cases

namespace casesens {

template<typename UTF8>
int compare_utf8(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
    static_assert(std::is_integral_v<UTF8>);

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_compare_utf8(string1.cbegin(), string1.cend(),
                                          string2.cbegin(), string2.cend(), false);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_compare_utf8(string1.data(), string1.data() + string1.size(),
                                          string2.data(), string2.data() + string2.size(), false);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_compare_utf8(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                          safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, false);
#endif
}

template<typename UTF16>
int compare_utf16(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_compare_utf16(string1.cbegin(), string1.cend(),
                                           string2.cbegin(), string2.cend(), false);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_compare_utf16(string1.data(), string1.data() + string1.size(),
                                           string2.data(), string2.data() + string2.size(), false);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_compare_utf16(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                           safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, false);
#endif
}

#ifndef UNI_ALGO_DISABLE_COLLATE
template<typename UTF8>
int collate_utf8(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
    static_assert(std::is_integral_v<UTF8>);

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_collate_utf8(string1.cbegin(), string1.cend(),
                                          string2.cbegin(), string2.cend(), false);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_collate_utf8(string1.data(), string1.data() + string1.size(),
                                          string2.data(), string2.data() + string2.size(), false);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_collate_utf8(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                          safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, false);
#endif
}

template<typename UTF16>
int collate_utf16(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_collate_utf16(string1.cbegin(), string1.cend(),
                                           string2.cbegin(), string2.cend(), false);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_collate_utf16(string1.data(), string1.data() + string1.size(),
                                           string2.data(), string2.data() + string2.size(), false);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_collate_utf16(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                           safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, false);
#endif
}
#endif // UNI_ALGO_DISABLE_COLLATE

template<typename UTF8>
uni::search search_utf8(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
    static_assert(std::is_integral_v<UTF8>);

    size_t pos = detail::impl_npos, end = detail::impl_npos;
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    bool ret = detail::impl_case_search_utf8(string1.cbegin(), string1.cend(),
                                             string2.cbegin(), string2.cend(), false, &pos, &end);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    bool ret = detail::impl_case_search_utf8(string1.data(), string1.data() + string1.size(),
                                             string2.data(), string2.data() + string2.size(), false, &pos, &end);
#else // Safe layer
    namespace safe = detail::safe;
    bool ret = detail::impl_case_search_utf8(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                             safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, false, &pos, &end);
#endif

    return uni::search{ret, pos, end};
}
template<typename UTF16>

uni::search search_utf16(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    size_t pos = detail::impl_npos, end = detail::impl_npos;
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    bool ret = detail::impl_case_search_utf16(string1.cbegin(), string1.cend(),
                                              string2.cbegin(), string2.cend(), false, &pos, &end);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    bool ret = detail::impl_case_search_utf16(string1.data(), string1.data() + string1.size(),
                                              string2.data(), string2.data() + string2.size(), false, &pos, &end);
#else // Safe layer
    namespace safe = detail::safe;
    bool ret = detail::impl_case_search_utf16(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                              safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, false, &pos, &end);
#endif

    return uni::search{ret, pos, end};
}

inline int compare_utf8(std::string_view string1, std::string_view string2)
{
    return compare_utf8<char>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int collate_utf8(std::string_view string1, std::string_view string2)
{
    return collate_utf8<char>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search search_utf8(std::string_view string1, std::string_view string2)
{
    return search_utf8<char>(string1, string2);
}
inline int compare_utf16(std::u16string_view string1, std::u16string_view string2)
{
    return compare_utf16<char16_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int collate_utf16(std::u16string_view string1, std::u16string_view string2)
{
    return collate_utf16<char16_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search search_utf16(std::u16string_view string1, std::u16string_view string2)
{
    return search_utf16<char16_t>(string1, string2);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline int compare_utf16(std::wstring_view string1, std::wstring_view string2)
{
    return compare_utf16<wchar_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int collate_utf16(std::wstring_view string1, std::wstring_view string2)
{
    return collate_utf16<wchar_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search search_utf16(std::wstring_view string1, std::wstring_view string2)
{
    return search_utf16<wchar_t>(string1, string2);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef UNI_ALGO_EXPERIMENTAL
#ifndef UNI_ALGO_DISABLE_COLLATE
template<typename UTF8, typename Alloc = std::allocator<char>>
std::basic_string<char, std::char_traits<char>, Alloc>
sortkey_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<char, std::char_traits<char>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_sortkey_utf8, detail::impl_case_sortkey_loc_utf8>(alloc, source, false);
}
template<typename UTF16, typename Alloc = std::allocator<char>>
std::basic_string<char, std::char_traits<char>, Alloc>
sortkey_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<char, std::char_traits<char>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_sortkey_utf16, detail::impl_case_sortkey_loc_utf16>(alloc, source, false);
}
inline std::string sortkey_utf8(std::string_view source)
{
    return sortkey_utf8<char>(source);
}
inline std::string sortkey_utf16(std::u16string_view source)
{
    return sortkey_utf16<char16_t>(source);
}
inline std::string sortkey_utf16(std::wstring_view source)
{
    return sortkey_utf16<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_COLLATE
#endif // UNI_ALGO_EXPERIMENTAL

} // namespace casesens

namespace caseless {

template<typename UTF8>
int compare_utf8(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
    static_assert(std::is_integral_v<UTF8>);

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_compare_utf8(string1.cbegin(), string1.cend(),
                                          string2.cbegin(), string2.cend(), true);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_compare_utf8(string1.data(), string1.data() + string1.size(),
                                          string2.data(), string2.data() + string2.size(), true);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_compare_utf8(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                          safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, true);
#endif
}

template<typename UTF16>
int compare_utf16(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_compare_utf16(string1.cbegin(), string1.cend(),
                                           string2.cbegin(), string2.cend(), true);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_compare_utf16(string1.data(), string1.data() + string1.size(),
                                           string2.data(), string2.data() + string2.size(), true);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_compare_utf16(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                           safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, true);
#endif
}

#ifndef UNI_ALGO_DISABLE_COLLATE
template<typename UTF8>
int collate_utf8(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
    static_assert(std::is_integral_v<UTF8>);

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_collate_utf8(string1.cbegin(), string1.cend(),
                                          string2.cbegin(), string2.cend(), true);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_collate_utf8(string1.data(), string1.data() + string1.size(),
                                          string2.data(), string2.data() + string2.size(), true);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_collate_utf8(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                          safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, true);
#endif
}

template<typename UTF16>
int collate_utf16(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_collate_utf16(string1.cbegin(), string1.cend(),
                                           string2.cbegin(), string2.cend(), true);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_collate_utf16(string1.data(), string1.data() + string1.size(),
                                           string2.data(), string2.data() + string2.size(), true);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_collate_utf16(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                           safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, true);
#endif
}
#endif // UNI_ALGO_DISABLE_COLLATE

template<typename UTF8>
uni::search search_utf8(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
    static_assert(std::is_integral_v<UTF8>);

    size_t pos = detail::impl_npos, end = detail::impl_npos;
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    bool ret = detail::impl_case_search_utf8(string1.cbegin(), string1.cend(),
                                             string2.cbegin(), string2.cend(), true, &pos, &end);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    bool ret = detail::impl_case_search_utf8(string1.data(), string1.data() + string1.size(),
                                             string2.data(), string2.data() + string2.size(), true, &pos, &end);
#else // Safe layer
    namespace safe = detail::safe;
    bool ret = detail::impl_case_search_utf8(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                             safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, true, &pos, &end);
#endif

    return uni::search{ret, pos, end};
}

template<typename UTF16>
uni::search search_utf16(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    size_t pos = detail::impl_npos, end = detail::impl_npos;
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    bool ret = detail::impl_case_search_utf16(string1.cbegin(), string1.cend(),
                                              string2.cbegin(), string2.cend(), true, &pos, &end);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    bool ret = detail::impl_case_search_utf16(string1.data(), string1.data() + string1.size(),
                                              string2.data(), string2.data() + string2.size(), true, &pos, &end);
#else // Safe layer
    namespace safe = detail::safe;
    bool ret = detail::impl_case_search_utf16(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                              safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, true, &pos, &end);
#endif

    return uni::search{ret, pos, end};
}

inline int compare_utf8(std::string_view string1, std::string_view string2)
{
    return compare_utf8<char>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int collate_utf8(std::string_view string1, std::string_view string2)
{
    return collate_utf8<char>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search search_utf8(std::string_view string1, std::string_view string2)
{
    return search_utf8<char>(string1, string2);
}
inline int compare_utf16(std::u16string_view string1, std::u16string_view string2)
{
    return compare_utf16<char16_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int collate_utf16(std::u16string_view string1, std::u16string_view string2)
{
    return collate_utf16<char16_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search search_utf16(std::u16string_view string1, std::u16string_view string2)
{
    return search_utf16<char16_t>(string1, string2);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline int compare_utf16(std::wstring_view string1, std::wstring_view string2)
{
    return compare_utf16<wchar_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int collate_utf16(std::wstring_view string1, std::wstring_view string2)
{
    return collate_utf16<wchar_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search search_utf16(std::wstring_view string1, std::wstring_view string2)
{
    return search_utf16<wchar_t>(string1, string2);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef UNI_ALGO_EXPERIMENTAL

#ifndef UNI_ALGO_DISABLE_COLLATE
template<typename UTF8, typename Alloc = std::allocator<char>>
std::basic_string<char, std::char_traits<char>, Alloc>
sortkey_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_map<std::basic_string<char, std::char_traits<char>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_case_sortkey_utf8, detail::impl_case_sortkey_loc_utf8>(alloc, source, true);
}
template<typename UTF16, typename Alloc = std::allocator<char>>
std::basic_string<char, std::char_traits<char>, Alloc>
sortkey_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<char, std::char_traits<char>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_case_sortkey_utf16, detail::impl_case_sortkey_loc_utf16>(alloc, source, true);
}
inline std::string sortkey_utf8(std::string_view source)
{
    return sortkey_utf8<char>(source);
}
inline std::string sortkey_utf16(std::u16string_view source)
{
    return sortkey_utf16<char16_t>(source);
}
inline std::string sortkey_utf16(std::wstring_view source)
{
    return sortkey_utf16<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_COLLATE

template<typename UTF8>
bool like_utf8(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2, char32_t escape = 0)
{
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return detail::impl_case_like_utf8(string1.cbegin(), string1.cend(),
                                       string2.cbegin(), string2.cend(), true, '%', '_', escape);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return detail::impl_case_like_utf8(string1.data(), string1.data() + string1.size(),
                                       string2.data(), string2.data() + string2.size(), true, '%', '_', escape);
#else // Safe layer
    namespace safe = detail::safe;
    return detail::impl_case_like_utf8(safe::in{string1.data(), string1.size()}, safe::end{string1.data() + string1.size()},
                                       safe::in{string2.data(), string2.size()}, safe::end{string2.data() + string2.size()}, true, '%', '_', escape);
#endif
}
inline bool like_utf8(std::string_view string1, std::string_view string2, char32_t escape = 0)
{
    return like_utf8<char>(string1, string2, escape);
}

#endif // UNI_ALGO_EXPERIMENTAL

} // namespace caseless

#ifdef __cpp_lib_char8_t

namespace cases {

inline std::u8string to_lowercase_utf8(std::u8string_view source)
{
    return to_lowercase_utf8<char8_t>(source);
}
inline std::u8string to_uppercase_utf8(std::u8string_view source)
{
    return to_uppercase_utf8<char8_t>(source);
}
inline std::u8string to_casefold_utf8(std::u8string_view source)
{
    return to_casefold_utf8<char8_t>(source);
}
inline std::u8string to_lowercase_utf8(std::u8string_view source, const uni::locale& locale)
{
    return to_lowercase_utf8<char8_t>(source, locale);
}
inline std::u8string to_uppercase_utf8(std::u8string_view source, const uni::locale& locale)
{
    return to_uppercase_utf8<char8_t>(source, locale);
}
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
inline std::u8string to_titlecase_utf8(std::u8string_view source)
{
    return to_titlecase_utf8<char8_t>(source);
}
inline std::u8string to_titlecase_utf8(std::u8string_view source, const uni::locale& locale)
{
    return to_titlecase_utf8<char8_t>(source, locale);
}
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD

} // namespace cases

namespace casesens {

inline int compare_utf8(std::u8string_view string1, std::u8string_view string2)
{
    return compare_utf8<char8_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int collate_utf8(std::u8string_view string1, std::u8string_view string2)
{
    return collate_utf8<char8_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search search_utf8(std::u8string_view string1, std::u8string_view string2)
{
    return search_utf8<char8_t>(string1, string2);
}

} // namespace casesens

namespace caseless {

inline int compare_utf8(std::u8string_view string1, std::u8string_view string2)
{
    return compare_utf8<char8_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int collate_utf8(std::u8string_view string1, std::u8string_view string2)
{
    return collate_utf8<char8_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search search_utf8(std::u8string_view string1, std::u8string_view string2)
{
    return search_utf8<char8_t>(string1, string2);
}

} // namespace caseless

#endif // __cpp_lib_char8_t

// ----------
// PROPERTIES
// ----------

namespace codepoint {

class prop_case
{
private:
    detail::type_codept data = 0;

public:
    prop_case() = delete;
    explicit prop_case(char32_t c) noexcept : data{detail::impl_case_get_prop(c)} {}

    bool Lowercase() const noexcept
    {
        // The Unicode Standard: DerivedCoreProperties.txt -> Lowercase
        return detail::impl_case_is_lowercase_prop(data);
    }
    bool Uppercase() const noexcept
    {
        // The Unicode Standard: DerivedCoreProperties.txt -> Uppercase
        return detail::impl_case_is_uppercase_prop(data);
    }
    bool Cased() const noexcept
    {
        // The Unicode Standard: DerivedCoreProperties.txt -> Cased
        return detail::impl_case_is_cased_prop(data);
    }
    bool Case_Ignorable() const noexcept
    {
        // The Unicode Standard: DerivedCoreProperties.txt -> Case_Ignorable
        return detail::impl_case_is_case_ignorable_prop(data);
    }
    bool Soft_Dotted() const noexcept
    {
        // The Unicode Standard: PropList.txt -> Soft_Dotted
        return detail::impl_case_is_soft_dotted_prop(data);
    }
};

inline bool is_lowercase(char32_t c) noexcept
{
    return prop_case{c}.Lowercase();
}

inline bool is_lowercase(const prop_case& p) noexcept
{
    return p.Lowercase();
}

inline bool is_uppercase(char32_t c) noexcept
{
    return prop_case{c}.Uppercase();
}

inline bool is_uppercase(const prop_case& p) noexcept
{
    return p.Uppercase();
}

inline char32_t to_simple_lowercase(char32_t c) noexcept
{
    return detail::impl_case_to_simple_lowercase(c);
}

inline char32_t to_simple_uppercase(char32_t c) noexcept
{
    return detail::impl_case_to_simple_uppercase(c);
}

inline char32_t to_simple_casefold(char32_t c) noexcept
{
    return detail::impl_case_to_simple_casefold(c);
}

#ifndef UNI_ALGO_DISABLE_BREAK_WORD
inline char32_t to_simple_titlecase(char32_t c) noexcept
{
    return detail::impl_case_to_simple_titlecase(c);
}
#endif // UNI_ALGO_DISABLE_BREAK_WORD

#ifndef UNI_ALGO_DISABLE_FULL_CASE

inline std::u32string to_lowercase_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(3); // TODO: Magic number
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    dst.resize(detail::impl_case_to_lowercase(c, dst.data()));
#else
    dst.resize(detail::impl_case_to_lowercase(c, dst.begin()));
#endif
    return dst;
}

inline std::u32string to_uppercase_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(3); // TODO: Magic number
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    dst.resize(detail::impl_case_to_uppercase(c, dst.data()));
#else
    dst.resize(detail::impl_case_to_uppercase(c, dst.begin()));
#endif
    return dst;
}

inline std::u32string to_casefold_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(3); // TODO: Magic number
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    dst.resize(detail::impl_case_to_casefold(c, dst.data()));
#else
    dst.resize(detail::impl_case_to_casefold(c, dst.begin()));
#endif
    return dst;
}

#ifndef UNI_ALGO_DISABLE_BREAK_WORD
inline std::u32string to_titlecase_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(3); // TODO: Magic number
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    dst.resize(detail::impl_case_to_titlecase(c, dst.data()));
#else
    dst.resize(detail::impl_case_to_titlecase(c, dst.begin()));
#endif
    return dst;
}
#endif // UNI_ALGO_DISABLE_BREAK_WORD

#endif // UNI_ALGO_DISABLE_FULL_CASE

} // namespace codepoint

} // namespace uni

#endif // UNI_ALGO_CASE_H_UAIH

/* Public Domain License
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
