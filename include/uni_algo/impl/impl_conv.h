/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_IMPL_CONV_H_UAIH
#define UNI_ALGO_IMPL_CONV_H_UAIH

#include "internal_defines.h"

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// The length of a destination (result) string must be premultiplied with one of these
// Example: destination_length = source_length * impl_x_function_name
uaix_const size_t impl_x_utf8to16  = 1; // tag_unicode_stable_value
uaix_const size_t impl_x_utf16to8  = 3; // tag_unicode_stable_value
uaix_const size_t impl_x_utf8to32  = 1; // tag_unicode_stable_value
uaix_const size_t impl_x_utf32to8  = 4; // tag_unicode_stable_value
uaix_const size_t impl_x_utf16to32 = 1; // tag_unicode_stable_value
uaix_const size_t impl_x_utf32to16 = 2; // tag_unicode_stable_value

// Forward declaration for fast ASCII functions
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool fast_ascii_utf8to16(it_in_utf8* s, it_end_utf8 last, it_out_utf16* dst);
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf32>
#endif
uaix_always_inline_tmpl
uaix_static bool fast_ascii_utf8to32(it_in_utf8* s, it_end_utf8 last, it_out_utf32* dst);

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf16, bool contiguous = true>
#endif
uaix_static size_t impl_utf8to16(it_in_utf8 first, it_end_utf8 last, it_out_utf16 result, size_t* const error)
{
    // If first > last the behaviour is undefined
    // If error is used and wasn't initialized with impl_npos the behaviour is undefined
    // C++ Note: works with input iterators: random access, contiguous, with proxy: input, forward, bidirectional
    // C++ Note: works with output iterators: random access, contiguous, with proxy: output
    // C++ Note: for info about proxy iterators see: impl/example/cpp_proxy_iterator.h
    // Other functions in this file have the same restrictions

    /* The algorithm is compatible with W3C Standard: https://www.w3.org/TR/encoding/#utf-8-decoder
     * For better understanding see The Unicode Standard: 3.9 -> Table 3-7. Well-Formed UTF-8 Byte Sequences
     * https://www.unicode.org/versions/Unicode12.0.0/ch03.pdf
     * Note that The Unicode Standard now recommends W3C Standard practice to handle U+FFFD.
     * See The Unicode Standard: 3.9 -> U+FFFD Substitution of Maximal Subparts
     * and The Unicode Standard: 5.22 U+FFFD Substitution in Conversion
     * For better understanding about different practices in handling U+FFFD
     * see this article: https://hsivonen.fi/broken-utf-8
     */

    /* Performance notes:
     * Zero unnecessary instructions zero unnecessary jumps all branches are straightforward.
     * Number of checks less than in W3C algorithm and they are simpler.
     * The branches are reordered it gives a bit better performance in some cases.
     * __builtin_expect is used because GCC sometimes messes up ASCII fast loop for some reason.
     * Also I tried to implement it a bit different but it seems a compiler can optimize
     * it better when it written like that even though it looks a bit ugly.
     * Note that we don't use iterator functions here and inline everything because
     * it gives a better performance. See test/test_perf_iter_conv.h
     */

    it_in_utf8 s = first;
    it_in_utf8 prev = s;
    it_out_utf16 dst = result;

#ifdef __cplusplus
    if constexpr (contiguous)
#endif
        fast_ascii_utf8to16(&s, last, &dst);

    while (s != last)
    {
        type_codept c = (*s & 0xFF), c2 = 0, c3 = 0, c4 = 0; // c2, c3, c4 tag_can_be_uninitialized
        prev = s; // Save previous position for error

        // NOLINTBEGIN(bugprone-assignment-in-if-condition)

        if (uaix_likely(c <= 0x7F)) // Fast route for ASCII
        {
            // It is possible to use the fast ASCII function here instead of before the main loop
            // but it can degrade the performance of UTF-8 conversion in some cases.
            // Note that uaix_likely must be removed too for better performance.
#if 0
#ifdef __cplusplus
            if constexpr (contiguous)
#endif
                if (fast_ascii_utf8to16(&s, last, &dst))
                    continue;
#endif // 0

            *dst++ = (type_char16)c;
            ++s;
            continue;
        }
        else if (c >= 0xC2 && c <= 0xDF)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF))
            {
                c = ((c & 0x1F) << 6) + (c2 & 0x3F);
                *dst++ = (type_char16)c;
                ++s;
                continue;
            }
        }
        else if (c >= 0xE1 && c <= 0xEC)
        { // NOLINT(bugprone-branch-clone)
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
                *dst++ = (type_char16)c;
                ++s;
                continue;
            }
        }
        else if (c >= 0xEE && c <= 0xEF)
        { // NOLINT(bugprone-branch-clone)
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
                *dst++ = (type_char16)c;
                ++s;
                continue;
            }
        }
        else if (uaix_likely(c == 0xE0))
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0xA0 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
                *dst++ = (type_char16)c;
                ++s;
                continue;
            }
        }
        else if (c == 0xED)
        {
            // REMINDER:
            // For WTF-8 change 0x9F to 0xBF here that means branches:
            // this (c == 0xED), (c >= 0xE1 && c <= 0xEC), (c >= 0xEE && c <= 0xEF)
            // can be merged into one.
            // For CESU-8 it's more complicated but it seems branches:
            // (c == 0xF0), (c == 0xF4), (c >= 0xF1 && c <= 0xF3)
            // must be removed too and surrogates must be handled differently.
            // All of this is just initial observations it doesn't mean
            // it will work properly like this.

            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x9F) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
                *dst++ = (type_char16)c;
                ++s;
                continue;
            }
        }
        else if (c == 0xF0)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x90 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
                // Make a surrogate pair
                *dst++ = (type_char16)(0xD7C0 + (c >> 10));
                *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));
                ++s;
                continue;
            }
        }
        else if (c == 0xF4)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x8F) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
                // Make a surrogate pair
                *dst++ = (type_char16)(0xD7C0 + (c >> 10));
                *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));
                ++s;
                continue;
            }
        }
        else if (c >= 0xF1 && c <= 0xF3)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
                // Make a surrogate pair
                *dst++ = (type_char16)(0xD7C0 + (c >> 10));
                *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));
                ++s;
                continue;
            }
        }
        else
        {
            // invalid code unit
            ++s;
        }

        // NOLINTEND(bugprone-assignment-in-if-condition)

        // Error: invalid code unit or overlong code point or truncated sequence in UTF-8

        if (error)
        {
            // *error points to the start of ill-formed sequence
            *error = (size_t)(prev - first);
            return (size_t)(dst - result);
        }

        *dst++ = (type_char16)0xFFFD; // Replacement char U+FFFD
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf8, bool = true>
#endif
uaix_static size_t impl_utf16to8(it_in_utf16 first, it_end_utf16 last, it_out_utf8 result, size_t* const error)
{
    it_in_utf16 src = first;
    it_out_utf8 dst = result;

    while (src != last)
    {
        const type_codept h = (*src & 0xFFFF);
        ++src;

        if (h <= 0x7F)
        {
            *dst++ = (type_char8)h;

            continue;
        }
        else if (h <= 0x7FF)
        {
            *dst++ = (type_char8)(0xC0 | (h >> 6));
            *dst++ = (type_char8)(0x80 | (h & 0x3F));

            continue;
        }
        else if (h >= 0xD800 && h <= 0xDFFF) // Surrogate pair
        {
            if (/*h >= 0xD800 &&*/ h <= 0xDBFF) // High surrogate is in range
            {
                if (src != last) // Unpaired high surrogate if reached the end here
                {
                    const type_codept l = (*src & 0xFFFF);

                    if (l >= 0xDC00 && l <= 0xDFFF) // Low surrogate is in range
                    {
                        const type_codept c = ((h - 0xD800) << 10) + (l - 0xDC00) + 0x10000;

                        *dst++ = (type_char8)(0xF0 |  (c >> 18));
                        *dst++ = (type_char8)(0x80 | ((c >> 12) & 0x3F));
                        *dst++ = (type_char8)(0x80 | ((c >> 6)  & 0x3F));
                        *dst++ = (type_char8)(0x80 |  (c        & 0x3F));

                        ++src;
                        continue;
                    }
                }
            }
        }
        else
        {
            *dst++ = (type_char8)(0xE0 |  (h >> 12));
            *dst++ = (type_char8)(0x80 | ((h >> 6) & 0x3F));
            *dst++ = (type_char8)(0x80 |  (h       & 0x3F));

            continue;
        }

        // Error: lone low surrogate or broken surrogate pair in UTF-16

        if (error)
        {
            // *error points to the start of ill-formed sequence
            *error = (size_t)(src - first) - 1;
            return (size_t)(dst - result);
        }

        // Replacement char U+FFFD
        *dst++ = (type_char8)(type_codept)0xEF;
        *dst++ = (type_char8)(type_codept)0xBF;
        *dst++ = (type_char8)(type_codept)0xBD;
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf32, bool contiguous = true>
#endif
uaix_static size_t impl_utf8to32(it_in_utf8 first, it_end_utf8 last, it_out_utf32 result, size_t* const error)
{
    it_in_utf8 s = first;
    it_in_utf8 prev = s;
    it_out_utf32 dst = result;

#ifdef __cplusplus
    if constexpr (contiguous)
#endif
        fast_ascii_utf8to32(&s, last, &dst);

    while (s != last)
    {
        type_codept c = (*s & 0xFF), c2 = 0, c3 = 0, c4 = 0; // c2, c3, c4 tag_can_be_uninitialized
        prev = s; // Save previous position for error

        // NOLINTBEGIN(bugprone-assignment-in-if-condition)

        if (uaix_likely(c <= 0x7F)) // Fast route for ASCII
        {
            *dst++ = (type_char32)c;
            ++s;
            continue;
        }
        else if (c >= 0xC2 && c <= 0xDF)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF))
            {
                c = ((c & 0x1F) << 6) + (c2 & 0x3F);
                *dst++ = (type_char32)c;
                ++s;
                continue;
            }
        }
        else if (c >= 0xE1 && c <= 0xEC)
        { // NOLINT(bugprone-branch-clone)
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
                *dst++ = (type_char32)c;
                ++s;
                continue;
            }
        }
        else if (c >= 0xEE && c <= 0xEF)
        { // NOLINT(bugprone-branch-clone)
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
                *dst++ = (type_char32)c;
                ++s;
                continue;
            }
        }
        else if (uaix_likely(c == 0xE0))
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0xA0 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
                *dst++ = (type_char32)c;
                ++s;
                continue;
            }
        }
        else if (c == 0xED)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x9F) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
                *dst++ = (type_char32)c;
                ++s;
                continue;
            }
        }
        else if (c == 0xF0)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x90 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
                *dst++ = (type_char32)c;
                ++s;
                continue;
            }
        }
        else if (c == 0xF4)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x8F) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
                *dst++ = (type_char32)c;
                ++s;
                continue;
            }
        }
        else if (c >= 0xF1 && c <= 0xF3)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
                *dst++ = (type_char32)c;
                ++s;
                continue;
            }
        }
        else
        {
            // invalid code unit
            ++s;
        }

        // NOLINTEND(bugprone-assignment-in-if-condition)

        // Error: invalid code unit or overlong code point or truncated sequence in UTF-8

        if (error)
        {
            // *error points to the start of ill-formed sequence
            *error = (size_t)(prev - first);
            return (size_t)(dst - result);
        }

        *dst++ = 0xFFFD; // Replacement char U+FFFD
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf32, typename it_end_utf32, typename it_out_utf8, bool = true>
#endif
uaix_static size_t impl_utf32to8(it_in_utf32 first, it_end_utf32 last, it_out_utf8 result, size_t* const error)
{
    it_in_utf32 src = first;
    it_out_utf8 dst = result;

    while (src != last)
    {
        const type_codept c = ((type_codept)*src & 0xFFFFFFFF);
        ++src;

        if (c <= 0x7F)
        {
            *dst++ = (type_char8)c;

            continue;
        }
        else if (c <= 0x7FF)
        {
            *dst++ = (type_char8)(0xC0 | (c >> 6));
            *dst++ = (type_char8)(0x80 | (c & 0x3F));

            continue;
        }
        else if (c <= 0xFFFF)
        {
            if (!(c >= 0xD800 && c <= 0xDFFF)) // If not in surrogate pairs range
            {
                *dst++ = (type_char8)(0xE0 |  (c >> 12));
                *dst++ = (type_char8)(0x80 | ((c >> 6) & 0x3F));
                *dst++ = (type_char8)(0x80 |  (c       & 0x3F));

                continue;
            }
        }
        else if (c <= 0x10FFFF)
        {
            *dst++ = (type_char8)(0xF0 |  (c >> 18));
            *dst++ = (type_char8)(0x80 | ((c >> 12) & 0x3F));
            *dst++ = (type_char8)(0x80 | ((c >> 6)  & 0x3F));
            *dst++ = (type_char8)(0x80 |  (c        & 0x3F));

            continue;
        }

        // Error: code point > 0x10FFFF or surrogate in UTF-32

        if (error)
        {
            // *error points to invalid code point
            *error = (size_t)(src - first) - 1;
            return (size_t)(dst - result);
        }

        // Replacement char U+FFFD
        *dst++ = (type_char8)(type_codept)0xEF;
        *dst++ = (type_char8)(type_codept)0xBF;
        *dst++ = (type_char8)(type_codept)0xBD;
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf32, bool = true>
#endif
uaix_static size_t impl_utf16to32(it_in_utf16 first, it_end_utf16 last, it_out_utf32 result, size_t* const error)
{
    it_in_utf16 src = first;
    it_out_utf32 dst = result;

    while (src != last)
    {
        const type_codept h = (*src & 0xFFFF);
        ++src;

        if (h >= 0xD800 && h <= 0xDFFF) // Surrogate pair
        {
            if (/*h >= 0xD800 &&*/ h <= 0xDBFF) // High surrogate is in range
            {
                if (src != last) // Unpaired high surrogate if reached the end here
                {
                    const type_codept l = (*src & 0xFFFF);

                    if (l >= 0xDC00 && l <= 0xDFFF) // Low surrogate is in range
                    {
                        const type_codept c = ((h - 0xD800) << 10) + (l - 0xDC00) + 0x10000;

                        *dst++ = (type_char32)c;

                        ++src;
                        continue;
                    }
                }
            }
        }
        else
        {
            *dst++ = (type_char32)h;

            continue;
        }

        // Error: lone low surrogate or broken surrogate pair in UTF-16

        if (error)
        {
            // *error points to the start of ill-formed sequence
            *error = (size_t)(src - first) - 1;
            return (size_t)(dst - result);
        }

        *dst++ = 0xFFFD; // Replacement char U+FFFD
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf32, typename it_end_utf32, typename it_out_utf16, bool = true>
#endif
uaix_static size_t impl_utf32to16(it_in_utf32 first, it_end_utf32 last, it_out_utf16 result, size_t* const error)
{
    it_in_utf32 src = first;
    it_out_utf16 dst = result;

    while (src != last)
    {
        const type_codept c = ((type_codept)*src & 0xFFFFFFFF);
        ++src;

        if (c <= 0xFFFF)
        {
            if (!(c >= 0xD800 && c <= 0xDFFF)) // If not in surrogate pairs range
            {
                *dst++ = (type_char16)c;

                continue;
            }
        }
        else if (c <= 0x10FFFF) // Make a surrogate pair
        {
            *dst++ = (type_char16)(0xD7C0 + (c >> 10));
            *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));

            continue;
        }

        // Error: code point > 0x10FFFF or surrogate in UTF-32

        if (error)
        {
            // *error points to invalid code point
            *error = (size_t)(src - first) - 1;
            return (size_t)(dst - result);
        }

        *dst++ = (type_char16)0xFFFD; // Replacement char U+FFFD
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static bool impl_is_valid_utf8(it_in_utf8 first, it_end_utf8 last, size_t* const error)
{
    // Based on impl_utf8to16 function

    it_in_utf8 s = first;
    it_in_utf8 prev = s;

    while (s != last)
    {
        // NOTE: NOLINT is used here because the function should be consistent with impl_utf8to16
        // NOLINTNEXTLINE(misc-const-correctness)
        type_codept c = (*s & 0xFF), c2 = 0, c3 = 0, c4 = 0; // c2, c3, c4 tag_can_be_uninitialized
        prev = s; // Save previous position for error

        // NOLINTBEGIN(bugprone-assignment-in-if-condition)

        if (uaix_likely(c <= 0x7F)) // Fast route for ASCII
        {
            ++s;
            continue;
        }
        else if (c >= 0xC2 && c <= 0xDF)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF))
            {
                ++s;
                continue;
            }
        }
        else if (c >= 0xE1 && c <= 0xEC)
        { // NOLINT(bugprone-branch-clone)
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                ++s;
                continue;
            }
        }
        else if (c >= 0xEE && c <= 0xEF)
        { // NOLINT(bugprone-branch-clone)
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                ++s;
                continue;
            }
        }
        else if (c == 0xE0)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0xA0 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                ++s;
                continue;
            }
        }
        else if (c == 0xED)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x9F) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
            {
                ++s;
                continue;
            }
        }
        else if (c == 0xF0)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x90 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                ++s;
                continue;
            }
        }
        else if (c == 0xF4)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x8F) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                ++s;
                continue;
            }
        }
        else if (c >= 0xF1 && c <= 0xF3)
        {
            if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
                ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
                ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
            {
                ++s;
                continue;
            }
        }
        else
        {
            // invalid code unit
            ++s;
        }

        // NOLINTEND(bugprone-assignment-in-if-condition)

        // Error: invalid code unit or overlong code point or truncated sequence in UTF-8

        if (error) // *error points to the start of ill-formed sequence
            *error = (size_t)(prev - first);

        return false;
    }

    return true;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static bool impl_is_valid_utf16(it_in_utf16 first, it_end_utf16 last, size_t* const error)
{
    // Based on impl_utf16to8 function

    it_in_utf16 src = first;

    while (src != last)
    {
        const type_codept h = (*src & 0xFFFF);
        ++src;

        if (h <= 0x7F)
        { // NOLINT(bugprone-branch-clone)
            continue;
        }
        else if (h <= 0x7FF)
        { // NOLINT(bugprone-branch-clone)
            continue;
        }
        else if (h >= 0xD800 && h <= 0xDFFF) // Surrogate pair
        {
            if (/*h >= 0xD800 &&*/ h <= 0xDBFF) // High surrogate is in range
            {
                if (src != last) // Unpaired high surrogate if reached the end here
                {
                    const type_codept l = (*src & 0xFFFF);

                    if (l >= 0xDC00 && l <= 0xDFFF) // Low surrogate is in range
                    {
                        ++src;
                        continue;
                    }
                }
            }
        }
        else
        { // NOLINT(bugprone-branch-clone)
            continue;
        }

        // Error: lone low surrogate or broken surrogate pair in UTF-16

        if (error) // *error points to the start of ill-formed sequence
            *error = (size_t)(src - first) - 1;

        return false;
    }

    return true;
}

