/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_IMPL_NORM_H_UAIH
#define UNI_ALGO_IMPL_NORM_H_UAIH

#include "impl_iter.h"

#ifndef UNI_ALGO_DISABLE_PROP
#include "impl_prop.h"
#endif

#include "internal_defines.h"
#include "internal_stages.h"

#ifndef UNI_ALGO_STATIC_DATA
#include "data/extern_norm.h"
#else
#include "data/data_norm.h"
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// The buffer must not be less than 4(3)+30+18=52(51)
// 4 is 4 starters, check norm_decomp_return why 4(3)
// 30 is the max number of non-starters
// 18 is max decomposition (we use the end of the buffer to store the next decomposed code point)
// Use 70 just because it means we need 70*4+70=350 bytes on stack for any normalization
// TODO: 70 is too much, now we have better safe facilities to guarantee that the size of the buffer will be enough in future Unicode versions
// and smaller buffer gives a bit better performance for some strange reason (CPU cache maybe? Needs more testing).
#ifdef __cplusplus
uaix_const size_t norm_buffer_size = 70; // tag_unicode_unstable_value
#else // tag_synchronize
#define norm_buffer_size 70
#endif

uaix_const size_t impl_max_norm_non_starters    = 30; // tag_unicode_stable_value
// Quote "...sequences of non-starters longer than 30 characters in length..." from:
// https://unicode.org/reports/tr15/#Stream_Safe_Text_Format

uaix_const size_t impl_max_norm_decomp_canon    = 4;  // tag_unicode_unstable_value
uaix_const size_t impl_max_norm_decomp_compat   = 18; // tag_unicode_unstable_value
// Quote "A canonical mapping ... pair of characters, but is never longer than two characters"
// and quote "Compatibility mappings are guaranteed to be no longer than 18 characters" from:
// https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
// but these values are for full decomposition so they are unstable
// because in theory they may change in Unicode but it is very unlikely.

// http://www.unicode.org/faq/normalization.html#12
// The length of a destination (result) string must be premultiplied with one of these
// Example: destination_length = source_length * impl_x_function_name
uaix_const size_t impl_x_norm_to_nfc_utf8       = 3;  // tag_unicode_stable_value
uaix_const size_t impl_x_norm_to_nfd_utf8       = 3;  // tag_unicode_unstable_value
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
uaix_const size_t impl_x_norm_to_nfkc_utf8      = 11; // tag_unicode_unstable_value
uaix_const size_t impl_x_norm_to_nfkd_utf8      = 11; // tag_unicode_unstable_value
#endif
uaix_const size_t impl_x_norm_to_nfc_utf16      = 3;  // tag_unicode_stable_value
uaix_const size_t impl_x_norm_to_nfd_utf16      = 4;  // tag_unicode_unstable_value
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
uaix_const size_t impl_x_norm_to_nfkc_utf16     = 18; // tag_unicode_unstable_value
uaix_const size_t impl_x_norm_to_nfkd_utf16     = 18; // tag_unicode_unstable_value
#endif
#ifndef UNI_ALGO_DISABLE_PROP
uaix_const size_t impl_x_norm_to_unaccent_utf8  = 3;  // tag_unicode_stable_value
uaix_const size_t impl_x_norm_to_unaccent_utf16 = 3;  // tag_unicode_stable_value
#endif
// Only Decomposition_Mapping of NFC is Unicode stable value for strings.
// https://www.unicode.org/policies/stability_policy.html#Property_Value

// Return values for normalization detection functions
uaix_const int impl_norm_is_yes                 = 0;
uaix_const int impl_norm_is_ill_formed          = 8;
uaix_const int impl_norm_is_not_canonical_order = 16;
uaix_const int impl_norm_is_not_stream_safe     = 32;
//uaix_const int impl_norm_is_no                = 1; // Reserved
//uaix_const int impl_norm_is_maybe             = 2; // Reserved
uaix_const int norm_is_no_or_maybe              = 3; // Can be changed in the future. Must never be used in a wrapper.

struct norm_buffer
{
    uaix_array(type_codept, cps, norm_buffer_size);
    uaix_array(unsigned char, ccc, norm_buffer_size);
};

struct norm_multi
{
    size_t size;
    size_t last_qc;
    size_t count_ns;
};

uaix_always_inline
uaix_static size_t stages_decomp_nfd(type_codept c)
{
    // return offset to decomposed code points in NFD
    return stages(c, stage1_decomp_nfd, stage2_decomp_nfd);
}

uaix_always_inline
uaix_static size_t stages_decomp_nfd_number(size_t offset)
{
    // return number of decomposed code points in NFD
    return stage3_decomp_nfd[offset];
}

