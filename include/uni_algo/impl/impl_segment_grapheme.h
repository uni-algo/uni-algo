/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_SEGMENT_GRAPHEME_H_UAIH
#define UNI_ALGO_SEGMENT_GRAPHEME_H_UAIH

#include "impl_iter.h"

#include "internal_defines.h"
#include "internal_stages.h"

#ifndef UNI_ALGO_STATIC_DATA
#include "data/extern_segment_grapheme.h"
#else
#include "data/data_segment_grapheme.h"
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// See generator_segment_grapheme in gen/gen.h

uaix_const type_codept prop_GB_Prepend               = 1;
uaix_const type_codept prop_GB_CR                    = 2;
uaix_const type_codept prop_GB_LF                    = 3;
uaix_const type_codept prop_GB_Control               = 4;
uaix_const type_codept prop_GB_Extend                = 5;
uaix_const type_codept prop_GB_Regional_Indicator    = 6;
uaix_const type_codept prop_GB_SpacingMark           = 7;
uaix_const type_codept prop_GB_L                     = 8;
uaix_const type_codept prop_GB_V                     = 9;
uaix_const type_codept prop_GB_T                     = 10;
uaix_const type_codept prop_GB_LV                    = 11;
uaix_const type_codept prop_GB_LVT                   = 12;
uaix_const type_codept prop_GB_ZWJ                   = 13;
uaix_const type_codept prop_GB_Extended_Pictographic = 14;

uaix_const type_codept prop_GB_InCB_Linker           = 1;
uaix_const type_codept prop_GB_InCB_Consonant        = 2;
uaix_const type_codept prop_GB_InCB_Extend           = 3;

uaix_const int state_segment_grapheme_begin    = 0;
uaix_const int state_segment_grapheme_continue = 1;
uaix_const int state_segment_grapheme_RI       = 2;
uaix_const int state_segment_grapheme_RI_RI    = 3;
uaix_const int state_segment_grapheme_EP       = 4;
uaix_const int state_segment_grapheme_EP_ZWJ   = 5;
uaix_const int state_segment_grapheme_InCB_C   = 6;
uaix_const int state_segment_grapheme_InCB_CL  = 7;

uaix_always_inline
uaix_static type_codept stages_segment_grapheme_prop(type_codept c)
{
    // This function returns raw property that contains property with InCB (see GB9c rule)
    // Two functions below must be used to get the real property and InCB
    return stages(c, stage1_segment_grapheme, stage2_segment_grapheme);
}

uaix_always_inline
uaix_static type_codept segment_grapheme_prop(type_codept prop)
{
    return (prop & 0x3F); // 6 right bits are used for properties
}

uaix_always_inline
uaix_static type_codept segment_grapheme_prop_inCB(type_codept prop)
{
    return (prop >> 6); // 2 first (left) bits are used for InCB
}

struct impl_segment_grapheme_state
{
    type_codept prev_cp;
    type_codept prev_cp_prop;

    int state;
};

