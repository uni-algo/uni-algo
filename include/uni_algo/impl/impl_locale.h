/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_IMPL_LOCALE_H_UAIH
#define UNI_ALGO_IMPL_LOCALE_H_UAIH

#include "internal_defines.h"

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// TODO:
// Everything related to locales is a mess in Unicode so what is the best approach is still a question.
// For now this file contains only conversion functions that store locale subtags in type_codept
// See Unicode Technical Standard #35 for all things that ICU supports in locale tags
// but we definitely don't want most of those things and want to KISS and follow BCP47.

// https://www.rfc-editor.org/rfc/bcp/bcp47.txt
// https://en.wikipedia.org/wiki/Script_(Unicode)
// https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
// https://en.wikipedia.org/wiki/ISO_15924
// https://en.wikipedia.org/wiki/ISO_3166-1
// https://en.wikipedia.org/wiki/UN_M49
// https://en.wikipedia.org/wiki/ISO_4217
// https://www.unicode.org/reports/tr35/#Unknown_or_Invalid_Identifiers

uaix_const type_codept impl_locale_language_und = 0x756E6400; // ISO 639-1 / BCP47 (Undetermined) primary language
uaix_const type_codept impl_locale_script_Zzzz  = 0x5A7A7A7A; // ISO 15924
uaix_const type_codept impl_locale_region_ZZ    = 0x5A5A0000; // ISO 3166-1 alpha-2
//uaix_const type_codept impl_locale_region_ZZZ   = 0x5A5A5A00; // ISO 3166-1 alpha-3
//uaix_const type_codept impl_locale_region_999   = 0x39393900; // ISO 3166-1 numeric (UN M.49)
//uaix_const type_codept impl_locale_currency_XXX = 0x58585800; // ISO 4217

// This function must be always constexpr in C++ even if constexpr low-level is disabled
#ifdef __cplusplus
template <typename it_in_utf8>
constexpr   type_codept impl_locate_from_tag(it_in_utf8 s, size_t size)
#else
uaix_static type_codept impl_locate_from_tag(it_in_utf8 s, size_t size)
#endif
{
    if (size == 1)
        return ((s[0] & 0xFF) << 24);
    if (size == 2)
        return ((s[0] & 0xFF) << 24) | ((s[1] & 0xFF) << 16);
    if (size == 3)
        return ((s[0] & 0xFF) << 24) | ((s[1] & 0xFF) << 16) | ((s[2] & 0xFF) << 8);
    if (size == 4)
        return ((s[0] & 0xFF) << 24) | ((s[1] & 0xFF) << 16) | ((s[2] & 0xFF) << 8) | (s[3] & 0xFF);

    return 0;
}

#ifdef __cplusplus
template <typename it_out_utf8>
#endif
uaix_static size_t impl_locate_to_tag(type_codept v, it_out_utf8 s)
{
    size_t size = 0;

    if ((v >> 24) & 0xFF)
        s[size++] = (type_char8)((v >> 24) & 0xFF);
    if ((v >> 16) & 0xFF)
        s[size++] = (type_char8)((v >> 16) & 0xFF);
    if ((v >> 8) & 0xFF)
        s[size++] = (type_char8)((v >> 8) & 0xFF);
    if (v & 0xFF)
        s[size++] = (type_char8)(v & 0xFF);

    return size;
}

#ifdef __cplusplus
template <typename it_in_utf8>
#endif
uaix_static type_codept impl_locale_from_language(it_in_utf8 s, size_t size, type_codept unknown)
{
    // EXAMPLE: en/und (shortest ISO 639 code)
    type_codept result = 0;

    if (size != 2 && size != 3)
        return unknown;

    if (s[0] >= 'A' && s[0] <= 'Z')
        result |= (((s[0] ^ 32) & 0xFF) << 24);
    else if (s[0] >= 'a' && s[0] <= 'z')
        result |= ((s[0] & 0xFF) << 24);
    else
        return unknown;

    if (s[1] >= 'A' && s[1] <= 'Z')
        result |= (((s[1] ^ 32) & 0xFF) << 16);
    else if (s[1] >= 'a' && s[1] <= 'z')
        result |= ((s[1] & 0xFF) << 16);
    else
        return unknown;

    if (size == 3)
    {
        if (s[2] >= 'A' && s[2] <= 'Z')
            result |= (((s[2] ^ 32) & 0xFF) << 8);
        else if (s[2] >= 'a' && s[2] <= 'z')
            result |= ((s[2] & 0xFF) << 8);
        else
            return unknown;
    }

    return result;
}

