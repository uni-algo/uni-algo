/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef IMPL_BREAK_WORD_H_UAIX
#define IMPL_BREAK_WORD_H_UAIX

#include <uni/impl/iterator.h>

#include <uni/internal/defines.h>
#include <uni/internal/stages.h>

#ifndef UNI_ALGO_STATIC_DATA
#include <uni/impl/break_word_data_extern.h>
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// See generator_break_word in gen/gen.h

uaix_const type_codept prop_WB_Double_Quote          = 1;
uaix_const type_codept prop_WB_Single_Quote          = 2;
uaix_const type_codept prop_WB_Hebrew_Letter         = 3;
uaix_const type_codept prop_WB_CR                    = 4;
uaix_const type_codept prop_WB_LF                    = 5;
uaix_const type_codept prop_WB_Newline               = 6;
uaix_const type_codept prop_WB_Extend                = 7;
uaix_const type_codept prop_WB_Regional_Indicator    = 8;
uaix_const type_codept prop_WB_Format                = 9;
uaix_const type_codept prop_WB_Katakana              = 10;
uaix_const type_codept prop_WB_ALetter               = 11;
uaix_const type_codept prop_WB_MidLetter             = 12;
uaix_const type_codept prop_WB_MidNum                = 13;
uaix_const type_codept prop_WB_MidNumLet             = 14;
uaix_const type_codept prop_WB_Numeric               = 15;
uaix_const type_codept prop_WB_ExtendNumLet          = 16;
uaix_const type_codept prop_WB_ZWJ                   = 17;
uaix_const type_codept prop_WB_WSegSpace             = 18;
uaix_const type_codept prop_WB_Extended_Pictographic = 19;
uaix_const type_codept prop_WU_Remaining_Alphabetic  = 20;

uaix_const int state_break_word_begin    = 0;
uaix_const int state_break_word_process  = 1;
uaix_const int state_break_word_EP       = 2;
uaix_const int state_break_word_EP_ZWJ   = 3;
uaix_const int state_break_word_RI       = 4;
uaix_const int state_break_word_RI_RI    = 5;

uaix_always_inline
uaix_static type_codept stages_break_word_prop(type_codept c)
{
    return stages(c, stage1_break_word, stage2_break_word);
}

struct impl_break_word_state
{
    type_codept prev_cp;
    type_codept prev_cp_prop;
    int state;

    type_codept prev_cp1;
    type_codept prev_cp1_prop;

    type_codept prev_cp2;
    type_codept prev_cp2_prop;
};

uaix_always_inline
uaix_static void impl_break_word_state_reset(struct impl_break_word_state* state)
{
    state->prev_cp = 0;
    state->prev_cp_prop = 0;
    state->state = state_break_word_begin;

    state->prev_cp1 = 0;
    state->prev_cp1_prop = 0;

    state->prev_cp2 = 0;
    state->prev_cp2_prop = 0;
}

uaix_always_inline
uaix_static bool impl_break_is_word(type_codept word_prop)
{
    if (word_prop == prop_WB_ALetter ||
        word_prop == prop_WB_Hebrew_Letter ||
        word_prop == prop_WB_Numeric ||
        word_prop == prop_WB_Katakana ||
        word_prop == prop_WU_Remaining_Alphabetic)
        return true;

    return false;
}

