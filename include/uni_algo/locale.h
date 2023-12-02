/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_LOCALE_H_UAIH
#define UNI_ALGO_LOCALE_H_UAIH

#include <string>
#include <string_view>
#include <cassert>

#include "config.h"

#include "impl/impl_locale.h"

// DESIGN:
// Locale tags can contain too many values so using enum classes for them is no go.
// So we use our own "enum strings" that are similar in usage:
// una::locale::language{"en"} instead of una::locale::language::en
// That allow us to use them with all possible language/script/region tags.
// They are constexpr so it's possible to use them in all places
// where enum classes can be used for example in switch case.

namespace una {

// Forward declaration for system locale stuff
#ifndef UNI_ALGO_DISABLE_SYSTEM_LOCALE
#ifndef UNI_ALGO_STATIC_DATA
class locale;

namespace detail {

UNI_ALGO_DLL const una::locale& locale_system();
#ifdef UNI_ALGO_EXPERIMENTAL
UNI_ALGO_DLL const una::locale& locale_thread();
UNI_ALGO_DLL void locale_thread_reinit();
#endif // UNI_ALGO_EXPERIMENTAL

} // namespace detail
#endif // UNI_ALGO_STATIC_DATA
#endif // UNI_ALGO_DISABLE_SYSTEM_LOCALE

// Forward declaration for locale friend helper
namespace detail {
class locale_friend;
} // namespace detail

class locale
{
public:
    class language
    {
        friend class locale;
    private:
        // Use char32_t for value because that is what low-level uses
        char32_t value = 0;
        constexpr explicit language(char32_t v) noexcept : value{v} {}
        constexpr void set_value(char32_t v) noexcept { value = v; }
        constexpr char32_t get_value() const noexcept { return value; }
    public:
        constexpr language() = default;
        template <std::size_t N> // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        constexpr explicit language(const char (&s)[N]) noexcept : value{detail::impl_locate_from_tag(s, N ? N - 1 : 0)} {}
        uaiw_constexpr explicit language(std::string_view s) noexcept : value{detail::impl_locate_from_tag(s, s.size())} {}

        // Enable emplicit conversion to make this class work in switch case
        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr operator char32_t() const noexcept { return value; }

        friend constexpr bool operator==(const language& x, const language& y) noexcept { return x.value == y.value; }
        friend constexpr bool operator!=(const language& x, const language& y) noexcept { return x.value != y.value; }

        // Remove comparison operators because implicit conversion made them
        friend bool operator==(const language&, char32_t) = delete;
        friend bool operator==(char32_t, const language&) = delete;
        friend bool operator!=(const language&, char32_t) = delete;
        friend bool operator!=(char32_t, const language&) = delete;
        friend bool operator<=(const language&, char32_t) = delete;
        friend bool operator<=(char32_t, const language&) = delete;
        friend bool operator>=(const language&, char32_t) = delete;
        friend bool operator>=(char32_t, const language&) = delete;
        friend bool operator<(const language&, char32_t) = delete;
        friend bool operator<(char32_t, const language&) = delete;
        friend bool operator>(const language&, char32_t) = delete;
        friend bool operator>(char32_t, const language&) = delete;
    };
    class region
    {
        friend class locale;
    private:
        char32_t value = 0;
        constexpr explicit region(char32_t v) noexcept : value{v} {}
        constexpr void set_value(char32_t v) noexcept { value = v; }
        constexpr char32_t get_value() const noexcept { return value; }
    public:
        constexpr region() = default;
        template <std::size_t N> // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        constexpr explicit region(const char (&s)[N]) noexcept : value{detail::impl_locate_from_tag(s, N ? N - 1 : 0)} {}
        uaiw_constexpr explicit region(std::string_view s) noexcept : value{detail::impl_locate_from_tag(s, s.size())} {}
        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr operator char32_t() const noexcept { return value; }

        friend constexpr bool operator==(const region& x, const region& y) noexcept { return x.value == y.value; }
        friend constexpr bool operator!=(const region& x, const region& y) noexcept { return x.value != y.value; }