#ifdef __cplusplus
template <typename it_in_utf8>
#endif
uaix_static type_codept impl_locale_from_script(it_in_utf8 s, size_t size, type_codept unknown)
{
    // EXAMPLE: Zzzz (ISO 15924 code)
    type_codept result = 0;

    if (size != 4)
        return unknown;

    if (s[0] >= 'A' && s[0] <= 'Z')
        result |= ((s[0] & 0xFF) << 24);
    else if (s[0] >= 'a' && s[0] <= 'z')
        result |= (((s[0] ^ 32) & 0xFF) << 24);
    else
        return unknown;

    if (s[1] >= 'A' && s[1] <= 'Z')
        result |= (((s[1] ^ 32) & 0xFF) << 16);
    else if (s[1] >= 'a' && s[1] <= 'z')
        result |= ((s[1] & 0xFF) << 16);
    else
        return unknown;

    if (s[2] >= 'A' && s[2] <= 'Z')
        result |= (((s[2] ^ 32) & 0xFF) << 8);
    else if (s[2] >= 'a' && s[2] <= 'z')
        result |= ((s[2] & 0xFF) << 8);
    else
        return unknown;

    if (s[3] >= 'A' && s[3] <= 'Z')
        result |= ((s[3] ^ 32) & 0xFF);
    else if (s[3] >= 'a' && s[3] <= 'z')
        result |= (s[3] & 0xFF);
    else
        return unknown;

    return result;
}

#ifdef __cplusplus
template <typename it_in_utf8>
#endif
uaix_static type_codept impl_locale_from_region(it_in_utf8 s, size_t size, type_codept unknown)
{
    // NOTE: Even though it's possible to use numeric codes for script subtag too
    // it's not handled there because BCP47 allows numeric codes for region subtag only.

    // EXAMPLE: 999 (UN M.49 code)
    if (size == 3)
    {
        if (s[0] >= '0' && s[0] <= '9' &&
            s[1] >= '0' && s[1] <= '9' &&
            s[2] >= '0' && s[2] <= '9')
            return ((s[0] & 0xFF) << 24) | ((s[1] & 0xFF) << 16) | ((s[2] & 0xFF) << 8);
            // return ((s[0] - 48) * 100) + ((s[1] - 48) * 10) + (s[2] - 48); // REMINDER: Numeric result

        return unknown;
    }

    // EXAMPLE: ZZ (ISO 3166-1 code)
    type_codept result = 0;

    if (size != 2)
        return unknown;

    if (s[0] >= 'A' && s[0] <= 'Z')
        result |= ((s[0] & 0xFF) << 24);
    else if (s[0] >= 'a' && s[0] <= 'z')
        result |= (((s[0] ^ 32) & 0xFF) << 24);
    else
        return unknown;

    if (s[1] >= 'A' && s[1] <= 'Z')
        result |= ((s[1] & 0xFF) << 16);
    else if (s[1] >= 'a' && s[1] <= 'z')
        result |= (((s[1] ^ 32) & 0xFF) << 16);
    else
        return unknown;

    return result;
}

#ifdef UNI_ALGO_EXPERIMENTAL

uaix_inline type_codept impl_locale_norm_language(type_codept v, type_codept unknown)
{
    // EXAMPLE: en/und (shortest ISO 639 code)
    type_codept result = 0;

    if (((v >> 24) & 0xFF) >= 'A' && ((v >> 24) & 0xFF) <= 'Z')
        result |= ((((v >> 24) & 0xFF) ^ 32) << 24);
    else if (((v >> 24) & 0xFF) >= 'a' && ((v >> 24) & 0xFF) <= 'z')
        result |= (((v >> 24) & 0xFF) << 24);
    else
        return unknown;

    if (((v >> 16) & 0xFF) >= 'A' && ((v >> 16) & 0xFF) <= 'Z')
        result |= ((((v >> 16) & 0xFF) ^ 32) << 16);
    else if (((v >> 16) & 0xFF) >= 'a' && ((v >> 16) & 0xFF) <= 'z')
        result |= (((v >> 16) & 0xFF) << 16);
    else
        return unknown;

    if (((v >> 8) & 0xFF) == 0)
        return result;

    if (((v >> 8) & 0xFF) >= 'A' && ((v >> 8) & 0xFF) <= 'Z')
        result |= ((((v >> 8) & 0xFF) ^ 32) << 8);
    else if (((v >> 8) & 0xFF) >= 'a' && ((v >> 8) & 0xFF) <= 'z')
        result |= (((v >> 8) & 0xFF) << 8);
    else
        return unknown;

    if ((v & 0xFF) != 0)
        return unknown;

    return result;
}