uaix_always_inline
uaix_static type_codept stages_decomp_nfd_cp(size_t offset, size_t i)
{
    // return code point by index in NFD
    return stage3_decomp_nfd[offset + i + 1];
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static size_t stages_decomp_nfkd(type_codept c)
{
    // return offset to decomposed code points in NFKD
    return stages(c, stage1_decomp_nfkd, stage2_decomp_nfkd);
}

uaix_always_inline
uaix_static size_t stages_decomp_nfkd_number(size_t offset)
{
    // return number of decomposed code points in NFKD
    return stage3_decomp_nfkd[offset];
}

uaix_always_inline
uaix_static type_codept stages_decomp_nfkd_cp(size_t offset, size_t i)
{
    // return code point by index in NFKD
    return stage3_decomp_nfkd[offset + i + 1];
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static type_codept stages_comp(type_codept c1, type_codept c2)
{
    // NOTE: The approach with two-dimensional array is used here as recommended in:
    // 14.1.2 Optimizing Tables for NFC Composition
    // https://unicode.org/reports/tr15/#NFC_Table_Optimization
    // but two-dimensional array is packed into optimized one-dimensional array
    // to reduce the size of the data.

    const size_t i1 = stages(c1, stage1_comp_cp1, stage2_comp_cp1);
    if (i1 != 0)
    {
        const size_t i2 = stages(c2, stage1_comp_cp2, stage2_comp_cp2);
        if (i2 != 0 && i2 >= stage3_comp[i1] && i2 <= stage3_comp[i1 + 1])
            return stage3_comp[i1 + 2 + (i2 - stage3_comp[i1])];
    }
    return 0;

    // REMINDER: For reference this is the old version that use
    // two-dimensional table instead of packed composition table.
    // Also see function new_generator_unicodedata_compose in gen/gen.h
/*
    const size_t i1 = stages(c1, stage1_comp_cp1, stage2_comp_cp1);
    if (i1 != 0)
    {
        const size_t i2 = stages(c2, stage1_comp_cp2, stage2_comp_cp2);
        if (i2 != 0)
            return stage3_comp[i1][i2];
    }
    return 0;
*/
}

uaix_always_inline
uaix_static unsigned char stages_ccc(type_codept c)
{
    /* ccc_qc format: zzyydcbaxxxxxxxx (16-bit)
     * xxxxxxxx (bit 0 - 7) - Canonical_Combining_Class (0 - 254)
     * a (bit 8)  - NFC_Quick_Check=No || NFC_Quick_Check=Maybe (0, 1)
     * b (bit 9)  - NFD_Quick_Check=No (0, 1)
     * c (bit 10) - NFKC_Quick_Check=No || NFKC_Quick_Check=Maybe (0, 1)
     * d (bit 11) - NFKD_Quick_Check=No (0, 1)
     * yy (bit 12, 13) - Trailing non-starters in NFKD (for Stream-Safe Text Process) (0 - 3)
     * zz (bit 14, 15) - Initial non-starters in NFKD (for Stream-Safe Text Process) (0 - 2)
     * Note 1: there cannot be both initial and trailing only one of those
     * Note 2: we merge Maybe and No together just because we need some space for bits
     * for initial/trailing non-starters, we don't need Maybe in this algorithm anyway
     */

    const type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
    return (unsigned char)(ccc_qc & 0xFF);
}

uaix_const type_codept norm_bit_nfc  = 8;
uaix_const type_codept norm_bit_nfd  = 9;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
uaix_const type_codept norm_bit_nfkc = 10;
uaix_const type_codept norm_bit_nfkd = 11;
#endif

uaix_const type_codept norm_bound_nfc  = 0x0300;
uaix_const type_codept norm_bound_nfd  = 0x00C0;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
uaix_const type_codept norm_bound_nfkc = 0x00A0;
uaix_const type_codept norm_bound_nfkd = 0x00A0;
#endif

uaix_always_inline
uaix_static bool stages_ccc_qc_yes(type_codept ccc_qc, type_codept bit)
{
    // Canonical_Combining_Class=0 && Quick_Check=Yes (!Quick_Check=No && !Quick_Check=Maybe)
    return (ccc_qc & 0xFF) == 0 && !(ccc_qc & (type_codept)1 << bit);
}

uaix_always_inline
uaix_static bool stages_ccc_qc_detect(type_codept ccc_qc, type_codept bit)
{
    // Quick_Check=Yes (!Quick_Check=No && !Quick_Check=Maybe)
    return !(ccc_qc & (type_codept)1 << bit);
}

uaix_always_inline
uaix_static bool stages_ccc_qc_ns(type_codept ccc_qc, size_t* const count_ns)
{
    // https://unicode.org/reports/tr15/#Stream_Safe_Text_Format

    if (ccc_qc >> 14) // Initial non-starters in NFKD
    {
        *count_ns += ccc_qc >> 14;
        if (*count_ns > impl_max_norm_non_starters)
            return false;
    }
    else
    {
        *count_ns = ccc_qc >> 12; // Trailing non-starters in NFKD
    }
    return true;
}

uaix_always_inline
uaix_static bool stages_qc_yes_ns(type_codept c, size_t* const count_ns, type_codept norm_bit)
{
    /* Note that we always use NFKD lower bound in this
     * function because we need to count initial/trailing
     * non-starters in NFKD for Stream-Safe Text Process.
     * This disallow us to use NFC lower bound for example,
     * that is much higher, to achive a better performance,
     * but it cannot be done other way.
     * Note that we can still use real lower bounds
     * in normalization detection functions.
     * NOTE: Lower bound means everything below that has
     * Quick_Check=Yes and Canonical_Combining_Class=0.
     */

    if (c >= norm_bound_nfkd) // NFKD lower bound
    {
        const type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (!stages_ccc_qc_ns(ccc_qc, count_ns))
            return false;
        return stages_ccc_qc_yes(ccc_qc, norm_bit);
    }
    *count_ns = 0;
    return true;
}

uaix_always_inline
uaix_static bool stages_qc_yes(type_codept c, type_codept norm_bit)
{
    const type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
    return stages_ccc_qc_yes(ccc_qc, norm_bit);
}

uaix_always_inline
uaix_static bool stages_ccc_qc_order(type_codept ccc_qc, unsigned char* const last_ccc)
{
    // https://unicode.org/reports/tr15/#Detecting_Normalization_Forms
    // Note: "if (Character.isSupplementaryCodePoint(ch)) ++i;"
    // check is irrelevant it's because Java uses UTF-16 internally.

    const unsigned char ccc = (ccc_qc & 0xFF);

    if (*last_ccc > ccc && ccc != 0)
    {
        *last_ccc = ccc;
        return false;
    }
    *last_ccc = ccc;
    return true;
}

uaix_always_inline
uaix_static void norm_order(struct norm_buffer* const buffer, size_t size)
{
    // Canonical Ordering Algorithm

    if (size > 0)
    {
        size_t last = size - 1;

        while (last > 0)
        {
            size_t new_last = 0; // tag_must_be_initialized

            for (size_t curr = 0; curr < last; ++curr)
            {
                const size_t next = curr + 1;

                if (buffer->ccc[next] > 0 && buffer->ccc[next] < buffer->ccc[curr])
                {
                    // Swap

                    const type_codept   temp_cps = buffer->cps[curr];
                    buffer->cps[curr] = buffer->cps[next];
                    buffer->cps[next] = temp_cps;

                    const unsigned char temp_ccc = buffer->ccc[curr];
                    buffer->ccc[curr] = buffer->ccc[next];
                    buffer->ccc[next] = temp_ccc;

                    new_last = curr;
                }
            }

            last = new_last;
        }
    }
}

uaix_always_inline
uaix_static size_t norm_decomp_hangul(type_codept c, size_t i, struct norm_buffer* const buffer)
{
    const type_codept SBase = 0xAC00;
    const type_codept LBase = 0x1100;
    const type_codept VBase = 0x1161;
    const type_codept TBase = 0x11A7;
    const type_codept TCount = 28;
    const type_codept NCount = 588;
    const type_codept SCount = 11172;

    if (c >= SBase && c < SBase + SCount)
    {
        const type_codept SIndex = c - SBase;
        const type_codept LPart = LBase + SIndex / NCount;
        const type_codept VPart = VBase + (SIndex % NCount) / TCount;
        const type_codept TPart = TBase + SIndex % TCount;

        buffer->cps[i] = LPart;
        buffer->ccc[i] = 0;

        buffer->cps[i + 1] = VPart;
        buffer->ccc[i + 1] = 0;

        if (TPart != TBase)
        {
            buffer->cps[i + 2] = TPart;
            buffer->ccc[i + 2] = 0;

            return 3;
        }

        return 2;
    }

    return 0;
}

uaix_always_inline
uaix_static size_t norm_comp_hangul(size_t i, struct norm_buffer* const buffer, size_t size)
{
    // Note: CCC=255 is used to mark composed code points

    const type_codept SBase = 0xAC00;
    const type_codept LBase = 0x1100;
    const type_codept VBase = 0x1161;
    const type_codept TBase = 0x11A7;
    const type_codept LCount = 19;
    const type_codept VCount = 21;
    const type_codept TCount = 28;
    const type_codept SCount = 11172;

    const type_codept c1 = buffer->cps[i];

    if (c1 >= LBase && c1 < LBase + LCount) // L+V+T
    {
        if (i + 1 < size)
        {
            const type_codept c2 = buffer->cps[i + 1];

            if (c2 >= VBase && c2 < VBase + VCount)
            {
                const type_codept LIndex = c1 - LBase;
                const type_codept VIndex = c2 - VBase;

                buffer->cps[i] = SBase + (LIndex * VCount + VIndex) * TCount;
                buffer->ccc[i + 1] = 255;

                if (i + 2 < size)
                {
                    const type_codept c3 = buffer->cps[i + 2];

                    if (c3 > TBase && c3 < TBase + TCount)
                    {
                        buffer->cps[i] += c3 - TBase;
                        buffer->ccc[i + 2] = 255;

                        return 2;
                    }
                }

                return 1;
            }
        }
    }
    else if (c1 >= SBase && c1 < SBase + SCount && ((c1 - SBase) % TCount) == 0) // LV+T
    {
        if (i + 1 < size)
        {
            const type_codept c2 = buffer->cps[i + 1];

            if (c2 > TBase && c2 < TBase + TCount)
            {
                buffer->cps[i] += c2 - TBase;
                buffer->ccc[i + 1] = 255;

                return 1;
            }

        }
    }

    return 0;
}

uaix_always_inline
uaix_static void norm_comp(struct norm_buffer* const buffer, size_t size)
{
    // Note: CCC=255 is used to mark composed code points

    if (size > 0)
    {
        size_t starter = 0; // tag_must_be_initialized

        for (size_t i = 0; i < size - 1; ++i)
        {
            const size_t hangul = norm_comp_hangul(i, buffer, size);
            if (hangul)
            {
                i += hangul;
                continue;
            }

            if (buffer->ccc[i] == 0)
                starter = i;

            // If there is a starter and non-blocked non-starter
            if (starter == i || !(buffer->ccc[i] != 255 && buffer->ccc[i] >= buffer->ccc[i + 1]))
            {
                const type_codept c = stages_comp(buffer->cps[starter], buffer->cps[i + 1]);
                if (c != 0)
                {
                    buffer->cps[starter] = c;
                    buffer->ccc[i + 1] = 255;
                }
            }
        }
    }
}

uaix_always_inline
uaix_static bool norm_decomp_return(struct norm_buffer* const buffer, struct norm_multi* const m)
{
    // UNUSED: buffer->cps, m->count_ns

    // Flush the buffer by Quick_Check=Yes boundary.
    if (m->last_qc > 0)
        return true;

    // More than 4 starters cannot compose together so flush the buffer.
    // In other words flush the buffer by CCC=0 boundary at this point.
    // Note 1: actually 3 starters (Hanguls L+V+T and 0CCB) but use 4 (max canonical decomposition) just in case.
    // Note 2: this check only matters for NFC/NFKC because in NFD/NFKD such cases are not possible.
    if (m->size > impl_max_norm_decomp_canon && buffer->ccc[m->size - 1] == 0)
    {
        m->last_qc = m->size - 1;
        return true;
    }

    // This must never happen so flush the buffer and stop to make it noticeable in tests.
    // 18 is max compatibility decomposition so the next turn can exhaust the buffer.
    // NOTE: C++ safe layer can prevent this error even without this check.
    if (m->size + impl_max_norm_decomp_compat > norm_buffer_size)
    {
#ifdef UNI_ALGO_TEST_ASSERT
        uaix_assert(false);
#endif
        m->last_qc = m->size;
        return true;
    }

    return false;
}

uaix_always_inline
uaix_static void norm_decomp_count_ns(struct norm_buffer* const buffer, struct norm_multi* const m)
{
    // Insert U+034F COMBINING GRAPHEME JOINER (CGJ) if there is more than 30 non-starters

    if (m->count_ns > impl_max_norm_non_starters)
    {
        m->count_ns -= impl_max_norm_non_starters;
        buffer->cps[m->size] = 0x034F;
        buffer->ccc[m->size] = 0;
        m->last_qc = m->size;
        ++m->size;
    }
}

uaix_always_inline
uaix_static bool norm_decomp_nfc(type_codept c, struct norm_buffer* const buffer, struct norm_multi* const m)
{
    norm_decomp_count_ns(buffer, m);

    // Need to decompose the first code point when NFC/NFKC,
    // it can be still composed if we drop here from fast loop
    if (m->size == 1)
    {
        const size_t offset = stages_decomp_nfd(buffer->cps[0]);
        if (offset)
        {
            m->size = 0;
            const size_t number = stages_decomp_nfd_number(offset);
            for (size_t i = 0; i < number; ++i, ++m->size)
            {
                buffer->cps[m->size] = stages_decomp_nfd_cp(offset, i);
                buffer->ccc[m->size] = stages_ccc(buffer->cps[m->size]);
                if (stages_qc_yes(buffer->cps[m->size], norm_bit_nfc))
                    m->last_qc = m->size;
            }
        }
    }

    // Do not decompose Hanguls like we do in norm_decomp_nfd, LV+T cases are handled in norm_comp

    // This additional quick check branch is almost irrelevant but in NFC/NFKC
    // in some cases we can stuck in slow loop a bit longer than needed and
    // we need the check to avoid it. This does not affect behaviour.
    // Test string: "\x005A\x0301\x0179\x0179\x0179\x0179\x0179"
    if (stages_qc_yes(c, norm_bit_nfc))
    {
        buffer->cps[m->size] = c;
        buffer->ccc[m->size] = stages_ccc(c);
        m->last_qc = m->size;
        ++m->size;
    }
    else
    {
        const size_t offset = stages_decomp_nfd(c);
        if (offset == 0)
        {
            buffer->cps[m->size] = c;
            buffer->ccc[m->size] = stages_ccc(c);
            if (stages_qc_yes(c, norm_bit_nfc))
                m->last_qc = m->size;
            ++m->size;
        }
        else
        {
            const size_t number = stages_decomp_nfd_number(offset);
            for (size_t i = 0; i < number; ++i, ++m->size)
            {
                buffer->cps[m->size] = stages_decomp_nfd_cp(offset, i);
                buffer->ccc[m->size] = stages_ccc(buffer->cps[m->size]);
                if (stages_qc_yes(buffer->cps[m->size], norm_bit_nfc))
                    m->last_qc = m->size;
            }
        }
    }

    return norm_decomp_return(buffer, m);
}

uaix_always_inline
uaix_static bool norm_decomp_nfd(type_codept c, struct norm_buffer* const buffer, struct norm_multi* const m)
{
    // Almost the same as norm_decomp_nfc but decompose everything

    norm_decomp_count_ns(buffer, m);

    const size_t hangul = norm_decomp_hangul(c, m->size, buffer);
    if (hangul)
    {
        m->last_qc = m->size;
        m->size += hangul;
    }
    else
    {
        const size_t offset = stages_decomp_nfd(c);
        if (offset == 0)
        {
            buffer->cps[m->size] = c;
            buffer->ccc[m->size] = stages_ccc(c);
            if (stages_qc_yes(c, norm_bit_nfd))
                m->last_qc = m->size;
            ++m->size;
        }
        else
        {
            const size_t number = stages_decomp_nfd_number(offset);
            for (size_t i = 0; i < number; ++i, ++m->size)
            {
                buffer->cps[m->size] = stages_decomp_nfd_cp(offset, i);
                buffer->ccc[m->size] = stages_ccc(buffer->cps[m->size]);
                if (stages_qc_yes(buffer->cps[m->size], norm_bit_nfd))
                    m->last_qc = m->size;
            }
        }
    }

    return norm_decomp_return(buffer, m);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool norm_decomp_nfkc(type_codept c, struct norm_buffer* const buffer, struct norm_multi* const m)
{
    // The same as norm_decomp_nfc but uses NFKC data

    norm_decomp_count_ns(buffer, m);

    if (m->size == 1)
    {
        const size_t offset = stages_decomp_nfkd(buffer->cps[0]);
        if (offset)
        {
            m->size = 0;
            const size_t number = stages_decomp_nfkd_number(offset);
            for (size_t i = 0; i < number; ++i, ++m->size)
            {
                buffer->cps[m->size] = stages_decomp_nfkd_cp(offset, i);
                buffer->ccc[m->size] = stages_ccc(buffer->cps[m->size]);
                if (stages_qc_yes(buffer->cps[m->size], norm_bit_nfkc))
                    m->last_qc = m->size;
            }
        }
    }

    if (stages_qc_yes(c, norm_bit_nfkc))
    {
        buffer->cps[m->size] = c;
        buffer->ccc[m->size] = stages_ccc(c);
        m->last_qc = m->size;
        ++m->size;
    }
    else
    {
        const size_t offset = stages_decomp_nfkd(c);
        if (offset == 0)
        {
            buffer->cps[m->size] = c;
            buffer->ccc[m->size] = stages_ccc(c);
            if (stages_qc_yes(c, norm_bit_nfkc))
                m->last_qc = m->size;
            ++m->size;
        }
        else
        {
            const size_t number = stages_decomp_nfkd_number(offset);
            for (size_t i = 0; i < number; ++i, ++m->size)
            {
                buffer->cps[m->size] = stages_decomp_nfkd_cp(offset, i);
                buffer->ccc[m->size] = stages_ccc(buffer->cps[m->size]);
                if (stages_qc_yes(buffer->cps[m->size], norm_bit_nfkc))
                    m->last_qc = m->size;
            }
        }
    }

    return norm_decomp_return(buffer, m);
}

uaix_always_inline
uaix_static bool norm_decomp_nfkd(type_codept c, struct norm_buffer* const buffer, struct norm_multi* const m)
{
    // The same as norm_decomp_nfd but uses NFKD data

    norm_decomp_count_ns(buffer, m);

    const size_t hangul = norm_decomp_hangul(c, m->size, buffer);
    if (hangul)
    {
        m->last_qc = m->size;
        m->size += hangul;
    }
    else
    {
        const size_t offset = stages_decomp_nfkd(c);
        if (offset == 0)
        {
            buffer->cps[m->size] = c;
            buffer->ccc[m->size] = stages_ccc(c);
            if (stages_qc_yes(c, norm_bit_nfkd))
                m->last_qc = m->size;
            ++m->size;
        }
        else
        {
            const size_t number = stages_decomp_nfkd_number(offset);
            for (size_t i = 0; i < number; ++i, ++m->size)
            {
                buffer->cps[m->size] = stages_decomp_nfkd_cp(offset, i);
                buffer->ccc[m->size] = stages_ccc(buffer->cps[m->size]);
                if (stages_qc_yes(buffer->cps[m->size], norm_bit_nfkd))
                    m->last_qc = m->size;
            }
        }
    }

    return norm_decomp_return(buffer, m);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_PROP

uaix_always_inline
uaix_static bool norm_decomp_unaccent(type_codept c, struct norm_buffer* const buffer, struct norm_multi* const m)
{
    // Ignore Hanguls, decompose to NFD, skip Nonspacing Mark
    // The full algorithm is: NFD -> remove Nonspacing Mark -> NFC

    norm_decomp_count_ns(buffer, m);

    const size_t offset = stages_decomp_nfd(c);
    if (offset == 0)
    {
        if (impl_prop_get_gc_prop(impl_prop_get_prop(c)) != impl_General_Category_Mn)
        {
            buffer->cps[m->size] = c;
            buffer->ccc[m->size] = stages_ccc(c);
            if (stages_qc_yes(c, norm_bit_nfd))
                m->last_qc = m->size;
            ++m->size;
        }
    }
    else
    {
        const size_t number = stages_decomp_nfd_number(offset);
        for (size_t i = 0; i < number; ++i)
        {
            const type_codept cp = stages_decomp_nfd_cp(offset, i);
            if (impl_prop_get_gc_prop(impl_prop_get_prop(cp)) != impl_General_Category_Mn)
            {
                buffer->cps[m->size] = cp;
                buffer->ccc[m->size] = stages_ccc(cp);
                if (stages_qc_yes(cp, norm_bit_nfd))
                    m->last_qc = m->size;
                ++m->size;
            }
        }
    }

    return norm_decomp_return(buffer, m);
}

#endif // UNI_ALGO_DISABLE_PROP

uaix_always_inline
uaix_static void norm_proc_buffer(struct norm_buffer* const buffer, struct norm_multi* const m)
{
    // UNUSED: m->count_ns

    // Move the last decomposed code point that between last_qc and size to the start
    // last_qc is last Quick_Check=Yes mark

    for (size_t i = 0, j = m->last_qc; j < m->size; ++i, ++j)
    {
        buffer->cps[i] = buffer->cps[j];
        buffer->ccc[i] = buffer->ccc[j];
    }
    m->size = m->size - m->last_qc;
    m->last_qc = 0;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_norm_to_nfc_utf8(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    /* while (src != last) is the fast loop, any slow down there reduces the performance drastically,
     * so everything moved outside this loop and we drop there if we actually need to do something.
     * Note that we always need to read 1 code point ahead in the algorithm that supports streams
     * so our fast loop is not that fast, probably it can be optimized a bit more but it's fine. Anyway
     * it's impossible to achieve the same speed of the fast loop as in an algorithm that doesn't support streams.
     * ifs are written like that to avoid unnecessary jumps so it gives the best performance.
     * __builtin_expect is used because it achieves a better performance in GCC.
     * Other normalization functions are based on this function.
     */

    do
    {
        while (src != last)
        {
            src = iter_utf8(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfc)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf8(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue; // Fast loop ends here
                }
            }
            if (norm_decomp_nfc(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0) // The end of the data if we are here without last_qc
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);
        norm_comp(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
        {
            if (buffer.ccc[i] != 255)
                dst = codepoint_to_utf8(buffer.cps[i], dst);
        }

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_norm_to_nfd_utf8(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    // The same as impl_utf8_nfc but without composition and uses decomposition data

    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    do
    {
        while (src != last)
        {
            src = iter_utf8(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfd)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf8(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfd(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0)
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
            dst = codepoint_to_utf8(buffer.cps[i], dst);

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_norm_to_nfkc_utf8(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    // The same as impl_utf8_nfc but uses NFKC data

    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    do
    {
        while (src != last)
        {
            src = iter_utf8(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfkc)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf8(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfkc(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0)
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);
        norm_comp(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
        {
            if (buffer.ccc[i] != 255)
                dst = codepoint_to_utf8(buffer.cps[i], dst);
        }

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_norm_to_nfkd_utf8(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    // The same as impl_utf8_nfd but uses NFKD data

    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    do
    {
        while (src != last)
        {
            src = iter_utf8(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfkd)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf8(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfkd(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0)
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
            dst = codepoint_to_utf8(buffer.cps[i], dst);

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_PROP

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_norm_to_unaccent_utf8(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    // Algorithm: NFD -> remove Nonspacing Mark -> NFC

    do
    {
        while (src != last)
        {
            src = iter_utf8(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfd)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf8(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue;
                }
            }
            if (norm_decomp_unaccent(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0)
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);
        norm_comp(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
        {
            if (buffer.ccc[i] != 255)
                dst = codepoint_to_utf8(buffer.cps[i], dst);
        }

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_PROP

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int norm_detect_utf8(it_in_utf8 first, it_end_utf8 last, type_codept norm_bit, type_codept norm_bound)
{
    it_in_utf8 src = first;

    type_codept c = 0; // tag_can_be_uninitialized
    unsigned char last_ccc = 0; // tag_must_be_initialized

    while (src != last)
    {
        src = iter_utf8(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (c < norm_bound)
            last_ccc = 0;
        else
        {
            const type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
            if (!stages_ccc_qc_detect(ccc_qc, norm_bit))
                return norm_is_no_or_maybe;
            if (!stages_ccc_qc_order(ccc_qc, &last_ccc))
                return impl_norm_is_not_canonical_order;
        }
    }
    return impl_norm_is_yes;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int norm_detect_css_utf8(it_in_utf8 first, it_end_utf8 last, type_codept norm_bit, type_codept norm_bound)
{
    it_in_utf8 src = first;

    type_codept c = 0; // tag_can_be_uninitialized
    unsigned char last_ccc = 0; // tag_must_be_initialized
    size_t count_ns = 0; // tag_must_be_initialized

    while (src != last)
    {
        src = iter_utf8(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (c < norm_bound)
        {
            last_ccc = 0;
            count_ns = 0;
        }
        else
        {
            const type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
            if (!stages_ccc_qc_detect(ccc_qc, norm_bit))
                return norm_is_no_or_maybe;
            if (!stages_ccc_qc_order(ccc_qc, &last_ccc))
                return impl_norm_is_not_canonical_order;
            if (!stages_ccc_qc_ns(ccc_qc, &count_ns))
                return impl_norm_is_not_stream_safe;
        }
    }
    return impl_norm_is_yes;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_norm_is_nfc_utf8(it_in_utf8 first, it_end_utf8 last, bool check_stream_safe)
{
    if (!check_stream_safe)
        return norm_detect_utf8(first, last, norm_bit_nfc, norm_bound_nfc);
    return norm_detect_css_utf8(first, last, norm_bit_nfc, norm_bound_nfc);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_norm_is_nfd_utf8(it_in_utf8 first, it_end_utf8 last, bool check_stream_safe)
{
    if (!check_stream_safe)
        return norm_detect_utf8(first, last, norm_bit_nfd, norm_bound_nfd);
    return norm_detect_css_utf8(first, last, norm_bit_nfd, norm_bound_nfd);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_norm_is_nfkc_utf8(it_in_utf8 first, it_end_utf8 last, bool check_stream_safe)
{
    if (!check_stream_safe)
        return norm_detect_utf8(first, last, norm_bit_nfkc, norm_bound_nfkc);
    return norm_detect_css_utf8(first, last, norm_bit_nfkc, norm_bound_nfkc);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_norm_is_nfkd_utf8(it_in_utf8 first, it_end_utf8 last, bool check_stream_safe)
{
    if (!check_stream_safe)
        return norm_detect_utf8(first, last, norm_bit_nfkd, norm_bound_nfkd);
    return norm_detect_css_utf8(first, last, norm_bit_nfkd, norm_bound_nfkd);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

// BEGIN: GENERATED UTF-16 FUNCTIONS
#ifndef UNI_ALGO_DOC_GENERATED_UTF16

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_norm_to_nfc_utf16(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    do
    {
        while (src != last)
        {
            src = iter_utf16(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfc)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf16(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue; // Fast loop ends here
                }
            }
            if (norm_decomp_nfc(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0) // The end of the data if we are here without last_qc
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);
        norm_comp(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
        {
            if (buffer.ccc[i] != 255)
                dst = codepoint_to_utf16(buffer.cps[i], dst);
        }

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_norm_to_nfd_utf16(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    do
    {
        while (src != last)
        {
            src = iter_utf16(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfd)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf16(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfd(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0)
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
            dst = codepoint_to_utf16(buffer.cps[i], dst);

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_norm_to_nfkc_utf16(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    do
    {
        while (src != last)
        {
            src = iter_utf16(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfkc)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf16(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfkc(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0)
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);
        norm_comp(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
        {
            if (buffer.ccc[i] != 255)
                dst = codepoint_to_utf16(buffer.cps[i], dst);
        }

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_norm_to_nfkd_utf16(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    do
    {
        while (src != last)
        {
            src = iter_utf16(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfkd)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf16(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfkd(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0)
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
            dst = codepoint_to_utf16(buffer.cps[i], dst);

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_PROP

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_norm_to_unaccent_utf16(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept c = 0; // tag_can_be_uninitialized

    struct norm_buffer buffer = {{0}, {0}}; // tag_can_be_uninitialized
    struct norm_multi m = {0, 0, 0}; // tag_must_be_initialized

    do
    {
        while (src != last)
        {
            src = iter_utf16(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns(c, &m.count_ns, norm_bit_nfd)))
            {
                if (uaix_likely(m.size == 1))
                {
                    dst = codepoint_to_utf16(buffer.cps[0], dst);
                    m.size = 0;
                }
                if (uaix_likely(m.size == 0))
                {
                    buffer.cps[0] = c;
                    buffer.ccc[0] = 0;
                    m.size = 1;
                    continue;
                }
            }
            if (norm_decomp_unaccent(c, &buffer, &m))
                break;
        }

        if (m.last_qc == 0)
            m.last_qc = m.size;

        norm_order(&buffer, m.last_qc);
        norm_comp(&buffer, m.last_qc);

        for (size_t i = 0; i < m.last_qc; ++i)
        {
            if (buffer.ccc[i] != 255)
                dst = codepoint_to_utf16(buffer.cps[i], dst);
        }

        norm_proc_buffer(&buffer, &m);
    }
    while (m.size > 0);

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_PROP

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int norm_detect_utf16(it_in_utf16 first, it_end_utf16 last, type_codept norm_bit, type_codept norm_bound)
{
    it_in_utf16 src = first;

    type_codept c = 0; // tag_can_be_uninitialized
    unsigned char last_ccc = 0; // tag_must_be_initialized

    while (src != last)
    {
        src = iter_utf16(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (c < norm_bound)
            last_ccc = 0;
        else
        {
            const type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
            if (!stages_ccc_qc_detect(ccc_qc, norm_bit))
                return norm_is_no_or_maybe;
            if (!stages_ccc_qc_order(ccc_qc, &last_ccc))
                return impl_norm_is_not_canonical_order;
        }
    }
    return impl_norm_is_yes;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int norm_detect_css_utf16(it_in_utf16 first, it_end_utf16 last, type_codept norm_bit, type_codept norm_bound)
{
    it_in_utf16 src = first;

    type_codept c = 0; // tag_can_be_uninitialized
    unsigned char last_ccc = 0; // tag_must_be_initialized
    size_t count_ns = 0; // tag_must_be_initialized

    while (src != last)
    {
        src = iter_utf16(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (c < norm_bound)
        {
            last_ccc = 0;
            count_ns = 0;
        }
        else
        {
            const type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
            if (!stages_ccc_qc_detect(ccc_qc, norm_bit))
                return norm_is_no_or_maybe;
            if (!stages_ccc_qc_order(ccc_qc, &last_ccc))
                return impl_norm_is_not_canonical_order;
            if (!stages_ccc_qc_ns(ccc_qc, &count_ns))
                return impl_norm_is_not_stream_safe;
        }
    }
    return impl_norm_is_yes;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_norm_is_nfc_utf16(it_in_utf16 first, it_end_utf16 last, bool check_stream_safe)
{
    if (!check_stream_safe)
        return norm_detect_utf16(first, last, norm_bit_nfc, norm_bound_nfc);
    return norm_detect_css_utf16(first, last, norm_bit_nfc, norm_bound_nfc);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_norm_is_nfd_utf16(it_in_utf16 first, it_end_utf16 last, bool check_stream_safe)
{
    if (!check_stream_safe)
        return norm_detect_utf16(first, last, norm_bit_nfd, norm_bound_nfd);
    return norm_detect_css_utf16(first, last, norm_bit_nfd, norm_bound_nfd);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_norm_is_nfkc_utf16(it_in_utf16 first, it_end_utf16 last, bool check_stream_safe)
{
    if (!check_stream_safe)
        return norm_detect_utf16(first, last, norm_bit_nfkc, norm_bound_nfkc);
    return norm_detect_css_utf16(first, last, norm_bit_nfkc, norm_bound_nfkc);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_norm_is_nfkd_utf16(it_in_utf16 first, it_end_utf16 last, bool check_stream_safe)
{
    if (!check_stream_safe)
        return norm_detect_utf16(first, last, norm_bit_nfkd, norm_bound_nfkd);
    return norm_detect_css_utf16(first, last, norm_bit_nfkd, norm_bound_nfkd);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // UNI_ALGO_DOC_GENERATED_UTF16
// END: GENERATED UTF-16 FUNCTIONS

// Functions for normalization iterators that use the same algorithms

struct impl_norm_iter_state
{
    size_t pos; // Iterator position

    struct norm_multi m;
    struct norm_buffer buffer;
};

uaix_inline void impl_norm_iter_state_reset(struct impl_norm_iter_state* const s)
{
    s->pos = 0;

    s->m.size = 0;
    s->m.last_qc = 0;
    s->m.count_ns = 0;
}

uaix_always_inline
uaix_static bool inline_norm_iter_ready(struct impl_norm_iter_state* const s)
{
    return s->pos == 0 ? false : true;
}

uaix_always_inline
uaix_static bool norm_state_fast_1(struct impl_norm_iter_state* const s, type_codept c)
{
    s->pos = 0;

    s->m.size = 2;
    s->m.last_qc = 1;

    s->buffer.cps[1] = c;
    s->buffer.ccc[1] = 0;

    return true;
}

uaix_always_inline
uaix_static bool norm_state_fast_0(struct impl_norm_iter_state* const s, type_codept c)
{
    s->m.size = 1;

    s->buffer.cps[0] = c;
    s->buffer.ccc[0] = 0;

    return false;
}

uaix_always_inline
uaix_static bool inline_norm_iter_nfc(struct impl_norm_iter_state* const s, type_codept c)
{
    // Note that we cannot implement a fast loop inside the normalization iterators
    // but we can use the same idea to make them a bit faster.

    c = (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF)) ? c : 0xFFFD;

    if (stages_qc_yes_ns(c, &s->m.count_ns, norm_bit_nfc))
    {
        if (s->m.size == 1)
            return norm_state_fast_1(s, c);
        if (s->m.size == 0)
            return norm_state_fast_0(s, c);
    }
    if (!norm_decomp_nfc(c, &s->buffer, &s->m))
        return false;

    return true;
}

uaix_always_inline
uaix_static bool inline_norm_iter_nfd(struct impl_norm_iter_state* const s, type_codept c)
{
    c = (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF)) ? c : 0xFFFD;

    if (stages_qc_yes_ns(c, &s->m.count_ns, norm_bit_nfd))
    {
        if (s->m.size == 1)
            return norm_state_fast_1(s, c);
        if (s->m.size == 0)
            return norm_state_fast_0(s, c);
    }
    if (!norm_decomp_nfd(c, &s->buffer, &s->m))
        return false;

    return true;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool inline_norm_iter_nfkc(struct impl_norm_iter_state* const s, type_codept c)
{
    c = (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF)) ? c : 0xFFFD;

    if (stages_qc_yes_ns(c, &s->m.count_ns, norm_bit_nfkc))
    {
        if (s->m.size == 1)
            return norm_state_fast_1(s, c);
        if (s->m.size == 0)
            return norm_state_fast_0(s, c);
    }
    if (!norm_decomp_nfkc(c, &s->buffer, &s->m))
        return false;

    return true;
}

uaix_always_inline
uaix_static bool inline_norm_iter_nfkd(struct impl_norm_iter_state* const s, type_codept c)
{
    c = (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF)) ? c : 0xFFFD;

    if (stages_qc_yes_ns(c, &s->m.count_ns, norm_bit_nfkd))
    {
        if (s->m.size == 1)
            return norm_state_fast_1(s, c);
        if (s->m.size == 0)
            return norm_state_fast_0(s, c);
    }
    if (!norm_decomp_nfkd(c, &s->buffer, &s->m))
        return false;

    return true;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool inline_norm_iter_next_comp(struct impl_norm_iter_state* const s, type_codept* const codepoint)
{
    // The function must be used together with inline_norm_iter_nfc or inline_norm_iter_nfkc

    if (s->m.size == 0) // The end of the data
        return false;

    if (s->m.last_qc == 0) // This is the last buffer
        s->m.last_qc = s->m.size;

    // Reorder and compose the buffer if this is the first iteration on this buffer
    if (s->pos == 0)
    {
        norm_order(&s->buffer, s->m.last_qc);
        norm_comp(&s->buffer, s->m.last_qc);
    }

    // Skip all CCC=255 and get the next code point
    bool found = false;
    for (; s->pos < s->m.last_qc; ++s->pos)
    {
        if (s->buffer.ccc[s->pos] != 255)
        {
            if (found) break;
            *codepoint = s->buffer.cps[s->pos];
            found = true;
        }
    }

    // This is the last iteration on this buffer, request more data
    if (s->pos == s->m.last_qc)
    {
        norm_proc_buffer(&s->buffer, &s->m);
        s->pos = 0;
    }

    return true;
}

uaix_always_inline
uaix_static bool inline_norm_iter_next_decomp(struct impl_norm_iter_state* const s, type_codept* const codepoint)
{
    // The function must be used together with inline_norm_iter_nfd or inline_norm_iter_nfkd

    if (s->m.size == 0) // The end of the data
        return false;

    if (s->m.last_qc == 0) // This is the last buffer
        s->m.last_qc = s->m.size;

    // Reorder the buffer if this is the first iteration on this buffer
    if (s->pos == 0)
        norm_order(&s->buffer, s->m.last_qc);

    // Get the next code point
    if (s->pos < s->m.last_qc)
    {
        *codepoint = s->buffer.cps[s->pos];
        ++s->pos;
    }

    // This is the last iteration on this buffer, request more data
    if (s->pos == s->m.last_qc)
    {
        norm_proc_buffer(&s->buffer, &s->m);
        s->pos = 0;
    }

    return true;
}

// Expose properties for a wrapper
// Must always be at the end of the file

uaix_always_inline
uaix_static type_codept impl_norm_get_prop(type_codept c)
{
    // Treat all invalid as replacement character (U+FFFD)
    if (c > 0x10FFFF)
        c = 0xFFFD;

    return stages(c, stage1_ccc_qc, stage2_ccc_qc);
}

uaix_always_inline
uaix_static unsigned char impl_norm_get_ccc_prop(type_codept prop)
{
    // The Unicode Standard: UnicodeData.txt -> Canonical_Combining_Class
    return (unsigned char)(prop & 0xFF);
}

uaix_always_inline
uaix_static bool impl_norm_is_nfc_qc_yes_prop(type_codept prop)
{
    // The Unicode Standard: DerivedNormalizationProps.txt -> NFC_Quick_Check=Yes
    return (prop & (type_codept)1 << norm_bit_nfc) ? false : true;
}

uaix_always_inline
uaix_static bool impl_norm_is_nfd_qc_yes_prop(type_codept prop)
{
    // The Unicode Standard: DerivedNormalizationProps.txt -> NFD_Quick_Check=Yes
    return (prop & (type_codept)1 << norm_bit_nfd) ? false : true;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool impl_norm_is_nfkc_qc_yes_prop(type_codept prop)
{
    // The Unicode Standard: DerivedNormalizationProps.txt -> NFKC_Quick_Check=Yes
    return (prop & (type_codept)1 << norm_bit_nfkc) ? false : true;
}

uaix_always_inline
uaix_static bool impl_norm_is_nfkd_qc_yes_prop(type_codept prop)
{
    // The Unicode Standard: DerivedNormalizationProps.txt -> NFKD_Quick_Check=Yes
    return (prop & (type_codept)1 << norm_bit_nfkd) ? false : true;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static type_codept norm_to_comp_hangul(type_codept c1, type_codept c2)
{
    // Simplified version of norm_comp_hangul

    const type_codept SBase = 0xAC00;
    const type_codept LBase = 0x1100;
    const type_codept VBase = 0x1161;
    const type_codept TBase = 0x11A7;
    const type_codept LCount = 19;
    const type_codept VCount = 21;
    const type_codept TCount = 28;
    const type_codept SCount = 11172;

    if (c1 >= LBase && c1 < LBase + LCount) // L+V
    {
        if (c2 >= VBase && c2 < VBase + VCount)
        {
            const type_codept LIndex = c1 - LBase;
            const type_codept VIndex = c2 - VBase;

            return (SBase + (LIndex * VCount + VIndex) * TCount);
        }
    }
    else if (c1 >= SBase && c1 < SBase + SCount && ((c1 - SBase) % TCount) == 0) // LV+T
    {
        if (c2 > TBase && c2 < TBase + TCount)
            return (c1 + (c2 - TBase));
    }
    return 0;
}

#if 0
uaix_always_inline
uaix_static type_codept impl_norm_to_compose_hangul(type_codept c1, type_codept c2)
{
    if (c1 <= 0x10FFFF && c2 <= 0x10FFFF)
    {
        type_codept c = norm_to_comp_hangul(c1, c2);
        return (c != 0) ? c : 0xFFFD;
    }
    return 0xFFFD;
}
#endif

uaix_always_inline
uaix_static type_codept impl_norm_to_compose(type_codept c1, type_codept c2)
{
    if (c1 <= 0x10FFFF && c2 <= 0x10FFFF)
    {
        type_codept c = norm_to_comp_hangul(c1, c2);
        if (c != 0)
            return c;

        c = stages_comp(c1, c2);
        return (c != 0) ? c : 0xFFFD;
    }
    return 0xFFFD;
}

#ifdef __cplusplus
template<typename it_out_utf32>
#endif
uaix_always_inline_tmpl
uaix_static size_t norm_to_decomp_hangul(type_codept c, it_out_utf32 dst)
{
    // Simplified version of norm_decomp_hangul

    const type_codept SBase = 0xAC00;
    const type_codept LBase = 0x1100;
    const type_codept VBase = 0x1161;
    const type_codept TBase = 0x11A7;
    const type_codept TCount = 28;
    const type_codept NCount = 588;
    const type_codept SCount = 11172;

    if (c >= SBase && c < SBase + SCount)
    {
        const type_codept SIndex = c - SBase;
        const type_codept LPart = LBase + SIndex / NCount;
        const type_codept VPart = VBase + (SIndex % NCount) / TCount;
        const type_codept TPart = TBase + SIndex % TCount;

        *dst++ = (type_char32)LPart;
        *dst++ = (type_char32)VPart;

        if (TPart != TBase)
        {
            *dst++ = (type_char32)TPart;
            return 3;
        }
        return 2;
    }
    return 0;
}

#ifdef __cplusplus
template<typename it_out_utf32>
#endif
uaix_always_inline_tmpl
uaix_static size_t impl_norm_to_decompose_hangul(type_codept c, it_out_utf32 dst)
{
    if (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF))
    {
        const size_t hangul = norm_to_decomp_hangul(c, dst);
        if (hangul)
            return hangul;

        *dst = (type_char32)c;
    }
    else
        *dst = 0xFFFD;

    return 1;
}

#ifdef __cplusplus
template<typename it_out_utf32>
#endif
uaix_always_inline_tmpl
uaix_static size_t impl_norm_to_decompose(type_codept c, it_out_utf32 dst)
{
    if (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF))
    {
        const size_t hangul = norm_to_decomp_hangul(c, dst);
        if (hangul)
            return hangul;

        const size_t offset = stages_decomp_nfd(c);
        if (offset)
        {
            const size_t number = stages_decomp_nfd_number(offset);
            for (size_t i = 0; i < number; ++i)
                *dst++ = (type_char32)stages_decomp_nfd_cp(offset, i);

            return number;
        }

        *dst = (type_char32)c;
    }
    else
        *dst = 0xFFFD;

    return 1;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_out_utf32>
#endif
uaix_always_inline_tmpl
uaix_static size_t impl_norm_to_decompose_compat(type_codept c, it_out_utf32 dst)
{
    if (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF))
    {
        const size_t hangul = norm_to_decomp_hangul(c, dst);
        if (hangul)
            return hangul;

        const size_t offset = stages_decomp_nfkd(c);
        if (offset)
        {
            const size_t number = stages_decomp_nfkd_number(offset);
            for (size_t i = 0; i < number; ++i)
                *dst++ = (type_char32)stages_decomp_nfkd_cp(offset, i);

            return number;
        }

        *dst = (type_char32)c;
    }
    else
        *dst = 0xFFFD;

    return 1;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_IMPL_NORM_H_UAIH

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
