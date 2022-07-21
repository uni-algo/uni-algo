/* Transliterator for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef MACEDONIAN_TO_LATIN_DOCS_H_UAIX
#define MACEDONIAN_TO_LATIN_DOCS_H_UAIX

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <uni/iterator.h>
//#include "../../cpp_uni_norm.h"

// If it fails a compiler messed up the encoding of this file
// It the compiler is MSVC then /utf-8 command line option must be used
static_assert(U'㋡' == 0x32E1);

namespace uni::translit {

// Data and all C++ crap inside the class, the actual translit function at the end of the file
// Use the same design for all transliterators
class macedonian_to_latin_docs
{
private:
    // https://en.wikipedia.org/wiki/Romanization_of_Macedonian
    const std::unordered_map<char32_t, char32_t> simple_map {
    {U'а',U'a'},{U'А',U'A'},
    {U'б',U'b'},{U'Б',U'B'},
    {U'в',U'v'},{U'В',U'V'},
    {U'г',U'g'},{U'Г',U'G'},
    {U'д',U'd'},{U'Д',U'D'},
    {U'е',U'e'},{U'Е',U'E'},
    {U'з',U'z'},{U'З',U'Z'},
    {U'и',U'i'},{U'И',U'I'},
    {U'ј',U'j'},{U'Ј',U'J'},
    {U'к',U'k'},{U'К',U'K'},
    {U'л',U'l'},{U'Л',U'L'},
    {U'м',U'm'},{U'М',U'M'},
    {U'н',U'n'},{U'Н',U'N'},
    {U'о',U'o'},{U'О',U'O'},
    {U'п',U'p'},{U'П',U'P'},
    {U'р',U'r'},{U'Р',U'R'},
    {U'с',U's'},{U'С',U'S'},
    {U'т',U't'},{U'Т',U'T'},
    {U'у',U'u'},{U'У',U'U'},
    {U'ф',U'f'},{U'Ф',U'F'},
    {U'х',U'h'},{U'Х',U'H'},
    {U'ц',U'c'},{U'Ц',U'C'}};

    const std::unordered_map<char32_t, std::pair<std::u32string_view, std::u32string_view>> complex_map {
    {U'ѓ',{U"gj",U"gj"}},{U'Ѓ',{U"GJ",U"Gj"}},
    {U'ж',{U"zh",U"zh"}},{U'Ж',{U"ZH",U"Zh"}},
    {U'ѕ',{U"dz",U"dz"}},{U'Ѕ',{U"DZ",U"Dz"}},
    {U'љ',{U"lj",U"lj"}},{U'Љ',{U"LJ",U"Lj"}},
    {U'њ',{U"nj",U"nj"}},{U'Њ',{U"NJ",U"Nj"}},
    {U'ќ',{U"kj",U"kj"}},{U'Ќ',{U"KJ",U"Kj"}},
    {U'ч',{U"ch",U"ch"}},{U'Ч',{U"CH",U"Ch"}},
    {U'џ',{U"dj",U"dj"}},{U'Џ',{U"DJ",U"Dj"}},
    {U'ш',{U"sh",U"sh"}},{U'Ш',{U"SH",U"Sh"}}};

    const std::unordered_set<char32_t> lowercase_set {
    U'а',U'б',U'в',U'г',U'д',U'ѓ',U'е',U'ж',U'з',U'ѕ',U'й',
    U'ј',U'к',U'л',U'љ',U'м',U'н',U'њ',U'о',U'п',U'р',U'с',
    U'т',U'ќ',U'у',U'ф',U'х',U'ц',U'ч',U'џ',U'ш'};

    template<class Iter, class Sent, class Dest>
    void translit(Iter first, Sent last, Dest result);

public:
    macedonian_to_latin_docs() = default;

    template<typename UTF8>
    std::basic_string<UTF8> utf8(std::basic_string_view<UTF8> source)
    {
        uni::iter::utf8 input{source.cbegin(), source.cend()};

        std::basic_string<UTF8> result;
        result.reserve(source.size());

        uni::iter::output::utf8 output{std::back_inserter(result)};

        translit(input, uni::sentinel, output);

        result.shrink_to_fit();
        return result;
    }
    template<typename UTF16>
    std::basic_string<UTF16> utf16(std::basic_string_view<UTF16> source)
    {
        uni::iter::utf16 input{source.cbegin(), source.cend()};

        std::basic_string<UTF16> result;
        result.reserve(source.size());

        uni::iter::output::utf16 output{std::back_inserter(result)};

        translit(input, uni::sentinel, output);

        result.shrink_to_fit();
        return result;
    }
    std::string utf8(std::string_view source) { return utf8<char>(source); }
    std::u16string utf16(std::u16string_view source) { return utf16<char16_t>(source); }
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    std::wstring utf16(std::wstring_view source) { return utf16<wchar_t>(source); }
#endif
};

template<class Iter, class Sent, class Dest>
void macedonian_to_latin_docs::translit(Iter first, Sent last, Dest result)
{
    // https://en.wikipedia.org/wiki/Romanization_of_Macedonian

    // Translit iterator is very powerfull it can do everything that
    // uni::iter:func::filter and uni::iter:func::transform can do
    // and much more but it can be dangerous for example it's possible
    // to cause an endless loop when it used improperly so it's important
    // what value must be returned in the function below.
    // This is the reason why the iterator is not available for a user.
    // Note the function have only one parameter - buffer, you can do whatever
    // you want with the buffer just make sure you return the proper value.

    auto func = [this](std::u32string& buffer) -> std::size_t
    {
        // Compose ѓ/Ѓ and ќ/Ќ first.
        // In Macedonian there are only 4 cases when letters can be decomposed
        // we can handle it like this so we don't need to use NFC iterator
        // it will be faster, even though it won't work in some corner cases
        // but it doesn't matter much for the transliteration.
        if (buffer.size() > 1)
        {
            if (buffer[0] == U'г' && buffer[1] == 0x301)
            {
                buffer.replace(0, 2, 1, U'ѓ');
                return 0;
                // Read example below why we return 0 here
            }
            if (buffer[0] == U'Г' && buffer[1] == 0x301)
            {
                buffer.replace(0, 2, 1, U'Ѓ');
                return 0;
            }
            if (buffer[0] == U'к' && buffer[1] == 0x301)
            {
                buffer.replace(0, 2, 1, U'ќ');
                return 0;
            }
            if (buffer[0] == U'К' && buffer[1] == 0x301)
            {
                buffer.replace(0, 2, 1, U'Ќ');
                return 0;
            }
        }
        // This is an example if we need to remove a code point
        // Note that we don't need buffer.size() check here
        // It is guaranteed that the size of the buffer is at least 1
        //if (lowercase_set.count(buffer[0])) // Remove all lowercase letters for example
        //{
        //	buffer.erase(0, 1);
        //	return 0;
        //	// Note that we return 0 here. If we remove a code point(s)
        //	// returning 0 will cause the buffer to refill.
        //	// Never return 0 if you don't remove or change a code point(s)
        //	// because returning 0 also means to stay at the same position.
        //}
        auto simple = simple_map.find(buffer[0]);
        if (simple != simple_map.cend())
        {
            buffer[0] = simple->second;
            return 1;
            // We changed only 1 code point
            // so just proceed by one code point.
        }
        auto complex = complex_map.find(buffer[0]);
        if (complex != complex_map.cend())
        {
            if (buffer.size() > 1 && lowercase_set.count(buffer[1]))
            {
                buffer.replace(0, 1, complex->second.second);
                return complex->second.second.size();
                // We changed multiple code points
                // so we need to skip them to get the next code point.
                // It is fine to skip less code points than you changed
                // if you need it for some complicated rules but be aware
                // if the rules interfere with each other it can cause an endless loop.
            }
            else
            {
                buffer.replace(0, 1, complex->second.first);
                return complex->second.first.size();
            }
        }

        // Otherwise proceed by one code point
        return 1;

        // This is a simple example how rules can interfere with each other
        // that leads to an endless loop.
        // For example you need to replace all "a" to "aa" if you do it like this:
        //if (buffer[0] == U'a')
        //{
        //	buffer.replace(0, 1, U"aa");
        //	return 1;
        //}
        // It will lead to an endless loop because you skip only 1 "a"
        // and then get the second "a" replace it again and again
        // so you must return 2 here to skip both "a".
    };

    // Note that we initialize the iterator with buffer size 2
    // because it is enought to translit Macedonian to Latin
    // the smaller the buffer the faster it works.
    uni::detail::iter::translit func_begin(func, 2, first, last);

    // It is possible to use NFC iterator instead of our compose code above.
    // it will make it slower but it can handle properly some very rare corner cases.
    //uni::iter::norm::nfc nfc_begin(first, last);
    //uni::detail::iter::translit func_begin(func, 2, nfc_begin, uni::sentinel);

    // It is also possible to output NFC normalized string if needed. In this case we need NFC iterator after our func.
    // We can use 2 NFC iterators to get rid of our compose code and output in NFC. Such approach will provide the best possible result.
    // Note that nfc_begin2 must be used instead of func_begin in for loop below for this to work properly.
    //uni::iter::norm::nfc nfc_begin1(first, last);
    //uni::detail::iter::translit func_begin(func, 2, nfc_begin1, uni::sentinel);
    //uni::iter::norm::nfc nfc_begin2(func_begin, uni::sentinel);

    for (auto it = func_begin; it != uni::sentinel; ++it)
        *result++ = *it;
}

} // namespace uni::translit

#endif // MACEDONIAN_TO_LATIN_DOCS_H_UAIX

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