        friend bool operator==(const region&, char32_t) = delete;
        friend bool operator==(char32_t, const region&) = delete;
        friend bool operator!=(const region&, char32_t) = delete;
        friend bool operator!=(char32_t, const region&) = delete;
        friend bool operator<=(const region&, char32_t) = delete;
        friend bool operator<=(char32_t, const region&) = delete;
        friend bool operator>=(const region&, char32_t) = delete;
        friend bool operator>=(char32_t, const region&) = delete;
        friend bool operator<(const region&, char32_t) = delete;
        friend bool operator<(char32_t, const region&) = delete;
        friend bool operator>(const region&, char32_t) = delete;
        friend bool operator>(char32_t, const region&) = delete;
    };
    class script
    {
        friend class locale;
        friend class detail::locale_friend;
    private:
        char32_t value = 0;
        constexpr explicit script(char32_t v) noexcept : value{v} {}
        constexpr void set_value(char32_t v) noexcept { value = v; }
        constexpr char32_t get_value() const noexcept { return value; }
    public:
        constexpr script() = default;
        template <std::size_t N> // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        constexpr explicit script(const char (&s)[N]) noexcept : value{detail::impl_locate_from_tag(s, N ? N - 1 : 0)} {}
        uaiw_constexpr explicit script(std::string_view s) noexcept : value{detail::impl_locate_from_tag(s, s.size())} {}
        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr operator char32_t() const noexcept { return value; }

        friend constexpr bool operator==(const script& x, const script& y) noexcept { return x.value == y.value; }
        friend constexpr bool operator!=(const script& x, const script& y) noexcept { return x.value != y.value; }

        friend bool operator==(const script&, char32_t) = delete;
        friend bool operator==(char32_t, const script&) = delete;
        friend bool operator!=(const script&, char32_t) = delete;
        friend bool operator!=(char32_t, const script&) = delete;
        friend bool operator<=(const script&, char32_t) = delete;
        friend bool operator<=(char32_t, const script&) = delete;
        friend bool operator>=(const script&, char32_t) = delete;
        friend bool operator>=(char32_t, const script&) = delete;
        friend bool operator<(const script&, char32_t) = delete;
        friend bool operator<(char32_t, const script&) = delete;
        friend bool operator>(const script&, char32_t) = delete;
        friend bool operator>(char32_t, const script&) = delete;
    };
private:
    language lang; //{detail::impl_locale_language_und};
    script scpt; //{detail::impl_locale_script_Zzzz};
    region regn; //{detail::impl_locale_region_ZZ};

public:
    constexpr locale() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    constexpr locale(language l) noexcept : lang{l} {}
    constexpr locale(language l, region r) noexcept : lang{l}, regn{r} {}
    constexpr locale(language l, script s) noexcept : lang{l}, scpt{s} {}
    constexpr locale(language l, script s, region r) noexcept : lang{l}, scpt{s}, regn{r} {}
    constexpr language get_language() const noexcept { return lang; }
    constexpr script get_script() const noexcept { return scpt; }
    constexpr region get_region() const noexcept { return regn; }
    constexpr bool is_empty() const noexcept
    {
        return lang.get_value() == 0 &&
               scpt.get_value() == 0 &&
               regn.get_value() == 0;
    }
#ifndef UNI_ALGO_DISABLE_SYSTEM_LOCALE
#ifndef UNI_ALGO_STATIC_DATA
    static locale system() { return detail::locale_system(); }
#ifdef UNI_ALGO_EXPERIMENTAL
    static locale thread() { return detail::locale_thread(); }
    static void thread_reinit() { detail::locale_thread_reinit(); }
#endif // UNI_ALGO_EXPERIMENTAL
#endif // UNI_ALGO_STATIC_DATA
#endif // UNI_ALGO_DISABLE_SYSTEM_LOCALE
#ifdef UNI_ALGO_EXPERIMENTAL
    uaiw_constexpr void normalize() noexcept
    {
        lang.set_value(detail::impl_locale_norm_language(lang.get_value(), 0));
        if (lang.get_value() == 0)
        {
            scpt.set_value(0);
            regn.set_value(0);
        }
        scpt.set_value(detail::impl_locale_norm_script(scpt.get_value(), 0));
        regn.set_value(detail::impl_locale_norm_region(regn.get_value(), 0));
    }
#endif // UNI_ALGO_EXPERIMENTAL
    uaiw_constexpr std::string to_string() const
    {
        std::string result;
        result.resize(4 + 1 + 4 + 1 + 4);

        std::size_t size = 0;

        // REMINDER 1: Uncomment "size &&" parts to output empty string if ill-formed locale
        // REMINDER 2: Uncomment "if (size)" parts to not prepend ill-formed locale with '-'
        size += detail::impl_locate_to_tag(lang.get_value(), result.begin() + static_cast<std::ptrdiff_t>(size));
        if (/*size && */scpt.get_value())
        {
            /*if (size) */result[size++] = '-';
            size += detail::impl_locate_to_tag(scpt.get_value(), result.begin() + static_cast<std::ptrdiff_t>(size));
        }
        if (/*size && */regn.get_value())
        {
            /*if (size) */result[size++] = '-';
            size += detail::impl_locate_to_tag(regn.get_value(), result.begin() + static_cast<std::ptrdiff_t>(size));
        }

        result.resize(size);
        return result;
    }
private:
    template <typename T>
    uaiw_constexpr void parse(std::basic_string_view<T> s)
    {
        // Examples:
        // en-ZZ
        // und-ZZ
        // en-Zzzz-ZZ
        // und-Zzzz-ZZ

        if (s.size() < 2)
            return;

        bool has_lang = false;
        bool has_scpt = false;
        bool has_regn = false;

        bool found = false;
        for (std::size_t i = 0, prev = 0; i <= s.size(); ++i)
        {
            // Be aware that i <= s.size() so we parse last char too to simplify the algo
            // don't forget to check that

            if (i == s.size() || s[i] == '-' || s[i] == '_' || s[i] == '.')
            {
                if (found) // Multiple -_
                    break;

                const std::basic_string_view<T> x = s.substr(prev, i - prev);

                //std::cout << x << '\n'; // Test

                if (!has_lang)
                {
                    has_lang = true;
                    if (x.size() == 2 || x.size() == 3)
                        lang.set_value(detail::impl_locale_from_language(x, x.size(), 0));
                    else
                        break;
                }
                else if (!has_scpt && x.size() == 4)
                {
                    has_scpt = true;
                    scpt.set_value(detail::impl_locale_from_script(x, x.size(), 0));
                }
                else if (!has_regn)
                {
                    has_regn = true;
                    if (x.size() == 2 || x.size() == 3)
                        regn.set_value(detail::impl_locale_from_region(x, x.size(), 0));
                    break;
                }

                if (i != s.size() && s[i] == '.')
                    break;

                found = true;
                prev = i;
            }
            else if (found)
            {
                found = false;
                prev = i;
            }
        }
    }
public:
    uaiw_constexpr explicit locale(std::string_view s) { parse<char>(s); }
    uaiw_constexpr explicit locale(std::wstring_view s) { parse<wchar_t>(s); }