uaix_always_inline
uaix_static void impl_segment_grapheme_state_reset(struct impl_segment_grapheme_state* const state)
{
    state->prev_cp = 0;
    state->prev_cp_prop = 0;

    state->state = state_segment_grapheme_begin;
}
/*
// TODO: see TODO below.
// Extend_ExtCccZwj and ZWJ_ExtCccZwj should not be used.
// ZWJ must be the same as Extend.
uaix_static const bool segment_table_grapheme[15][15] =
{
//   Oth CR LF Con Ext RI Pre SpM L  V  T  LV LVT EP ZWJ
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 1, 1, 1, 1,  1, 0}, // Other
    {1,  1, 0, 1,  1,  1, 1,  1,  1, 1, 1, 1, 1,  1, 1}, // CR
    {1,  1, 1, 1,  1,  1, 1,  1,  1, 1, 1, 1, 1,  1, 1}, // LF
    {1,  1, 1, 1,  1,  1, 1,  1,  1, 1, 1, 1, 1,  1, 1}, // Control
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 1, 1, 1, 1,  1, 0}, // Extend
    {1,  1, 1, 1,  0,  0, 1,  0,  1, 1, 1, 1, 1,  1, 0}, // RI
    {0,  1, 1, 1,  0,  0, 0,  0,  0, 0, 0, 0, 0,  0, 0}, // Prepend
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 1, 1, 1, 1,  1, 0}, // SpacingMark
    {1,  1, 1, 1,  0,  1, 1,  0,  0, 0, 1, 0, 0,  1, 0}, // L
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 0, 0, 1, 1,  1, 0}, // V
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 1, 0, 1, 1,  1, 0}, // T
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 0, 0, 1, 1,  1, 0}, // LV
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 1, 0, 1, 1,  1, 0}, // LVT
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 1, 1, 1, 1,  1, 0}, // ExtPict
    {1,  1, 1, 1,  0,  1, 1,  0,  1, 1, 1, 1, 1,  1, 0}, // ZWJ
};
*/
uaix_always_inline
uaix_static bool segment_grapheme(struct impl_segment_grapheme_state* const state, type_codept c)
{
    // TODO: https://unicode.org/reports/tr29/#State_Machines
    // ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/GraphemeBreakTest.html
    // See state table above.
    // Compared the performance with ICU it's already much faster so it can wait.

    const type_codept raw_prop = stages_segment_grapheme_prop(c);

    const type_codept c_prop = segment_grapheme_prop(raw_prop);
    const type_codept p_prop = segment_grapheme_prop(state->prev_cp_prop);

    const type_codept c_inCB = segment_grapheme_prop_inCB(raw_prop);

    bool result = false; // tag_can_be_uninitialized

    // https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
    // Unicode 15.1 rules

    if (state->state == state_segment_grapheme_begin)
        state->state = state_segment_grapheme_continue;
    else if (p_prop == prop_GB_CR && c_prop == prop_GB_LF) // GB3
        result = false; // NOLINT
    else if (p_prop == prop_GB_Control || p_prop == prop_GB_CR || p_prop == prop_GB_LF) // GB4
        result = true; // NOLINT
    else if (c_prop == prop_GB_Control || c_prop == prop_GB_CR || c_prop == prop_GB_LF) // GB5
        result = true; // NOLINT
    else if (p_prop == prop_GB_L && (c_prop == prop_GB_L || c_prop == prop_GB_V || c_prop == prop_GB_LV || c_prop == prop_GB_LVT)) // GB6
        result = false; // NOLINT
    else if ((p_prop == prop_GB_LV || p_prop == prop_GB_V) && (c_prop == prop_GB_V || c_prop == prop_GB_T)) // GB7
        result = false; // NOLINT
    else if ((p_prop == prop_GB_LVT || p_prop == prop_GB_T) && c_prop == prop_GB_T) // GB8
        result = false; // NOLINT
    else if (c_prop == prop_GB_Extend || c_prop == prop_GB_ZWJ) // GB9
        result = false; // NOLINT
    else if (c_prop == prop_GB_SpacingMark) // GB9a
        result = false; // NOLINT
    else if (p_prop == prop_GB_Prepend) // GB9b
        result = false; // NOLINT
    else if (state->state == state_segment_grapheme_InCB_CL && c_inCB == prop_GB_InCB_Consonant) // GB9c
        result = false; // NOLINT
    else if (state->state == state_segment_grapheme_EP_ZWJ && c_prop == prop_GB_Extended_Pictographic) // GB11
        result = false; // NOLINT
    else if (state->state == state_segment_grapheme_RI && c_prop == prop_GB_Regional_Indicator) // GB12/GB13
        result = false; // NOLINT
    else // GB999
        result = true; // NOLINT

    // GB12/GB13
    if (c_prop == prop_GB_Regional_Indicator)
    {
        if (state->state == state_segment_grapheme_RI)
            state->state = state_segment_grapheme_RI_RI;
        else
            state->state = state_segment_grapheme_RI;
    }
    // GB11
    else if (c_prop == prop_GB_Extended_Pictographic)
        state->state = state_segment_grapheme_EP; // NOLINT
    else if (state->state == state_segment_grapheme_EP && c_prop == prop_GB_Extend)
        state->state = state_segment_grapheme_EP; // NOLINT
    else if (state->state == state_segment_grapheme_EP && c_prop == prop_GB_ZWJ)
        state->state = state_segment_grapheme_EP_ZWJ;
    // GB9c
    else if (c_inCB == prop_GB_InCB_Consonant)
        state->state = state_segment_grapheme_InCB_C; // NOLINT
    else if (state->state == state_segment_grapheme_InCB_C && c_inCB == prop_GB_InCB_Linker)
        state->state = state_segment_grapheme_InCB_CL; // NOLINT
    else if (state->state == state_segment_grapheme_InCB_C && (c_inCB == prop_GB_InCB_Extend || c_inCB == prop_GB_InCB_Linker))
        state->state = state_segment_grapheme_InCB_C; // NOLINT
    else if (state->state == state_segment_grapheme_InCB_CL && (c_inCB == prop_GB_InCB_Extend || c_inCB == prop_GB_InCB_Linker))
        state->state = state_segment_grapheme_InCB_CL; // NOLINT
    // Otherwise state continue
    else
        state->state = state_segment_grapheme_continue;

    state->prev_cp = c;
    state->prev_cp_prop = raw_prop;

    return result;
}

