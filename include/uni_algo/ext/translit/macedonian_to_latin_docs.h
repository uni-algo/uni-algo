/* Transliterator for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_MACEDONIAN_TO_LATIN_DOCS_H_UAIH
#define UNI_ALGO_MACEDONIAN_TO_LATIN_DOCS_H_UAIH

#include <array>
#include <string>
#include <string_view>

#include "../../ranges_conv.h"
#include "../../internal/ranges_translit.h"

namespace una::detail::translit {

// This translit class must be static and internal, use the same desigh for all transliterators.
class macedonian_to_latin_docs
{
    // https://en.wikipedia.org/wiki/Romanization_of_Macedonian
    // Official Documents/Cadastre rules.
private:
    static constexpr std::array<unsigned char, 96> simple_map = {{
//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    { },{ },{ },{ },{ },{ },{ },{ },'J',{ },{ },{ },{ },{ },{ },{ },   // 040x
    'A','B','V','G','D','E',{ },'Z','I',{ },'K','L','M','N','O','P',   // 041x
    'R','S','T','U','F','H','C',{ },{ },{ },{ },{ },{ },{ },{ },{ },   // 042x
    'a','b','v','g','d','e',{ },'z','i',{ },'k','l','m','n','o','p',   // 043x
    'r','s','t','u','f','h','c',{ },{ },{ },{ },{ },{ },{ },{ },{ },   // 044x
    { },{ },{ },{ },{ },{ },{ },{ },'j',{ },{ },{ },{ },{ },{ },{ }}}; // 045x

    // NOLINTBEGIN(modernize-use-bool-literals)
    static constexpr std::array<bool, 96> lowercase_map = {{
//  0 1 2 3 4 5 6 7 8 9 A B C D E F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   // 040x
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   // 041x
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   // 042x
    1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,   // 043x
    1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,   // 044x
    0,0,0,1,0,1,0,0,1,1,1,0,1,0,0,1}}; // 045x
    // NOLINTEND(modernize-use-bool-literals)

    static constexpr std::array<std::array<std::array<unsigned char, 2>, 2>, 96> complex_map = {{
//            0/4/8/C                     1/5/9/D                     2/6/A/E                     3/7/B/F
                { },                        { },                        { },            {{{{'G','J'}},{{'G','j'}}}}, // 040x
                { },            {{{{'D','Z'}},{{'D','z'}}}},            { },                        { },
                { },            {{{{'L','J'}},{{'L','j'}}}},{{{{'N','J'}},{{'N','j'}}}},            { },
    {{{{'K','J'}},{{'K','j'}}}},            { },                        { },            {{{{'D','J'}},{{'D','j'}}}},
                { },                        { },                        { },                        { },             // 041x
                { },                        { },            {{{{'Z','H'}},{{'Z','h'}}}},            { },
                { },                        { },                        { },                        { },
                { },                        { },                        { },                        { },
                { },                        { },                        { },                        { },             // 042x
                { },                        { },                        { },            {{{{'C','H'}},{{'C','h'}}}},
    {{{{'S','H'}},{{'S','h'}}}},            { },                        { },                        { },
                { },                        { },                        { },                        { },
                { },                        { },                        { },                        { },             // 043x
                { },                        { },            {{{{'z','h'}},{{'z','h'}}}},            { },
                { },                        { },                        { },                        { },
                { },                        { },                        { },                        { },
                { },                        { },                        { },                        { },             // 044x
                { },                        { },                        { },            {{{{'c','h'}},{{'c','h'}}}},
    {{{{'s','h'}},{{'s','h'}}}},            { },                        { },                        { },
                { },                        { },                        { },                        { },
                { },                        { },                        { },            {{{{'g','j'}},{{'g','j'}}}}, // 045x
                { },            {{{{'d','z'}},{{'d','z'}}}},            { },                        { },
                { },            {{{{'l','j'}},{{'l','j'}}}},{{{{'n','j'}},{{'n','j'}}}},            { },
    {{{{'k','j'}},{{'k','j'}}}},            { },                        { },            {{{{'d','j'}},{{'d','j'}}}}}};

    // Cyrilic Unicode Block
    static constexpr bool is_block(char32_t c) { return (c >= 0x0400 && c <= 0x045F); }
    static constexpr std::size_t to_block(char32_t c) { return (c - 0x0400); }

public:
    macedonian_to_latin_docs() = delete;

    // Buffer size 2 is enought to translit Macedonian to Latin
    // the smaller the buffer the faster the translit view works.
    static constexpr std::size_t buf_size = 2;

    // The function below is used by the translit view.
    // Translit view is very powerfull it can do everything that
    // una::ranges::filter_view and una::ranges::transform_view can do
    // and much more but it can be dangerous for example it's possible
    // to cause an endless loop when it used improperly so it's important
    // what value must be returned in the function below.
    // This is the reason why the view is not available for a user.
    // Note the function have only one parameter - buffer, you can do whatever
    // you want with the buffer just make sure you return the proper value.
    static constexpr std::size_t buf_func(detail::translit::buffer& buf)
    {
        // Compose small/capital letters GJE/KJE first.
        // In Macedonian there are only 4 cases when letters can be decomposed
        // we can handle it like this so we don't need to use NFC view
        // it will be faster, even though it won't work in some corner cases
        // but it doesn't matter much for the transliteration.
        if (buf.size() > 1)
        {
            if (buf[0] == 0x0433 && buf[1] == 0x301) // CYRILLIC SMALL LETTER GHE
            {
                buf.replace(0, 2, 1, 0x0453);        // CYRILLIC SMALL LETTER GJE
                return 0;
                // Read example below why we return 0 here
            }
            if (buf[0] == 0x0413 && buf[1] == 0x301) // CYRILLIC CAPITAL LETTER GHE
            {
                buf.replace(0, 2, 1, 0x0403);        // CYRILLIC CAPITAL LETTER GJE
                return 0;
            }
            if (buf[0] == 0x043A && buf[1] == 0x301) // CYRILLIC SMALL LETTER KA
            {
                buf.replace(0, 2, 1, 0x045C);        // CYRILLIC SMALL LETTER KJE
                return 0;
            }
            if (buf[0] == 0x041A && buf[1] == 0x301) // CYRILLIC CAPITAL LETTER KA
            {
                buf.replace(0, 2, 1, 0x040C);        // CYRILLIC CAPITAL LETTER KJE
                return 0;
            }
        }

        // This is an example if we need to remove a code point
        // Note that we don't need buf.size() check here
        // It is guaranteed that the size of the buffer is at least 1
        //if (buf[0] == U' ') // Remove all spaces for example
        //{
        //	buffer.erase(0, 1);
        //	return 0;
        //	// Note that we return 0 here. If we remove a code point(s)
        //	// returning 0 will cause the buffer to refill.
        //	// Never return 0 if you don't remove or change a code point(s)
        //	// because returning 0 also means to stay at the same position.
        //}

        // The code below works with Cyrilic Unicode block
        // if it's not then proceed by one code point
        std::size_t m = 0;
        if (is_block(buf[0]))
            m = to_block(buf[0]);
        else
            return 1;

        if (simple_map[m])
        {
            buf[0] = simple_map[m];
            return 1;
            // We changed only 1 code point
            // so just proceed by one code point.
        }

        if (complex_map[m][0][0])
        {
            // If the next code point is lowercase Macedonian letter
            if (buf.size() > 1 && is_block(buf[1]) && lowercase_map[to_block(buf[1])])
                buf.replace(0, 1, complex_map[m][1], 0, 2);
            else
                buf.replace(0, 1, complex_map[m][0], 0, 2);

            return 2;
            // We changed multiple code points
            // so we need to skip them to get the next code point.
            // It is fine to skip less code points than you changed
            // if you need it for some complicated rules but be aware
            // if the rules interfere with each other it can cause an endless loop.
        }

        // Otherwise proceed by one code point
        return 1;

        // This is a simple example how rules can interfere with each other
        // that leads to an endless loop.
        // For example you need to replace all "x" to "xx" if you do it like this:
        //if (buf[0] == U'x')
        //{
        //    buf.replace(0, 1, U"xx");
        //    return 1;
        //}
        // It will lead to an endless loop because you skip only 1 "x"
        // and then get the second "x" replace it again and again
        // so you must return 2 here to skip both "x".
    }
};

} // namespace una::detail::translit

// Expose translit functions that use the translit view with the translit class above.

namespace unx::translit {

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
macedonian_to_latin_docs_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    using namespace una; // NOLINT(google-build-using-namespace)

    // Note that we use views from una::ranges instead of adaptors from una::views
    // because translit view is internal and doesn't have view adaptor
    // and we want to maximize the compilation speed.

    using tr = detail::translit::macedonian_to_latin_docs;

    auto result = detail::rng::translit_view{ranges::utf8_view{source}, tr::buf_func, tr::buf_size}
        | ranges::to_utf8_reserve<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>>(source.size(), alloc);

#ifndef UNI_ALGO_NO_SHRINK_TO_FIT
    result.shrink_to_fit();
#endif
    return result;
}
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
macedonian_to_latin_docs_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    using namespace una; // NOLINT(google-build-using-namespace)

    using tr = detail::translit::macedonian_to_latin_docs;

    auto result = detail::rng::translit_view{ranges::utf16_view{source}, tr::buf_func, tr::buf_size}
        | ranges::to_utf16_reserve<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>>(source.size(), alloc);

#ifndef UNI_ALGO_NO_SHRINK_TO_FIT
    result.shrink_to_fit();
#endif
    return result;
}
inline uaiw_constexpr std::string macedonian_to_latin_docs_utf8(std::string_view source)
{
    return macedonian_to_latin_docs_utf8<char>(source);
}
inline uaiw_constexpr std::u16string macedonian_to_latin_docs_utf16(std::u16string_view source)
{
    return macedonian_to_latin_docs_utf16<char16_t>(source);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline uaiw_constexpr std::wstring macedonian_to_latin_docs_utf16(std::wstring_view source)
{
    return macedonian_to_latin_docs_utf16<wchar_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#ifdef __cpp_lib_char8_t
inline uaiw_constexpr std::u8string macedonian_to_latin_docs_utf8(std::u8string_view source)
{
    return macedonian_to_latin_docs_utf8<char8_t>(source);
}
#endif // __cpp_lib_char8_t

} // namespace unx::translit

#endif // UNI_ALGO_MACEDONIAN_TO_LATIN_DOCS_H_UAIH

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
