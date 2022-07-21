/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef IMPL_CASE_H_UAIX
#define IMPL_CASE_H_UAIX

#include <uni/impl/iterator.h>
// Note that title case has cross dependency with break word module
// and if it's disabled all title case functions must be disabled too
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#include <uni/impl/break_word.h>
#endif

#include <uni/internal/defines.h>
#include <uni/internal/stages.h>

#ifndef UNI_ALGO_STATIC_DATA
#include <uni/impl/case_data_extern.h>
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// The length of a destination (result) string must be premultiplied with one of these
// Example: destination_length = source_length * impl_x_function_name
#ifndef UNI_ALGO_DISABLE_FULL_CASE
uaix_const size_t impl_x_utf8_casemap = 3;
uaix_const size_t impl_x_utf16_casemap = 3;
#else
uaix_const size_t impl_x_utf8_casemap = 3;
uaix_const size_t impl_x_utf16_casemap = 1;
#endif

// Modes for casemap functions
//uaix_const int impl_casemap_mode_null = 0;
uaix_const int impl_casemap_mode_fold   = 1;
uaix_const int impl_casemap_mode_upper  = 2;
uaix_const int impl_casemap_mode_lower  = 3;
uaix_const int impl_casemap_mode_title  = 4;

uaix_const type_codept prop_Cased            = 1 << 0;
uaix_const type_codept prop_Cased_Ignorable  = 1 << 1;
uaix_const type_codept prop_Lowercase        = 1 << 2; // not used
uaix_const type_codept prop_Uppercase        = 1 << 3; // not used
uaix_const type_codept prop_Soft_Dotted      = 1 << 4; // impl_case_locale.h
uaix_const type_codept prop_CCC_NOT_0        = 1 << 5; // impl_case_locale.h
uaix_const type_codept prop_CCC_230          = 1 << 6; // impl_case_locale.h

uaix_always_inline
uaix_static type_codept stages_lower(type_codept c)
{
    type_codept v = stages(c, stage1_lower, stage2_lower);
    return v ? v : c;
}

uaix_always_inline
uaix_static type_codept stages_upper(type_codept c)
{
    type_codept v = stages(c, stage1_upper, stage2_upper);
    return v ? v : c;
}

uaix_always_inline
uaix_static type_codept stages_fold(type_codept c)
{
    type_codept v = stages(c, stage1_fold, stage2_fold);
    return v ? v : c;
}

#ifndef UNI_ALGO_DISABLE_BREAK_WORD

uaix_always_inline
uaix_static type_codept stages_title(type_codept c)
{
    type_codept v = stages(c, stage1_title, stage2_title);
    return v ? v : c;
}

#endif // UNI_ALGO_DISABLE_BREAK_WORD

#ifndef UNI_ALGO_DISABLE_COLLATE

uaix_always_inline
uaix_static type_codept stages_order(type_codept c)
{
    type_codept v = stages(c, stage1_order, stage2_order);
    return v ? v : c + 0x110000;
    // Sort code points that are not in DUCET in code point order
    // The result must never be > 0x3FBFFF or it will break our experimental sort keys
}

#endif // UNI_ALGO_DISABLE_COLLATE

uaix_always_inline
uaix_static type_codept stages_case_prop(type_codept c)
{
    return stages(c, stage1_case_prop, stage2_case_prop);
}

#ifndef UNI_ALGO_DISABLE_FULL_CASE

uaix_always_inline
uaix_static size_t stages_special_fold(type_codept c, type_codept special[3])
{
    if (c <= 0xFFFF)
    {
        size_t n = stages(c, stage1_special_fold, stage2_special_fold);
        special[0] = stage3_special_fold[n][1];
        special[1] = stage3_special_fold[n][2];
        special[2] = stage3_special_fold[n][3];
        return stage3_special_fold[n][0];
    }
    return 0;
}

uaix_always_inline
uaix_static bool stages_special_fold_check(type_codept c)
{
    // This function itself is optimization

    if (c <= 0xFFFF)
    {
        size_t n = stages(c, stage1_special_fold, stage2_special_fold);
        return n ? true : false;
    }
    return false;
}

uaix_always_inline
uaix_static size_t stages_special_upper(type_codept c, type_codept special[3])
{
    if (c <= 0xFFFF)
    {
        size_t n = stages(c, stage1_special_upper, stage2_special_upper);
        special[0] = stage3_special_upper[n][1];
        special[1] = stage3_special_upper[n][2];
        special[2] = stage3_special_upper[n][3];
        return stage3_special_upper[n][0];
    }
    return 0;
}

uaix_always_inline
uaix_static bool stages_special_upper_check(type_codept c)
{
    // This function itself is optimization

    if (c <= 0xFFFF)
    {
        size_t n = stages(c, stage1_special_upper, stage2_special_upper);
        return n ? true : false;
    }
    return false;
}

#ifndef UNI_ALGO_DISABLE_BREAK_WORD

