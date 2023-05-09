/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_IMPL_SEGMENT_WORD_H_UAIH
#define UNI_ALGO_IMPL_SEGMENT_WORD_H_UAIH

#include "impl_iter.h"

#include "internal_defines.h"
#include "internal_stages.h"

#ifndef UNI_ALGO_STATIC_DATA
#include "data/extern_segment_word.h"
#else
#include "data/data_segment_word.h"
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// See generator_segment_word in gen/gen.h

// NOTE: The order of the word properties is important here:
// 1. New lines, punctuation, space etc. must be in this order:
// CR/LF/Newline -> Extend -> ZWJ -> Format -> Punctuation -> WSegSpace
uaix_const type_codept prop_WB_CR                    = 1;
uaix_const type_codept prop_WB_LF                    = 2;
uaix_const type_codept prop_WB_Newline               = 3;
uaix_const type_codept prop_WB_Extend                = 4;
uaix_const type_codept prop_WB_ZWJ                   = 5;
uaix_const type_codept prop_WB_Format                = 6;
uaix_const type_codept prop_WB_Single_Quote          = 7;
uaix_const type_codept prop_WB_Double_Quote          = 8;
uaix_const type_codept prop_WB_MidNumLet             = 9;
uaix_const type_codept prop_WB_MidLetter             = 10;
uaix_const type_codept prop_WB_MidNum                = 11;
uaix_const type_codept prop_WB_ExtendNumLet          = 12;
uaix_const type_codept prop_WB_WSegSpace             = 13;
// 2. Everything else that can be considered a word must be in this order:
// Numeric -> Alphabetic -> Kana -> Ideographic -> Emoji
uaix_const type_codept prop_WB_Numeric               = 14;
uaix_const type_codept prop_WB_ALetter               = 15;
uaix_const type_codept prop_WB_Hebrew_Letter         = 16;
uaix_const type_codept prop_WX_Remaining_Alphabetic  = 17;
uaix_const type_codept prop_WB_Katakana              = 18;
uaix_const type_codept prop_WX_Remaining_Hiragana    = 19;
uaix_const type_codept prop_WX_Remaining_Ideographic = 20;
// 3. Regional_Indicator must be the last because it can be considered emoji
uaix_const type_codept prop_WB_Regional_Indicator    = 21;

uaix_const int state_segment_word_begin    = 0;
uaix_const int state_segment_word_continue = 1;
uaix_const int state_segment_word_RI       = 2;
uaix_const int state_segment_word_RI_RI    = 3;

uaix_always_inline
uaix_static type_codept stages_segment_word_prop(type_codept c)
{
    // This function returns raw property that contains property with Extended_Pictographic
    // Two functions below must be used to get the real property and Extended_Pictographic
    return stages(c, stage1_segment_word, stage2_segment_word);
}

uaix_always_inline
uaix_static type_codept segment_word_prop(type_codept prop)
{
    return (prop & 0x7F); // 7 right bits are used for properties
}

uaix_always_inline
uaix_static bool segment_word_prop_ext_pic(type_codept prop)
{
    return (prop & 0x80) ? true : false; // First (left) bit is used for Extended_Pictographic
}

struct impl_segment_word_state
{
    type_codept prev_cp;
    type_codept prev_cp_prop;

    type_codept prev_cp1;
    type_codept prev_cp1_prop;

    type_codept prev_cp2;
    type_codept prev_cp2_prop;

    int state;
};

uaix_always_inline
uaix_static void impl_segment_word_state_reset(struct impl_segment_word_state* const state)
{
    state->prev_cp = 0;
    state->prev_cp_prop = 0;

    state->prev_cp1 = 0;
    state->prev_cp1_prop = 0;

    state->prev_cp2 = 0;
    state->prev_cp2_prop = 0;

    state->state = state_segment_word_begin;
}

uaix_always_inline
uaix_static bool impl_segment_is_word(type_codept word_prop)
{
    // ICU analog: !UBRK_WORD_NONE
    // Excludes spaces and most punctuation and emojis
    return (word_prop >= prop_WB_Numeric && word_prop <= prop_WX_Remaining_Ideographic);
    // REMINDER: Treat all emojis as words too
    //return (word_prop >= prop_WB_Numeric);
    // REMINDER: Treat "letter emojis" (ALetter | Extended_Pictographic) as words too
    //return (word_prop >= prop_WB_Numeric && word_prop <= prop_WX_Remaining_Ideographic) || word_prop == (prop_WB_ALetter | 0x80);
}

