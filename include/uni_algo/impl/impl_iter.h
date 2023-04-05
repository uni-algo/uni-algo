/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_IMPL_ITER_H_UAIH
#define UNI_ALGO_IMPL_ITER_H_UAIH

#include "internal_defines.h"

UNI_ALGO_IMPL_NAMESPACE_BEGIN

uaix_const type_codept iter_replacement = 0xFFFD; // REPLACEMENT CHARACTER (U+FFFD)
uaix_const type_codept iter_error = 0xFFFFFFFF; // Any number outside Unicode range is fine

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf8 iter_utf8(it_in_utf8 first, it_end_utf8 last, type_codept* const codepoint, type_codept error)
{
    // If first >= last the behaviour is undefined
    // C++ Note: works with iterators: input, forward, bidirectional, random access, contiguous

    // See impl_utf8to16 in impl_conv.h for details

    it_in_utf8 s = first;

    type_codept c = (*s & 0xFF), c2 = 0, c3 = 0, c4 = 0; // c2, c3, c4 tag_can_be_uninitialized

    // NOLINTBEGIN(bugprone-assignment-in-if-condition)

    if (uaix_likely(c <= 0x7F)) // Fast route for ASCII
    {
        *codepoint = c;
        return ++s;
    }
    else if (c >= 0xC2 && c <= 0xDF)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF))
        {
            c = ((c & 0x1F) << 6) + (c2 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c >= 0xE1 && c <= 0xEC)
    { // NOLINT(bugprone-branch-clone)
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
        {
            c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c >= 0xEE && c <= 0xEF)
    { // NOLINT(bugprone-branch-clone)
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
        {
            c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (uaix_likely(c == 0xE0))
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0xA0 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
        {
            c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c == 0xED)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x9F) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
        {
            c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c == 0xF0)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x90 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
            ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
        {
            c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c == 0xF4)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x8F) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
            ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
        {
            c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c >= 0xF1 && c <= 0xF3)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
            ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
        {
            c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else
    {
        // invalid code unit
        ++s;
    }

    // NOLINTEND(bugprone-assignment-in-if-condition)

    // Error: invalid code unit or overlong code point or truncated sequence in UTF-8

    *codepoint = error;

    return s;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf16 iter_utf16(it_in_utf16 first, it_end_utf16 last, type_codept* const codepoint, type_codept error)
{
    // If first >= last the behaviour is undefined
    // C++ Note: works with iterators: input, forward, bidirectional, random access, contiguous

    it_in_utf16 src = first;

    const type_codept h = (*src & 0xFFFF);
    ++src;

    if (uaix_unlikely(h >= 0xD800 && h <= 0xDFFF)) // Surrogate pair
    {
        if (/*h >= 0xD800 &&*/ h <= 0xDBFF) // High surrogate is in range
        {
            if (src != last) // Unpaired high surrogate if reached the end here
            {
                const type_codept l = (*src & 0xFFFF);

                if (l >= 0xDC00 && l <= 0xDFFF) // Low surrogate is in range
                {
                    const type_codept c = ((h - 0xD800) << 10) + (l - 0xDC00) + 0x10000;
                    *codepoint = c;
                    ++src;
                    return src;
                }
            }
        }
    }
    else
    {
        *codepoint = h;
        return src;
    }

    // Error: lone low surrogate or broken surrogate pair in UTF-16

    *codepoint = error;
    return src;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf8 iter_rev_utf8(it_in_utf8 first, it_in_utf8 last, type_codept* const codepoint, type_codept error)
{
    // If first >= last the behaviour is undefined
    // C++ Note: works with iterators: bidirectional, random access, contiguous

    it_in_utf8 src = last;
    it_in_utf8 forward = last;

    // Go back by 1 code point by skipping tails
    while (--src != first && ((*src & 0xFF) & 0xC0) == 0x80);

    // Read next code point
    forward = iter_utf8(src, last, codepoint, error);

    if (forward == last)
        return src;

    // Multiple invalid code units then
    *codepoint = error; // For readability, iter_utf8 returns error anyway
    src = last;
    return --src; // Go back by 1 code unit
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf16 iter_rev_utf16(it_in_utf16 first, it_in_utf16 last, type_codept* const codepoint, type_codept error)
{
    // If first >= last the behaviour is undefined
    // C++ Note: works with iterators: bidirectional, random access, contiguous

    it_in_utf16 src = last;
    it_in_utf16 forward = last;

    // Go back by 1 code point by skipping low surrogates
    while (--src != first && ((*src & 0xFFFF) >= 0xDC00 && (*src & 0xFFFF) <= 0xDFFF));

    // Read next code point
    forward = iter_utf16(src, last, codepoint, error);

    if (forward == last)
        return src;

    // Multiple invalid code units then
    *codepoint = error; // For readability, iter_utf16 returns error anyway
    src = last;
    return --src; // Go back by 1 code unit
}

#ifdef __cplusplus
template<typename it_out_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf8 codepoint_to_utf8(type_codept c, it_out_utf8 dst)
{
    if (c <= 0x7F)
    {
        *dst++ = (type_char8)c;
    }
    else if (c <= 0x7FF)
    {
        *dst++ = (type_char8)(0xC0 | (c >> 6));
        *dst++ = (type_char8)(0x80 | (c & 0x3F));
    }
    else if (c <= 0xFFFF)
    {
        *dst++ = (type_char8)(0xE0 |  (c >> 12));
        *dst++ = (type_char8)(0x80 | ((c >> 6) & 0x3F));
        *dst++ = (type_char8)(0x80 |  (c       & 0x3F));
    }
    else
    {
        *dst++ = (type_char8)(0xF0 |  (c >> 18));
        *dst++ = (type_char8)(0x80 | ((c >> 12) & 0x3F));
        *dst++ = (type_char8)(0x80 | ((c >> 6)  & 0x3F));
        *dst++ = (type_char8)(0x80 |  (c        & 0x3F));
    }

    return dst;
}

#ifdef __cplusplus
template<typename it_out_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf16 codepoint_to_utf16(type_codept c, it_out_utf16 dst)
{
    if (c <= 0xFFFF)
    {
        *dst++ = (type_char16)c;
    }
    else
    {
        *dst++ = (type_char16)(0xD7C0 + (c >> 10));
        *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));
    }

    return dst;
}

// impl_* versions don't have always inline attribute. They can be used on a higher level
// and a compiler should decide better what to do with them.
// Hovewer we have inline versions of these functions for a higher level too
// they can be used for a critical performance code to guarantee to omit function calls always.

uaix_const type_codept impl_iter_replacement = 0xFFFD; // iter_replacement
uaix_const type_codept impl_iter_error = 0xFFFFFFFF; // iter_error

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static it_in_utf8 impl_iter_utf8(it_in_utf8 first, it_end_utf8 last, type_codept* const codepoint, type_codept error)
{
    return iter_utf8(first, last, codepoint, error);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static it_in_utf16 impl_iter_utf16(it_in_utf16 first, it_end_utf16 last, type_codept* const codepoint, type_codept error)
{
    return iter_utf16(first, last, codepoint, error);
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static it_in_utf8 impl_iter_rev_utf8(it_in_utf8 first, it_in_utf8 last, type_codept* const codepoint, type_codept error)
{
    return iter_rev_utf8(first, last, codepoint, error);
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static it_in_utf16 impl_iter_rev_utf16(it_in_utf16 first, it_in_utf16 last, type_codept* const codepoint, type_codept error)
{
    return iter_rev_utf16(first, last, codepoint, error);
}

// Inline versions

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf8 inline_iter_utf8(it_in_utf8 first, it_end_utf8 last, type_codept* const codepoint, type_codept error)
{
    return iter_utf8(first, last, codepoint, error);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf16 inline_iter_utf16(it_in_utf16 first, it_end_utf16 last, type_codept* const codepoint, type_codept error)
{
    return iter_utf16(first, last, codepoint, error);
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf8 inline_iter_rev_utf8(it_in_utf8 first, it_in_utf8 last, type_codept* const codepoint, type_codept error)
{
    return iter_rev_utf8(first, last, codepoint, error);
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf16 inline_iter_rev_utf16(it_in_utf16 first, it_in_utf16 last, type_codept* const codepoint, type_codept error)
{
    return iter_rev_utf16(first, last, codepoint, error);
}

// Output

#ifdef __cplusplus
template<typename it_out_utf8>
#endif
uaix_static it_out_utf8 impl_output_utf8(type_codept c, it_out_utf8 dst)
{
    // TODO: try to merge it with the function below can give a bit better performance
    if (c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
        c = 0xFFFD;

    return codepoint_to_utf8(c, dst);
}

#ifdef __cplusplus
template<typename it_out_utf16>
#endif
uaix_static it_out_utf16 impl_output_utf16(type_codept c, it_out_utf16 dst)
{
    // TODO: try to merge it with the function below can give a bit better performance
    if (c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
        c = 0xFFFD;

    return codepoint_to_utf16(c, dst);
}

UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_IMPL_ITER_H_UAIH

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
