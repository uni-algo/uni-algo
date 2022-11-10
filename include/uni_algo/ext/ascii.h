/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_EXT_ASCII_H_UAIH
#define UNI_ALGO_EXT_ASCII_H_UAIH

#include <type_traits>
#include <array>
#include <string>
#include <string_view>

#include "../config.h"
#include "../version.h"

namespace uni::detail::ascii {

#ifndef UNI_ALGO_IMPL_DISABLE_COLLATE

// Generator in test/test_ascii.h

inline constexpr const std::array<unsigned char, 128> data_collate_case = {{
  1,   2,   3,   4,   5,   6,   7,   8,   9,  29,  30,  31,  32,  33,  10,  11,
 12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
 34,  35,  36,  37,  66,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
 67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  49,  50,  51,  52,  53,  54,
 55,  78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 102, 104, 106,
108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128,  56,  57,  58,  59,  60,
 61,  77,  79,  81,  83,  85,  87,  89,  91,  93,  95,  97,  99, 101, 103, 105,
107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127,  62,  63,  64,  65,  28}};

inline constexpr const std::array<unsigned char, 128> data_collate_nocase = {{
  1,   2,   3,   4,   5,   6,   7,   8,   9,  29,  30,  31,  32,  33,  10,  11,
 12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
 34,  35,  36,  37,  66,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
 67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  49,  50,  51,  52,  53,  54,
 55,  77,  79,  81,  83,  85,  87,  89,  91,  93,  95,  97,  99, 101, 103, 105,
107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127,  56,  57,  58,  59,  60,
 61,  77,  79,  81,  83,  85,  87,  89,  91,  93,  95,  97,  99, 101, 103, 105,
107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127,  62,  63,  64,  65,  28}};

#endif // UNI_ALGO_IMPL_DISABLE_COLLATE

} // namespace uni::detail::ascii

// REMINDER: to move everything to namespace uni
// rename namespace unx to uni and remove all "using namespace uni;" below.

namespace unx {

namespace cases {

template<typename T, typename Alloc = std::allocator<T>>
std::basic_string<T, std::char_traits<T>, Alloc>
inline constexpr to_lowercase_ascii(std::basic_string_view<T> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<T>);

    std::basic_string<T, std::char_traits<T>, Alloc> dst{source, alloc};

    for (T& c : dst)
    {
        if (c >= 'A' && c <= 'Z')
            c ^= 32;
    }

    return dst;
}

template<typename T, typename Alloc = std::allocator<T>>
std::basic_string<T, std::char_traits<T>, Alloc>
inline constexpr to_uppercase_ascii(std::basic_string_view<T> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<T>);

    std::basic_string<T, std::char_traits<T>, Alloc> dst{source, alloc};

    for (T& c : dst)
    {
        if (c >= 'a' && c <= 'z')
            c ^= 32;
    }

    return dst;
}

inline std::string to_lowercase_ascii(std::string_view source)
{
    return to_lowercase_ascii<char>(source);
}
inline std::string to_uppercase_ascii(std::string_view source)
{
    return to_uppercase_ascii<char>(source);
}

} // namespace case