uaix_always_inline
uaix_static bool break_word_skip(type_codept prop)
{
    return (prop == prop_WB_ZWJ || prop == prop_WB_Extend || prop == prop_WB_Format);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static type_codept utf8_break_word_skip(it_in_utf8 first, it_end_utf8 last)
{
    /* C++ Note: this function makes the word segmentation algorithm incompatible with input iterators.
     * This means Word Boundary Rules are not compatible with streams at all.
     * It can even look like it works but it never pass tests because we must go back after this function.
     * The test line where it fails: "\x0061\x003A\x0001" - AHLetter + MidLetter + Other.
     * Must be 3 words but if we don't go back it will be 2. */

    it_in_utf8 src = first + 0; // Make it compile only for contiguous or random access iterator for now
    type_codept c = 0;

    while (src != last)
    {
        src = utf8_iter(src, last, &c, iter_replacement);

        type_codept prop = stages_break_word_prop(c);

        if (break_word_skip(prop))
            continue;

        return prop;
    }
    return 0;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static bool utf8_break_word(struct impl_break_word_state* state, type_codept c, type_codept* word_prop,
                                 it_in_utf8 first, it_end_utf8 last)
{
    // word_prop property must be used only with impl_break_is_word

    // TODO: https://unicode.org/reports/tr29/#State_Machines
    // ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/WordBreakTest.html
    // Note that only WB5, WB7a, WB8, WB9, WB10, WB13 rules can be handled
    // with the state table so it's not even worth it probably.
    // Compared the performance with ICU it's already much faster so it can wait.

    type_codept c_prop = stages_break_word_prop(c);
    type_codept p_prop = state->prev_cp_prop;

    // Previous values of code points with WB4 rules
    type_codept p1_prop = state->prev_cp1_prop;
    type_codept p2_prop = state->prev_cp2_prop;

    type_codept t_prop = 0;

    bool result = false;

    // https://www.unicode.org/reports/tr29/#Word_Boundary_Rules
    // Unicode 11.0 - 14.0 rules

    if (state->state == state_break_word_begin)
    {
        state->state = state_break_word_process;
        *word_prop = c_prop;
        // TODO: fix word_prop
        //*word_prop = 0;
    }
    else if (p_prop == prop_WB_CR && c_prop == prop_WB_LF) // WB3
        result = false; // NOLINT
    else if (p_prop == prop_WB_Newline || p_prop == prop_WB_CR || p_prop == prop_WB_LF) // WB3a
        result = true; // NOLINT
    else if (c_prop == prop_WB_Newline || c_prop == prop_WB_CR || c_prop == prop_WB_LF) // WB3b
        result = true; // NOLINT
    else if (p_prop == prop_WB_ZWJ && c_prop == prop_WB_Extended_Pictographic) // WB3c
        result = false; // NOLINT
    else if (p_prop == prop_WB_WSegSpace && c_prop == prop_WB_WSegSpace) // WB3d
        result = false; // NOLINT
    else if (break_word_skip(c_prop)) // WB4
    {
        // Just skip forward without touching previous values of code points with WB4 rules
        state->prev_cp = c;
        state->prev_cp_prop = c_prop;
        return false;
    }

    // p and p_prop must not be used anymore because WB4 takes effect below this line

    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB5
        result = false; // NOLINT
    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (c_prop == prop_WB_MidLetter || c_prop == prop_WB_MidNumLet || c_prop == prop_WB_Single_Quote) &&
             ((t_prop = utf8_break_word_skip(first, last)) != 0 &&
              (t_prop == prop_WB_ALetter || t_prop == prop_WB_Hebrew_Letter))) // WB6
        result = false; // NOLINT
    else if ((p2_prop == prop_WB_ALetter || p2_prop == prop_WB_Hebrew_Letter) &&
             (p1_prop == prop_WB_MidLetter || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB7
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && c_prop == prop_WB_Single_Quote) // WB7a
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && c_prop == prop_WB_Double_Quote &&
             utf8_break_word_skip(first, last) == prop_WB_Hebrew_Letter) // WB7b
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
             utf8_break_word_skip(first, last) == prop_WB_Numeric) // WB12
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
    else if (state->state == state_break_word_RI && c_prop == prop_WB_Regional_Indicator) // WB15/WB16
        result = false; // NOLINT
    else // WB999
    {
        result = true;
        *word_prop = c_prop;
        // TODO: fix word_prop
        //*word_prop = 0;
    }

    // WB15/WB16
    if (c_prop == prop_WB_Regional_Indicator)
    {
        if (state->state == state_break_word_RI)
            state->state = state_break_word_RI_RI;
        else
            state->state = state_break_word_RI;
    }
    else
        state->state = state_break_word_process;

    // Set previous values of codepoints with WB4 rules
    state->prev_cp2 = state->prev_cp1;
    state->prev_cp2_prop = state->prev_cp1_prop;
    state->prev_cp1 = c;
    state->prev_cp1_prop = c_prop;
    state->prev_cp = c;
    state->prev_cp_prop = c_prop;

    // TODO: The logic of word_prop is incorrect right now, to make it work properly rearrage all word properties to:
    // all punctuations > prop_WB_Numeric > prop_WB_Hebrew_Letter > prop_WB_ALetter > prop_WB_Katakana > prop_WU_Remaining_Alphabetic
    // then document why such arrangement is required and and uncomment all todos: fix word_prop
    // TODO: fix word_prop
    //if (c_proc > *word_prop)
    //	*word_prop = c_prop;

    return result;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static bool impl_utf8_break_word(struct impl_break_word_state* state, type_codept c, type_codept* word_prop,
                                      it_in_utf8 first, it_end_utf8 last)
{
    return utf8_break_word(state, c, word_prop, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_always_inline_tmpl
uaix_static bool inline_utf8_break_word(struct impl_break_word_state* state, type_codept c, type_codept* word_prop,
                                        it_in_utf8 first, it_end_utf8 last)
{
    return utf8_break_word(state, c, word_prop, first, last);
}

// BEGIN: GENERATED UTF-16 FUNCTIONS
#ifndef UNI_ALGO_DOC_GENERATED_UTF16

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static type_codept utf16_break_word_skip(it_in_utf16 first, it_end_utf16 last)
{
    it_in_utf16 src = first;
    type_codept c = 0;

    while (src != last)
    {
        src = utf16_iter(src, last, &c, iter_replacement);

        type_codept prop = stages_break_word_prop(c);

        if (break_word_skip(prop))
            continue;

        return prop;
    }
    return 0;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool utf16_break_word(struct impl_break_word_state* state, type_codept c, type_codept* word_prop,
                                  it_in_utf16 first, it_end_utf16 last)
{
    // word_prop property must be used only with impl_break_is_word

    // TODO: https://unicode.org/reports/tr29/#State_Machines
    // ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/WordBreakTest.html
    // I compared the performance with ICU it's already much faster so it can wait.

    type_codept c_prop = stages_break_word_prop(c);
    type_codept p_prop = state->prev_cp_prop;

    // Previous values of code points with WB4 rules
    type_codept p1_prop = state->prev_cp1_prop;
    type_codept p2_prop = state->prev_cp2_prop;

    type_codept t_prop = 0;

    bool result = false;

    // https://www.unicode.org/reports/tr29/#Word_Boundary_Rules
    // Unicode 11.0 - 14.0 rules

    if (state->state == state_break_word_begin)
    {
        state->state = state_break_word_process;
        *word_prop = c_prop;
        // TODO: fix word_prop
        //*word_prop = 0;
    }
    else if (p_prop == prop_WB_CR && c_prop == prop_WB_LF) // WB3
        result = false; // NOLINT
    else if (p_prop == prop_WB_Newline || p_prop == prop_WB_CR || p_prop == prop_WB_LF) // WB3a
        result = true; // NOLINT
    else if (c_prop == prop_WB_Newline || c_prop == prop_WB_CR || c_prop == prop_WB_LF) // WB3b
        result = true; // NOLINT
    else if (p_prop == prop_WB_ZWJ && c_prop == prop_WB_Extended_Pictographic) // WB3c
        result = false; // NOLINT
    else if (p_prop == prop_WB_WSegSpace && c_prop == prop_WB_WSegSpace) // WB3d
        result = false; // NOLINT
    else if (break_word_skip(c_prop)) // WB4
    {
        // Just skip forward without touching previous values of code points with WB4 rules
        state->prev_cp = c;
        state->prev_cp_prop = c_prop;
        return false;
    }

    // p and p_prop must not be used anymore because WB4 takes effect below this line

    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB5
        result = false; // NOLINT
    else if ((p1_prop == prop_WB_ALetter || p1_prop == prop_WB_Hebrew_Letter) &&
             (c_prop == prop_WB_MidLetter || c_prop == prop_WB_MidNumLet || c_prop == prop_WB_Single_Quote) &&
             ((t_prop = utf16_break_word_skip(first, last)) != 0 &&
              (t_prop == prop_WB_ALetter || t_prop == prop_WB_Hebrew_Letter))) // WB6
        result = false; // NOLINT
    else if ((p2_prop == prop_WB_ALetter || p2_prop == prop_WB_Hebrew_Letter) &&
             (p1_prop == prop_WB_MidLetter || p1_prop == prop_WB_MidNumLet || p1_prop == prop_WB_Single_Quote) &&
             (c_prop == prop_WB_ALetter || c_prop == prop_WB_Hebrew_Letter)) // WB7
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && c_prop == prop_WB_Single_Quote) // WB7a
        result = false; // NOLINT
    else if (p1_prop == prop_WB_Hebrew_Letter && c_prop == prop_WB_Double_Quote &&
             utf16_break_word_skip(first, last) == prop_WB_Hebrew_Letter) // WB7b
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
             utf16_break_word_skip(first, last) == prop_WB_Numeric) // WB12
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
    else if (state->state == state_break_word_RI && c_prop == prop_WB_Regional_Indicator) // WB15/WB16
        result = false; // NOLINT
    else // WB999
    {
        result = true;
        *word_prop = c_prop;
        // TODO: fix word_prop
        //*word_prop = 0;
    }

    // WB15/WB16
    if (c_prop == prop_WB_Regional_Indicator)
    {
        if (state->state == state_break_word_RI)
            state->state = state_break_word_RI_RI;
        else
            state->state = state_break_word_RI;
    }
    else
        state->state = state_break_word_process;

    // Set previous values of codepoints with WB4 rules
    state->prev_cp2 = state->prev_cp1;
    state->prev_cp2_prop = state->prev_cp1_prop;
    state->prev_cp1 = c;
    state->prev_cp1_prop = c_prop;
    state->prev_cp = c;
    state->prev_cp_prop = c_prop;

    // TODO: fix word_prop
    //if (c_proc > *word_prop)
    //	*word_prop = c_prop;

    return result;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static bool impl_utf16_break_word(struct impl_break_word_state* state, type_codept c, type_codept* word_prop,
                                       it_in_utf16 first, it_end_utf16 last)
{
    return utf16_break_word(state, c, word_prop, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool inline_utf16_break_word(struct impl_break_word_state* state, type_codept c, type_codept* word_prop,
                                         it_in_utf16 first, it_end_utf16 last)
{
    return utf16_break_word(state, c, word_prop, first, last);
}

#endif // UNI_ALGO_DOC_GENERATED_UTF16
// END: GENERATED UTF-16 FUNCTIONS

UNI_ALGO_IMPL_NAMESPACE_END

#include <uni/internal/undefs.h>

#endif // IMPL_BREAK_WORD_H_UAIX

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