uaix_always_inline
uaix_static size_t stages_special_title(type_codept c, type_codept special[3])
{
    if (c <= 0xFFFF)
    {
        size_t n = stages(c, stage1_special_title, stage2_special_title);
        special[0] = stage3_special_title[n][1];
        special[1] = stage3_special_title[n][2];
        special[2] = stage3_special_title[n][3];
        return stage3_special_title[n][0];
    }
    return 0;
}

uaix_always_inline
uaix_static bool stages_special_title_check(type_codept c)
{
    // This function itself is optimization

    if (c <= 0xFFFF)
    {
        size_t n = stages(c, stage1_special_title, stage2_special_title);
        return n ? true : false;
    }
    return false;
}

#endif // UNI_ALGO_DISABLE_BREAK_WORD

#endif // UNI_ALGO_DISABLE_FULL_CASE

#ifndef UNI_ALGO_DISABLE_FULL_CASE

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static bool utf8_final_sigma(it_in_utf8 from, it_end_utf8 to, bool reverse)
{
    /* Final_Sigma special case from The Unicode Standard:
     * C is preceded by a sequence consisting of a cased letter and then zero or more
     * case-ignorable characters, and C is not followed by a sequence consisting of zero
     * or more case-ignorable characters and then a cased letter.
     * \p{cased}(\p{case-ignorable})* U+03A3 !((\p{case-ignorable})*\p{cased})
     */

    it_in_utf8 s = from;
    type_codept c = 0;

    while (s != to)
    {
        if (reverse)
            s = utf8_iter_rev(to, s, &c, iter_replacement);
        else
            s = utf8_iter(s, to, &c, iter_replacement);

        type_codept prop = stages_case_prop(c);

        if (prop & prop_Cased_Ignorable)
            continue;
        return (prop & prop_Cased) ? true : false;
    }

    return false;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static bool utf16_final_sigma(it_in_utf16 from, it_end_utf16 to, bool reverse)
{
    it_in_utf16 s = from;
    type_codept c = 0;

    while (s != to)
    {
        if (reverse)
            s = utf16_iter_rev(to, s, &c, iter_replacement);
        else
            s = utf16_iter(s, to, &c, iter_replacement);

        type_codept prop = stages_case_prop(c);

        if (prop & prop_Cased_Ignorable)
            continue;
        return (prop & prop_Cased) ? true : false;
    }

    return false;
}

#endif // UNI_ALGO_DISABLE_FULL_CASE

#ifndef UNI_ALGO_DISABLE_BREAK_WORD

// Forward declaration for title case functions

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t utf8_title(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result);

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t utf16_title(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result);

#endif // UNI_ALGO_DISABLE_BREAK_WORD

// The Unicode Standard: 3.13 Default Case Algorithms:
// Default Case Conversion (locale-independent)
// Default Case Folding
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_utf8_casemap(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result, int mode)
{
    it_in_utf8 src = first;
    it_out_utf8 dst = result;
    type_codept c = 0;

    if (mode == impl_casemap_mode_lower)
    {
        while (src != last)
        {
#ifndef UNI_ALGO_DISABLE_FULL_CASE
            it_in_utf8 prev = src;
#endif
            src = utf8_iter(src, last, &c, iter_replacement);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
            if (c == 0x0130) // Handled in place (checked in generator)
            {
                *dst++ = (type_char8)(type_codept)0x69;
                *dst++ = (type_char8)(type_codept)0xCC;
                *dst++ = (type_char8)(type_codept)0x87;
                continue;
            }
            if (c == 0x03A3) // Final_Sigma
            {
                if (!utf8_final_sigma(src, last, false) &&
                    utf8_final_sigma(prev, first, true))
                {
                    *dst++ = (type_char8)(type_codept)0xCF;
                    *dst++ = (type_char8)(type_codept)0x82;
                    continue;
                }
            }
#endif
            c = stages_lower(c);

            dst = codepoint_to_utf8(c, dst);
        }
    }
    else if (mode == impl_casemap_mode_upper)
    {
        while (src != last)
        {
            src = utf8_iter(src, last, &c, iter_replacement);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
            if (stages_special_upper_check(c))
            {
                type_codept special[3];
                size_t number = stages_special_upper(c, special);
                if (number)
                {
                    for (size_t i = 0; i < number; ++i)
                        dst = codepoint_to_utf8(special[i], dst);

                    continue;
                }
            }
#endif
            c = stages_upper(c);

            dst = codepoint_to_utf8(c, dst);
        }
    }
    else if (mode == impl_casemap_mode_fold)
    {
        while (src != last)
        {
            src = utf8_iter(src, last, &c, iter_replacement);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
            if (stages_special_fold_check(c))
            {
                type_codept special[3];
                size_t number = stages_special_fold(c, special);
                if (number)
                {
                    for (size_t i = 0; i < number; ++i)
                        dst = codepoint_to_utf8(special[i], dst);

                    continue;
                }
            }
#endif
            c = stages_fold(c);

            dst = codepoint_to_utf8(c, dst);
        }
    }
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
    else if (mode == impl_casemap_mode_title)
        return utf8_title(first, last, result);
#endif

    return (size_t)(dst - result);
}

// The Unicode Standard: 3.13 Default Case Algorithms:
// Default Case Conversion (locale-independent)
// Default Case Folding
#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_utf16_casemap(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result, int mode)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;
    type_codept c = 0;

    if (mode == impl_casemap_mode_lower)
    {
        while (src != last)
        {
#ifndef UNI_ALGO_DISABLE_FULL_CASE
            it_in_utf16 prev = src;
#endif
            src = utf16_iter(src, last, &c, iter_replacement);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
            if (c == 0x0130) // Handled in place (checked in generator)
            {
                *dst++ = (type_char16)0x0069;
                *dst++ = (type_char16)0x0307;
                continue;
            }
            if (c == 0x03A3) // Final_Sigma
            {
                if (!utf16_final_sigma(src, last, false) &&
                    utf16_final_sigma(prev, first, true))
                {
                    *dst++ = (type_char16)0x03C2;
                    continue;
                }
            }
#endif
            c = stages_lower(c);

            dst = codepoint_to_utf16(c, dst);
        }
    }
    else if (mode == impl_casemap_mode_upper)
    {
        while (src != last)
        {
            src = utf16_iter(src, last, &c, iter_replacement);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
            if (stages_special_upper_check(c))
            {
                type_codept special[3];
                size_t number = stages_special_upper(c, special);
                if (number)
                {
                    for (size_t i = 0; i < number; ++i)
                        dst = codepoint_to_utf16(special[i], dst);

                    continue;
                }
            }
#endif
            c = stages_upper(c);

            dst = codepoint_to_utf16(c, dst);
        }
    }
    else if (mode == impl_casemap_mode_fold)
    {
        while (src != last)
        {
            src = utf16_iter(src, last, &c, iter_replacement);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
            if (stages_special_fold_check(c))
            {
                type_codept special[3];
                size_t number = stages_special_fold(c, special);
                if (number)
                {
                    for (size_t i = 0; i < number; ++i)
                        dst = codepoint_to_utf16(special[i], dst);

                    continue;
                }
            }
#endif
            c = stages_fold(c);

            dst = codepoint_to_utf16(c, dst);
        }
    }
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
    else if (mode == impl_casemap_mode_title)
        return utf16_title(first, last, result);
#endif

    return (size_t)(dst - result);
}

