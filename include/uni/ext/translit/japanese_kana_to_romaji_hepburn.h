/* Transliterator for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef JAPANESE_KANA_TO_ROMAJI_HEPBURN_H_UAIX
#define JAPANESE_KANA_TO_ROMAJI_HEPBURN_H_UAIX

#include <string>
#include <string_view>
#include <unordered_map>

#include <uni/iterator.h>
//#include "../../cpp_uni_norm.h"

// If it fails a compiler messed up the encoding of this file
// It the compiler is MSVC then /utf-8 command line option must be used
static_assert(U'㋡' == 0x32E1);

namespace uni::translit {

// Data and all C++ crap inside the class, the actual translit function at the end of the file
// Use the same design for all transliterators
class japanese_kana_to_romaji_hepburn
{
private:
    // https://en.wikipedia.org/wiki/Hepburn_romanization#Romanization_charts
    // https://en.wikipedia.org/wiki/Romanization_of_Japanese#Differences_among_romanizations
    const std::unordered_map<char32_t, std::u32string_view> simple_map {
    // Hiragana, Katakana
    {U'あ',U"a"},{U'ア',U"a"},
    {U'い',U"i"},{U'イ',U"i"},
    {U'う',U"u"},{U'ウ',U"u"},
    {U'え',U"e"},{U'エ',U"e"},
    {U'お',U"o"},{U'オ',U"o"},
    {U'か',U"ka"},{U'カ',U"ka"},
    {U'き',U"ki"},{U'キ',U"ki"},
    {U'く',U"ku"},{U'ク',U"ku"},
    {U'け',U"ke"},{U'ケ',U"ke"},
    {U'こ',U"ko"},{U'コ',U"ko"},
    {U'さ',U"sa"},{U'サ',U"sa"},
    {U'し',U"shi"},{U'シ',U"shi"},
    {U'す',U"su"},{U'ス',U"su"},
    {U'せ',U"se"},{U'セ',U"se"},
    {U'そ',U"so"},{U'ソ',U"so"},
    {U'た',U"ta"},{U'タ',U"ta"},
    {U'ち',U"chi"},{U'チ',U"chi"},
    {U'つ',U"tsu"},{U'ツ',U"tsu"},
    {U'て',U"te"},{U'テ',U"te"},
    {U'と',U"to"},{U'ト',U"to"},
    {U'な',U"na"},{U'ナ',U"na"},
    {U'に',U"ni"},{U'ニ',U"ni"},
    {U'ぬ',U"nu"},{U'ヌ',U"nu"},
    {U'ね',U"ne"},{U'ネ',U"ne"},
    {U'の',U"no"},{U'ノ',U"no"},
    {U'は',U"ha"},{U'ハ',U"ha"},
    {U'ひ',U"hi"},{U'ヒ',U"hi"},
    {U'ふ',U"fu"},{U'フ',U"fu"},
    {U'へ',U"he"},{U'ヘ',U"he"},
    {U'ほ',U"ho"},{U'ホ',U"ho"},
    {U'ま',U"ma"},{U'マ',U"ma"},
    {U'み',U"mi"},{U'ミ',U"mi"},
    {U'む',U"mu"},{U'ム',U"mu"},
    {U'め',U"me"},{U'メ',U"me"},
    {U'も',U"mo"},{U'モ',U"mo"},
    {U'や',U"ya"},{U'ヤ',U"ya"},
    {U'ゆ',U"yu"},{U'ユ',U"yu"},
    {U'よ',U"yo"},{U'ヨ',U"yo"},
    {U'ャ',U"ya"}, // Katakana only
    {U'ュ',U"yu"}, // Katakana only
    {U'ョ',U"yo"}, // Katakana only
    // TODO: not sure if this is correct, from the table here:
    // https://en.wikipedia.org/wiki/Romanization_of_Japanese#Differences_among_romanizations
    // see this too:
    // https://en.wikipedia.org/wiki/Romanization_of_Japanese#Kana_without_standardized_forms_of_romanization
    {U'ら',U"ra"},{U'ラ',U"ra"},
    {U'り',U"ri"},{U'リ',U"ri"},
    {U'る',U"ru"},{U'ル',U"ru"},
    {U'れ',U"re"},{U'レ',U"re"},
    {U'ろ',U"ro"},{U'ロ',U"ro"},
    {U'わ',U"wa"},{U'ワ',U"wa"},
    {U'ゐ',U"i"},{U'ヰ',U"i"},
    {U'ゑ',U"e"},{U'ヱ',U"e"},
    {U'を',U"o"},{U'ヲ',U"o"},
    {U'が',U"ga"},{U'ガ',U"ga"},
    {U'ぎ',U"gi"},{U'ギ',U"gi"},
    {U'ぐ',U"gu"},{U'グ',U"gu"},
    {U'げ',U"ge"},{U'ゲ',U"ge"},
    {U'ご',U"go"},{U'ゴ',U"go"},
    {U'ざ',U"za"},{U'ザ',U"za"},
    {U'じ',U"ji"},{U'ジ',U"ji"},
    {U'ず',U"zu"},{U'ズ',U"zu"},
    {U'ぜ',U"ze"},{U'ゼ',U"ze"},
    {U'ぞ',U"zo"},{U'ゾ',U"zo"},
    {U'だ',U"da"},{U'ダ',U"da"},
    {U'ぢ',U"ji"},{U'ヂ',U"ji"},
    {U'づ',U"zu"},{U'ヅ',U"zu"},
    {U'で',U"de"},{U'デ',U"de"},
    {U'ど',U"do"},{U'ド',U"do"},
    {U'ば',U"ba"},{U'バ',U"ba"},
    {U'び',U"bi"},{U'ビ',U"bi"},
    {U'ぶ',U"bu"},{U'ブ',U"bu"},
    {U'べ',U"be"},{U'ベ',U"be"},
    {U'ぼ',U"bo"},{U'ボ',U"bo"},
    {U'ぱ',U"pa"},{U'パ',U"pa"},
    {U'ぴ',U"pi"},{U'ピ',U"pi"},
    {U'ぷ',U"pu"},{U'プ',U"pu"},
    {U'ぺ',U"pe"},{U'ペ',U"pe"},
    {U'ぽ',U"po"},{U'ポ',U"po"},
    {U'ん',U"n"},{U'ン',U"n"},
    // Punctuations
    // TODO: do we even need this?
    {U'。',U"."},
    {U'、',U","}};

    const std::unordered_map<std::u32string_view, std::u32string_view> complex_map {
    // Hiragana, Katakana
    {U"きゃ",U"kya"},{U"キャ",U"kya"},
    {U"きゅ",U"kyu"},{U"キュ",U"kyu"},
    {U"きょ",U"kyo"},{U"キョ",U"kyo"},
    {U"しゃ",U"sha"},{U"シャ",U"sha"},
    {U"しゅ",U"shu"},{U"シュ",U"shu"},
    {U"しょ",U"sho"},{U"ショ",U"sho"},
    {U"ちゃ",U"cha"},{U"チャ",U"cha"},
    {U"ちゅ",U"chu"},{U"チュ",U"chu"},
    {U"ちょ",U"cho"},{U"チョ",U"cho"},
    {U"にゃ",U"nya"},{U"ニャ",U"nya"},
    {U"にゅ",U"nyu"},{U"ニュ",U"nyu"},
    {U"にょ",U"nyo"},{U"ニョ",U"nyo"},
    {U"ひゃ",U"hya"},{U"ヒャ",U"hya"},
    {U"ひゅ",U"hyu"},{U"ヒュ",U"hyu"},
    {U"ひょ",U"hyo"},{U"ヒョ",U"hyo"},
    {U"みゃ",U"mya"},{U"ミャ",U"mya"},
    {U"みゅ",U"myu"},{U"ミュ",U"myu"},
    {U"みょ",U"myo"},{U"ミョ",U"myo"},
    {U"りゃ",U"rya"},{U"リャ",U"rya"},
    {U"りゅ",U"ryu"},{U"リュ",U"ryu"},
    {U"りょ",U"ryo"},{U"リョ",U"ryo"},
    {U"ぎゃ",U"gya"},{U"ギャ",U"gya"},
    {U"ぎゅ",U"gyu"},{U"ギュ",U"gyu"},
    {U"ぎょ",U"gyo"},{U"ギョ",U"gyo"},
    {U"じゃ",U"ja"},{U"ジャ",U"ja"},
    {U"じゅ",U"ju"},{U"ジュ",U"ju"},
    {U"じょ",U"jo"},{U"ジョ",U"jo"},
    {U"ぢゃ",U"ja"},{U"ヂャ",U"ja"},
    {U"ぢゅ",U"ju"},{U"ヂュ",U"ju"},
    {U"ぢょ",U"jo"},{U"ヂョ",U"jo"},
    {U"びゃ",U"bya"},{U"ビャ",U"bya"},
    {U"びゅ",U"byu"},{U"ビュ",U"bya"},
    {U"びょ",U"byo"},{U"ビョ",U"byu"},
    {U"ぴゃ",U"pya"},{U"ピャ",U"pyo"},
    {U"ぴゅ",U"pyu"},{U"ピュ",U"pyu"},
    {U"ぴょ",U"pyo"},{U"ピョ",U"pyo"}};

    template<class Iter, class Sent, class Dest>
    void translit(Iter first, Sent last, Dest result);

public:
    japanese_kana_to_romaji_hepburn() = default;

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
void japanese_kana_to_romaji_hepburn::translit(Iter first, Sent last, Dest result)
{
    // https://en.wikipedia.org/wiki/Hepburn_romanization#Features
    // https://en.wikipedia.org/wiki/Hepburn_romanization#Romanization_charts
    // https://en.wikipedia.org/wiki/Romanization_of_Japanese#Differences_among_romanizations

    // See macedonian_to_latin_docs.h for details about translit iterator

    // NFC is not needed for Kana but it guaranties that the result will be in NFC
    // TODO: it doesn't guaranties anything NFC iter should be after translit iter to guaranty it
    // actually NFC is not needed at all for Kana it just makes the algorithm slower for no reason
    // TODO: clean this
    //uni::iter::norm::nfc nfc_begin(first, last);

    auto is_vowel = [](char32_t c)
    {
        return c == U'a' ||  c == U'i' || c == U'u' || c == U'e' || c == U'o' || c == U'y';
    };

    bool prev = false;

    auto func = [this, &prev, &is_vowel](std::u32string& buffer) -> std::size_t
    {
        std::u32string_view view(buffer);

        // Long consonants or Syllabic n
        if (buffer[0] == U'っ' || (prev && buffer[0] == U'n'))
        {
            std::size_t size = 0;
            // Always try complex map first
            if (buffer.size() > 2)
            {
                auto complex = complex_map.find(view.substr(1, 3));
                if (complex != complex_map.cend())
                {
                    buffer.replace(1, 2, complex->second);
                    size = complex->second.size();
                }
            }
            if (!size && buffer.size() > 1)
            {
                auto simple = simple_map.find(buffer[1]);
                if (simple != simple_map.cend())
                {
                    buffer.replace(1, 1, simple->second);
                    size = simple->second.size();
                }
            }
            if (size)
            {
                prev = true;
                if (buffer[0] == U'n') // Syllabic n
                {
                    if (is_vowel(buffer[1]))
                    {
                        buffer.insert(1, 1, U'\''); // Replace n -> n'
                        return size + 1;
                    }
                }
                else if (size > 1 && buffer[1] == U'c' && buffer[2] == U'h') // Long consonant ch -> tch
                    buffer[0] = U't';
                else if (!is_vowel(buffer[1])) // Long consonants others
                    buffer[0] = buffer[1];
                return size;
            }
        }
        else if (!prev) // Simple mapping
        {
            if (buffer.size() > 1)
            {
                auto complex = complex_map.find(view.substr(0, 2));
                if (complex != complex_map.cend())
                {
                    buffer.replace(0, 2, complex->second);
                    prev = true;
                    return complex->second.size() - 1;
                    // -1 to leave the last code point in the buffer for long vowels etc.
                }
            }
            auto simple = simple_map.find(buffer[0]);
            if (simple != simple_map.cend())
            {
                buffer.replace(0, 1, simple->second);
                prev = true;
                return simple->second.size() - 1;
            }
        }
        else // Main path
        {
            if (buffer.size() > 2)
            {
                auto complex = complex_map.find(view.substr(1, 3));
                if (complex != complex_map.cend())
                {
                    buffer.replace(1, 2, complex->second);
                    prev = true;
                    return complex->second.size() - 1;
                }
            }
            // Long vowels
            if (buffer.size() > 1)
            {
                auto simple = simple_map.find(buffer[1]);
                if (simple != simple_map.cend())
                {
                    buffer.replace(1, 1, simple->second);
                    prev = false;
                    if (simple->second.size() == 1)
                    {
                        // Note that the rule: two adjacent syllables
                        // is impossible to check algorithmically
                        // so just use modified Hepburn rules always
                        if (buffer[0] == U'a' && buffer[1] == U'a')
                            buffer.replace(0, 2, 1, U'ā');
                        else if (buffer[0] == U'u' && buffer[1] == U'u')
                            buffer.replace(0, 2, 1, U'ū');
                        else if (buffer[0] == U'e' && buffer[1] == U'e')
                            buffer.replace(0, 2, 1, U'ē');
                        else if (buffer[0] == U'o' && buffer[1] == U'o')
                            buffer.replace(0, 2, 1, U'ō'); // NOLINT
                        else if (buffer[0] == U'o' && buffer[1] == U'u')
                            buffer.replace(0, 2, 1, U'ō'); // NOLINT
                        else if (buffer[1] == U'n') // Syllabic n special case
                            prev = true;
                    }
                    else
                        prev = true;
                    return simple->second.size();
                }
            }
            // Loanwords
            if (buffer.size() > 1 && buffer[1] == U'ー')
            {
                if (buffer[0] == U'a')
                    buffer.replace(0, 2, 1, U'ā');
                else if (buffer[0] == U'i')
                    buffer.replace(0, 2, 1, U'ī');
                else if (buffer[0] == U'u')
                    buffer.replace(0, 2, 1, U'ū');
                else if (buffer[0] == U'e')
                    buffer.replace(0, 2, 1, U'ē');
                else if (buffer[0] == U'o')
                    buffer.replace(0, 2, 1, U'ō');
                // Fallthrough to return 1
            }
        }

        // Otherwise proceed by one code point
        prev = false;
        return 1;
    };

    // The buffer size 3 is enough for the algorithm
    //uni::iter::func::translit func_begin(func, 3, nfc_begin, uni::sentinel);
    uni::detail::iter::translit func_begin(func, 3, first, last);

    for (auto it = func_begin; it != uni::sentinel; ++it)
        *result++ = *it;
}

} // namespace uni::translit

#endif // JAPANESE_KANA_TO_ROMAJI_HEPBURN_H_UAIX

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