namespace casesens {

template<typename T>
inline constexpr int compare_ascii(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    static_assert(std::is_integral_v<T>);

    // The same as binary comparison

    return string1.compare(string2);
}

template<typename T>
inline constexpr uni::search search_ascii(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    static_assert(std::is_integral_v<T>);

    // The same as binary find

    auto pos = string1.find(string2);

    if (pos == std::string::npos)
        return uni::search{false, 0, 0};

    return uni::search{true, pos, pos + string2.size()};
}

#ifndef UNI_ALGO_IMPL_DISABLE_COLLATE

template<typename T>
inline constexpr int collate_ascii(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    using namespace uni; // NOLINT(google-build-using-namespace)

    static_assert(std::is_integral_v<T>);

    // TODO: Maybe move to low-level
    // See those & 0xFF it can cause false positive warnings that need to be silenced
    // try to use the function with std::basic_string<long long> for example
    // it least on the low-level we can simple silence this with casts

    if (string1.size() > string2.size())
        return 1;
    if (string1.size() < string2.size())
        return -1;

    for (decltype(string1.size()) i = 0, size = string1.size(); i < size; ++i)
    {
        char32_t c1 = string1[i] & 0xFF;
        char32_t c2 = string2[i] & 0xFF;

        if (c1 <= 0x7F)
            c1 = detail::ascii::data_collate_case[c1];

        if (c2 <= 0x7F)
            c2 = detail::ascii::data_collate_case[c2];

        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;
    }

    return 0;
}

#endif // UNI_ALGO_IMPL_DISABLE_COLLATE

inline int compare_ascii(std::string_view string1, std::string_view string2)
{
    return compare_ascii<char>(string1, string2);
}
#ifndef UNI_ALGO_IMPL_DISABLE_COLLATE
inline int collate_ascii(std::string_view string1, std::string_view string2)
{
    return collate_ascii<char>(string1, string2);
}
#endif // UNI_ALGO_IMPL_DISABLE_COLLATE
inline uni::search search_ascii(std::string_view string1, std::string_view string2)
{
    return search_ascii<char>(string1, string2);
}

} // namespace casesens

namespace caseless {

template<typename T>
inline constexpr int compare_ascii(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    static_assert(std::is_integral_v<T>);

    // TODO: Maybe move to low-level

    if (string1.size() < string2.size())
        return -1;
    if (string1.size() > string2.size())
        return 1;

    for (decltype(string1.size()) i = 0, size = string1.size(); i < size; ++i)
    {
        T c1 = string1[i];
        T c2 = string2[i];

        if (c1 >= 'A' && c1 <= 'Z')
            c1 ^= 32;

        if (c2 >= 'A' && c2 <= 'Z')
            c2 ^= 32;

        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;
    }

    return 0;
}

template<typename T>
inline constexpr uni::search search_ascii(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    static_assert(std::is_integral_v<T>);

    // TODO: Maybe move to low-level
    // See comment in casesens::collate_ascii above

    auto m = string2.size();
    auto n = string1.size();

    for (decltype(m) i = 0; i <= n - m; ++i)
    {
        decltype(m) j = 0;

        for (j = 0; j < m; ++j)
        {
            T c1 = string1[i + j];
            T c2 = string2[j];

            if (c1 >= 'A' && c1 <= 'Z')
                c1 ^= 32;

            if (c2 >= 'A' && c2 <= 'Z')
                c2 ^= 32;

            if (c1 != c2)
                break;
        }

        if (j == m)
            return uni::search{true, i, i + string2.size()};
    }

    return uni::search{false, 0, 0};
}

#ifndef UNI_ALGO_IMPL_DISABLE_COLLATE

template<typename T>
inline constexpr int collate_ascii(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    using namespace uni; // NOLINT(google-build-using-namespace)

    static_assert(std::is_integral_v<T>);

    // TODO: Maybe move to low-level

    if (string1.size() < string2.size())
        return -1;
    if (string1.size() > string2.size())
        return 1;

    for (decltype(string1.size()) i = 0, size = string1.size(); i < size; ++i)
    {
        char32_t c1 = string1[i] & 0xFF;
        char32_t c2 = string2[i] & 0xFF;

        if (c1 <= 0x7F)
            c1 = detail::ascii::data_collate_nocase[c1];

        if (c2 <= 0x7F)
            c2 = detail::ascii::data_collate_nocase[c2];

        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;
    }

    return 0;
}

#endif // UNI_ALGO_IMPL_DISABLE_COLLATE

inline int compare_ascii(std::string_view string1, std::string_view string2)
{
    return compare_ascii<char>(string1, string2);
}
#ifndef UNI_ALGO_IMPL_DISABLE_COLLATE
inline int collate_ascii(std::string_view string1, std::string_view string2)
{
    return collate_ascii<char>(string1, string2);
}
#endif // UNI_ALGO_IMPL_DISABLE_COLLATE
inline uni::search search_ascii(std::string_view string1, std::string_view string2)
{
    return search_ascii<char>(string1, string2);
}

} // namespace caseless

} // namespace unx

#endif // UNI_ALGO_EXT_ASCII_H_UAIH

/* Public Domain License:
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
 * MIT License:
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