#ifndef UNI_ALGO_DISABLE_FULL_CASE

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf8 utf8_iter_fold(it_in_utf8 first, it_end_utf8 last, type_codept* codepoint,
                                      size_t* count, size_t* number, type_codept special[3])
{
    it_in_utf8 src = first;

    if (*count == 0)
    {
        src = utf8_iter(first, last, codepoint, iter_replacement);

        if (stages_special_fold_check(*codepoint))
        {
            *number = stages_special_fold(*codepoint, special);
            *count = *number;
        }
    }
    if (*count != 0)
        *codepoint = special[*number - (*count)--];

    return src;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf16 utf16_iter_fold(it_in_utf16 first, it_end_utf16 last, type_codept* codepoint,
                                        size_t* count, size_t* number, type_codept special[3])
{
    it_in_utf16 src = first;

    if (*count == 0)
    {
        src = utf16_iter(first, last, codepoint, iter_replacement);

        if (stages_special_fold_check(*codepoint))
        {
            *number = stages_special_fold(*codepoint, special);
            *count = *number;
        }
    }
    if (*count != 0)
        *codepoint = special[*number - (*count)--];

    return src;
}

#endif // UNI_ALGO_DISABLE_FULL_CASE

// if caseless == false then result in code point order
// if caseless == true then The Unicode Standard: 3.13 Default Caseless Matching
// with result in code point order
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_utf8_compare(it_in_utf8 first1, it_end_utf8 last1,
                                  it_in_utf8 first2, it_end_utf8 last2, bool caseless)
{
    it_in_utf8 src1 = first1;
    it_in_utf8 src2 = first2;
    type_codept c1 = 0;
    type_codept c2 = 0;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special1[3];
    type_codept special2[3];
    size_t count1 = 0, number1 = 0;
    size_t count2 = 0, number2 = 0;
#endif

    // Optimization for long mostly equal strings
    while (src1 != last1 && src2 != last2)
    {
        if (*src1 == *src2) // Fast comparison to locate the difference
        {
            src1 += 1;
            src2 += 1;
        }
        else // Go back by 3 for correct codepoint calculation below
        {
            src1 = (src1 - first1 >= 3) ? src1 - 3 : first1;
            src2 = (src2 - first2 >= 3) ? src2 - 3 : first2;
            break;
        }
    }

    if (!caseless)
    {
        while (src1 != last1 && src2 != last2)
        {
            src1 = utf8_iter(src1, last1, &c1, iter_replacement);
            src2 = utf8_iter(src2, last2, &c2, iter_replacement);

            if (c1 != c2)
                return (c1 < c2) ? -1 : 1;
        }
        return (src2 == last2) - (src1 == last1);
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while ((src1 != last1 || count1) && (src2 != last2 || count2))
    {
        src1 = utf8_iter_fold(src1, last1, &c1, &count1, &number1, special1);
        src2 = utf8_iter_fold(src2, last2, &c2, &count2, &number2, special2);
#else
    while (src1 != last1 && src2 != last2)
    {
        src1 = utf8_iter(src1, last1, &c1, iter_replacement);
        src2 = utf8_iter(src2, last2, &c2, iter_replacement);
#endif
        if (c1 == c2) // Optimization
            continue;

        c1 = stages_fold(c1);
        c2 = stages_fold(c2);

        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    return (src2 == last2 && count2 == 0) - (src1 == last1 && count1 == 0);
#else
    return (src2 == last2) - (src1 == last1);
#endif
}

// if caseless == false then result in code point order
// if caseless == true then The Unicode Standard: 3.13 Default Caseless Matching
// with result in code point order
#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_utf16_compare(it_in_utf16 first1, it_end_utf16 last1,
                                   it_in_utf16 first2, it_end_utf16 last2, bool caseless)
{
    it_in_utf16 src1 = first1;
    it_in_utf16 src2 = first2;
    type_codept c1 = 0;
    type_codept c2 = 0;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special1[3];
    type_codept special2[3];
    size_t count1 = 0, number1 = 0;
    size_t count2 = 0, number2 = 0;
#endif

    // Optimization for long mostly equal strings
    while (src1 != last1 && src2 != last2)
    {
        if (*src1 == *src2) // Fast comparison to locate the difference
        {
            src1 += 1;
            src2 += 1;
        }
        else // Go back by 1 for correct codepoint calculation below
        {
            if (src1 > first1) src1 -= 1;
            if (src2 > first2) src2 -= 1;
            break;
        }
    }

    if (!caseless)
    {
        while (src1 != last1 && src2 != last2)
        {
            src1 = utf16_iter(src1, last1, &c1, iter_replacement);
            src2 = utf16_iter(src2, last2, &c2, iter_replacement);

            if (c1 != c2)
                return (c1 < c2) ? -1 : 1;
        }
        return (src2 == last2) - (src1 == last1);
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while ((src1 != last1 || count1) && (src2 != last2 || count2))
    {
        src1 = utf16_iter_fold(src1, last1, &c1, &count1, &number1, special1);
        src2 = utf16_iter_fold(src2, last2, &c2, &count2, &number2, special2);
#else
    while (src1 != last1 && src2 != last2)
    {
        src1 = utf16_iter(src1, last1, &c1, iter_replacement);
        src2 = utf16_iter(src2, last2, &c2, iter_replacement);
#endif
        if (c1 == c2) // Optimization
            continue;

        c1 = stages_fold(c1);
        c2 = stages_fold(c2);

        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    return (src2 == last2 && count2 == 0) - (src1 == last1 && count1 == 0);
#else
    return (src2 == last2) - (src1 == last1);
#endif
}

#ifndef UNI_ALGO_DISABLE_COLLATE

// if caseless == false then result in DUCET order
// if caseless == true then The Unicode Standard: 3.13 Default Caseless Matching
// with result in DUCET order
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_utf8_collate(it_in_utf8 first1, it_end_utf8 last1,
                                  it_in_utf8 first2, it_end_utf8 last2, bool caseless)
{
    it_in_utf8 src1 = first1;
    it_in_utf8 src2 = first2;
    type_codept c1 = 0;
    type_codept c2 = 0;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special1[3];
    type_codept special2[3];
    size_t count1 = 0, number1 = 0;
    size_t count2 = 0, number2 = 0;
#endif

    // Optimization for long mostly equal strings
    while (src1 != last1 && src2 != last2)
    {
        if (*src1 == *src2) // Fast comparison to locate the difference
        {
            src1 += 1;
            src2 += 1;
        }
        else // Go back by 3 for correct codepoint calculation below
        {
            src1 = (src1 - first1 >= 3) ? src1 - 3 : first1;
            src2 = (src2 - first2 >= 3) ? src2 - 3 : first2;
            break;
        }
    }

    if (!caseless)
    {
        while (src1 != last1 && src2 != last2)
        {
            src1 = utf8_iter(src1, last1, &c1, iter_replacement);
            src2 = utf8_iter(src2, last2, &c2, iter_replacement);

            if (c1 == c2) // Optimization
                continue;

            c1 = stages_order(c1);
            c2 = stages_order(c2);

            if (c1 != c2)
                return (c1 < c2) ? -1 : 1;
        }
        return (src2 == last2) - (src1 == last1);
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while ((src1 != last1 || count1) && (src2 != last2 || count2))
    {
        src1 = utf8_iter_fold(src1, last1, &c1, &count1, &number1, special1);
        src2 = utf8_iter_fold(src2, last2, &c2, &count2, &number2, special2);
#else
    while (src1 != last1 && src2 != last2)
    {
        src1 = utf8_iter(src1, last1, &c1, iter_replacement);
        src2 = utf8_iter(src2, last2, &c2, iter_replacement);
#endif
        if (c1 == c2) // Optimization
            continue;

        c1 = stages_fold(c1);
        c2 = stages_fold(c2);

        c1 = stages_order(c1);
        c2 = stages_order(c2);

        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    return (src2 == last2 && count2 == 0) - (src1 == last1 && count1 == 0);
#else
    return (src2 == last2) - (src1 == last1);
#endif
}

// if caseless == false then result in DUCET order
// if caseless == true then The Unicode Standard: 3.13 Default Caseless Matching
// with result in DUCET order
#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_utf16_collate(it_in_utf16 first1, it_end_utf16 last1,
                                   it_in_utf16 first2, it_end_utf16 last2, bool caseless)
{
    it_in_utf16 src1 = first1;
    it_in_utf16 src2 = first2;
    type_codept c1 = 0;
    type_codept c2 = 0;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special1[3];
    type_codept special2[3];
    size_t count1 = 0, number1 = 0;
    size_t count2 = 0, number2 = 0;
#endif

    // Optimization for long mostly equal strings
    while (src1 != last1 && src2 != last2)
    {
        if (*src1 == *src2) // Fast comparison to locate the difference
        {
            src1 += 1;
            src2 += 1;
        }
        else // Go back by 1 for correct codepoint calculation below
        {
            if (src1 > first1) src1 -= 1;
            if (src2 > first2) src2 -= 1;
            break;
        }
    }

    if (!caseless)
    {
        while (src1 != last1 && src2 != last2)
        {
            src1 = utf16_iter(src1, last1, &c1, iter_replacement);
            src2 = utf16_iter(src2, last2, &c2, iter_replacement);

            if (c1 == c2) // Optimization
                continue;

            c1 = stages_order(c1);
            c2 = stages_order(c2);

            if (c1 != c2)
                return (c1 < c2) ? -1 : 1;
        }
        return (src2 == last2) - (src1 == last1);
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while ((src1 != last1 || count1) && (src2 != last2 || count2))
    {
        src1 = utf16_iter_fold(src1, last1, &c1, &count1, &number1, special1);
        src2 = utf16_iter_fold(src2, last2, &c2, &count2, &number2, special2);
#else
    while (src1 != last1 && src2 != last2)
    {
        src1 = utf16_iter(src1, last1, &c1, iter_replacement);
        src2 = utf16_iter(src2, last2, &c2, iter_replacement);
#endif
        if (c1 == c2) // Optimization
            continue;

        c1 = stages_fold(c1);
        c2 = stages_fold(c2);

        c1 = stages_order(c1);
        c2 = stages_order(c2);

        if (c1 != c2)
            return (c1 < c2) ? -1 : 1;
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    return (src2 == last2 && count2 == 0) - (src1 == last1 && count1 == 0);
#else
    return (src2 == last2) - (src1 == last1);
#endif
}

#endif // UNI_ALGO_DISABLE_COLLATE

// if caseless == false then search by code points
// if caseless == true then The Unicode Standard: 3.13 Default Caseless Matching
// with search by code points
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static bool impl_utf8_search(it_in_utf8 first1, it_end_utf8 last1,
                                  it_in_utf8 first2, it_end_utf8 last2, bool caseless,
                                  size_t* pos, size_t* end)
{
    it_in_utf8 src1 = first1;
    it_in_utf8 src2 = first2;
    it_in_utf8 back = src1;
    it_in_utf8 prev = src1;
    type_codept c1 = 0;
    type_codept c2 = 0;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special1[3];
    type_codept special2[3];
    size_t count1 = 0, number1 = 0;
    size_t count2 = 0, number2 = 0;
#endif

    if (!caseless)
    {
        while (src1 != last1 && src2 != last2)
        {
            src1 = utf8_iter(src1, last1, &c1, iter_replacement);
            src2 = utf8_iter(src2, last2, &c2, iter_replacement);

            if (back == prev)
                back = src1;

            if (c1 != c2) // Restart
            {
                src2 = first2;
                src1 = back;
                prev = back;
                back = prev;
            }
        }

        if (src2 == last2)
        {
            *pos = (size_t)(prev - first1);
            *end = (size_t)(src1 - first1);
            return true;
        }

        return false;
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while ((src1 != last1 || count1) && (src2 != last2 || count2))
    {
        src1 = utf8_iter_fold(src1, last1, &c1, &count1, &number1, special1);
        src2 = utf8_iter_fold(src2, last2, &c2, &count2, &number2, special2);
#else
    while (src1 != last1 && src2 != last2)
    {
        src1 = utf8_iter(src1, last1, &c1, iter_replacement);
        src2 = utf8_iter(src2, last2, &c2, iter_replacement);
#endif
        if (back == prev)
            back = src1;

        //if (c1 == c2) // Optimization // TODO: Test me
        //	continue;

        c1 = stages_fold(c1);
        c2 = stages_fold(c2);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
        if (c1 != c2 || (src2 == last2 && !count2 && count1)) // Restart
        {
            count2 = 0;
            count1 = 0;
#else
        if (c1 != c2) // Restart
        {
#endif
            src2 = first2;
            src1 = back;
            prev = back;
            back = prev;
        }
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    if (src2 == last2 && count2 == 0)
#else
    if (src2 == last2)
#endif
    {
        *pos = (size_t)(prev - first1);
        *end = (size_t)(src1 - first1);
        return true;
    }

    return false;
}

// if caseless == false then search by code points
// if caseless == true then The Unicode Standard: 3.13 Default Caseless Matching
// with search by code points
#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static bool impl_utf16_search(it_in_utf16 first1, it_end_utf16 last1,
                                   it_in_utf16 first2, it_end_utf16 last2, bool caseless,
                                   size_t* pos, size_t* end)
{
    it_in_utf16 src1 = first1;
    it_in_utf16 src2 = first2;
    it_in_utf16 back = src1;
    it_in_utf16 prev = src1;
    type_codept c1 = 0;
    type_codept c2 = 0;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special1[3];
    type_codept special2[3];
    size_t count1 = 0, number1 = 0;
    size_t count2 = 0, number2 = 0;
#endif

    if (!caseless)
    {
        while (src1 != last1 && src2 != last2)
        {
            src1 = utf16_iter(src1, last1, &c1, iter_replacement);
            src2 = utf16_iter(src2, last2, &c2, iter_replacement);

            if (back == prev)
                back = src1;

            if (c1 != c2) // Restart
            {
                src2 = first2;
                src1 = back;
                prev = back;
                back = prev;
            }
        }

        if (src2 == last2)
        {
            *pos = (size_t)(prev - first1);
            *end = (size_t)(src1 - first1);
            return true;
        }

        return false;
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while ((src1 != last1 || count1) && (src2 != last2 || count2))
    {
        src1 = utf16_iter_fold(src1, last1, &c1, &count1, &number1, special1);
        src2 = utf16_iter_fold(src2, last2, &c2, &count2, &number2, special2);
#else
    while (src1 != last1 && src2 != last2)
    {
        src1 = utf16_iter(src1, last1, &c1, iter_replacement);
        src2 = utf16_iter(src2, last2, &c2, iter_replacement);
#endif
        if (back == prev)
            back = src1;

        //if (c1 == c2) // Optimization // TODO: Test me
        //	continue;

        c1 = stages_fold(c1);
        c2 = stages_fold(c2);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
        if (c1 != c2 || (src2 == last2 && !count2 && count1)) // Restart
        {
            count2 = 0;
            count1 = 0;
#else
        if (c1 != c2) // Restart
        {
#endif
            src2 = first2;
            src1 = back;
            prev = back;
            back = prev;
        }
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    if (src2 == last2 && count2 == 0)
#else
    if (src2 == last2)
#endif
    {
        *pos = (size_t)(prev - first1);
        *end = (size_t)(src1 - first1);
        return true;
    }

    return false;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static bool impl_utf8_like(it_in_utf8 first1, it_end_utf8 last1,
                                it_in_utf8 first2, it_end_utf8 last2, bool caseless,
                                type_codept all, type_codept one, type_codept escape)
{
    it_in_utf8 src1 = first1;
    it_in_utf8 src2 = first2;
    it_in_utf8 back1 = src1;
    it_in_utf8 prev1 = src1;
    it_in_utf8 back2 = src2;
    type_codept c1 = 0;
    type_codept c2 = 0;
    bool skip1 = false;
    bool multi = false;
    bool prev_escape = false;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special1[3];
    type_codept special2[3];
    size_t count1 = 0, number1 = 0;
    size_t count2 = 0, number2 = 0;
#endif

    // Only ASCII is supported for all and one in a pattern
    // because we use code units in some places to simplify the code
    // it is not affects escape
    if (all > 0x7F || one > 0x7F)
        return false;

    // If all == escape or one == escape set it to a code point
    // that is outside Unicode range so we just ignore one of them
    if (all == escape) all = 0xFFFFFFFF;
    if (one == escape) one = 0xFFFFFFFF;

    if (!caseless) // no-op
        return false;

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while ((src1 != last1 || count1 || skip1) && (src2 != last2 || count2))
    {
        if (skip1)
            skip1 = false;
        else
            src1 = utf8_iter_fold(src1, last1, &c1, &count1, &number1, special1);
        src2 = utf8_iter_fold(src2, last2, &c2, &count2, &number2, special2);
#else
    while ((src1 != last1 || skip1) && src2 != last2)
    {
        if (skip1)
            skip1 = false;
        else
            src1 = utf8_iter(src1, last1, &c1, iter_replacement);
        src2 = utf8_iter(src2, last2, &c2, iter_replacement);
#endif

        if (back1 == prev1)
            back1 = src1;

        if (prev_escape)
            prev_escape = false;
        else
        {
            // If escape == 0 it won't be used
            if (escape && c2 == escape)
            {
                prev_escape = true;
                skip1 = true;
                continue;
            }
            if (c2 == all)
            {
                back1 = src1;
                back2 = src2;
                skip1 = true;
                multi = true;
                continue;
            }
            if (c2 == one)
                continue;
        }

        //if (c1 == c2) // Optimization // TODO: Test me
        //	continue;

        c1 = stages_fold(c1);
        c2 = stages_fold(c2);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
        if (c1 != c2
            || (src2 != last2 && ((type_codept)*src2 == all || (type_codept)*src2 == one)
            && !count2 && count1)) // Restart
        {
            count2 = 0;
            count1 = 0;
#else
        if (c1 != c2) // Restart
        {
#endif
            if (multi)
                src2 = back2;
            else
                return false;

            src1 = back1;
            prev1 = back1;
            back1 = prev1;
        }
    }

    //if (src2 == last2 && (src1 == last1 || c2 == '%'))
    //	return true;
    //if ((src2 == last2 || (*src2 == '%' && ++src2 == last2)) && (src1 == last1 || c2 == '%'))
    //	return true;

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    // Test: "abc123xyz789" "%abc%xyz%"
    //if ((src1 == last1 && count1 == 0) || (first2 == last2 || c2 == all)) // second empty = not match
    if ((src1 == last1 && count1 == 0) || (first2 != last2 && c2 == all)) // second empty = match
    {
#else
    //if (src1 == last1 || (first2 == last2 || c2 == all)) // second empty = not match
    if (src1 == last1 || (first2 != last2 && c2 == all)) // second empty = match
    {
#endif
        // Test: "abc123xyz" "%abc%xyz%" | "abc123xyz" "%abc%xyz%%%"
        while (src2 != last2 && (type_codept)*src2 == all) ++src2;

        if (src2 == last2)
            return true;
    }

    return false;
}

#ifndef UNI_ALGO_DISABLE_BREAK_WORD

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t utf8_title(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    /* Title case rules from The Unicode Standard:
     * toTitlecase(X): Find the word boundaries in X according to The Unicode Standard Annex #29,
     * "Unicode Text Segmentation." For each word boundary, find the first cased character F
     * following the word boundary. If F exists, map F to Titlecase_Mapping(F);
     * then map all characters C between F and the following word boundary to Lowercase_Mapping(C).
     */

    it_in_utf8 src = first;
    it_in_utf8 brk = first;
    it_in_utf8 prev_brk = first;
    it_out_utf8 dst = result;
    type_codept c = 0;
    bool found_break = false;
    bool make_lower = false;

    type_codept word_prop = 0; // Not used here
    struct impl_break_word_state state = {0,0,0,0,0,0,0};
    impl_break_word_state_reset(&state);

    while (src != last)
    {
        it_in_utf8 prev = src;
        src = utf8_iter(src, last, &c, iter_replacement);

        if (!found_break)
        {
            // Find break (if at the end then force break)
            if (utf8_break_word(&state, c, &word_prop, src, last) || src == last)
            {
                //brk = prev;
                brk = src;
                src = prev_brk;
                prev_brk = prev;
                found_break = true;
                make_lower = false;
            }
            continue;
        }
        //else if (brk == prev)
        else if (brk == src) // Found previous break
        {
            found_break = false;
            // If not at the end then find next break else output last code point
            if (src != last)
                continue;
        }

        if (!make_lower)
        {
            // If Cased then make it title else output as is
            if (stages_case_prop(c) & prop_Cased)
            {
                // Make lower all code points after title
                make_lower = true;

#ifndef UNI_ALGO_DISABLE_FULL_CASE
                if (stages_special_title_check(c))
                {
                    type_codept special[3];
                    size_t number = stages_special_title(c, special);
                    if (number)
                    {
                        for (size_t i = 0; i < number; ++i)
                            dst = codepoint_to_utf8(special[i], dst);

                        continue;
                    }
                }
#endif
                c = stages_title(c);
            }
        }
        else
        {
#ifndef UNI_ALGO_DISABLE_FULL_CASE
            if (c == 0x0130) // Handled in place (checked in generator)
            {
                *dst++ = (type_char8)(type_codept)0x69;
                *dst++ = (type_char8)(type_codept)0xCC;
                *dst++ = (type_char8)(type_codept)0x87;
                continue;
            }
            if (c == 0x03A3) // Final_Sigma
            {
                if (!utf8_final_sigma(src, last, false) &&
                    utf8_final_sigma(prev, first, true))
                {
                    *dst++ = (type_char8)(type_codept)0xCF;
                    *dst++ = (type_char8)(type_codept)0x82;
                    continue;
                }
            }
#endif
            c = stages_lower(c);
        }

        dst = codepoint_to_utf8(c, dst);
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t utf16_title(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_in_utf16 brk = first;
    it_in_utf16 prev_brk = first;
    it_out_utf16 dst = result;
    type_codept c = 0;
    bool found_break = false;
    bool make_lower = false;

    type_codept word_prop = 0; // Not used here
    struct impl_break_word_state state = {0,0,0,0,0,0,0};
    impl_break_word_state_reset(&state);

    while (src != last)
    {
        it_in_utf16 prev = src;
        src = utf16_iter(src, last, &c, iter_replacement);

        if (!found_break)
        {
            // Find break (if at the end then force break)
            if (utf16_break_word(&state, c, &word_prop, src, last) || src == last)
            {
                //brk = prev;
                brk = src;
                src = prev_brk;
                prev_brk = prev;
                found_break = true;
                make_lower = false;
            }
            continue;
        }
        //else if (brk == prev)
        else if (brk == src) // Found previous break
        {
            found_break = false;
            // If not at the end then find next break else output last code point
            if (src != last)
                continue;
        }

        if (!make_lower)
        {
            // If Cased then make it title else output as is
            if (stages_case_prop(c) & prop_Cased)
            {
                // Make lower all code points after title
                make_lower = true;

#ifndef UNI_ALGO_DISABLE_FULL_CASE
                if (stages_special_title_check(c))
                {
                    type_codept special[3];
                    size_t number = stages_special_title(c, special);
                    if (number)
                    {
                        for (size_t i = 0; i < number; ++i)
                            dst = codepoint_to_utf16(special[i], dst);

                        continue;
                    }
                }
#endif
                c = stages_title(c);
            }
        }
        else
        {
#ifndef UNI_ALGO_DISABLE_FULL_CASE
            if (c == 0x0130) // Handled in place (checked in generator)
            {
                *dst++ = (type_char16)0x0069;
                *dst++ = (type_char16)0x0307;
                continue;
            }
            else if (c == 0x03A3) // Final_Sigma
            {
                if (!utf16_final_sigma(src, last, false) &&
                    utf16_final_sigma(prev, first, true))
                {
                    *dst++ = (type_char16)0x03C2;
                    continue;
                }
            }
#endif
            c = stages_lower(c);
        }

        dst = codepoint_to_utf16(c, dst);
    }

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_BREAK_WORD

// This is just an example how sort keys can look with this simple collation algorithm.
// They are not that usefull so these functions are disabled for now.
#ifdef UNI_ALGO_EXPERIMENTAL

#ifndef UNI_ALGO_DISABLE_COLLATE

#ifndef UNI_ALGO_DISABLE_FULL_CASE
uaix_const size_t impl_x_utf8_sortkey = 5;
uaix_const size_t impl_x_utf16_sortkey = 9;
#else
uaix_const size_t impl_x_utf8_sortkey = 3;
uaix_const size_t impl_x_utf16_sortkey = 3;
#endif

#ifdef __cplusplus
template<typename it_out_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf8 codepoint_to_sortkey(type_codept c, it_out_utf8 dst)
{
    // Store sort key as 3 bytes for every code point, without null bytes
    // This is not very effective but at least this is simple and fast
    // xxxxxxxx+1 1xxxxxxx 1xxxxxxx (max is 0x3FBFFF (256-1)*128*128)
    // our code point weight cannot be > 0x3FBFFF
    *dst++ = (type_char8)(        (c >> 14) + 1);
    *dst++ = (type_char8)(0x80 | ((c >> 7) & 0x7F));
    *dst++ = (type_char8)(0x80 |  (c       & 0x7F));

    return dst;
}

// This function guarantees that comparing 2 sort keys with binary comparison
// gives exactly the same result as comparing 2 strings with impl_utf8_collate
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_utf8_sortkey(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result, int caseless)
{
    it_in_utf8 src = first;
    it_out_utf8 dst = result;
    type_codept c = 0;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special[3];
    size_t count = 0, number = 0;
#endif

    // The algorithm must be consistent with impl_utf8collate

    if (!caseless)
    {
        while (src != last)
        {
            src = utf8_iter(src, last, &c, iter_replacement);

            c = stages_order(c);

            dst = codepoint_to_sortkey(c, dst);
        }

        return (size_t)(dst - result);
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while (src != last || count)
    {
        src = utf8_iter_fold(src, last, &c, &count, &number, special);
#else
    while (src != last)
    {
        src = utf8_iter(src, last, &c, iter_replacement);
#endif
        c = stages_fold(c);
        c = stages_order(c);

        dst = codepoint_to_sortkey(c, dst);
    }

    return (size_t)(dst - result);
}

// This function guarantees that comparing 2 sort keys with binary comparison
// gives exactly the same result as comparing 2 strings with impl_utf16_collate
#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf8>
#endif
uaix_static size_t impl_utf16_sortkey(it_in_utf16 first, it_end_utf16 last, it_out_utf8 result, int caseless)
{
    it_in_utf16 src = first;
    it_out_utf8 dst = result;
    type_codept c = 0;
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    type_codept special[3];
    size_t count = 0, number = 0;
#endif

    // The algorithm must be consistent with impl_utf8collate

    if (!caseless)
    {
        while (src != last)
        {
            src = utf16_iter(src, last, &c, iter_replacement);

            c = stages_order(c);

            dst = codepoint_to_sortkey(c, dst);
        }

        return (size_t)(dst - result);
    }

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    while (src != last || count)
    {
        src = utf16_iter_fold(src, last, &c, &count, &number, special);
#else
    while (src != last)
    {
        src = utf16_iter(src, last, &c, iter_replacement);
#endif
        c = stages_fold(c);
        c = stages_order(c);

        dst = codepoint_to_sortkey(c, dst);
    }

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_COLLATE

#endif // UNI_ALGO_EXPERIMENTAL

UNI_ALGO_IMPL_NAMESPACE_END

#include <uni/internal/undefs.h>

#endif // IMPL_CASE_H_UAIX

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