uaix_inline type_codept impl_locale_norm_script(type_codept v, type_codept unknown)
{
    // EXAMPLE: Zzzz (ISO 15924 code)
    type_codept result = 0;

    if (((v >> 24) & 0xFF) >= 'A' && ((v >> 24) & 0xFF) <= 'Z')
        result |= (((v >> 24) & 0xFF) << 24);
    else if (((v >> 24) & 0xFF) >= 'a' && ((v >> 24) & 0xFF) <= 'z')
        result |= ((((v >> 24) & 0xFF) ^ 32) << 24);
    else
        return unknown;

    if (((v >> 16) & 0xFF) >= 'A' && ((v >> 16) & 0xFF) <= 'Z')
        result |= ((((v >> 16) & 0xFF) ^ 32) << 16);
    else if (((v >> 16) & 0xFF) >= 'a' && ((v >> 16) & 0xFF) <= 'z')
        result |= (((v >> 16) & 0xFF) << 16);
    else
        return unknown;

    if (((v >> 8) & 0xFF) >= 'A' && ((v >> 8) & 0xFF) <= 'Z')
        result |= ((((v >> 8) & 0xFF) ^ 32) << 8);
    else if (((v >> 8) & 0xFF) >= 'a' && ((v >> 8) & 0xFF) <= 'z')
        result |= (((v >> 8) & 0xFF) << 8);
    else
        return unknown;

    if ((v & 0xFF) >= 'A' && (v & 0xFF) <= 'Z')
        result |= ((v & 0xFF) ^ 32);
    else if ((v & 0xFF) >= 'a' && (v & 0xFF) <= 'z')
        result |= (v & 0xFF);
    else
        return unknown;

    return result;
}

uaix_inline type_codept impl_locale_norm_region(type_codept v, type_codept unknown)
{
    // EXAMPLE: 999 (UN M.49 code)
    if (((v >> 24) & 0xFF) >= '0' && ((v >> 24) & 0xFF) <= '9' &&
        ((v >> 16) & 0xFF) >= '0' && ((v >> 16) & 0xFF) <= '9' &&
        ((v >> 8)  & 0xFF) >= '0' && ((v >> 8)  & 0xFF) <= '9' && (v & 0xFF) == 0)
        return (((v >> 24) & 0xFF) << 24) | (((v >> 16) & 0xFF) << 16) | (((v >> 8) & 0xFF) << 8);

    // EXAMPLE: ZZ (ISO 3166-1 code)
    type_codept result = 0;

    if (((v >> 24) & 0xFF) >= 'A' && ((v >> 24) & 0xFF) <= 'Z')
        result |= (((v >> 24) & 0xFF) << 24);
    else if (((v >> 24) & 0xFF) >= 'a' && ((v >> 24) & 0xFF) <= 'z')
        result |= ((((v >> 24) & 0xFF) ^ 32) << 24);
    else
        return unknown;

    if (((v >> 16) & 0xFF) >= 'A' && ((v >> 16) & 0xFF) <= 'Z')
        result |= (((v >> 16) & 0xFF) << 16);
    else if (((v >> 16) & 0xFF) >= 'a' && ((v >> 16) & 0xFF) <= 'z')
        result |= ((((v >> 16) & 0xFF) ^ 32) << 16);
    else
        return unknown;

    if (((v >> 8) & 0xFF) != 0)
        return unknown;
    if ((v & 0xFF) != 0)
        return unknown;

    return result;
}

#endif // UNI_ALGO_EXPERIMENTAL

#if 0
uaix_inline type_codept impl_locale_script_to_stage(type_codept v)
{
    // REMINDER:
    // x ^ 64 == x - 'A' + 1 (when x >= 'A' && x <= 'Z')
    // x ^ 96 == x - 'a' + 1 (when x >= 'z' && x <= 'z')
    // so x fit 5 bits and xxxx = 20 bits and max value is 0xFFFFFF

    // A compiler optimizes this function to around 40 instructions
    // it is possible to write it better.

    // For example it can be used to get script properties from
    // cldr/common/properties/scriptMetadata.txt
    // using two-stage table O(n) approach
    // but binary search O(log n) probably be better for such particular case.

    type_codept result = 0;

    if (((v >> 24) & 0xFF) >= 'A' && ((v >> 24) & 0xFF) <= 'Z')
        result |= ((((v >> 24) & 0xFF) ^ 64) << 15);
    else
        return 0;

    if (((v >> 16) & 0xFF) >= 'a' && ((v >> 16) & 0xFF) <= 'z')
        result |= ((((v >> 16) & 0xFF) ^ 96) << 10);
    else
        return 0;

    if (((v >> 8) & 0xFF) >= 'a' && ((v >> 8) & 0xFF) <= 'z')
        result |= ((((v >> 8) & 0xFF) ^ 96) << 5);
    else
        return 0;

    if ((v & 0xFF) >= 'a' && (v & 0xFF) <= 'z')
        result |= ((v & 0xFF) ^ 96);
    else
        return 0;

    return result;
}
#endif // 0

UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_IMPL_LOCALE_H_UAIH

/* Public Domain Contract
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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO  THE  WARRANTIES  OF  MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE  SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT Contract
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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO  THE WARRANTIES OF  MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  TORT OR OTHERWISE, ARISING FROM,
 * OUT OF  OR  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
