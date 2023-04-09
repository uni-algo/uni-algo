/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_IMPL_CASE_LOCALE_H_UAIH
#define UNI_ALGO_IMPL_CASE_LOCALE_H_UAIH

// The file is an extension for Case module and must be included after impl_case.h
// The file provides only 2 functions: impl_utf8_casemap_locale and impl_utf16_casemap_locale
// Many things in the file are generated so edit it with care
// See SpecialCasing.txt for data and The Unicode Standard 3.13 for rules

#include "internal_defines.h"

#ifdef UNI_ALGO_DISABLE_FULL_CASE
#error "The file cannot be used if full case mapping is disabled"
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

uaix_const type_codept case_map_language_lt = 0x6C740000;
uaix_const type_codept case_map_language_tr = 0x74720000;
uaix_const type_codept case_map_language_az = 0x617A0000;
uaix_const type_codept case_map_language_el = 0x656C0000;
uaix_const type_codept case_map_language_nl = 0x6E6C0000;

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static bool case_after_soft_dotted_utf8(it_in_utf8 first, it_in_utf8 src)
{
    /* After_Soft_Dotted special case from The Unicode Standard:
     * There is a Soft_Dotted character before C,
     * with no intervening character of combining class 0 or 230 (Above).
     * [\p{Soft_Dotted}] ([^\p{ccc=230} \p{ccc=0}])*
     */

    it_in_utf8 s = src;
    type_codept c = 0; // tag_can_be_uninitialized

    while (s != first)
    {
        s = iter_rev_utf8(first, s, &c, iter_replacement);

        const type_codept prop = stages_case_prop(c);

        if (prop & prop_Soft_Dotted)
            return true;
        if (!(prop & prop_CCC_NOT_0) || (prop & prop_CCC_230))
            return false;
    }

    return false;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static bool case_more_above_utf8(it_in_utf8 src, it_end_utf8 last)
{
    /* More_Above special case from The Unicode Standard:
     * C is followed by a character of combining class 230 (Above)
     * with no intervening character of combining class 0 or 230 (Above).
     * [^\p{ccc=230}\p{ccc=0}]* [\p{ccc=230}]
     * NOTE: It seems the rule actually is [^\p{ccc=0}]* [\p{ccc=230}]
     * it passes all tests this way and the behaviour is the same as in ICU
     */

    it_in_utf8 s = src;
    type_codept c = 0; // tag_can_be_uninitialized

    while (s != last)
    {
        s = iter_utf8(s, last, &c, iter_replacement);

        const type_codept prop = stages_case_prop(c);

        if (prop & prop_CCC_230)
            return true;
        if (!(prop & prop_CCC_NOT_0))
            return false;
    }

    return false;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static bool case_before_dot_utf8(it_in_utf8 src, it_end_utf8 last)
{
    /* Before_Dot special case from The Unicode Standard:
     * C is followed by combining dot above (U+0307).
     * Any sequence of characters with a combining class that is neither 0 nor 230
     * may intervene between the current character and the combining dot above.
     * ([^\p{ccc=230} \p{ccc=0}])* [\u0307]
     */

    it_in_utf8 s = src;
    type_codept c = 0; // tag_can_be_uninitialized

    while (s != last)
    {
        s = iter_utf8(s, last, &c, iter_replacement);

        if (c == 0x0307)
            return true;

        const type_codept prop = stages_case_prop(c);

        if (!(prop & prop_CCC_NOT_0) || (prop & prop_CCC_230))
            return false;
    }

    return false;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static bool case_after_i_utf8(it_in_utf8 first, it_in_utf8 src)
{
    /* After_I special case from The Unicode Standard:
     * There is an uppercase I before C,
     * and there is no intervening combining character class 230 (Above) or 0.
     * [I] ([^\p{ccc=230} \p{ccc=0}])*
     */

    it_in_utf8 s = src;
    type_codept c = 0; // tag_can_be_uninitialized

    while (s != first)
    {
        s = iter_rev_utf8(first, s, &c, iter_replacement);

        if (c == 0x0049) // I
            return true;

        const type_codept prop = stages_case_prop(c);

        if (!(prop & prop_CCC_NOT_0) || (prop & prop_CCC_230))
            return false;
    }

    return false;
}

// BEGIN: GENERATED UTF-16 FUNCTIONS
#ifndef UNI_ALGO_DOC_GENERATED_UTF16

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static bool case_after_soft_dotted_utf16(it_in_utf16 first, it_in_utf16 src)
{
    it_in_utf16 s = src;
    type_codept c = 0; // tag_can_be_uninitialized

    while (s != first)
    {
        s = iter_rev_utf16(first, s, &c, iter_replacement);

        const type_codept prop = stages_case_prop(c);

        if (prop & prop_Soft_Dotted)
            return true;
        if (!(prop & prop_CCC_NOT_0) || (prop & prop_CCC_230))
            return false;
    }

    return false;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static bool case_more_above_utf16(it_in_utf16 src, it_end_utf16 last)
{
    it_in_utf16 s = src;
    type_codept c = 0; // tag_can_be_uninitialized

    while (s != last)
    {
        s = iter_utf16(s, last, &c, iter_replacement);

        const type_codept prop = stages_case_prop(c);

        if (prop & prop_CCC_230)
            return true;
        if (!(prop & prop_CCC_NOT_0))
            return false;
    }

    return false;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static bool case_before_dot_utf16(it_in_utf16 src, it_end_utf16 last)
{
    it_in_utf16 s = src;
    type_codept c = 0; // tag_can_be_uninitialized

    while (s != last)
    {
        s = iter_utf16(s, last, &c, iter_replacement);

        if (c == 0x0307)
            return true;

        const type_codept prop = stages_case_prop(c);

        if (!(prop & prop_CCC_NOT_0) || (prop & prop_CCC_230))
            return false;
    }

    return false;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static bool case_after_i_utf16(it_in_utf16 first, it_in_utf16 src)
{
    it_in_utf16 s = src;
    type_codept c = 0; // tag_can_be_uninitialized

    while (s != first)
    {
        s = iter_rev_utf16(first, s, &c, iter_replacement);

        if (c == 0x0049) // I
            return true;

        const type_codept prop = stages_case_prop(c);

        if (!(prop & prop_CCC_NOT_0) || (prop & prop_CCC_230))
            return false;
    }

    return false;
}

#endif // UNI_ALGO_DOC_GENERATED_UTF16
// END: GENERATED UTF-16 FUNCTIONS

#ifdef __cplusplus
template<typename it_out_utf8, typename it_in_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf8 case_locale_upper_lt_utf8(type_codept c, it_out_utf8 dst,
                                                  it_in_utf8 first, it_in_utf8 prev)
{
    // SpecialCasing.txt # Lithuanian

    if (c == 0x0307 && case_after_soft_dotted_utf8(first, prev))
        return dst;

    // The next part is the same as in impl_case_map_utf8

    if (stages_special_upper_check(c))
    {
        struct case_special_buffer buffer = {{0}}; // tag_can_be_uninitialized
        const size_t size = stages_special_upper(c, &buffer);

        for (size_t i = 0; i < size; ++i)
            dst = codepoint_to_utf8(buffer.cps[i], dst);

        return dst;
    }

    c = stages_upper(c);

    dst = codepoint_to_utf8(c, dst);

    return dst;
}

#ifdef __cplusplus
template<typename it_out_utf8, typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf8 case_locale_lower_lt_utf8(type_codept c, it_out_utf8 dst,
                                                  it_in_utf8 src, it_end_utf8 last,
                                                  it_in_utf8 first, it_in_utf8 prev)
{
    // SpecialCasing.txt # Lithuanian

    if (c == 0x0049 && case_more_above_utf8(src, last))
    {
        *dst++ = (type_char8)(type_codept)0x69;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x87;
        return dst;
    }
    if (c == 0x004A && case_more_above_utf8(src, last))
    {
        *dst++ = (type_char8)(type_codept)0x6A;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x87;
        return dst;
    }
    if (c == 0x012E && case_more_above_utf8(src, last))
    {
        *dst++ = (type_char8)(type_codept)0xC4;
        *dst++ = (type_char8)(type_codept)0xAF;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x87;
        return dst;
    }
    if (c == 0x00CC)
    {
        *dst++ = (type_char8)(type_codept)0x69;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x87;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x80;
        return dst;
    }
    if (c == 0x00CD)
    {
        *dst++ = (type_char8)(type_codept)0x69;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x87;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x81;
        return dst;
    }
    if (c == 0x0128)
    {
        *dst++ = (type_char8)(type_codept)0x69;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x87;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x83;
        return dst;
    }

    // The next part is the same as in impl_case_map_utf8

    if (c == 0x0130) // tag_lowercase_special
    {
        *dst++ = (type_char8)(type_codept)0x69;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x87;
        return dst;
    }
    if (c == 0x03A3) // Final_Sigma
    {
        if (!case_final_sigma_fwd_utf8(src, last) &&
            case_final_sigma_rev_utf8(first, prev))
        {
            *dst++ = (type_char8)(type_codept)0xCF;
            *dst++ = (type_char8)(type_codept)0x82;
            return dst;
        }
    }

    c = stages_lower(c);

    dst = codepoint_to_utf8(c, dst);

    return dst;
}

#ifdef __cplusplus
template<typename it_out_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf8 case_locale_upper_tr_az_utf8(type_codept c, it_out_utf8 dst)
{
    // SpecialCasing.txt # Turkish and Azeri

    if (c == 0x0069)
    {
        *dst++ = (type_char8)(type_codept)0xC4;
        *dst++ = (type_char8)(type_codept)0xB0;
        return dst;
    }

    // The next part is the same as in impl_case_map_utf8

    if (stages_special_upper_check(c))
    {
        struct case_special_buffer buffer = {{0}}; // tag_can_be_uninitialized
        const size_t size = stages_special_upper(c, &buffer);

        for (size_t i = 0; i < size; ++i)
            dst = codepoint_to_utf8(buffer.cps[i], dst);

        return dst;
    }

    c = stages_upper(c);

    dst = codepoint_to_utf8(c, dst);

    return dst;
}

#ifdef __cplusplus
template<typename it_out_utf8, typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf8 case_locale_lower_tr_az_utf8(type_codept c, it_out_utf8 dst,
                                                     it_in_utf8 src, it_end_utf8 last,
                                                     it_in_utf8 first, it_in_utf8 prev)
{
    // SpecialCasing.txt # Turkish and Azeri

    if (c == 0x0130)
    {
        *dst++ = (type_char8)(type_codept)0x69;
        return dst;
    }
    if (c == 0x0307 && case_after_i_utf8(first, prev))
    {
        return dst;
    }
    if (c == 0x0049 && !case_before_dot_utf8(src, last))
    {
        *dst++ = (type_char8)(type_codept)0xC4;
        *dst++ = (type_char8)(type_codept)0xB1;
        return dst;
    }

    // The next part is the same as in impl_case_map_utf8

    if (c == 0x0130) // tag_lowercase_special
    {
        *dst++ = (type_char8)(type_codept)0x69;
        *dst++ = (type_char8)(type_codept)0xCC;
        *dst++ = (type_char8)(type_codept)0x87;
        return dst;
    }
    if (c == 0x03A3) // Final_Sigma
    {
        if (!case_final_sigma_fwd_utf8(src, last) &&
            case_final_sigma_rev_utf8(first, prev))
        {
            *dst++ = (type_char8)(type_codept)0xCF;
            *dst++ = (type_char8)(type_codept)0x82;
            return dst;
        }
    }

    c = stages_lower(c);

    dst = codepoint_to_utf8(c, dst);

    return dst;
}

#ifdef __cplusplus
template<typename it_out_utf16, typename it_in_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf16 case_locale_upper_lt_utf16(type_codept c, it_out_utf16 dst,
                                                    it_in_utf16 first, it_in_utf16 prev)
{
    // SpecialCasing.txt # Lithuanian

    if (c == 0x0307 && case_after_soft_dotted_utf16(first, prev))
        return dst;

    // The next part is the same as in impl_case_map_utf16

    if (stages_special_upper_check(c))
    {
        struct case_special_buffer buffer = {{0}}; // tag_can_be_uninitialized
        const size_t size = stages_special_upper(c, &buffer);

        for (size_t i = 0; i < size; ++i)
            dst = codepoint_to_utf16(buffer.cps[i], dst);

        return dst;
    }

    c = stages_upper(c);

    dst = codepoint_to_utf16(c, dst);

    return dst;
}

#ifdef __cplusplus
template<typename it_out_utf16, typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf16 case_locale_lower_lt_utf16(type_codept c, it_out_utf16 dst,
                                                    it_in_utf16 src, it_end_utf16 last,
                                                    it_in_utf16 first, it_in_utf16 prev)
{
    // SpecialCasing.txt # Lithuanian

    if (c == 0x0049 && case_more_above_utf16(src, last))
    {
        *dst++ = (type_char16)0x0069;
        *dst++ = (type_char16)0x0307;
        return dst;
    }
    if (c == 0x004A && case_more_above_utf16(src, last))
    {
        *dst++ = (type_char16)0x006A;
        *dst++ = (type_char16)0x0307;
        return dst;
    }
    if (c == 0x012E && case_more_above_utf16(src, last))
    {
        *dst++ = (type_char16)0x012F;
        *dst++ = (type_char16)0x0307;
        return dst;
    }
    if (c == 0x00CC)
    {
        *dst++ = (type_char16)0x0069;
        *dst++ = (type_char16)0x0307;
        *dst++ = (type_char16)0x0300;
        return dst;
    }
    if (c == 0x00CD)
    {
        *dst++ = (type_char16)0x0069;
        *dst++ = (type_char16)0x0307;
        *dst++ = (type_char16)0x0301;
        return dst;
    }
    if (c == 0x0128)
    {
        *dst++ = (type_char16)0x0069;
        *dst++ = (type_char16)0x0307;
        *dst++ = (type_char16)0x0303;
        return dst;
    }

    // The next part is the same as in impl_case_map_utf16

    if (c == 0x0130) // tag_lowercase_special
    {
        *dst++ = (type_char16)0x0069;
        *dst++ = (type_char16)0x0307;
        return dst;
    }
    if (c == 0x03A3) // Final_Sigma
    {
        if (!case_final_sigma_fwd_utf16(src, last) &&
            case_final_sigma_rev_utf16(first, prev))
        {
            *dst++ = (type_char16)0x03C2;
            return dst;
        }
    }

    c = stages_lower(c);

    dst = codepoint_to_utf16(c, dst);

    return dst;
}

#ifdef __cplusplus
template<typename it_out_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf16 case_locale_upper_tr_az_utf16(type_codept c, it_out_utf16 dst)
{
    // SpecialCasing.txt # Turkish and Azeri

    if (c == 0x0069)
    {
        *dst++ = (type_char16)0x0130;
        return dst;
    }

    // The next part is the same as in impl_case_map_utf16

    if (stages_special_upper_check(c))
    {
        struct case_special_buffer buffer = {{0}}; // tag_can_be_uninitialized
        const size_t size = stages_special_upper(c, &buffer);

        for (size_t i = 0; i < size; ++i)
            dst = codepoint_to_utf16(buffer.cps[i], dst);

        return dst;
    }

    c = stages_upper(c);

    dst = codepoint_to_utf16(c, dst);

    return dst;
}

#ifdef __cplusplus
template<typename it_out_utf16, typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_out_utf16 case_locale_lower_tr_az_utf16(type_codept c, it_out_utf16 dst,
                                                       it_in_utf16 src, it_end_utf16 last,
                                                       it_in_utf16 first, it_in_utf16 prev)
{
    // SpecialCasing.txt # Turkish and Azeri

    if (c == 0x0130)
    {
        *dst++ = (type_char16)0x0069;
        return dst;
    }
    if (c == 0x0307 && case_after_i_utf16(first, prev))
    {
        return dst;
    }
    if (c == 0x0049 && !case_before_dot_utf16(src, last))
    {
        *dst++ = (type_char16)0x0131;
        return dst;
    }

    // The next part is the same as in impl_case_map_utf16

    if (c == 0x0130) // tag_lowercase_special
    {
        *dst++ = (type_char16)0x0069;
        *dst++ = (type_char16)0x0307;
        return dst;
    }
    if (c == 0x03A3) // Final_Sigma
    {
        if (!case_final_sigma_fwd_utf16(src, last) &&
            case_final_sigma_rev_utf16(first, prev))
        {
            *dst++ = (type_char16)0x03C2;
            return dst;
        }
    }

    c = stages_lower(c);

    dst = codepoint_to_utf16(c, dst);

    return dst;
}

uaix_const uaix_array(type_codept, case_locale_el_map_0370_03FF, 144) =
{
    // U+0370..03FF
    0x0370,
    0x0370,
    0x0372,
    0x0372,
    0,
    0,
    0x0376,
    0x0376,
    0,
    0,
    0x037A,
    0x03FD,
    0x03FE,
    0x03FF,
    0,
    0x037F,
    0,
    0,
    0,
    0,
    0,
    0,
    0x0391 | 0x1000 | 0x4000,
    0,
    0x0395 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0,
    0x039F | 0x1000 | 0x4000,
    0,
    0x03A5 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000 | 0x8000,
    0x0391 | 0x1000,
    0x0392,
    0x0393,
    0x0394,
    0x0395 | 0x1000,
    0x0396,
    0x0397 | 0x1000,
    0x0398,
    0x0399 | 0x1000,
    0x039A,
    0x039B,
    0x039C,
    0x039D,
    0x039E,
    0x039F | 0x1000,
    0x03A0,
    0x03A1,
    0,
    0x03A3,
    0x03A4,
    0x03A5 | 0x1000,
    0x03A6,
    0x03A7,
    0x03A8,
    0x03A9 | 0x1000,
    0x0399 | 0x1000 | 0x8000,
    0x03A5 | 0x1000 | 0x8000,
    0x0391 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000 | 0x8000,
    0x0391 | 0x1000,
    0x0392,
    0x0393,
    0x0394,
    0x0395 | 0x1000,
    0x0396,
    0x0397 | 0x1000,
    0x0398,
    0x0399 | 0x1000,
    0x039A,
    0x039B,
    0x039C,
    0x039D,
    0x039E,
    0x039F | 0x1000,
    0x03A0,
    0x03A1,
    0x03A3,
    0x03A3,
    0x03A4,
    0x03A5 | 0x1000,
    0x03A6,
    0x03A7,
    0x03A8,
    0x03A9 | 0x1000,
    0x0399 | 0x1000 | 0x8000,
    0x03A5 | 0x1000 | 0x8000,
    0x039F | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03CF,
    0x0392,
    0x0398,
    0x03D2,
    0x03D2 | 0x4000,
    0x03D2 | 0x8000,
    0x03A6,
    0x03A0,
    0x03CF,
    0x03D8,
    0x03D8,
    0x03DA,
    0x03DA,
    0x03DC,
    0x03DC,
    0x03DE,
    0x03DE,
    0x03E0,
    0x03E0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0x039A,
    0x03A1,
    0x03F9,
    0x037F,
    0x03F4,
    0x0395 | 0x1000,
    0,
    0x03F7,
    0x03F7,
    0x03F9,
    0x03FA,
    0x03FA,
    0x03FC,
    0x03FD,
    0x03FE,
    0x03FF,
};

uaix_const uaix_array(type_codept, case_locale_el_map_1F00_1FFF, 256) =
{
    // U+1F00..1FFF
    0x0391 | 0x1000,
    0x0391 | 0x1000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000,
    0x0391 | 0x1000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0395 | 0x1000,
    0x0395 | 0x1000,
    0x0395 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0,
    0,
    0x0395 | 0x1000,
    0x0395 | 0x1000,
    0x0395 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0,
    0,
    0x0397 | 0x1000,
    0x0397 | 0x1000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000,
    0x0397 | 0x1000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0399 | 0x1000,
    0x0399 | 0x1000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000,
    0x0399 | 0x1000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x039F | 0x1000,
    0x039F | 0x1000,
    0x039F | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0,
    0,
    0x039F | 0x1000,
    0x039F | 0x1000,
    0x039F | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0,
    0,
    0x03A5 | 0x1000,
    0x03A5 | 0x1000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0,
    0x03A5 | 0x1000,
    0,
    0x03A5 | 0x1000 | 0x4000,
    0,
    0x03A5 | 0x1000 | 0x4000,
    0,
    0x03A5 | 0x1000 | 0x4000,
    0x03A9 | 0x1000,
    0x03A9 | 0x1000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000,
    0x03A9 | 0x1000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0,
    0,
    0x0391 | 0x1000 | 0x2000,
    0x0391 | 0x1000 | 0x2000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000,
    0x0391 | 0x1000 | 0x2000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000,
    0x0397 | 0x1000 | 0x2000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000,
    0x0397 | 0x1000 | 0x2000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000,
    0x03A9 | 0x1000 | 0x2000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000,
    0x03A9 | 0x1000 | 0x2000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000,
    0x0391 | 0x1000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000 | 0x2000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x2000 | 0x4000,
    0x0391 | 0x1000,
    0x0391 | 0x1000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x4000,
    0x0391 | 0x1000 | 0x2000,
    0,
    0x0399 | 0x1000,
    0,
    0,
    0,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0397 | 0x1000 | 0x2000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x2000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0395 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x4000,
    0x0397 | 0x1000 | 0x2000,
    0,
    0,
    0,
    0x0399 | 0x1000,
    0x0399 | 0x1000,
    0x0399 | 0x1000 | 0x4000 | 0x8000,
    0x0399 | 0x1000 | 0x4000 | 0x8000,
    0,
    0,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000 | 0x8000,
    0x0399 | 0x1000,
    0x0399 | 0x1000,
    0x0399 | 0x1000 | 0x4000,
    0x0399 | 0x1000 | 0x4000,
    0,
    0,
    0,
    0,
    0x03A5 | 0x1000,
    0x03A5 | 0x1000,
    0x03A5 | 0x1000 | 0x4000 | 0x8000,
    0x03A5 | 0x1000 | 0x4000 | 0x8000,
    0x03A1,
    0x03A1,
    0x03A5 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000 | 0x8000,
    0x03A5 | 0x1000,
    0x03A5 | 0x1000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A5 | 0x1000 | 0x4000,
    0x03A1,
    0,
    0,
    0,
    0,
    0,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0x039F | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x4000,
    0x03A9 | 0x1000 | 0x2000,
    0,
    0,
    0,
};

uaix_always_inline
uaix_static type_codept case_greek_upper(type_codept c, bool* const prev_vowel_with_accent, bool* const maybe_eta_with_tonos)
{
    // The data and the algorithm is based on: http://site.icu-project.org/design/case/greek-upper

    const type_codept has_vowel = 0x1000;
    const type_codept has_ypogegrammeni = 0x2000;
    const type_codept has_accent = 0x4000;
    const type_codept has_dialytika = 0x8000;

    type_codept data = 0; // tag_can_be_uninitialized

    // Unicode ranges for Greek are U+0370..03FF and U+1F00..1FFF and one U+2126
    // Ignore other code points
    if (c < 0x0370 || c > 0x2126 || (c > 0x03FF && c < 0x1F00))
        data = 0; // NOLINT(bugprone-branch-clone)
    else if (c <= 0x03FF)
        data = case_locale_el_map_0370_03FF[c - 0x0370];
    else if (c <= 0x1FFF)
        data = case_locale_el_map_1F00_1FFF[c - 0x1F00];
    else if (c == 0x2126)
        data = 0x03A9 | has_vowel;
    else
        data = 0; // NOLINT(bugprone-branch-clone)

    if (data)
    {
        // Get a code point without accents in upper case
        type_codept upper = data & 0x03FF;

        if ((data & has_vowel) &&
            (upper == 0x0399 || upper == 0x03A5) &&
            *prev_vowel_with_accent)
            data |= has_dialytika;

        *prev_vowel_with_accent =
            (data & (has_vowel | has_accent | has_dialytika)) == (has_vowel | has_accent);

        // If Eta (U+0397) is alone it must be replaced with Eta with tonos (U+0389)
        // It can be checked with Final_Sigma "word boundary" rules
        *maybe_eta_with_tonos =
            upper == 0x0397 && (data & has_accent) && !(data & has_ypogegrammeni);

        // Iota and Upsilon must be replaced with Iota and Upsilon with dialytika
        // if previous vowel has accent
        if (data & has_dialytika)
        {
            if (upper == 0x0399)
                upper = 0x03AA;
            else if (upper == 0x03A5)
                upper = 0x03AB;
        }

        return upper;
    }

    *prev_vowel_with_accent = false;
    *maybe_eta_with_tonos = false;

    return 0;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t case_upper_el_utf8(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    it_in_utf8 src = first;
    it_out_utf8 dst = result;
    type_codept c = 0; // tag_can_be_uninitialized
    bool prev_vowel_with_accent = false; // tag_must_be_initialized
    bool maybe_eta_with_tonos = false; // tag_must_be_initialized

    while (src != last)
    {
        it_in_utf8 prev = src;

        src = iter_utf8(src, last, &c, iter_replacement);

        const type_codept upper = case_greek_upper(c, &prev_vowel_with_accent, &maybe_eta_with_tonos);
        if (upper)
        {
            // Use Final_Sigma function here because Eta follows the same "word boundary" rules
            if (maybe_eta_with_tonos &&
                !case_final_sigma_fwd_utf8(src, last) &&
                !case_final_sigma_rev_utf8(first, prev))
            {
                *dst++ = (type_char8)(type_codept)0xCE;
                *dst++ = (type_char8)(type_codept)0x89;
                continue;
            }

            c = upper;
        }
        else
        {
            if (stages_special_upper_check(c))
            {
                struct case_special_buffer buffer = {{0}};
                const size_t size = stages_special_upper(c, &buffer);

                for (size_t i = 0; i < size; ++i)
                    dst = codepoint_to_utf8(buffer.cps[i], dst);

                continue;
            }

            c = stages_upper(c);
        }

        dst = codepoint_to_utf8(c, dst);
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t case_upper_el_utf16(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;
    type_codept c = 0; // tag_can_be_uninitialized
    bool after_vowel_with_accent = false; // tag_must_be_initialized
    bool maybe_eta_with_tonos = false; // tag_must_be_initialized

    while (src != last)
    {
        it_in_utf16 prev = src;

        src = iter_utf16(src, last, &c, iter_replacement);

        const type_codept upper = case_greek_upper(c, &after_vowel_with_accent, &maybe_eta_with_tonos);
        if (upper)
        {
            // Use Final_Sigma function here because Eta follows the same "word boundary" rules
            if (maybe_eta_with_tonos &&
                !case_final_sigma_fwd_utf16(src, last) &&
                !case_final_sigma_rev_utf16(first, prev))
            {
                *dst++ = (type_char16)0x0389;
                continue;
            }

            c = upper;
        }
        else
        {
            if (stages_special_upper_check(c))
            {
                struct case_special_buffer buffer = {{0}};
                const size_t size = stages_special_upper(c, &buffer);

                for (size_t i = 0; i < size; ++i)
                    dst = codepoint_to_utf16(buffer.cps[i], dst);

                continue;
            }

            c = stages_upper(c);
        }

        dst = codepoint_to_utf16(c, dst);
    }

    return (size_t)(dst - result);
}

#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t case_title_locale_utf8(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result, type_codept language)
{
    // The function is the same as case_title_utf8 except LOCALE BEGIN - LOCALE END parts

    it_in_utf8 src = first;
    it_in_utf8 brk = first;
    it_in_utf8 prev_brk = first;
    it_out_utf8 dst = result;
    type_codept c = 0; // tag_can_be_uninitialized
    bool found_break = false; // tag_must_be_initialized
    bool make_lower = false; // tag_must_be_initialized

    type_codept word_prop = 0; // Not used here
    struct impl_segment_word_state state = {0,0,0,0,0,0,0}; // tag_can_be_uninitialized
    impl_segment_word_state_reset(&state);

    while (src != last)
    {
        it_in_utf8 prev = src;
        src = iter_utf8(src, last, &c, iter_replacement);

        if (!found_break)
        {
            // Find break (if at the end then force break)
            if (segment_word_utf8(&state, c, &word_prop, src, last) || src == last)
            {
                brk = src;
                src = prev_brk;
                prev_brk = prev;
                found_break = true;
                make_lower = false;
            }
            continue;
        }
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

                // LOCALE BEGIN

                if (language == case_map_language_tr || language == case_map_language_az)
                {
                    if (c == 0x0069)
                    {
                        *dst++ = (type_char8)(type_codept)0xC4;
                        *dst++ = (type_char8)(type_codept)0xB0;
                        continue;
                    }
                }
                else if (language == case_map_language_nl)
                {
                    // i or I with j or J
                    if ((c == 0x0069 || c == 0x0049) && src != last &&
                        ((type_codept)*src == 0x006A || (type_codept)*src == 0x004A))
                    {
                        ++src;
                        *dst++ = (type_char8)(type_codept)0x49; // I
                        *dst++ = (type_char8)(type_codept)0x4A; // J
                        continue;
                    }
                }
                // This path is not possible for lt
                /*else if (locale == case_map_language_lt)
                {
                    if (c == 0x0307 && case_after_soft_dotted_utf16(first, prev))
                        continue;
                }*/

                // LOCALE END

                if (stages_special_title_check(c))
                {
                    struct case_special_buffer buffer = {{0}};
                    const size_t size = stages_special_title(c, &buffer);

                    for (size_t i = 0; i < size; ++i)
                        dst = codepoint_to_utf8(buffer.cps[i], dst);

                    continue;
                }

                c = stages_title(c);
            }
        }
        else
        {
            // LOCALE BEGIN

            if (language == case_map_language_lt)
            {
                dst = case_locale_lower_lt_utf8(c, dst, src, last, first, prev);
                continue;
            }
            if (language == case_map_language_tr || language == case_map_language_az)
            {
                dst = case_locale_lower_tr_az_utf8(c, dst, src, last, first, prev);
                continue;
            }

            // LOCALE END

            if (c == 0x0130) // tag_lowercase_special
            {
                *dst++ = (type_char8)(type_codept)0x69;
                *dst++ = (type_char8)(type_codept)0xCC;
                *dst++ = (type_char8)(type_codept)0x87;
                continue;
            }
            if (c == 0x03A3) // Final_Sigma
            {
                if (!case_final_sigma_fwd_utf8(src, last) &&
                    case_final_sigma_rev_utf8(first, prev))
                {
                    *dst++ = (type_char8)(type_codept)0xCF;
                    *dst++ = (type_char8)(type_codept)0x82;
                    continue;
                }
            }

            c = stages_lower(c);
        }

        dst = codepoint_to_utf8(c, dst);
    }

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t case_title_locale_utf16(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result, type_codept language)
{
    // The function is the same as case_title_utf16 except LOCALE BEGIN - LOCALE END parts

    it_in_utf16 src = first;
    it_in_utf16 brk = first;
    it_in_utf16 prev_brk = first;
    it_out_utf16 dst = result;
    type_codept c = 0; // tag_can_be_uninitialized
    bool found_break = false; // tag_must_be_initialized
    bool make_lower = false; // tag_must_be_initialized

    type_codept word_prop = 0; // Not used here
    struct impl_segment_word_state state = {0,0,0,0,0,0,0}; // tag_can_be_uninitialized
    impl_segment_word_state_reset(&state);

    while (src != last)
    {
        it_in_utf16 prev = src;
        src = iter_utf16(src, last, &c, iter_replacement);

        if (!found_break)
        {
            // Find break (if at the end then force break)
            if (segment_word_utf16(&state, c, &word_prop, src, last) || src == last)
            {
                brk = src;
                src = prev_brk;
                prev_brk = prev;
                found_break = true;
                make_lower = false;
            }
            continue;
        }
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

                // LOCALE BEGIN

                if (language == case_map_language_tr || language == case_map_language_az)
                {
                    if (c == 0x0069)
                    {
                        *dst++ = (type_char16)0x0130;
                        continue;
                    }
                }
                else if (language == case_map_language_nl)
                {
                    // i or I with j or J
                    if ((c == 0x0069 || c == 0x0049) && src != last &&
                        ((type_codept)*src == 0x006A || (type_codept)*src == 0x004A))
                    {
                        ++src;
                        *dst++ = (type_char16)0x0049; // I
                        *dst++ = (type_char16)0x004A; // J
                        continue;
                    }
                }
                // This path is not possible for lt
                /*else if (locale == case_map_language_lt)
                {
                    if (c == 0x0307 && case_after_soft_dotted_utf16(first, prev))
                        continue;
                }*/

                // LOCALE END

                if (stages_special_title_check(c))
                {
                    struct case_special_buffer buffer = {{0}};
                    const size_t size = stages_special_title(c, &buffer);

                    for (size_t i = 0; i < size; ++i)
                        dst = codepoint_to_utf16(buffer.cps[i], dst);

                    continue;
                }

                c = stages_title(c);
            }
        }
        else
        {
            // LOCALE BEGIN

            if (language == case_map_language_lt)
            {
                dst = case_locale_lower_lt_utf16(c, dst, src, last, first, prev);
                continue;
            }
            if (language == case_map_language_tr || language == case_map_language_az)
            {
                dst = case_locale_lower_tr_az_utf16(c, dst, src, last, first, prev);
                continue;
            }

            // LOCALE END

            if (c == 0x0130) // tag_lowercase_special
            {
                *dst++ = (type_char16)0x0069;
                *dst++ = (type_char16)0x0307;
                continue;
            }
            if (c == 0x03A3) // Final_Sigma
            {
                if (!case_final_sigma_fwd_utf16(src, last) &&
                    case_final_sigma_rev_utf16(first, prev))
                {
                    *dst++ = (type_char16)0x03C2;
                    continue;
                }
            }

            c = stages_lower(c);
        }

        dst = codepoint_to_utf16(c, dst);
    }

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_SEGMENT_WORD

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_case_map_locale_utf8(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result, int mode, type_codept language)
{
    it_in_utf8 src = first;
    it_out_utf8 dst = result;
    type_codept c = 0; // tag_can_be_uninitialized

    if (language == case_map_language_lt)
    {
        if (mode == impl_case_map_mode_lowercase)
        {
            while (src != last)
            {
                it_in_utf8 prev = src;

                src = iter_utf8(src, last, &c, iter_replacement);

                dst = case_locale_lower_lt_utf8(c, dst, src, last, first, prev);
            }

            return (size_t)(dst - result);
        }
        if (mode == impl_case_map_mode_uppercase)
        {
            while (src != last)
            {
                it_in_utf8 prev = src;

                src = iter_utf8(src, last, &c, iter_replacement);

                dst = case_locale_upper_lt_utf8(c, dst, first, prev);
            }

            return (size_t)(dst - result);
        }
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
        if (mode == impl_case_map_mode_titlecase)
            return case_title_locale_utf8(first, last, result, language);
#endif
    }
    else if (language == case_map_language_tr || language == case_map_language_az)
    {
        if (mode == impl_case_map_mode_lowercase)
        {
            while (src != last)
            {
                it_in_utf8 prev = src;

                src = iter_utf8(src, last, &c, iter_replacement);

                dst = case_locale_lower_tr_az_utf8(c, dst, src, last, first, prev);
            }

            return (size_t)(dst - result);
        }
        if (mode == impl_case_map_mode_uppercase)
        {
            while (src != last)
            {
                src = iter_utf8(src, last, &c, iter_replacement);

                dst = case_locale_upper_tr_az_utf8(c, dst);
            }

            return (size_t)(dst - result);
        }
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
        if (mode == impl_case_map_mode_titlecase)
            return case_title_locale_utf8(first, last, result, language);
#endif
    }
    else if (language == case_map_language_el)
    {
        if (mode == impl_case_map_mode_uppercase)
            return case_upper_el_utf8(first, last, result);
    }
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
    else if (language == case_map_language_nl)
    {
        if (mode == impl_case_map_mode_titlecase)
            return case_title_locale_utf8(first, last, result, language);
    }
#endif

    return impl_case_map_utf8(first, last, result, mode);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_case_map_locale_utf16(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result, int mode, type_codept language)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;
    type_codept c = 0; // tag_can_be_uninitialized

    if (language == case_map_language_lt)
    {
        if (mode == impl_case_map_mode_lowercase)
        {
            while (src != last)
            {
                it_in_utf16 prev = src;

                src = iter_utf16(src, last, &c, iter_replacement);

                dst = case_locale_lower_lt_utf16(c, dst, src, last, first, prev);
            }

            return (size_t)(dst - result);
        }
        if (mode == impl_case_map_mode_uppercase)
        {
            while (src != last)
            {
                it_in_utf16 prev = src;

                src = iter_utf16(src, last, &c, iter_replacement);

                dst = case_locale_upper_lt_utf16(c, dst, first, prev);
            }

            return (size_t)(dst - result);
        }
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
        if (mode == impl_case_map_mode_titlecase)
            return case_title_locale_utf16(first, last, result, language);
#endif
    }
    else if (language == case_map_language_tr || language == case_map_language_az)
    {
        if (mode == impl_case_map_mode_lowercase)
        {
            while (src != last)
            {
                it_in_utf16 prev = src;

                src = iter_utf16(src, last, &c, iter_replacement);

                dst = case_locale_lower_tr_az_utf16(c, dst, src, last, first, prev);
            }

            return (size_t)(dst - result);
        }
        if (mode == impl_case_map_mode_uppercase)
        {
            while (src != last)
            {
                src = iter_utf16(src, last, &c, iter_replacement);

                dst = case_locale_upper_tr_az_utf16(c, dst);
            }

            return (size_t)(dst - result);
        }
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
        if (mode == impl_case_map_mode_titlecase)
            return case_title_locale_utf16(first, last, result, language);
#endif
    }
    else if (language == case_map_language_el)
    {
        if (mode == impl_case_map_mode_uppercase)
            return case_upper_el_utf16(first, last, result);
    }
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
    else if (language == case_map_language_nl)
    {
        if (mode == impl_case_map_mode_titlecase)
            return case_title_locale_utf16(first, last, result, language);
    }
#endif

    return impl_case_map_utf16(first, last, result, mode);
}

UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_IMPL_CASE_LOCALE_H_UAIH

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