    // NOTE: Comparison of locale objects is incorrect usage in most cases
#ifdef UNI_ALGO_EXPERIMENTAL
    friend constexpr bool operator==(const locale& x, const locale& y) noexcept
    {
        return x.lang == y.lang &&
               x.scpt == y.scpt &&
               x.regn == y.regn;
    }
    friend constexpr bool operator!=(const locale& x, const locale& y) noexcept { return !(x == y); }
#endif // UNI_ALGO_EXPERIMENTAL
    friend constexpr bool operator==(const language& x, const locale& y) noexcept { return x == y.lang; }
    friend constexpr bool operator!=(const language& x, const locale& y) noexcept { return x != y.lang; }
    friend constexpr bool operator==(const locale& x, const language& y) noexcept { return x.lang == y; }
    friend constexpr bool operator!=(const locale& x, const language& y) noexcept { return x.lang != y; }
    friend constexpr bool operator==(const region& x, const locale& y) noexcept { return x == y.regn; }
    friend constexpr bool operator!=(const region& x, const locale& y) noexcept { return x != y.regn; }
    friend constexpr bool operator==(const locale& x, const region& y) noexcept { return x.regn == y; }
    friend constexpr bool operator!=(const locale& x, const region& y) noexcept { return x.regn != y; }
    friend constexpr bool operator==(const script& x, const locale& y) noexcept { return x == y.scpt; }
    friend constexpr bool operator!=(const script& x, const locale& y) noexcept { return x != y.scpt; }
    friend constexpr bool operator==(const locale& x, const script& y) noexcept { return x.scpt == y; }
    friend constexpr bool operator!=(const locale& x, const script& y) noexcept { return x.scpt != y; }
};

// Locale friend helper
namespace detail {
class locale_friend
{
public:
    static constexpr una::locale::script script_from_value(char32_t v) noexcept { return una::locale::script{v}; }
};
} // namespace detail

} // namespace una

#endif // UNI_ALGO_LOCALE_H_UAIH

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