#ifdef __cplusplus
template<typename it_in_utf32, typename it_end_utf32>
#endif
uaix_static bool impl_is_valid_utf32(it_in_utf32 first, it_end_utf32 last, size_t* const error)
{
    // Based on impl_utf32to8 function

    it_in_utf32 src = first;

    while (src != last)
    {
        const type_codept c = ((type_codept)*src & 0xFFFFFFFF);
        ++src;

        if (c <= 0x7F)
        { // NOLINT(bugprone-branch-clone)
            continue;
        }
        else if (c <= 0x7FF)
        { // NOLINT(bugprone-branch-clone)
            continue;
        }
        else if (c <= 0xFFFF)
        {
            if (!(c >= 0xD800 && c <= 0xDFFF)) // If not in surrogate pairs range
            {
                continue;
            }
        }
        else if (c <= 0x10FFFF)
        { // NOLINT(bugprone-branch-clone)
            continue;
        }

        // Error: code point > 0x10FFFF or surrogate in UTF-32

        if (error) // *error points to invalid code point
            *error = (size_t)(src - first) - 1;

        return false;
    }

    return true;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool fast_ascii_utf8to16(it_in_utf8* s, it_end_utf8 last, it_out_utf16* dst)
{
    // This optimization makes processing of ASCII strings by about 20-30% faster
    // C++ Note: works only with contiguous or random access input iterators

    bool processed = false;

    for (it_in_utf8 end = *s + (last - *s) - ((last - *s) % 4); *s != end; *s += 4)
    {
        // There are 3 ways to perform unaligned load:
        // 1. uint32_t = *((uint32_t*)uint8_t*); // Unsafe and not portable garbage.
        // 2. memcpy(&uint32_t, uint8_t*, 4); // Better but won't work for us for many reasons.
        // 3. Manual load. Harder to optimize for a compiler but safe and without potential function call in the worst case.
        // So we use manual load here.
        // In the best case scenario the following 4 lines will be optimized into one mov instruction
        // in the worst case it won't but we only lose a bit performance, processing by 4 bytes is always faster.
        // Note that manual load is endian agnostic too compared to other ways so we don't need to deal with that crap.
        // This means the behaviour will be the same on little/big/middle endian systems the value c will be the same.
        type_codept c = 0;
        c |= ((type_codept)*(*s+0) & 0xFF);
        c |= ((type_codept)*(*s+1) & 0xFF) << 8;
        c |= ((type_codept)*(*s+2) & 0xFF) << 16;
        c |= ((type_codept)*(*s+3) & 0xFF) << 24;

        // If non-ASCII then drop from the function and proceed as usual
        if ((c & 0x80808080) != 0)
            break;

        // This is not unaligned store even though it looks like it, we just do the usual thing here.
        *(*dst)++ = (type_char16)(c & 0xFF);
        *(*dst)++ = (type_char16)((c >> 8) & 0xFF);
        *(*dst)++ = (type_char16)((c >> 16) & 0xFF);
        *(*dst)++ = (type_char16)((c >> 24) & 0xFF);

        // This is how unaligned store should look like it can be used for potential utf8to8 function.
        // Even though a compiler probably optimize both variants the same for such function.
        //*(*dst+0) = (impl_char8)(c & 0xFF);
        //*(*dst+1) = (impl_char8)((c >> 8) & 0xFF);
        //*(*dst+2) = (impl_char8)((c >> 16) & 0xFF);
        //*(*dst+3) = (impl_char8)((c >> 24) & 0xFF);
        //*dst += 4;

        processed = true;
    }

    return processed;

    // NOTE from mg152:
    // In my observations MSVC never optimize manual load, GCC and Clang optimize it starting with version 5.
    // See test/random_stuff/unaligned_load_store.h to check which compiler is able to optimize it properly.
    // It is possible to make it faster by using long long and processing by 8 bytes or even intrinsics like _mm_loadu_si128
    // but it will be much less portable and the number of defines to handle it will be enormous.
    // So the basic optimization should be enough it's over optimization anyway I just did it for fun. At least it's always safe.

    // For reference: https://blog.quarkslab.com/unaligned-accesses-in-cc-what-why-and-solutions-to-do-it-properly.html
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf32>
#endif
uaix_always_inline_tmpl
uaix_static bool fast_ascii_utf8to32(it_in_utf8* s, it_end_utf8 last, it_out_utf32* dst)
{
    bool processed = false;

    for (it_in_utf8 end = *s + (last - *s) - ((last - *s) % 4); *s != end; *s += 4)
    {
        type_codept c = 0;
        c |= ((type_codept)*(*s+0) & 0xFF);
        c |= ((type_codept)*(*s+1) & 0xFF) << 8;
        c |= ((type_codept)*(*s+2) & 0xFF) << 16;
        c |= ((type_codept)*(*s+3) & 0xFF) << 24;

        if ((c & 0x80808080) != 0)
            break;

        *(*dst)++ = (type_char32)(c & 0xFF);
        *(*dst)++ = (type_char32)((c >> 8) & 0xFF);
        *(*dst)++ = (type_char32)((c >> 16) & 0xFF);
        *(*dst)++ = (type_char32)((c >> 24) & 0xFF);

        processed = true;
    }

    return processed;
}

UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_IMPL_CONV_H_UAIH

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
