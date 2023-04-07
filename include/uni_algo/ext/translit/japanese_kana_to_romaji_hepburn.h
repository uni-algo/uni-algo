/* Transliterator for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_JAPANESE_KANA_TO_ROMAJI_HEPBURN_H_UAIH
#define UNI_ALGO_JAPANESE_KANA_TO_ROMAJI_HEPBURN_H_UAIH

#include <array>
#include <string>
#include <string_view>

#include "../../ranges_conv.h"
#include "../../internal/ranges_translit.h"

namespace una::detail::translit {

class japanese_kana_to_romaji_hepburn
{
    // See macedonian_to_latin_docs.h for details about translit view

    // https://en.wikipedia.org/wiki/Hepburn_romanization#Features
    // https://en.wikipedia.org/wiki/Hepburn_romanization#Romanization_charts
    // https://en.wikipedia.org/wiki/Romanization_of_Japanese#Differences_among_romanizations
private:
    static constexpr std::array<std::array<unsigned char, 3>, 96> simple_map = {{
//      0/8             1/9             2/A             3/B             4/C             5/D             6/E             7/F
        { },          {{'a'}},        {{'a'}},        {{'i'}},        {{'i'}},        {{'u'}},        {{'u'}},        {{'e'}},     // 304x
      {{'e'}},        {{'o'}},        {{'o'}},      {{'k','a'}},    {{'g','a'}},    {{'k','i'}},    {{'g','i'}},    {{'k','u'}},
    {{'g','u'}},    {{'k','e'}},    {{'g','e'}},    {{'k','o'}},    {{'g','o'}},    {{'s','a'}},    {{'z','a'}},  {{'s','h','i'}}, // 305x
    {{'j','i'}},    {{'s','u'}},    {{'z','u'}},    {{'s','e'}},    {{'z','e'}},    {{'s','o'}},    {{'z','o'}},    {{'t','a'}},
    {{'d','a'}},  {{'c','h','i'}},  {{'d','i'}},  {{'t','s','u'}},{{'t','s','u'}},  {{'d','u'}},    {{'t','e'}},    {{'d','e'}},   // 306x
    {{'t','o'}},    {{'d','o'}},    {{'n','a'}},    {{'n','i'}},    {{'n','u'}},    {{'n','e'}},    {{'n','o'}},    {{'h','a'}},
    {{'b','a'}},    {{'p','a'}},    {{'h','i'}},    {{'b','i'}},    {{'p','i'}},    {{'f','u'}},    {{'b','u'}},    {{'p','u'}},   // 307x
    {{'h','e'}},    {{'b','e'}},    {{'p','e'}},    {{'h','o'}},    {{'b','o'}},    {{'p','o'}},    {{'m','a'}},    {{'m','i'}},
    {{'m','u'}},    {{'m','e'}},    {{'m','o'}},    {{'y','a'}},    {{'y','a'}},    {{'y','u'}},    {{'y','u'}},    {{'y','o'}},   // 308x
    {{'y','o'}},    {{'r','a'}},    {{'r','i'}},    {{'r','u'}},    {{'r','e'}},    {{'r','o'}},    {{'w','a'}},    {{'w','a'}},
    {{'w','i'}},    {{'w','e'}},    {{'w','o'}},      {{'n'}},      {{'v','u'}},    {{'k','a'}},    {{'k','e'}},        { },       // 309x
        { },            { },            { },            { },            { },            { },            { },            { }    }};

    static constexpr std::array<std::array<unsigned char, 2>, 96> complex_map = {{
//   0       1         2           3           4       5   6       7         8     9       A       B       C       D           E       F
    { },    { },      { },        { },        { },    { },{ },    { },      { },  { },    { },    { },    { },{{'k','y'}},{{'g','y'}},{ },     // 304x
    { },    { },      { },        { },        { },    { },{ },{{'s','h'}},{{'j'}},{ },    { },    { },    { },    { },        { },    { },     // 305x
    { },{{'c','h'}},{{'j'}},      { },        { },    { },{ },    { },      { },  { },    { },{{'n','y'}},{ },    { },        { },    { },     // 306x
    { },    { },  {{'h','y'}},{{'b','y'}},{{'p','y'}},{ },{ },    { },      { },  { },    { },    { },    { },    { },        { },{{'m','y'}}, // 307x
    { },    { },      { },        { },        { },    { },{ },    { },      { },  { },{{'r','y'}},{ },    { },    { },        { },    { },     // 308x
    { },    { },      { },        { },        { },    { },{ },{{'s','h'}},  { },  { },    { },    { },    { },    { },        { },    { }  }}; // 309x

    static constexpr bool is_vowel(char32_t c)
    {
        return c == U'a' || c == U'i' || c == U'u' || c == U'e' || c == U'o' || c == U'y';
    }

    static constexpr std::size_t simple_fn(detail::translit::buffer& buf, char32_t c, std::size_t i)
    {
        std::size_t m = 0;
        if (c >= 0x3040 && c <= 0x309F) // Hiragana
        {
            if (simple_map[c - 0x3040][0])
                m = c - 0x3040;
        }
        else if (c >= 0x30A0 && c <= 0x30FF) // Katakana
        {
            if (simple_map[c - 0x30A0][0])
                m = c - 0x30A0;
        }
        if (m == 0) return 0;

        if (simple_map[m][2])
        {
            buf.replace(i, 1, simple_map[m], 0, 3);
            return 3;
        }
        else if (simple_map[m][1])
        {
            buf.replace(i, 1, simple_map[m], 0, 2);
            return 2;
        }

        buf[i] = simple_map[m][0];
        return 1;
    }

    static constexpr std::size_t complex_fn(detail::translit::buffer& buf, std::u32string_view view, std::size_t i)
    {
        if (view.size() != 2)
            return 0;

        if (view[0] >= 0x3040 && view[0] <= 0x309F) // Hiragana
        {
            const std::size_t m = view[0] - 0x3040;
            if (complex_map[m][0])
            {
                // HIRAGANA LETTER SMALL YA/YU/YO
                const char32_t v = (view[1] == 0x3083) ? U'a' : (view[1] == 0x3085) ? U'u' : (view[1] == 0x3087) ? U'o' : 0;
                if (v)
                {
                    if (complex_map[m][1])
                    {
                        buf.insert(i, 1, 0);
                        buf[i + 0] = complex_map[m][0];
                        buf[i + 1] = complex_map[m][1];
                        buf[i + 2] = v;
                        return 3;
                    }

                    buf[i + 0] = complex_map[m][0];
                    buf[i + 1] = v;
                    return 2;
                }
            }
        }
        else if (view[0] >= 0x30A0 && view[0] <= 0x30FF) // Katakana
        {
            const std::size_t m = view[0] - 0x30A0;
            if (complex_map[m][0])
            {
                // KATAKANA LETTER SMALL YA/YU/YO
                const char32_t v = (view[1] == 0x30E3) ? U'a' : (view[1] == 0x30E5) ? U'u' : (view[1] == 0x30E7) ? U'o' : 0;
                if (v)
                {
                    if (complex_map[m][1])
                    {
                        buf.insert(i, 1, 0);
                        buf[i + 0] = complex_map[m][0];
                        buf[i + 1] = complex_map[m][1];
                        buf[i + 2] = v;
                        return 3;
                    }

                    buf[i + 0] = complex_map[m][0];
                    buf[i + 1] = v;
                    return 2;
                }
            }
        }
        return 0;
    }

public:
    japanese_kana_to_romaji_hepburn() = delete;

    // The buffer size 3 is enough for the algorithm
    static constexpr std::size_t buf_size = 3;

    // Note that this function has additional data parameter prev
    // so a proxy function is required to use it with translit view
    static constexpr std::size_t buf_func(detail::translit::buffer& buf, bool& prev)
    {
        const std::u32string_view view = buf;

        // Syllabic n (prev is only needed for n) or Long consonants
        if ((prev && buf[0] == U'n') || buf[0] == 0x3063 || buf[0] == 0x30C3) // HIRAGANA/KATAKANA LETTER SMALL TU
        {
            std::size_t size = 0;
            // Always try complex map first
            if (buf.size() > 2)
                size = complex_fn(buf, view.substr(1, 2), 1);
            if (!size && buf.size() > 1)
                size = simple_fn(buf, buf[1], 1);
            if (size)
            {
                prev = true;
                if (buf[0] == U'n') // Syllabic n
                {
                    if (is_vowel(buf[1]))
                    {
                        buf.insert(1, 1, U'\''); // Replace n -> n'
                        return size + 1;
                    }
                }
                else if (size > 1 && buf[1] == U'c' && buf[2] == U'h') // Long consonant ch -> tch
                    buf[0] = U't';
                else if (!is_vowel(buf[1])) // Long consonants others
                    buf[0] = buf[1];
                return size;
            }
            if (buf[0] == 0x3063 || buf[0] == 0x30C3) // HIRAGANA/KATAKANA LETTER SMALL TU
                return simple_fn(buf, buf[0], 0);
        }
        else if (!prev) // Simple mapping
        {
            if (buf.size() > 1)
            {
                const std::size_t size = complex_fn(buf, view.substr(0, 2), 0);
                if (size)
                {
                    prev = true;
                    return size - 1;
                    // -1 to leave the last code point in the buffer for long vowels etc.
                }
            }
            const std::size_t size = simple_fn(buf, buf[0], 0);
            if (size)
            {
                prev = true;
                return size - 1;
            }
        }
        else // Main path
        {
            if (buf.size() > 2)
            {
                const std::size_t size = complex_fn(buf, view.substr(1, 2), 1);
                if (size)
                {
                    prev = true;
                    return size - 1;
                }
            }
            // Long vowels
            if (buf.size() > 1 && buf[1] != 0x3063 && buf[1] != 0x30C3) // HIRAGANA/KATAKANA LETTER SMALL TU
            {
                const std::size_t size = simple_fn(buf, buf[1], 1);
                if (size)
                {
                    prev = false;
                    if (size == 1)
                    {
                        // Note that the rule: two adjacent syllables
                        // is impossible to check algorithmically
                        // so just use modified Hepburn rules always
                        if (buf[0] == U'a' && buf[1] == U'a')
                            buf.replace(0, 2, 1, 0x0101); // LATIN SMALL LETTER A WITH MACRON
                        else if (buf[0] == U'u' && buf[1] == U'u')
                            buf.replace(0, 2, 1, 0x016B); // LATIN SMALL LETTER U WITH MACRON
                        else if (buf[0] == U'e' && buf[1] == U'e')
                            buf.replace(0, 2, 1, 0x0113); // LATIN SMALL LETTER E WITH MACRON
                        else if (buf[0] == U'o' && buf[1] == U'o')
                            buf.replace(0, 2, 1, 0x014D); // LATIN SMALL LETTER O WITH MACRON // NOLINT
                        else if (buf[0] == U'o' && buf[1] == U'u')
                            buf.replace(0, 2, 1, 0x014D); // LATIN SMALL LETTER O WITH MACRON // NOLINT
                        else if (buf[1] == U'n') // Syllabic n special case
                            prev = true;
                    }
                    else
                        prev = true;
                    return size;
                }
            }
            // Loanwords
            if (buf.size() > 1 && buf[1] == 0x30FC) // KATAKANA-HIRAGANA PROLONGED SOUND MARK
            {
                if (buf[0] == U'a')
                    buf.replace(0, 2, 1, 0x0101); // LATIN SMALL LETTER A WITH MACRON
                else if (buf[0] == U'i')
                    buf.replace(0, 2, 1, 0x012B); // LATIN SMALL LETTER I WITH MACRON
                else if (buf[0] == U'u')
                    buf.replace(0, 2, 1, 0x016B); // LATIN SMALL LETTER U WITH MACRON
                else if (buf[0] == U'e')
                    buf.replace(0, 2, 1, 0x0113); // LATIN SMALL LETTER E WITH MACRON
                else if (buf[0] == U'o')
                    buf.replace(0, 2, 1, 0x014D); // LATIN SMALL LETTER O WITH MACRON
                // Fallthrough to return 1
            }
        }

        // Otherwise proceed by one code point
        prev = false;
        return 1;
    }
};

} // namespace una::detail::translit

namespace unx::translit {

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
japanese_kana_to_romaji_hepburn_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    using namespace una; // NOLINT(google-build-using-namespace)

    using tr = detail::translit::japanese_kana_to_romaji_hepburn;

    bool prev = false;
    auto func = [&prev](detail::translit::buffer& buf) { return tr::buf_func(buf, prev); };

    auto result = detail::rng::translit_view{ranges::utf8_view{source}, func, tr::buf_size}
        | ranges::to_utf8_reserve<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>>(source.size(), alloc);

#ifndef UNI_ALGO_NO_SHRINK_TO_FIT
    result.shrink_to_fit();
#endif
    return result;
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
japanese_kana_to_romaji_hepburn_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    using namespace una; // NOLINT(google-build-using-namespace)

    using tr = detail::translit::japanese_kana_to_romaji_hepburn;

    bool prev = false;
    auto func = [&prev](detail::translit::buffer& buf) { return tr::buf_func(buf, prev); };

    auto result = detail::rng::translit_view{ranges::utf16_view{source}, func, tr::buf_size}
        | ranges::to_utf16_reserve<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>>(source.size(), alloc);

#ifndef UNI_ALGO_NO_SHRINK_TO_FIT
    result.shrink_to_fit();
#endif
    return result;
}
inline uaiw_constexpr std::string japanese_kana_to_romaji_hepburn_utf8(std::string_view source)
{
    return japanese_kana_to_romaji_hepburn_utf8<char>(source);
}
inline uaiw_constexpr std::u16string japanese_kana_to_romaji_hepburn_utf16(std::u16string_view source)
{
    return japanese_kana_to_romaji_hepburn_utf16<char16_t>(source);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline uaiw_constexpr std::wstring japanese_kana_to_romaji_hepburn_utf16(std::wstring_view source)
{
    return japanese_kana_to_romaji_hepburn_utf16<wchar_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#ifdef __cpp_lib_char8_t
inline uaiw_constexpr std::u8string utf8_japanese_kana_to_romaji_hepburn(std::u8string_view source)
{
    return japanese_kana_to_romaji_hepburn_utf8<char8_t>(source);
}
#endif // __cpp_lib_char8_t

} // namespace unx::translit

#endif // UNI_ALGO_JAPANESE_KANA_TO_ROMAJI_HEPBURN_H_UAIH

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
