/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_CASE_H_UAIX
#define CPP_UNI_CASE_H_UAIX

#ifdef UNI_ALGO_DISABLE_CASE
#  error "Case module is disabled via define UNI_ALGO_DISABLE_CASE"
#endif

#include <type_traits>
#include <string>
#include <string_view>
#include <cassert>

#include <uni/config.h>
#include <uni/version.h>

#include <uni/impl/case.h>
#ifndef UNI_ALGO_DISABLE_FULL_CASE
#include <uni/impl/case_locale.h>
#endif

// For info about defines see uni_cpp_convert.h

namespace uni {

#ifndef UNI_ALGO_DISABLE_FULL_CASE

class locale
{
    // TODO: The locale class is just placeholder for now so leave it here
    // because we support locales only for lower, upper and title case functions
    // Implement only what we need for now but in a way so the class can be extented
public:
    enum class language {und = 0, lt, tr, az, el, nl};
private:
    language lang = language::und;
public:
    locale() = default;
    explicit locale(uni::locale::language l) : lang(l) {}
    explicit locale(std::string_view s)
    {
        if (s.size() > 2 && (s[2] == '-' || s[2] == '_'))
            s = s.substr(0, 2);

        if (s.size() != 2) lang = language::und; // NOLINT
        else if (s == "lt") lang = language::lt;
        else if (s == "tr") lang = language::tr;
        else if (s == "az") lang = language::az;
        else if (s == "el") lang = language::el;
        else if (s == "nl") lang = language::nl;
        else lang = language::und; // NOLINT
    }
    language get_lang() const { return lang; }
    friend bool operator==(const locale& lhs, const locale::language& rhs) { return (lhs.lang == rhs); }
    friend bool operator!=(const locale& lhs, const locale::language& rhs) { return (lhs.lang != rhs); }
    friend bool operator==(const locale::language& lhs, const locale& rhs) { return (lhs == rhs.lang); }
    friend bool operator!=(const locale::language& lhs, const locale& rhs) { return (lhs != rhs.lang); }
};

#endif // UNI_ALGO_DISABLE_FULL_CASE

class search
{
public:
    search() noexcept = default;
    explicit search(bool f, std::size_t p, std::size_t e) noexcept
        : found(f), position(p), end_position(e) {}
    explicit operator bool() const noexcept { return found; }
    void reset() { found = false; position = detail::impl_npos; end_position = detail::impl_npos; }
    std::size_t pos() const noexcept { assert(found); return position; }
    std::size_t end_pos() const noexcept { assert(found); return end_position; }
private:
    bool found = false;
    std::size_t position = detail::impl_npos;
    std::size_t end_position = detail::impl_npos;
};

namespace detail {

template<typename DST, typename SRC, size_t SZ,
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    size_t(*FNMAP)(typename SRC::const_pointer, typename SRC::const_pointer, typename DST::pointer, int)>
#else
    size_t(*FNMAP)(typename SRC::const_iterator, typename SRC::const_iterator, typename DST::iterator, int)>
#endif
DST t_map(SRC source, int mode)
{
    DST destination;

    std::size_t length = source.size();

    if (length)
    {
        destination.resize(length * SZ);
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
        destination.resize(FNMAP(source.data(), source.data() + source.size(), destination.data(), mode));
#else
        destination.resize(FNMAP(source.cbegin(), source.cend(), destination.begin(), mode));
#endif
#ifndef UNI_ALGO_DISABLE_CPP_SHRINK_TO_FIT
        destination.shrink_to_fit();
#endif
    }

    return destination;
}

} // namespace detail

namespace cases {

template<typename UTF8>
std::basic_string<UTF8> utf8_lower(std::basic_string_view<UTF8> source)
{
    return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_casemap, detail::impl_utf8_casemap>(source,
            detail::impl_casemap_mode_lower);
}
template<typename UTF16>
std::basic_string<UTF16> utf16_lower(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_casemap, detail::impl_utf16_casemap>(source,
            detail::impl_casemap_mode_lower);
}
template<typename UTF8>
std::basic_string<UTF8> utf8_upper(std::basic_string_view<UTF8> source)
{
    return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_casemap, detail::impl_utf8_casemap>(source,
            detail::impl_casemap_mode_upper);
}
template<typename UTF16>
std::basic_string<UTF16> utf16_upper(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_casemap, detail::impl_utf16_casemap>(source,
            detail::impl_casemap_mode_upper);
}
template<typename UTF8>
std::basic_string<UTF8> utf8_fold(std::basic_string_view<UTF8> source)
{
    return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_casemap, detail::impl_utf8_casemap>(source,
            detail::impl_casemap_mode_fold);
}
template<typename UTF16>
std::basic_string<UTF16> utf16_fold(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_casemap, detail::impl_utf16_casemap>(source,
            detail::impl_casemap_mode_fold);
}

inline std::string utf8_lower(std::string_view source)
{
    return utf8_lower<char>(source);
}
inline std::string utf8_upper(std::string_view source)
{
    return utf8_upper<char>(source);
}
inline std::string utf8_fold(std::string_view source)
{
    return utf8_fold<char>(source);
}
inline std::u16string utf16_lower(std::u16string_view source)
{
    return utf16_lower<char16_t>(source);
}
inline std::u16string utf16_upper(std::u16string_view source)
{
    return utf16_upper<char16_t>(source);
}
inline std::u16string utf16_fold(std::u16string_view source)
{
    return utf16_fold<char16_t>(source);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring utf16_lower(std::wstring_view source)
{
    return utf16_lower<wchar_t>(source);
}
inline std::wstring utf16_upper(std::wstring_view source)
{
    return utf16_upper<wchar_t>(source);
}
inline std::wstring utf16_fold(std::wstring_view source)
{
    return utf16_fold<wchar_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifndef UNI_ALGO_DISABLE_FULL_CASE
template<typename UTF8>
std::basic_string<UTF8> utf8_lower(std::basic_string_view<UTF8> source, const uni::locale& locale)
{
    using language = uni::locale::language;

    switch (locale.get_lang()) {
        case language::lt:
            return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
                    detail::impl_x_utf8_casemap, detail::impl_utf8_casemap_locale>(source,
                    detail::impl_casemap_locale_lower_lt);
        case language::tr:
        case language::az:
            return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
                    detail::impl_x_utf8_casemap, detail::impl_utf8_casemap_locale>(source,
                    detail::impl_casemap_locale_lower_tr_az);
        default:
            return utf8_lower<UTF8>(source);
    }
}
template<typename UTF16>
std::basic_string<UTF16> utf16_lower(std::basic_string_view<UTF16> source, const uni::locale& locale)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    using language = uni::locale::language;

    switch (locale.get_lang()) {
        case language::lt:
            return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
                    detail::impl_x_utf16_casemap, detail::impl_utf16_casemap_locale>(source,
                    detail::impl_casemap_locale_lower_lt);
        case language::tr:
        case language::az:
            return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
                    detail::impl_x_utf16_casemap, detail::impl_utf16_casemap_locale>(source,
                    detail::impl_casemap_locale_lower_tr_az);
        default:
            return utf16_lower<UTF16>(source);
    }

}
template<typename UTF8>
std::basic_string<UTF8> utf8_upper(std::basic_string_view<UTF8> source, const uni::locale& locale)
{
    using language = uni::locale::language;

    switch (locale.get_lang()) {
        case language::lt:
            return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
                    detail::impl_x_utf8_casemap, detail::impl_utf8_casemap_locale>(source,
                    detail::impl_casemap_locale_upper_lt);
        case language::tr:
        case language::az:
            return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
                    detail::impl_x_utf8_casemap, detail::impl_utf8_casemap_locale>(source,
                    detail::impl_casemap_locale_upper_tr_az);
        case language::el:
            return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
                    detail::impl_x_utf8_casemap, detail::impl_utf8_casemap_locale>(source,
                    detail::impl_casemap_locale_upper_el);
        default:
            return utf8_upper<UTF8>(source);
    }
}
template<typename UTF16>
std::basic_string<UTF16> utf16_upper(std::basic_string_view<UTF16> source, const uni::locale& locale)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    using language = uni::locale::language;

    switch (locale.get_lang()) {
        case language::lt:
            return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
                    detail::impl_x_utf16_casemap, detail::impl_utf16_casemap_locale>(source,
                    detail::impl_casemap_locale_upper_lt);
        case language::tr:
        case language::az:
            return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
                    detail::impl_x_utf16_casemap, detail::impl_utf16_casemap_locale>(source,
                    detail::impl_casemap_locale_upper_tr_az);
        case language::el:
            return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
                    detail::impl_x_utf16_casemap, detail::impl_utf16_casemap_locale>(source,
                    detail::impl_casemap_locale_upper_el);
        default:
            return utf16_upper<UTF16>(source);
    }
}
inline std::string utf8_lower(std::string_view source, const uni::locale& locale)
{
    return utf8_lower<char>(source, locale);
}
inline std::string utf8_upper(std::string_view source, const uni::locale& locale)
{
    return utf8_upper<char>(source, locale);
}
inline std::u16string utf16_lower(std::u16string_view source, const uni::locale& locale)
{
    return utf16_lower<char16_t>(source, locale);
}
inline std::u16string utf16_upper(std::u16string_view source, const uni::locale& locale)
{
    return utf16_upper<char16_t>(source, locale);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring utf16_lower(std::wstring_view source, const uni::locale& locale)
{
    return utf16_lower<wchar_t>(source, locale);
}
inline std::wstring utf16_upper(std::wstring_view source, const uni::locale& locale)
{
    return utf16_upper<wchar_t>(source, locale);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#endif // UNI_ALGO_DISABLE_FULL_CASE

#ifndef UNI_ALGO_DISABLE_BREAK_WORD
template<typename UTF8>
std::basic_string<UTF8> utf8_title(std::basic_string_view<UTF8> source)
{
    return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_casemap, detail::impl_utf8_casemap>(source,
            detail::impl_casemap_mode_title);
}
template<typename UTF16>
std::basic_string<UTF16> utf16_title(std::basic_string_view<UTF16> source)
{
    return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_casemap, detail::impl_utf16_casemap>(source,
            detail::impl_casemap_mode_title);
}
inline std::string utf8_title(std::string_view source)
{
    return utf8_title<char>(source);
}
inline std::u16string utf16_title(std::u16string_view source)
{
    return utf16_title<char16_t>(source);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring utf16_title(std::wstring_view source)
{
    return utf16_title<wchar_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#ifndef UNI_ALGO_DISABLE_FULL_CASE
template<typename UTF8>
std::basic_string<UTF8> utf8_title(std::basic_string_view<UTF8> source, const uni::locale& locale)
{
    using language = uni::locale::language;

    switch (locale.get_lang()) {
        case language::lt:
            return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
                    detail::impl_x_utf8_casemap, detail::impl_utf8_casemap_locale>(source,
                    detail::impl_casemap_locale_title_lt);
        case language::tr:
        case language::az:
            return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
                    detail::impl_x_utf8_casemap, detail::impl_utf8_casemap_locale>(source,
                    detail::impl_casemap_locale_title_tr_az);
        case language::nl:
            return detail::t_map<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
                    detail::impl_x_utf8_casemap, detail::impl_utf8_casemap_locale>(source,
                    detail::impl_casemap_locale_title_nl);
        default:
            return utf8_title<UTF8>(source);
    }
}
template<typename UTF16>
std::basic_string<UTF16> utf16_title(std::basic_string_view<UTF16> source, const uni::locale& locale)
{
    using language = uni::locale::language;

    switch (locale.get_lang()) {
        case language::lt:
            return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
                    detail::impl_x_utf16_casemap, detail::impl_utf16_casemap_locale>(source,
                    detail::impl_casemap_locale_title_lt);
        case language::tr:
        case language::az:
            return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
                    detail::impl_x_utf16_casemap, detail::impl_utf16_casemap_locale>(source,
                    detail::impl_casemap_locale_title_tr_az);
        case language::nl:
            return detail::t_map<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
                    detail::impl_x_utf16_casemap, detail::impl_utf16_casemap_locale>(source,
                    detail::impl_casemap_locale_title_nl);
        default:
            return utf16_title<UTF16>(source);
    }
}
inline std::string utf8_title(std::string_view source, const uni::locale& locale)
{
    return utf8_title<char>(source, locale);
}
inline std::u16string utf16_title(std::u16string_view source, const uni::locale& locale)
{
    return utf16_title<char16_t>(source, locale);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring utf16_title(std::wstring_view source, const uni::locale& locale)
{
    return utf16_title<wchar_t>(source, locale);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD

} // namespace cases

namespace casesens {

template<typename UTF8>
int utf8_compare(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_compare(string1.data(), string1.data() + string1.size(),
                                     string2.data(), string2.data() + string2.size(), false);
#else
    return detail::impl_utf8_compare(string1.cbegin(), string1.cend(),
                                     string2.cbegin(), string2.cend(), false);
#endif
}

template<typename UTF16>
int utf16_compare(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf16_compare(string1.data(), string1.data() + string1.size(),
                                      string2.data(), string2.data() + string2.size(), false);
#else
    return detail::impl_utf16_compare(string1.cbegin(), string1.cend(),
                                      string2.cbegin(), string2.cend(), false);
#endif
}

#ifndef UNI_ALGO_DISABLE_COLLATE
template<typename UTF8>
int utf8_collate(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_collate(string1.data(), string1.data() + string1.size(),
                                     string2.data(), string2.data() + string2.size(), false);
#else
    return detail::impl_utf8_collate(string1.cbegin(), string1.cend(),
                                     string2.cbegin(), string2.cend(), false);
#endif
}

template<typename UTF16>
int utf16_collate(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf16_collate(string1.data(), string1.data() + string1.size(),
                                      string2.data(), string2.data() + string2.size(), false);
#else
    return detail::impl_utf16_collate(string1.cbegin(), string1.cend(),
                                      string2.cbegin(), string2.cend(), false);
#endif
}
#endif // UNI_ALGO_DISABLE_COLLATE

template<typename UTF8>
uni::search utf8_search(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
    size_t pos = detail::impl_npos, end = detail::impl_npos;
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    bool ret = detail::impl_utf8_search(string1.data(), string1.data() + string1.size(),
                                        string2.data(), string2.data() + string2.size(), false, &pos, &end);
#else
    bool ret = detail::impl_utf8_search(string1.cbegin(), string1.cend(),
                                        string2.cbegin(), string2.cend(), false, &pos, &end);
#endif

    return uni::search{ret, pos, end};
}
template<typename UTF16>

uni::search utf16_search(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    size_t pos = detail::impl_npos, end = detail::impl_npos;
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    bool ret = detail::impl_utf16_search(string1.data(), string1.data() + string1.size(),
                                         string2.data(), string2.data() + string2.size(), false, &pos, &end);
#else
    bool ret = detail::impl_utf16_search(string1.cbegin(), string1.cend(),
                                         string2.cbegin(), string2.cend(), false, &pos, &end);
#endif

    return uni::search{ret, pos, end};
}

inline int utf8_compare(std::string_view string1, std::string_view string2)
{
    return utf8_compare<char>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int utf8_collate(std::string_view string1, std::string_view string2)
{
    return utf8_collate<char>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search utf8_search(std::string_view string1, std::string_view string2)
{
    return utf8_search<char>(string1, string2);
}
inline int utf16_compare(std::u16string_view string1, std::u16string_view string2)
{
    return utf16_compare<char16_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int utf16_collate(std::u16string_view string1, std::u16string_view string2)
{
    return utf16_collate<char16_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search utf16_search(std::u16string_view string1, std::u16string_view string2)
{
    return utf16_search<char16_t>(string1, string2);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline int utf16_compare(std::wstring_view string1, std::wstring_view string2)
{
    return utf16_compare<wchar_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int utf16_collate(std::wstring_view string1, std::wstring_view string2)
{
    return utf16_collate<wchar_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search utf16_search(std::wstring_view string1, std::wstring_view string2)
{
    return utf16_search<wchar_t>(string1, string2);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef UNI_ALGO_EXPERIMENTAL
#ifndef UNI_ALGO_DISABLE_COLLATE
template<typename UTF8>
std::string utf8_sortkey(std::basic_string_view<UTF8> source)
{
    return detail::t_map<std::string, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_sortkey, detail::impl_utf8_sortkey>(source, false);
}
template<typename UTF16>
std::string utf16_sortkey(std::basic_string_view<UTF16> source)
{
    return detail::t_map<std::string, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_sortkey, detail::impl_utf16_sortkey>(source, false);
}
inline std::string utf8_sortkey(std::string_view source)
{
    return utf8_sortkey<char>(source);
}
inline std::string utf16_sortkey(std::u16string_view source)
{
    return utf16_sortkey<char16_t>(source);
}
inline std::string utf16_sortkey(std::wstring_view source)
{
    return utf16_sortkey<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_COLLATE
#endif // UNI_ALGO_EXPERIMENTAL

} // namespace casesens

namespace caseless {

template<typename UTF8>
int utf8_compare(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_compare(string1.data(), string1.data() + string1.size(),
                                     string2.data(), string2.data() + string2.size(), true);
#else
    return detail::impl_utf8_compare(string1.cbegin(), string1.cend(),
                                     string2.cbegin(), string2.cend(), true);
#endif
}

template<typename UTF16>
int utf16_compare(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf16_compare(string1.data(), string1.data() + string1.size(),
                                      string2.data(), string2.data() + string2.size(), true);
#else
    return detail::impl_utf16_compare(string1.cbegin(), string1.cend(),
                                      string2.cbegin(), string2.cend(), true);
#endif
}

#ifndef UNI_ALGO_DISABLE_COLLATE
template<typename UTF8>
int utf8_collate(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_collate(string1.data(), string1.data() + string1.size(),
                                     string2.data(), string2.data() + string2.size(), true);
#else
    return detail::impl_utf8_collate(string1.cbegin(), string1.cend(),
                                     string2.cbegin(), string2.cend(), true);
#endif
}

template<typename UTF16>
int utf16_collate(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf16_collate(string1.data(), string1.data() + string1.size(),
                                      string2.data(), string2.data() + string2.size(), true);
#else
    return detail::impl_utf16_collate(string1.cbegin(), string1.cend(),
                                      string2.cbegin(), string2.cend(), true);
#endif
}
#endif // UNI_ALGO_DISABLE_COLLATE

template<typename UTF8>
uni::search utf8_search(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2)
{
    size_t pos = detail::impl_npos, end = detail::impl_npos;
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    bool ret = detail::impl_utf8_search(string1.data(), string1.data() + string1.size(),
                                        string2.data(), string2.data() + string2.size(), true, &pos, &end);
#else
    bool ret = detail::impl_utf8_search(string1.cbegin(), string1.cend(),
                                        string2.cbegin(), string2.cend(), true, &pos, &end);
#endif

    return uni::search{ret, pos, end};
}

template<typename UTF16>
uni::search utf16_search(std::basic_string_view<UTF16> string1, std::basic_string_view<UTF16> string2)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    size_t pos = detail::impl_npos, end = detail::impl_npos;
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    bool ret = detail::impl_utf16_search(string1.data(), string1.data() + string1.size(),
                                         string2.data(), string2.data() + string2.size(), true, &pos, &end);
#else
    bool ret = detail::impl_utf16_search(string1.cbegin(), string1.cend(),
                                         string2.cbegin(), string2.cend(), true, &pos, &end);
#endif

    return uni::search{ret, pos, end};
}

inline int utf8_compare(std::string_view string1, std::string_view string2)
{
    return utf8_compare<char>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int utf8_collate(std::string_view string1, std::string_view string2)
{
    return utf8_collate<char>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search utf8_search(std::string_view string1, std::string_view string2)
{
    return utf8_search<char>(string1, string2);
}
inline int utf16_compare(std::u16string_view string1, std::u16string_view string2)
{
    return utf16_compare<char16_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int utf16_collate(std::u16string_view string1, std::u16string_view string2)
{
    return utf16_collate<char16_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search utf16_search(std::u16string_view string1, std::u16string_view string2)
{
    return utf16_search<char16_t>(string1, string2);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline int utf16_compare(std::wstring_view string1, std::wstring_view string2)
{
    return utf16_compare<wchar_t>(string1, string2);
}
#ifndef UNI_ALGO_DISABLE_COLLATE
inline int utf16_collate(std::wstring_view string1, std::wstring_view string2)
{
    return utf16_collate<wchar_t>(string1, string2);
}
#endif // UNI_ALGO_DISABLE_COLLATE
inline uni::search utf16_search(std::wstring_view string1, std::wstring_view string2)
{
    return utf16_search<wchar_t>(string1, string2);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef UNI_ALGO_EXPERIMENTAL

#ifndef UNI_ALGO_DISABLE_COLLATE
template<typename UTF8>
std::string utf8_sortkey(std::basic_string_view<UTF8> source)
{
    return detail::t_map<std::string, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_sortkey, detail::impl_utf8_sortkey>(source, true);
}
template<typename UTF16>
std::string utf16_sortkey(std::basic_string_view<UTF16> source)
{
    return detail::t_map<std::string, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_sortkey, detail::impl_utf16_sortkey>(source, true);
}
inline std::string utf8_sortkey(std::string_view source)
{
    return utf8_sortkey<char>(source);
}
inline std::string utf16_sortkey(std::u16string_view source)
{
    return utf16_sortkey<char16_t>(source);
}
inline std::string utf16_sortkey(std::wstring_view source)
{
    return utf16_sortkey<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_COLLATE

template<typename UTF8>
bool utf8_like(std::basic_string_view<UTF8> string1, std::basic_string_view<UTF8> string2, char32_t escape = 0)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_like(string1.data(), string1.data() + string1.size(),
                                  string2.data(), string2.data() + string2.size(), true, '%', '_', escape);
#else
    return detail::impl_utf8_like(string1.cbegin(), string1.cend(),
                                  string2.cbegin(), string2.cend(), true, '%', '_', escape);
#endif
}
inline bool utf8_like(std::string_view string1, std::string_view string2, char32_t escape = 0)
{
    return utf8_like<char>(string1, string2, escape);
}

#endif // UNI_ALGO_EXPERIMENTAL

} // namespace caseless

} // namespace uni

#endif // CPP_UNI_CASE_H_UAIX

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