uaix_always_inline
uaix_static bool impl_segment_is_word_number(type_codept word_prop)
{
    // ICU analog: UBRK_WORD_NUMBER
    // Includes only numbers
    return (word_prop == prop_WB_Numeric);
}

uaix_always_inline
uaix_static bool impl_segment_is_word_letter(type_codept word_prop)
{
    // ICU analog: UBRK_WORD_LETTER
    // Includes words that contain letters
    // Excludes numbers and kana and ideographic
    return (word_prop >= prop_WB_ALetter && word_prop <= prop_WX_Remaining_Alphabetic);
}

uaix_always_inline
uaix_static bool impl_segment_is_word_kana(type_codept word_prop)
{
    // ICU analog: UBRK_WORD_KANA
    // Includes only kana
    return (word_prop == prop_WB_Katakana || word_prop == prop_WX_Remaining_Hiragana);
}

uaix_always_inline
uaix_static bool impl_segment_is_word_ideo(type_codept word_prop)
{
    // ICU analog: UBRK_WORD_IDEO
    // Includes only ideographic
    return (word_prop == prop_WX_Remaining_Ideographic);
}

uaix_always_inline
uaix_static bool impl_segment_is_word_emoji(type_codept word_prop)
{
    // Includes only emojis
    return (word_prop >= prop_WB_Regional_Indicator); // Includes Extended_Pictographic
}

uaix_always_inline
uaix_static bool impl_segment_is_word_punct(type_codept word_prop)
{
    // Includes only punctuation
    return (word_prop > prop_WB_Format && word_prop < prop_WB_WSegSpace);
}

uaix_always_inline
uaix_static bool impl_segment_is_word_space(type_codept word_prop)
{
    // Includes only spaces
    return (word_prop == prop_WB_WSegSpace);
}

uaix_always_inline
uaix_static bool impl_segment_is_word_newline(type_codept word_prop)
{
    // Includes only spaces
    return (word_prop == prop_WB_CR || word_prop == prop_WB_LF || word_prop == prop_WB_Newline);
}