uaix_inline bool impl_segment_grapheme(struct impl_segment_grapheme_state* const state, type_codept c)
{
    return segment_grapheme(state, c);
}

uaix_always_inline
uaix_static bool inline_segment_grapheme(struct impl_segment_grapheme_state* const state, type_codept c)
{
    return segment_grapheme(state, c);
}

// -------------
// REVERSE RULES
// -------------

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static bool segment_grapheme_rev_EP_utf8(it_in_utf8 first, it_in_utf8 last)
{
    it_in_utf8 src = last;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != first)
    {
        src = iter_rev_utf8(first, src, &c, iter_replacement);

        const type_codept prop = segment_grapheme_prop(stages_segment_grapheme_prop(c));

        if (prop == prop_GB_Extend)
            continue;
        if (prop == prop_GB_Extended_Pictographic)
            return false;

        break;
    }
    return true;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static bool segment_grapheme_rev_RI_utf8(it_in_utf8 first, it_in_utf8 last)
{
    it_in_utf8 src = last;
    type_codept c = 0; // tag_can_be_uninitialized
    size_t count_RI = 0; // tag_must_be_initialized

    while (src != first)
    {
        src = iter_rev_utf8(first, src, &c, iter_replacement);

        const type_codept prop = segment_grapheme_prop(stages_segment_grapheme_prop(c));

        if (prop == prop_GB_Regional_Indicator)
            ++count_RI;
        else
            break;
    }
    return (count_RI % 2) != 0;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static bool segment_grapheme_rev_inCB_utf8(it_in_utf8 first, it_in_utf8 last, bool inCB_Linker)
{
    it_in_utf8 src = last;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != first)
    {
        src = iter_rev_utf8(first, src, &c, iter_replacement);

        const type_codept inCB = segment_grapheme_prop_inCB(stages_segment_grapheme_prop(c));

        if (inCB == prop_GB_InCB_Consonant && inCB_Linker)
            return false;

        if (inCB == prop_GB_InCB_Linker)
        {
            inCB_Linker = true;
            continue;
        }
        if (inCB == prop_GB_InCB_Extend)
            continue;

        break;
    }
    return true;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_always_inline_tmpl
uaix_static bool segment_grapheme_rev_utf8(struct impl_segment_grapheme_state* const state, type_codept c,
                                           it_in_utf8 first, it_in_utf8 last)
{
    const type_codept raw_prop = stages_segment_grapheme_prop(c);

    const type_codept c_prop = segment_grapheme_prop(raw_prop);
    const type_codept p_prop = segment_grapheme_prop(state->prev_cp_prop);

    const type_codept c_inCB = segment_grapheme_prop_inCB(raw_prop);

    bool result = false; // tag_can_be_uninitialized

    // https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
    // Unicode 15.1 rules

    if (state->state == state_segment_grapheme_begin)
        state->state = state_segment_grapheme_continue;
    else if (c_prop == prop_GB_CR && p_prop == prop_GB_LF) // GB3
        result = false; // NOLINT
    else if (c_prop == prop_GB_Control || c_prop == prop_GB_CR || c_prop == prop_GB_LF) // GB4
        result = true; // NOLINT
    else if (p_prop == prop_GB_Control || p_prop == prop_GB_CR || p_prop == prop_GB_LF) // GB5
        result = true; // NOLINT
    else if (c_prop == prop_GB_L && (p_prop == prop_GB_L || p_prop == prop_GB_V || p_prop == prop_GB_LV || p_prop == prop_GB_LVT)) // GB6
        result = false; // NOLINT
    else if ((c_prop == prop_GB_LV || c_prop == prop_GB_V) && (p_prop == prop_GB_V || p_prop == prop_GB_T)) // GB7
        result = false; // NOLINT
    else if ((c_prop == prop_GB_LVT || c_prop == prop_GB_T) && p_prop == prop_GB_T) // GB8
        result = false; // NOLINT
    else if (p_prop == prop_GB_Extend || p_prop == prop_GB_ZWJ) // GB9
        result = false; // NOLINT
    else if (p_prop == prop_GB_SpacingMark) // GB9a
        result = false; // NOLINT
    else if (c_prop == prop_GB_Prepend) // GB9b
        result = false; // NOLINT
    else if ((c_inCB == prop_GB_InCB_Linker || c_inCB == prop_GB_InCB_Extend) &&
             segment_grapheme_prop_inCB(state->prev_cp_prop) == prop_GB_InCB_Consonant) // GB9c
        result = segment_grapheme_rev_inCB_utf8(first, last, c_inCB == prop_GB_InCB_Linker);
    else if (c_prop == prop_GB_ZWJ && p_prop == prop_GB_Extended_Pictographic) // GB11
        result = segment_grapheme_rev_EP_utf8(first, last);
    else if (c_prop == prop_GB_Regional_Indicator && p_prop == prop_GB_Regional_Indicator) // GB12/GB13
        result = segment_grapheme_rev_RI_utf8(first, last);
    else // GB999
        result = true; // NOLINT

    state->prev_cp = c;
    state->prev_cp_prop = raw_prop;

    return result;
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_static bool impl_segment_grapheme_rev_utf8(struct impl_segment_grapheme_state* const state, type_codept c,
                                                it_in_utf8 first, it_in_utf8 last)
{
    return segment_grapheme_rev_utf8(state, c, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf8>
#endif
uaix_always_inline_tmpl
uaix_static bool inline_segment_grapheme_rev_utf8(struct impl_segment_grapheme_state* const state, type_codept c,
                                                  it_in_utf8 first, it_in_utf8 last)
{
    return segment_grapheme_rev_utf8(state, c, first, last);
}

// BEGIN: GENERATED UTF-16 FUNCTIONS
#ifndef UNI_ALGO_DOC_GENERATED_UTF16

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static bool segment_grapheme_rev_EP_utf16(it_in_utf16 first, it_in_utf16 last)
{
    it_in_utf16 src = last;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != first)
    {
        src = iter_rev_utf16(first, src, &c, iter_replacement);

        const type_codept prop = segment_grapheme_prop(stages_segment_grapheme_prop(c));

        if (prop == prop_GB_Extend)
            continue;
        if (prop == prop_GB_Extended_Pictographic)
            return false;

        break;
    }
    return true;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static bool segment_grapheme_rev_RI_utf16(it_in_utf16 first, it_in_utf16 last)
{
    it_in_utf16 src = last;
    type_codept c = 0; // tag_can_be_uninitialized
    size_t count_RI = 0; // tag_must_be_initialized

    while (src != first)
    {
        src = iter_rev_utf16(first, src, &c, iter_replacement);

        const type_codept prop = segment_grapheme_prop(stages_segment_grapheme_prop(c));

        if (prop == prop_GB_Regional_Indicator)
            ++count_RI;
        else
            break;
    }
    return (count_RI % 2) != 0;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static bool segment_grapheme_rev_inCB_utf16(it_in_utf16 first, it_in_utf16 last, bool inCB_Linker)
{
    it_in_utf16 src = last;
    type_codept c = 0; // tag_can_be_uninitialized

    while (src != first)
    {
        src = iter_rev_utf16(first, src, &c, iter_replacement);

        const type_codept inCB = segment_grapheme_prop_inCB(stages_segment_grapheme_prop(c));

        if (inCB == prop_GB_InCB_Consonant && inCB_Linker)
            return false;

        if (inCB == prop_GB_InCB_Linker)
        {
            inCB_Linker = true;
            continue;
        }
        if (inCB == prop_GB_InCB_Extend)
            continue;

        break;
    }
    return true;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool segment_grapheme_rev_utf16(struct impl_segment_grapheme_state* const state, type_codept c,
                                            it_in_utf16 first, it_in_utf16 last)
{
    const type_codept raw_prop = stages_segment_grapheme_prop(c);

    const type_codept c_prop = segment_grapheme_prop(raw_prop);
    const type_codept p_prop = segment_grapheme_prop(state->prev_cp_prop);

    const type_codept c_inCB = segment_grapheme_prop_inCB(raw_prop);

    bool result = false; // tag_can_be_uninitialized

    // https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
    // Unicode 15.1 rules

    if (state->state == state_segment_grapheme_begin)
        state->state = state_segment_grapheme_continue;
    else if (c_prop == prop_GB_CR && p_prop == prop_GB_LF) // GB3
        result = false; // NOLINT
    else if (c_prop == prop_GB_Control || c_prop == prop_GB_CR || c_prop == prop_GB_LF) // GB4
        result = true; // NOLINT
    else if (p_prop == prop_GB_Control || p_prop == prop_GB_CR || p_prop == prop_GB_LF) // GB5
        result = true; // NOLINT
    else if (c_prop == prop_GB_L && (p_prop == prop_GB_L || p_prop == prop_GB_V || p_prop == prop_GB_LV || p_prop == prop_GB_LVT)) // GB6
        result = false; // NOLINT
    else if ((c_prop == prop_GB_LV || c_prop == prop_GB_V) && (p_prop == prop_GB_V || p_prop == prop_GB_T)) // GB7
        result = false; // NOLINT
    else if ((c_prop == prop_GB_LVT || c_prop == prop_GB_T) && p_prop == prop_GB_T) // GB8
        result = false; // NOLINT
    else if (p_prop == prop_GB_Extend || p_prop == prop_GB_ZWJ) // GB9
        result = false; // NOLINT
    else if (p_prop == prop_GB_SpacingMark) // GB9a
        result = false; // NOLINT
    else if (c_prop == prop_GB_Prepend) // GB9b
        result = false; // NOLINT
    else if ((c_inCB == prop_GB_InCB_Linker || c_inCB == prop_GB_InCB_Extend) &&
             segment_grapheme_prop_inCB(state->prev_cp_prop) == prop_GB_InCB_Consonant) // GB9c
        result = segment_grapheme_rev_inCB_utf16(first, last, c_inCB == prop_GB_InCB_Linker);
    else if (c_prop == prop_GB_ZWJ && p_prop == prop_GB_Extended_Pictographic) // GB11
        result = segment_grapheme_rev_EP_utf16(first, last);
    else if (c_prop == prop_GB_Regional_Indicator && p_prop == prop_GB_Regional_Indicator) // GB12/GB13
        result = segment_grapheme_rev_RI_utf16(first, last);
    else // GB999
        result = true; // NOLINT

    state->prev_cp = c;
    state->prev_cp_prop = raw_prop;

    return result;
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_static bool impl_segment_grapheme_rev_utf16(struct impl_segment_grapheme_state* const state, type_codept c,
                                                 it_in_utf16 first, it_in_utf16 last)
{
    return segment_grapheme_rev_utf16(state, c, first, last);
}

#ifdef __cplusplus
template<typename it_in_utf16>
#endif
uaix_always_inline_tmpl
uaix_static bool inline_segment_grapheme_rev_utf16(struct impl_segment_grapheme_state* const state, type_codept c,
                                                   it_in_utf16 first, it_in_utf16 last)
{
    return segment_grapheme_rev_utf16(state, c, first, last);
}

#endif // UNI_ALGO_DOC_GENERATED_UTF16
// END: GENERATED UTF-16 FUNCTIONS


UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_SEGMENT_GRAPHEME_H_UAIH

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