uaix_always_inline
uaix_static bool segment_word_skip(type_codept prop)
{
    return (prop == prop_WB_ZWJ || prop == prop_WB_Extend || prop == prop_WB_Format);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static type_codept segment_word_skip_utf8(it_in_utf8 first, it_end_utf8 last)
{
    /* C++ Note: this function makes the word segmentation algorithm incompatible with input iterators.
     * This means Word Boundary Rules are not compatible with streams at all and require at least forward iterator.
     * It can even look like it works but it never pass tests because we must go back after this function.
     * The test line where it fails: "\x0061\x003A\x0001" - AHLetter + MidLetter + Other.
     * Must be 3 words but if we don't go back it will be 2. */

    it_in_utf8 src = first;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != last)
    {
        src = iter_utf8(src, last, &c, iter_replacement);

        const type_codept prop = segment_word_prop(stages_segment_word_prop(c));

        if (segment_word_skip(prop))
            continue;

        return prop;
    }
    return 0;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static bool segment_word_utf8(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                   it_in_utf8 first, it_end_utf8 last)
{
    // word_prop property must be used only with impl_segment_is_word* functions

    // TODO: https://unicode.org/reports/tr29/#State_Machines
    // ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/WordBreakTest.html
    // Note that only WB5, WB7a, WB8, WB9, WB10, WB13 rules can be handled
    // with the state table so it's not even worth it probably.
    // Compared the performance with ICU it's already much faster so it can wait.

    const type_codept raw_prop = stages_segment_word_prop(c);

    const type_codept c_prop = segment_word_prop(raw_prop);
    const type_codept p_prop = segment_word_prop(state->prev_cp_prop);

    // Previous values of code points with WB4 rules
    const type_codept p1_prop = segment_word_prop(state->prev_cp1_prop);
    const type_codept p2_prop = segment_word_prop(state->prev_cp2_prop);

    type_codept s_prop = 0; // tag_can_be_uninitialized

    bool result = false; // tag_can_be_uninitialized

    // https://www.unicode.org/reports/tr29/#Word_Boundary_Rules
    // Unicode 11.0 - 15.0 rules

    if (state->state == state_segment_word_begin)
    {
        state->state = state_segment_word_continue;
        *word_prop = 0;
    }
    else if (p_prop == prop_WB_CR && c_prop == prop_WB_LF) // WB3
        result = false; // NOLINT
    else if (p_prop == prop_WB_Newline || p_prop == prop_WB_CR || p_prop == prop_WB_LF) // WB3a
        result = true; // NOLINT
    else if (c_prop == prop_WB_Newline || c_prop == prop_WB_CR || c_prop == prop_WB_LF) // WB3b
        result = true; // NOLINT
    else if (p_prop == prop_WB_ZWJ && segment_word_prop_ext_pic(raw_prop)) // WB3c
        result = false; // NOLINT
    else if (p_prop == prop_WB_WSegSpace && c_prop == prop_WB_WSegSpace) // WB3d
        result = false; // NOLINT
    else if (segment_word_skip(c_prop)) // WB4
        result = false; // NOLINT

    // p and p_prop must not be used anymore because WB4 takes effect below this line

    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB5
        result = false; // NOLINT
    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (c_prop == prop_WB_MidLetter || c_prop == prop_WB_MidNumLet || c_prop == prop_WB_Single_Quote) &&
             ((s_prop = segment_word_skip_utf8(first, last)) != 0 && // NOLINT(bugprone-assignment-in-if-condition)
              (s_prop == prop_WB_ALetter || s_prop == prop_WB_Hebrew_Letter))) // WB6
        result = false; // NOLINT
    else if ((p2_prop == prop_WB_ALetter || p2_prop == prop_WB_Hebrew_Letter) &&
             (p1_prop == prop_WB_MidLetter || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB7
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && c_prop == prop_WB_Single_Quote) // WB7a
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && c_prop == prop_WB_Double_Quote &&
             segment_word_skip_utf8(first, last) == prop_WB_Hebrew_Letter) // WB7b
        result = false; // NOLINT
    else if (p2_prop == prop_WB_Hebrew_Letter && p1_prop == prop_WB_Double_Quote && c_prop == prop_WB_Hebrew_Letter) // WB7c
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Numeric && c_prop == prop_WB_Numeric) // WB8
        result = false; // NOLINT
    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) && c_prop == prop_WB_Numeric) // WB9
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Numeric && (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB10
        result = false; // NOLINT
    else if (p2_prop == prop_WB_Numeric &&
             (p1_prop == prop_WB_MidNum || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             c_prop == prop_WB_Numeric) // WB11
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Numeric &&
             (c_prop == prop_WB_MidNum || c_prop == prop_WB_MidNumLet || c_prop == prop_WB_Single_Quote) &&
             segment_word_skip_utf8(first, last) == prop_WB_Numeric) // WB12
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Katakana && c_prop == prop_WB_Katakana) // WB13
        result = false; // NOLINT
    else if (((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) ||
              p1_prop == prop_WB_Numeric || p1_prop == prop_WB_Katakana || p1_prop == prop_WB_ExtendNumLet) &&
             c_prop == prop_WB_ExtendNumLet) // WB13a
        result = false; // NOLINT
    else if (p1_prop == prop_WB_ExtendNumLet &&
             ((c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter) ||
              c_prop == prop_WB_Numeric || c_prop == prop_WB_Katakana)) // WB13b
        result = false; // NOLINT
    else if (state->state == state_segment_word_RI && c_prop == prop_WB_Regional_Indicator) // WB15/WB16
        result = false; // NOLINT
    else // WB999
    {
        result = true;
        *word_prop = 0;
    }

    state->prev_cp = c;
    state->prev_cp_prop = raw_prop;

    if (segment_word_skip(c_prop))
        return result;

    // WB15/WB16
    if (c_prop == prop_WB_Regional_Indicator)
    {
        if (state->state == state_segment_word_RI)
            state->state = state_segment_word_RI_RI;
        else
            state->state = state_segment_word_RI;
    }
    else
        state->state = state_segment_word_continue;

    // Set previous values of codepoints with WB4 rules
    state->prev_cp2 = state->prev_cp1;
    state->prev_cp2_prop = state->prev_cp1_prop;
    state->prev_cp1 = c;
    state->prev_cp1_prop = raw_prop;

    if (raw_prop > *word_prop)
        *word_prop = raw_prop;

    return result;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static bool impl_segment_word_utf8(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                        it_in_utf8 first, it_end_utf8 last)
{
    return segment_word_utf8(state, c, word_prop, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static bool inline_segment_word_utf8(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                          it_in_utf8 first, it_end_utf8 last)
{
    return segment_word_utf8(state, c, word_prop, first, last);
}

// -------------
// REVERSE RULES
// -------------

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static type_codept segment_word_skip_rev_utf8(it_in_utf8 first, it_in_utf8 last)
{
    it_in_utf8 src = last;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != first)
    {
        src = iter_rev_utf8(first, src, &c, iter_replacement);

        const type_codept prop = segment_word_prop(stages_segment_word_prop(c));

        if (segment_word_skip(prop))
            continue;

        return prop;
    }
    return 0;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf8 segment_word_skip_rev2_utf8(it_in_utf8 first, it_in_utf8 last, type_codept* const new_prop)
{
    it_in_utf8 src = last;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != first)
    {
        src = iter_rev_utf8(first, src, &c, iter_replacement);

        const type_codept prop = segment_word_prop(stages_segment_word_prop(c));

        if (segment_word_skip(prop))
            continue;

        *new_prop = prop;
        return src;
    }
    return src;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static bool segment_word_rev_RI_utf8(it_in_utf8 first, it_in_utf8 last)
{
    it_in_utf8 src = last;
    type_codept c = 0; // tag_can_be_uninitialized
    size_t count_RI = 0; // tag_must_be_initialized

    while (src != first)
    {
        src = iter_rev_utf8(first, src, &c, iter_replacement);

        const type_codept prop = segment_word_prop(stages_segment_word_prop(c));

        if (segment_word_skip(prop))
            continue;

        if (prop == prop_WB_Regional_Indicator)
            ++count_RI;
        else
            break;
    }
    return (count_RI % 2) != 0;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_always_inline_tmpl
uaix_static bool segment_word_rev_utf8(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                       it_in_utf8 first, it_in_utf8 last)
{
    // word_prop property must be used only with impl_segment_is_word* functions

    const type_codept raw_prop = stages_segment_word_prop(c);

    const type_codept c_prop = segment_word_prop(raw_prop);
    const type_codept p_prop = segment_word_prop(state->prev_cp_prop);

    // Previous values of code points with WB4 rules
    const type_codept p1_prop = segment_word_prop(state->prev_cp1_prop);
    const type_codept p2_prop = segment_word_prop(state->prev_cp2_prop);

    type_codept s_prop = 0; // tag_can_be_uninitialized

    bool result = false; // tag_can_be_uninitialized

    // https://www.unicode.org/reports/tr29/#Word_Boundary_Rules
    // Unicode 11.0 - 15.0 rules

    if (state->state == state_segment_word_begin)
    {
        state->state = state_segment_word_continue;
        *word_prop = 0;
    }
    else if (c_prop == prop_WB_CR && p_prop == prop_WB_LF) // WB3
        result = false; // NOLINT
    else if (c_prop == prop_WB_Newline || c_prop == prop_WB_CR || c_prop == prop_WB_LF) // WB3a
        result = true; // NOLINT
    else if (p_prop == prop_WB_Newline || p_prop == prop_WB_CR || p_prop == prop_WB_LF) // WB3b
        result = true; // NOLINT
    else if (c_prop == prop_WB_ZWJ && segment_word_prop_ext_pic(state->prev_cp_prop)) // WB3c
        result = false; // NOLINT
    else if (c_prop == prop_WB_WSegSpace && p_prop == prop_WB_WSegSpace) // WB3d
        result = false; // NOLINT
    else if (segment_word_skip(p_prop)) // WB4 First Part
        result = false; // NOLINT
    else
    {
        // WB4 Second Part
        // This is the complex one, c_prop is no good anymore because we must skip backward all Extend etc.
        // and find the real code point and get a new prop. All rules after WB4 must use it instead of c_prop
        // p_prop also cannot be used anymore by the same reasons as in forward rules.
        it_in_utf8 src = last;
        type_codept n_prop = c_prop;
        if (segment_word_skip(c_prop))
            src = segment_word_skip_rev2_utf8(first, last, &n_prop);

    if ((n_prop == prop_WB_ALetter || n_prop == prop_WB_Hebrew_Letter) &&
        (p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter)) // WB5
        result = false; // NOLINT
    else if ((n_prop == prop_WB_ALetter || n_prop == prop_WB_Hebrew_Letter) &&
             (p1_prop == prop_WB_MidLetter || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             (p2_prop == prop_WB_ALetter || p2_prop == prop_WB_Hebrew_Letter)) // WB6
        result = false; // NOLINT
    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (n_prop == prop_WB_MidLetter || n_prop == prop_WB_MidNumLet || n_prop == prop_WB_Single_Quote) &&
             ((s_prop = segment_word_skip_rev_utf8(first, src)) != 0 && // NOLINT(bugprone-assignment-in-if-condition)
              (s_prop == prop_WB_ALetter || s_prop == prop_WB_Hebrew_Letter))) // WB7
        result = false; // NOLINT
    else if (n_prop == prop_WB_Hebrew_Letter && p1_prop == prop_WB_Single_Quote) // WB7a
        result = false; // NOLINT
    else if (n_prop == prop_WB_Hebrew_Letter && p1_prop == prop_WB_Double_Quote && p2_prop == prop_WB_Hebrew_Letter) // WB7b
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && n_prop == prop_WB_Double_Quote &&
             segment_word_skip_rev_utf8(first, src) == prop_WB_Hebrew_Letter) // WB7c
        result = false; // NOLINT
    else if (n_prop == prop_WB_Numeric && p1_prop == prop_WB_Numeric) // WB8
        result = false; // NOLINT
    else if ((n_prop == prop_WB_ALetter || n_prop == prop_WB_Hebrew_Letter) && p1_prop == prop_WB_Numeric) // WB9
        result = false; // NOLINT
    else if (n_prop == prop_WB_Numeric && (p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter)) // WB10
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Numeric &&
             (n_prop == prop_WB_MidNum || n_prop == prop_WB_MidNumLet || n_prop == prop_WB_Single_Quote) &&
             segment_word_skip_rev_utf8(first, src) == prop_WB_Numeric) // WB11
        result = false; // NOLINT
    else if (n_prop == prop_WB_Numeric &&
             (p1_prop == prop_WB_MidNum || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             p2_prop == prop_WB_Numeric) // WB12
        result = false; // NOLINT
    else if (n_prop == prop_WB_Katakana && p1_prop == prop_WB_Katakana) // WB13
        result = false; // NOLINT
    else if (((n_prop == prop_WB_ALetter || n_prop == prop_WB_Hebrew_Letter) ||
              n_prop == prop_WB_Numeric || n_prop == prop_WB_Katakana || n_prop == prop_WB_ExtendNumLet) &&
             p1_prop == prop_WB_ExtendNumLet) // WB13a
        result = false; // NOLINT
    else if (n_prop == prop_WB_ExtendNumLet &&
             ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) ||
              p1_prop == prop_WB_Numeric || p1_prop == prop_WB_Katakana)) // WB13b
        result = false; // NOLINT
    else if (n_prop == prop_WB_Regional_Indicator && p1_prop == prop_WB_Regional_Indicator) // WB15/WB16
        result = segment_word_rev_RI_utf8(first, src);
    else // WB999
        {
            result = true;
            *word_prop = 0;
        }
    }

    state->prev_cp = c;
    state->prev_cp_prop = raw_prop;

    if (segment_word_skip(c_prop))
        return result;

    // Set previous values of codepoints with WB4 rules
    state->prev_cp2 = state->prev_cp1;
    state->prev_cp2_prop = state->prev_cp1_prop;
    state->prev_cp1 = c;
    state->prev_cp1_prop = raw_prop;

    if (raw_prop > *word_prop)
        *word_prop = raw_prop;

    return result;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static bool impl_segment_word_rev_utf8(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                            it_in_utf8 first, it_in_utf8 last)
{
    return segment_word_rev_utf8(state, c, word_prop, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_always_inline_tmpl
uaix_static bool inline_segment_word_rev_utf8(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                              it_in_utf8 first, it_in_utf8 last)
{
    return segment_word_rev_utf8(state, c, word_prop, first, last);
}

// BEGIN: GENERATED UTF-16 FUNCTIONS
#ifndef UNI_ALGO_DOC_GENERATED_UTF16

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static type_codept segment_word_skip_utf16(it_in_utf16 first, it_end_utf16 last)
{
    it_in_utf16 src = first;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != last)
    {
        src = iter_utf16(src, last, &c, iter_replacement);

        const type_codept prop = segment_word_prop(stages_segment_word_prop(c));

        if (segment_word_skip(prop))
            continue;

        return prop;
    }
    return 0;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool segment_word_utf16(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                    it_in_utf16 first, it_end_utf16 last)
{
    // word_prop property must be used only with impl_segment_is_word* functions

    const type_codept raw_prop = stages_segment_word_prop(c);

    const type_codept c_prop = segment_word_prop(raw_prop);
    const type_codept p_prop = segment_word_prop(state->prev_cp_prop);

    // Previous values of code points with WB4 rules
    const type_codept p1_prop = segment_word_prop(state->prev_cp1_prop);
    const type_codept p2_prop = segment_word_prop(state->prev_cp2_prop);

    type_codept s_prop = 0; // tag_can_be_uninitialized

    bool result = false; // tag_can_be_uninitialized

    // https://www.unicode.org/reports/tr29/#Word_Boundary_Rules
    // Unicode 11.0 - 15.0 rules

    if (state->state == state_segment_word_begin)
    {
        state->state = state_segment_word_continue;
        *word_prop = 0;
    }
    else if (p_prop == prop_WB_CR && c_prop == prop_WB_LF) // WB3
        result = false; // NOLINT
    else if (p_prop == prop_WB_Newline || p_prop == prop_WB_CR || p_prop == prop_WB_LF) // WB3a
        result = true; // NOLINT
    else if (c_prop == prop_WB_Newline || c_prop == prop_WB_CR || c_prop == prop_WB_LF) // WB3b
        result = true; // NOLINT
    else if (p_prop == prop_WB_ZWJ && segment_word_prop_ext_pic(raw_prop)) // WB3c
        result = false; // NOLINT
    else if (p_prop == prop_WB_WSegSpace && c_prop == prop_WB_WSegSpace) // WB3d
        result = false; // NOLINT
    else if (segment_word_skip(c_prop)) // WB4
        result = false; // NOLINT

    // p and p_prop must not be used anymore because WB4 takes effect below this line

    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB5
        result = false; // NOLINT
    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (c_prop == prop_WB_MidLetter || c_prop == prop_WB_MidNumLet || c_prop == prop_WB_Single_Quote) &&
             ((s_prop = segment_word_skip_utf16(first, last)) != 0 && // NOLINT(bugprone-assignment-in-if-condition)
              (s_prop == prop_WB_ALetter || s_prop == prop_WB_Hebrew_Letter))) // WB6
        result = false; // NOLINT
    else if ((p2_prop == prop_WB_ALetter || p2_prop == prop_WB_Hebrew_Letter) &&
             (p1_prop == prop_WB_MidLetter || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB7
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && c_prop == prop_WB_Single_Quote) // WB7a
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && c_prop == prop_WB_Double_Quote &&
             segment_word_skip_utf16(first, last) == prop_WB_Hebrew_Letter) // WB7b
        result = false; // NOLINT
    else if (p2_prop == prop_WB_Hebrew_Letter && p1_prop == prop_WB_Double_Quote && c_prop == prop_WB_Hebrew_Letter) // WB7c
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Numeric && c_prop == prop_WB_Numeric) // WB8
        result = false; // NOLINT
    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) && c_prop == prop_WB_Numeric) // WB9
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Numeric && (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB10
        result = false; // NOLINT
    else if (p2_prop == prop_WB_Numeric &&
             (p1_prop == prop_WB_MidNum || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             c_prop == prop_WB_Numeric) // WB11
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Numeric &&
             (c_prop == prop_WB_MidNum || c_prop == prop_WB_MidNumLet || c_prop == prop_WB_Single_Quote) &&
             segment_word_skip_utf16(first, last) == prop_WB_Numeric) // WB12
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Katakana && c_prop == prop_WB_Katakana) // WB13
        result = false; // NOLINT
    else if (((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) ||
              p1_prop == prop_WB_Numeric || p1_prop == prop_WB_Katakana || p1_prop == prop_WB_ExtendNumLet) &&
             c_prop == prop_WB_ExtendNumLet) // WB13a
        result = false; // NOLINT
    else if (p1_prop == prop_WB_ExtendNumLet &&
             ((c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter) ||
              c_prop == prop_WB_Numeric || c_prop == prop_WB_Katakana)) // WB13b
        result = false; // NOLINT
    else if (state->state == state_segment_word_RI && c_prop == prop_WB_Regional_Indicator) // WB15/WB16
        result = false; // NOLINT
    else // WB999
    {
        result = true;
        *word_prop = 0;
    }

    state->prev_cp = c;
    state->prev_cp_prop = raw_prop;

    if (segment_word_skip(c_prop))
        return result;

    // WB15/WB16
    if (c_prop == prop_WB_Regional_Indicator)
    {
        if (state->state == state_segment_word_RI)
            state->state = state_segment_word_RI_RI;
        else
            state->state = state_segment_word_RI;
    }
    else
        state->state = state_segment_word_continue;

    // Set previous values of codepoints with WB4 rules
    state->prev_cp2 = state->prev_cp1;
    state->prev_cp2_prop = state->prev_cp1_prop;
    state->prev_cp1 = c;
    state->prev_cp1_prop = raw_prop;

    if (raw_prop > *word_prop)
        *word_prop = raw_prop;

    return result;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static bool impl_segment_word_utf16(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                         it_in_utf16 first, it_end_utf16 last)
{
    return segment_word_utf16(state, c, word_prop, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool inline_segment_word_utf16(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                           it_in_utf16 first, it_end_utf16 last)
{
    return segment_word_utf16(state, c, word_prop, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static type_codept segment_word_skip_rev_utf16(it_in_utf16 first, it_in_utf16 last)
{
    it_in_utf16 src = last;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != first)
    {
        src = iter_rev_utf16(first, src, &c, iter_replacement);

        const type_codept prop = segment_word_prop(stages_segment_word_prop(c));

        if (segment_word_skip(prop))
            continue;

        return prop;
    }
    return 0;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_always_inline_tmpl
uaix_static it_in_utf16 segment_word_skip_rev2_utf16(it_in_utf16 first, it_in_utf16 last, type_codept* const new_prop)
{
    it_in_utf16 src = last;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != first)
    {
        src = iter_rev_utf16(first, src, &c, iter_replacement);

        const type_codept prop = segment_word_prop(stages_segment_word_prop(c));

        if (segment_word_skip(prop))
            continue;

        *new_prop = prop;
        return src;
    }
    return src;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static bool segment_word_rev_RI_utf16(it_in_utf16 first, it_in_utf16 last)
{
    it_in_utf16 src = last;
    type_codept c = 0; // tag_can_be_uninitialized
    size_t count_RI = 0; // tag_must_be_initialized

    while (src != first)
    {
        src = iter_rev_utf16(first, src, &c, iter_replacement);

        const type_codept prop = segment_word_prop(stages_segment_word_prop(c));

        if (segment_word_skip(prop))
            continue;

        if (prop == prop_WB_Regional_Indicator)
            ++count_RI;
        else
            break;
    }
    return (count_RI % 2) != 0;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool segment_word_rev_utf16(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                        it_in_utf16 first, it_in_utf16 last)
{
    // word_prop property must be used only with impl_segment_is_word* functions

    const type_codept raw_prop = stages_segment_word_prop(c);

    const type_codept c_prop = segment_word_prop(raw_prop);
    const type_codept p_prop = segment_word_prop(state->prev_cp_prop);

    // Previous values of code points with WB4 rules
    const type_codept p1_prop = segment_word_prop(state->prev_cp1_prop);
    const type_codept p2_prop = segment_word_prop(state->prev_cp2_prop);

    type_codept s_prop = 0; // tag_can_be_uninitialized

    bool result = false; // tag_can_be_uninitialized

    // https://www.unicode.org/reports/tr29/#Word_Boundary_Rules
    // Unicode 11.0 - 15.0 rules

    if (state->state == state_segment_word_begin)
    {
        state->state = state_segment_word_continue;
        *word_prop = 0;
    }
    else if (c_prop == prop_WB_CR && p_prop == prop_WB_LF) // WB3
        result = false; // NOLINT
    else if (c_prop == prop_WB_Newline || c_prop == prop_WB_CR || c_prop == prop_WB_LF) // WB3a
        result = true; // NOLINT
    else if (p_prop == prop_WB_Newline || p_prop == prop_WB_CR || p_prop == prop_WB_LF) // WB3b
        result = true; // NOLINT
    else if (c_prop == prop_WB_ZWJ && segment_word_prop_ext_pic(state->prev_cp_prop)) // WB3c
        result = false; // NOLINT
    else if (c_prop == prop_WB_WSegSpace && p_prop == prop_WB_WSegSpace) // WB3d
        result = false; // NOLINT
    else if (segment_word_skip(p_prop)) // WB4
        result = false; // NOLINT
    else
    {
        it_in_utf16 src = last;
        type_codept n_prop = c_prop;
        if (segment_word_skip(c_prop))
            src = segment_word_skip_rev2_utf16(first, last, &n_prop);

    if ((n_prop == prop_WB_ALetter || n_prop == prop_WB_Hebrew_Letter) &&
        (p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter)) // WB5
        result = false; // NOLINT
    else if ((n_prop == prop_WB_ALetter || n_prop == prop_WB_Hebrew_Letter) &&
             (p1_prop == prop_WB_MidLetter || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             (p2_prop == prop_WB_ALetter || p2_prop == prop_WB_Hebrew_Letter)) // WB6
        result = false; // NOLINT
    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (n_prop == prop_WB_MidLetter || n_prop == prop_WB_MidNumLet || n_prop == prop_WB_Single_Quote) &&
             ((s_prop = segment_word_skip_rev_utf16(first, src)) != 0 && // NOLINT(bugprone-assignment-in-if-condition)
              (s_prop == prop_WB_ALetter || s_prop == prop_WB_Hebrew_Letter))) // WB7
        result = false; // NOLINT
    else if (n_prop == prop_WB_Hebrew_Letter && p1_prop == prop_WB_Single_Quote) // WB7a
        result = false; // NOLINT
    else if (n_prop == prop_WB_Hebrew_Letter && p1_prop == prop_WB_Double_Quote && p2_prop == prop_WB_Hebrew_Letter) // WB7b
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && n_prop == prop_WB_Double_Quote &&
             segment_word_skip_rev_utf16(first, src) == prop_WB_Hebrew_Letter) // WB7c
        result = false; // NOLINT
    else if (n_prop == prop_WB_Numeric && p1_prop == prop_WB_Numeric) // WB8
        result = false; // NOLINT
    else if ((n_prop == prop_WB_ALetter || n_prop == prop_WB_Hebrew_Letter) && p1_prop == prop_WB_Numeric) // WB9
        result = false; // NOLINT
    else if (n_prop == prop_WB_Numeric && (p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter)) // WB10
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Numeric &&
             (n_prop == prop_WB_MidNum || n_prop == prop_WB_MidNumLet || n_prop == prop_WB_Single_Quote) &&
             segment_word_skip_rev_utf16(first, src) == prop_WB_Numeric) // WB11
        result = false; // NOLINT
    else if (n_prop == prop_WB_Numeric &&
             (p1_prop == prop_WB_MidNum || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             p2_prop == prop_WB_Numeric) // WB12
        result = false; // NOLINT
    else if (n_prop == prop_WB_Katakana && p1_prop == prop_WB_Katakana) // WB13
        result = false; // NOLINT
    else if (((n_prop == prop_WB_ALetter || n_prop == prop_WB_Hebrew_Letter) ||
              n_prop == prop_WB_Numeric || n_prop == prop_WB_Katakana || n_prop == prop_WB_ExtendNumLet) &&
             p1_prop == prop_WB_ExtendNumLet) // WB13a
        result = false; // NOLINT
    else if (n_prop == prop_WB_ExtendNumLet &&
             ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) ||
              p1_prop == prop_WB_Numeric || p1_prop == prop_WB_Katakana)) // WB13b
        result = false; // NOLINT
    else if (n_prop == prop_WB_Regional_Indicator && p1_prop == prop_WB_Regional_Indicator) // WB15/WB16
        result = segment_word_rev_RI_utf16(first, src);
    else // WB999
        {
            result = true;
            *word_prop = 0;
        }
    }

    state->prev_cp = c;
    state->prev_cp_prop = raw_prop;

    if (segment_word_skip(c_prop))
        return result;

    // Set previous values of codepoints with WB4 rules
    state->prev_cp2 = state->prev_cp1;
    state->prev_cp2_prop = state->prev_cp1_prop;
    state->prev_cp1 = c;
    state->prev_cp1_prop = raw_prop;

    if (raw_prop > *word_prop)
        *word_prop = raw_prop;

    return result;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static bool impl_segment_word_rev_utf16(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                             it_in_utf16 first, it_in_utf16 last)
{
    return segment_word_rev_utf16(state, c, word_prop, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool inline_segment_word_rev_utf16(struct impl_segment_word_state* const state, type_codept c, type_codept* const word_prop,
                                               it_in_utf16 first, it_in_utf16 last)
{
    return segment_word_rev_utf16(state, c, word_prop, first, last);
}

#endif // UNI_ALGO_DOC_GENERATED_UTF16
// END: GENERATED UTF-16 FUNCTIONS

UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_IMPL_SEGMENT_WORD_H_UAIH

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
