/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef IMPL_NORM_H_UAIX
#define IMPL_NORM_H_UAIX

#include <uni/impl/iterator.h>

#include <uni/internal/defines.h>
#include <uni/internal/stages.h>

#ifndef UNI_ALGO_STATIC_DATA
#include <uni/impl/norm_data_extern.h>
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// The buffer must not be less than 4+30+18=52
// 4 is 4 starters, check norm_decomp_return why 4
// 30 is the max number of non-starters
// 18 is max decomposition (we use the end of the buffer to store the next decomposed code point)
// Use 70 just because it means we need 70*4+70=260 bytes on stack for any normalization
#ifdef __cplusplus
uaix_const size_t norm_buffer_size = 70;
#else
#define norm_buffer_size 70
#endif
uaix_const size_t norm_buffer_max_non_starters = 30; // This is in The Unicode Standard and must never be changed
uaix_const size_t norm_max_decomposition = 18; // Checked in generator

uaix_const int impl_norm_is_yes = 0;
uaix_const int impl_norm_is_ill_formed = 8;
//uaix_const int impl_norm_is_not_stream_safe = 16; // Reserved
//uaix_const int impl_norm_is_no = 1; // Reserved
//uaix_const int impl_norm_is_maybe = 2; // Reserved
uaix_const int norm_is_no_or_maybe = 3; // Can be changed in the future. Must never be used in a wrapper.

// http://www.unicode.org/faq/normalization.html#12
// The length of a destination (result) string must be premultiplied with one of these
// Example: destination_length = source_length * impl_x_function_name
uaix_const size_t impl_x_utf8_nfc = 3;
uaix_const size_t impl_x_utf8_nfd = 3;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
uaix_const size_t impl_x_utf8_nfkc = 11;
uaix_const size_t impl_x_utf8_nfkd = 11;
#endif
uaix_const size_t impl_x_utf16_nfc = 3;
uaix_const size_t impl_x_utf16_nfd = 4;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
uaix_const size_t impl_x_utf16_nfkc = 18;
uaix_const size_t impl_x_utf16_nfkd = 18;
#endif
#ifndef UNI_ALGO_DISABLE_UNACCENT
uaix_const size_t impl_x_utf8_unaccent = 3;
uaix_const size_t impl_x_utf16_unaccent = 3;
#endif

/* TODO: from yesterdays me to tomorrows me.
 * Refactoring merge:
 * stages_decomp_nfd + stages_decomp_nfkd -> stages_decomp[_offset?] (use bool in if, inlined anyway)
 * stages_decomp_nfd_number + stages_decomp_nfkd_number -> stages_decomp_number (use the same)
 * stages_decomp_nfkd_cp + stages_decomp_nfkd_cp -> stages_decomp_cp (use the same)
 * stages_qc_yes_ns_nfc + stages_qc_yes_ns_nfd + stages_qc_yes_ns_nfkc + stages_qc_yes_ns_nfkd -> stages_qc_yes_ns
 * stages_qc_yes_nfc + stages_qc_yes_nfd + stages_qc_yes_nfkc + stages_qc_yes_nfkd -> stages_qc_yes
 * stages_qc_yes_is_nfc + stages_qc_yes_is_nfd + stages_qc_yes_is_nfkc + stages_qc_yes_is_nfkd -> stages_qc_yes_is
 * norm_decomp_nfc + norm_decomp_nfkc -> norm_decomp_c?
 * norm_decomp_nfd + norm_decomp_nfkd -> norm_decomp_d?
 * Add const variables norm_bound_nf[k][cd] for lower bounds for stages_qc_yes_is.
 * Then merge algorithm functions, always inline them and wrap with the functions with old names.
 * Everything is prepared for this. And it must be done in 1 step just use this checklist.
 * If done properly then must be the same code, the same performance, check to be sure and then recheck everything.
 * This should be the last step of the refactoring probably.
 * I'm done with normalization for now it just kills me inside.
 * ---
 * Gee thanks me, like I even remember what all of this means.
 * I just pretend this todo doesn't exist for now. The refatoring will make some things uglier.
 * ---
 * Some things changed already so the plan won't work probably.
 * But I still leave the TODO some good ideas still here.
 * ---
 * Stop talking to yourself already and rewrite this crap.
 * ---
 * NYOOO
 * ---
 * Only normalization detection functions can be refactored like this at this point.
 */

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
    // Note that we use two-dimensional array here as recommended in
    // 14.1.2 Optimizing Tables for NFC Composition
    // https://unicode.org/reports/tr15/#Optimization_Strategies

    size_t i1 = stages(c1, stage1_comp_cp1, stage2_comp_cp1);
    if (i1)
    {
        size_t i2 = stages(c2, stage1_comp_cp2, stage2_comp_cp2);
        return stage3_comp[i1][i2];
    }

    return 0;
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

    type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
    return (unsigned char)(ccc_qc & 0xFF);
}

uaix_const type_codept norm_bit_nfc  = 8;
uaix_const type_codept norm_bit_nfd  = 9;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
uaix_const type_codept norm_bit_nfkc = 10;
uaix_const type_codept norm_bit_nfkd = 11;
#endif

uaix_always_inline
uaix_static bool stages_qc_yes_impl(type_codept ccc_qc, type_codept bit)
{
    // Canonical_Combining_Class=0 && Quick_Check=Yes (!Quick_Check=No && !Quick_Check=Maybe)
    return (ccc_qc & 0xFF) == 0 && !(ccc_qc & (type_codept)1 << bit);
}

uaix_always_inline
uaix_static bool stages_qc_yes_is_impl(type_codept ccc_qc, type_codept bit)
{
    // Quick_Check=Yes (!Quick_Check=No && !Quick_Check=Maybe)
    return !(ccc_qc & (type_codept)1 << bit);
}

uaix_always_inline
uaix_static bool stages_qc_yes_ns_impl(type_codept ccc_qc, size_t* count_ns)
{
    // https://unicode.org/reports/tr15/#Stream_Safe_Text_Format

    if (ccc_qc >> 14) // Initial non-starters in NFKD
    {
        *count_ns += ccc_qc >> 14;
        if (*count_ns > norm_buffer_max_non_starters)
            return true;
    }
    else
    {
        *count_ns = ccc_qc >> 12; // Trailing non-starters in NFKD
    }
    return false;
}

uaix_always_inline
uaix_static bool stages_qc_yes_ns_nfc(type_codept c, size_t* count_ns)
{
    /* Note that we always use NFKD lower bound in these
     * functions because we need to count initial/trailing
     * non-starters in NFKD for Stream-Safe Text Process.
     * This disallow us to use NFC lower bound for example,
     * that is much higher, to achive a better performance,
     * but it cannot be done other way.
     * We still can use real lower bounds in
     * normalization detection functions below.
     * Note: lower bound means everything below that has
     * Quick_Check=Yes and Canonical_Combining_Class=0.
     */

    if (c >= 0x00A0) // NFKD lower bound
    {
        type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (stages_qc_yes_ns_impl(ccc_qc, count_ns))
            return false;
        return stages_qc_yes_impl(ccc_qc, norm_bit_nfc);
    }
    *count_ns = 0;
    return true;
}

uaix_always_inline
uaix_static bool stages_qc_yes_ns_nfd(type_codept c, size_t* count_ns)
{
    if (c >= 0x00A0) // NFKD lower bound
    {
        type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (stages_qc_yes_ns_impl(ccc_qc, count_ns))
            return false;
        return stages_qc_yes_impl(ccc_qc, norm_bit_nfd);
    }
    *count_ns = 0;
    return true;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool stages_qc_yes_ns_nfkc(type_codept c, size_t* count_ns)
{
    if (c >= 0x00A0) // NFKD lower bound
    {
        type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (stages_qc_yes_ns_impl(ccc_qc, count_ns))
            return false;
        return stages_qc_yes_impl(ccc_qc, norm_bit_nfkc);
    }
    *count_ns = 0;
    return true;
}

uaix_always_inline
uaix_static bool stages_qc_yes_ns_nfkd(type_codept c, size_t* count_ns)
{
    if (c >= 0x00A0) // NFKD lower bound
    {
        type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (stages_qc_yes_ns_impl(ccc_qc, count_ns))
            return false;
        return stages_qc_yes_impl(ccc_qc, norm_bit_nfkd);
    }
    *count_ns = 0;
    return true;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool stages_qc_yes_nfc(type_codept c)
{
    type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
    return stages_qc_yes_impl(ccc_qc, norm_bit_nfc);
}

uaix_always_inline
uaix_static bool stages_qc_yes_nfd(type_codept c)
{
    type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
    return stages_qc_yes_impl(ccc_qc, norm_bit_nfd);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool stages_qc_yes_nfkc(type_codept c)
{
    type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
    return stages_qc_yes_impl(ccc_qc, norm_bit_nfkc);
}

uaix_always_inline
uaix_static bool stages_qc_yes_nfkd(type_codept c)
{
    type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
    return stages_qc_yes_impl(ccc_qc, norm_bit_nfkd);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool stages_qc_yes_is_ccc_impl(type_codept ccc_qc, unsigned char* last_ccc)
{
    // https://unicode.org/reports/tr15/#Detecting_Normalization_Forms
    // Note: "if (Character.isSupplementaryCodePoint(ch)) ++i;"
    // check is irrelevant it's because Java uses UTF-16 internally.

    unsigned char ccc = (ccc_qc & 0xFF);

    if (*last_ccc > ccc && ccc != 0)
    {
        *last_ccc = ccc;
        return false;
    }
    *last_ccc = ccc;
    return true;
}

uaix_always_inline
uaix_static bool stages_qc_yes_is_nfc(type_codept c, unsigned char* last_ccc)
{
    if (c >= 0x0300) // NFC lower bound
    {
        type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (!stages_qc_yes_is_ccc_impl(ccc_qc, last_ccc))
            return false;
        return stages_qc_yes_is_impl(ccc_qc, norm_bit_nfc);
    }
    *last_ccc = 0;
    return true;
}

uaix_always_inline
uaix_static bool stages_qc_yes_is_nfd(type_codept c, unsigned char* last_ccc)
{
    if (c >= 0x00C0) // NFD lower bound
    {
        type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (!stages_qc_yes_is_ccc_impl(ccc_qc, last_ccc))
            return false;
        return stages_qc_yes_is_impl(ccc_qc, norm_bit_nfd);
    }
    *last_ccc = 0;
    return true;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool stages_qc_yes_is_nfkc(type_codept c, unsigned char* last_ccc)
{
    if (c >= 0x00A0) // NFKC lower bound
    {
        type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (!stages_qc_yes_is_ccc_impl(ccc_qc, last_ccc))
            return false;
        return stages_qc_yes_is_impl(ccc_qc, norm_bit_nfkc);
    }
    *last_ccc = 0;
    return true;
}

uaix_always_inline
uaix_static bool stages_qc_yes_is_nfkd(type_codept c, unsigned char* last_ccc)
{
    if (c >= 0x00A0) // NFKD lower bound
    {
        type_codept ccc_qc = stages(c, stage1_ccc_qc, stage2_ccc_qc);
        if (!stages_qc_yes_is_ccc_impl(ccc_qc, last_ccc))
            return false;
        return stages_qc_yes_is_impl(ccc_qc, norm_bit_nfkd);
    }
    *last_ccc = 0;
    return true;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static void norm_order(type_codept buffer[], unsigned char buffer_ccc[], size_t size)
{
    if (size > 0)
    {
        size_t last = size - 1;

        while (last > 0)
        {
            size_t curr = 0;
            size_t curr_ccc = 0;
            size_t new_last = 0;

            for (; curr < last; ++curr, ++curr_ccc)
            {
                size_t next = curr + 1;
                size_t next_ccc = curr_ccc + 1;

                if (buffer_ccc[next_ccc] > 0 && buffer_ccc[next_ccc] < buffer_ccc[curr_ccc])
                {
                    // Swap

                    type_codept   temp     = buffer[curr];
                    unsigned char temp_ccc = buffer_ccc[curr_ccc];

                    buffer[curr] = buffer[next];
                    buffer[next] = temp;

                    buffer_ccc[curr_ccc] = buffer_ccc[next_ccc];
                    buffer_ccc[next_ccc] = temp_ccc;

                    new_last = curr;
                }
            }
            last = new_last;
        }
    }
}

uaix_always_inline
uaix_static size_t norm_decomp_hangul(type_codept c, size_t i, type_codept buffer[], unsigned char buffer_ccc[])
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
        type_codept SIndex = c - SBase;
        type_codept LPart = LBase + SIndex / NCount;
        type_codept VPart = VBase + (SIndex % NCount) / TCount;
        type_codept TPart = TBase + SIndex % TCount;

        buffer[i] = LPart;
        buffer_ccc[i] = 0;

        buffer[i + 1] = VPart;
        buffer_ccc[i + 1] = 0;

        if (TPart != TBase)
        {
            buffer[i + 2] = TPart;
            buffer_ccc[i + 2] = 0;

            return 3;
        }

        return 2;
    }

    return 0;
}

uaix_always_inline
uaix_static size_t norm_comp_hangul(size_t i, type_codept buffer[], unsigned char buffer_ccc[], size_t size)
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

    type_codept c1 = buffer[i];

    if (c1 >= LBase && c1 < LBase + LCount) // L+V+T
    {
        if (i + 1 < size)
        {
            type_codept c2 = buffer[i + 1];

            if (c2 >= VBase && c2 < VBase + VCount)
            {
                type_codept LIndex = c1 - LBase;
                type_codept VIndex = c2 - VBase;

                buffer[i] = SBase + (LIndex * VCount + VIndex) * TCount;
                buffer_ccc[i + 1] = 255;

                if (i + 2 < size)
                {
                    type_codept c3 = buffer[i + 2];

                    if (c3 > TBase && c3 < TBase + TCount)
                    {
                        buffer[i] += c3 - TBase;
                        buffer_ccc[i + 2] = 255;

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
            type_codept c2 = buffer[i + 1];

            if (c2 > TBase && c2 < TBase + TCount)
            {
                buffer[i] += c2 - TBase;
                buffer_ccc[i + 1] = 255;

                return 1;
            }

        }
    }

    return 0;
}

uaix_always_inline
uaix_static void norm_comp(type_codept buffer[], unsigned char buffer_ccc[], size_t size)
{
    // Note: CCC=255 is used to mark composed code points

    if (size > 0)
    {
        size_t starter = 0;
        for (size_t i = 0; i < size - 1; ++i)
        {
            size_t hangul = norm_comp_hangul(i, buffer, buffer_ccc, size);
            if (hangul)
            {
                i += hangul;
                continue;
            }

            if (buffer_ccc[i] == 0)
                starter = i;

            // If there is a starter and non-blocked non-starter
            if (starter == i || !(buffer_ccc[i] != 255 && buffer_ccc[i] >= buffer_ccc[i + 1]))
            {
                type_codept c = stages_comp(buffer[starter], buffer[i + 1]);
                if (c != 0)
                {
                    buffer[starter] = c;
                    buffer_ccc[i + 1] = 255;
                }
            }
        }
    }
}

uaix_always_inline
uaix_static bool norm_decomp_return(unsigned char buffer_ccc[], size_t* size, size_t* last_qc)
{
    // Flush the buffer by Quick_Check=Yes boundary.
    if (*last_qc > 0)
        return true;

    // More than 4 starters cannot compose together so flush the buffer.
    // In other words flush the buffer by CCC=0 boundary at this point.
    // Note 1: actually 3 starters (Hanguls L+V+T and 0CCB) but use 4 (max composition) just in case.
    // Note 2: this check only matters for NFC/NFKC because in NFD/NFKD such cases are not possible.
    if (*size > 4 && buffer_ccc[*size - 1] == 0)
    {
        *last_qc = *size - 1;
        return true;
    }

    // This must never happen so flush the buffer and stop to make it noticeable in tests.
    // 18 is max decomposition so the next turn can exhaust the buffer.
    if (*size + norm_max_decomposition > norm_buffer_size)
    {
#ifdef UNI_ALGO_TEST_ASSERT
        uaix_assert(false);
#endif
        *last_qc = *size;
        return true;
    }

    return false;
}

uaix_always_inline
uaix_static void norm_decomp_count_ns(type_codept buffer[], unsigned char buffer_ccc[],
                                      size_t* size, size_t* last_qc, size_t* count_ns)
{
    // Insert U+034F COMBINING GRAPHEME JOINER (CGJ) if there is more than 30 non-starters

    if (*count_ns > norm_buffer_max_non_starters)
    {
        *count_ns -= norm_buffer_max_non_starters;
        buffer[*size] = 0x034F;
        buffer_ccc[*size] = 0;
        *last_qc = *size;
        *size += 1;
    }
}

uaix_always_inline
uaix_static bool norm_decomp_nfc(type_codept c, type_codept buffer[], unsigned char buffer_ccc[],
                                 size_t* size, size_t* last_qc, size_t* count_ns)
{
    norm_decomp_count_ns(buffer, buffer_ccc, size, last_qc, count_ns);

    // Need to decompose the first code point when NFC/NFKC,
    // it can be still composed if we drop here from fast loop
    if (*size == 1)
    {
        size_t offset = stages_decomp_nfd(buffer[0]);
        if (offset)
        {
            *size = 0;
            size_t number = stages_decomp_nfd_number(offset);
            for (size_t i = 0; i < number; ++i, *size += 1)
            {
                buffer[*size] = stages_decomp_nfd_cp(offset, i);
                buffer_ccc[*size] = stages_ccc(buffer[*size]);
                if (stages_qc_yes_nfc(buffer[*size]))
                    *last_qc = *size;
            }
        }
    }

    // Do not decompose Hanguls like we do in norm_decomp_nfd, LV+T cases are handled in norm_comp

    // This additional quick check branch is almost irrelevant but in NFC/NFKC
    // in some cases we can stuck in slow loop a bit longer than needed and
    // we need the check to avoid it. This does not affect behaviour.
    // Test string: "\x005A\x0301\x0179\x0179\x0179\x0179\x0179"
    if (stages_qc_yes_nfc(c))
    {
        buffer[*size] = c;
        buffer_ccc[*size] = stages_ccc(c);
        *last_qc = *size;
        *size += 1;
    }
    else
    {
        size_t offset = stages_decomp_nfd(c);
        if (offset == 0)
        {
            buffer[*size] = c;
            buffer_ccc[*size] = stages_ccc(c);
            if (stages_qc_yes_nfc(c))
                *last_qc = *size;
            *size += 1;
        }
        else
        {
            size_t number = stages_decomp_nfd_number(offset);
            for (size_t i = 0; i < number; ++i, *size += 1)
            {
                buffer[*size] = stages_decomp_nfd_cp(offset, i);
                buffer_ccc[*size] = stages_ccc(buffer[*size]);
                if (stages_qc_yes_nfc(buffer[*size]))
                    *last_qc = *size;
            }
        }
    }

    return norm_decomp_return(buffer_ccc, size, last_qc);
}

uaix_always_inline
uaix_static bool norm_decomp_nfd(type_codept c, type_codept buffer[], unsigned char buffer_ccc[],
                                 size_t* size, size_t* last_qc, size_t* count_ns)
{
    // Almost the same as norm_decomp_nfc but decompose everything

    norm_decomp_count_ns(buffer, buffer_ccc, size, last_qc, count_ns);

    size_t hangul = norm_decomp_hangul(c, *size, buffer, buffer_ccc);
    if (hangul)
    {
        *last_qc = *size;
        *size += hangul;
    }
    else
    {
        size_t offset = stages_decomp_nfd(c);
        if (offset == 0)
        {
            buffer[*size] = c;
            buffer_ccc[*size] = stages_ccc(c);
            if (stages_qc_yes_nfd(c))
                *last_qc = *size;
            *size += 1;
        }
        else
        {
            size_t number = stages_decomp_nfd_number(offset);
            for (size_t i = 0; i < number; ++i, *size += 1)
            {
                buffer[*size] = stages_decomp_nfd_cp(offset, i);
                buffer_ccc[*size] = stages_ccc(buffer[*size]);
                if (stages_qc_yes_nfd(buffer[*size]))
                    *last_qc = *size;
            }
        }
    }

    return norm_decomp_return(buffer_ccc, size, last_qc);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool norm_decomp_nfkc(type_codept c, type_codept buffer[], unsigned char buffer_ccc[],
                                  size_t* size, size_t* last_qc, size_t* count_ns)
{
    // The same as norm_decomp_nfc but uses NFKC data

    norm_decomp_count_ns(buffer, buffer_ccc, size, last_qc, count_ns);

    if (*size == 1)
    {
        size_t offset = stages_decomp_nfkd(buffer[0]);
        if (offset)
        {
            *size = 0;
            size_t number = stages_decomp_nfkd_number(offset);
            for (size_t i = 0; i < number; ++i, *size += 1)
            {
                buffer[*size] = stages_decomp_nfkd_cp(offset, i);
                buffer_ccc[*size] = stages_ccc(buffer[*size]);
                if (stages_qc_yes_nfkc(buffer[*size]))
                    *last_qc = *size;
            }
        }
    }

    if (stages_qc_yes_nfkc(c))
    {
        buffer[*size] = c;
        buffer_ccc[*size] = stages_ccc(c);
        *last_qc = *size;
        *size += 1;
    }
    else
    {
        size_t offset = stages_decomp_nfkd(c);
        if (offset == 0)
        {
            buffer[*size] = c;
            buffer_ccc[*size] = stages_ccc(c);
            if (stages_qc_yes_nfkc(c))
                *last_qc = *size;
            *size += 1;
        }
        else
        {
            size_t number = stages_decomp_nfkd_number(offset);
            for (size_t i = 0; i < number; ++i, *size += 1)
            {
                buffer[*size] = stages_decomp_nfkd_cp(offset, i);
                buffer_ccc[*size] = stages_ccc(buffer[*size]);
                if (stages_qc_yes_nfkc(buffer[*size]))
                    *last_qc = *size;
            }
        }
    }

    return norm_decomp_return(buffer_ccc, size, last_qc);
}

uaix_always_inline
uaix_static bool norm_decomp_nfkd(type_codept c, type_codept buffer[], unsigned char buffer_ccc[],
                                  size_t* size, size_t* last_qc, size_t* count_ns)
{
    // The same as norm_decomp_nfd but uses NFKD data

    norm_decomp_count_ns(buffer, buffer_ccc, size, last_qc, count_ns);

    size_t hangul = norm_decomp_hangul(c, *size, buffer, buffer_ccc);
    if (hangul)
    {
        *last_qc = *size;
        *size += hangul;
    }
    else
    {
        size_t offset = stages_decomp_nfkd(c);
        if (offset == 0)
        {
            buffer[*size] = c;
            buffer_ccc[*size] = stages_ccc(c);
            if (stages_qc_yes_nfkd(c))
                *last_qc = *size;
            *size += 1;
        }
        else
        {
            size_t number = stages_decomp_nfkd_number(offset);
            for (size_t i = 0; i < number; ++i, *size += 1)
            {
                buffer[*size] = stages_decomp_nfkd_cp(offset, i);
                buffer_ccc[*size] = stages_ccc(buffer[*size]);
                if (stages_qc_yes_nfkd(buffer[*size]))
                    *last_qc = *size;
            }
        }
    }

    return norm_decomp_return(buffer_ccc, size, last_qc);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UNACCENT

uaix_always_inline
uaix_static bool norm_decomp_unaccent(type_codept c, type_codept buffer[], unsigned char buffer_ccc[],
                                      size_t* size, size_t* last_qc, size_t* count_ns)
{
    // Ignore Hanguls, decompose to NFD, skip Nonspacing Mark
    // The full algorithm is: NFD -> remove Nonspacing Mark -> NFC

    norm_decomp_count_ns(buffer, buffer_ccc, size, last_qc, count_ns);

    size_t offset = stages_decomp_nfd(c);
    if (offset == 0)
    {
        if (!stages(c, stage1_norm_mn, stage2_norm_mn))
        {
            buffer[*size] = c;
            buffer_ccc[*size] = stages_ccc(c);
            if (stages_qc_yes_nfd(c))
                *last_qc = *size;
            *size += 1;
        }
    }
    else
    {
        size_t number = stages_decomp_nfd_number(offset);
        for (size_t i = 0; i < number; ++i)
        {
            type_codept cp = stages_decomp_nfd_cp(offset, i);
            if (!stages(cp, stage1_norm_mn, stage2_norm_mn))
            {
                buffer[*size] = cp;
                buffer_ccc[*size] = stages_ccc(cp);
                if (stages_qc_yes_nfd(cp))
                    *last_qc = *size;
                *size += 1;
            }
        }
    }

    return norm_decomp_return(buffer_ccc, size, last_qc);
}

#endif // UNI_ALGO_DISABLE_UNACCENT

uaix_always_inline
uaix_static void norm_buffer(type_codept buffer[], unsigned char buffer_ccc[], size_t* size, size_t* last_qc)
{
    // Move the last decomposed code point that between last_qc and size to the start
    // last_qc is last Quick_Check=Yes mark

    for (size_t i = 0, j = *last_qc; j < *size; ++i, ++j)
    {
        buffer[i] = buffer[j];
        buffer_ccc[i] = buffer_ccc[j];
    }
    *size = *size - *last_qc;
    *last_qc = 0;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_utf8_nfc(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    /* while (src != last) is the fast loop, any slow down there reduces the performance drastically,
     * so everything moved outside this loop and we drop there if we actually need to do something.
     * Note that we always need to read 1 code point ahead in the algorithm that supports streams
     * so our fast loop is not that fast, probably it can be optimized a bit more but it's fine. Anyway
     * it's impossible to achieve the same speed of the fast loop as in an algorithm that doesn't support streams.
     * ifs are written like that to avoid unnecessary jumps so it gives the best performance.
     * __builtin_expect is used because it achieves a better performance in GCC.
     * TODO: questionable, this likely in while looks funny, should be retested with different GCC versions.
     * Other normalization functions are based on this function.
     */

    do
    {
        while (src != last)
        {
            src = utf8_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfc(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf8(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue; // Fast loop ends here
                }
            }
            if (norm_decomp_nfc(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0) // The end of the data if we are here without last_qc
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);
        norm_comp(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
        {
            if (buffer_ccc[i] != 255)
                dst = codepoint_to_utf8(buffer[i], dst);
        }

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_utf8_nfd(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    // The same as impl_utf8_nfc but without composition and uses decomposition data

    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    do
    {
        while (src != last)
        {
            src = utf8_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfd(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf8(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfd(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0)
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
            dst = codepoint_to_utf8(buffer[i], dst);

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_utf8_nfkc(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    // The same as impl_utf8_nfc but uses NFKC data

    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    do
    {
        while (src != last)
        {
            src = utf8_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfkc(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf8(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfkc(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0)
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);
        norm_comp(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
        {
            if (buffer_ccc[i] != 255)
                dst = codepoint_to_utf8(buffer[i], dst);
        }

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_utf8_nfkd(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    // The same as impl_utf8_nfd but uses NFKD data

    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    do
    {
        while (src != last)
        {
            src = utf8_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfkd(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf8(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfkd(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0)
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
            dst = codepoint_to_utf8(buffer[i], dst);

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UNACCENT

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf8>
#endif
uaix_static size_t impl_utf8_unaccent(it_in_utf8 first, it_end_utf8 last, it_out_utf8 result)
{
    it_in_utf8 src = first;
    it_out_utf8 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    // Algorithm: NFD -> remove Nonspacing Mark -> NFC

    do
    {
        while (src != last)
        {
            src = utf8_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfd(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf8(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue;
                }
            }
            if (norm_decomp_unaccent(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0)
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);
        norm_comp(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
        {
            if (buffer_ccc[i] != 255)
                dst = codepoint_to_utf8(buffer[i], dst);
        }

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_UNACCENT

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_utf8_is_nfc(it_in_utf8 first, it_end_utf8 last)
{
    it_in_utf8 src = first;

    unsigned char last_ccc = 0;
    type_codept c = 0;

    while (src != last)
    {
        src = utf8_iter(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (!stages_qc_yes_is_nfc(c, &last_ccc))
            return norm_is_no_or_maybe;
    }
    return impl_norm_is_yes;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_utf8_is_nfd(it_in_utf8 first, it_end_utf8 last)
{
    it_in_utf8 src = first;

    unsigned char last_ccc = 0;
    type_codept c = 0;

    while (src != last)
    {
        src = utf8_iter(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (!stages_qc_yes_is_nfd(c, &last_ccc))
            return norm_is_no_or_maybe;
    }
    return impl_norm_is_yes;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_utf8_is_nfkc(it_in_utf8 first, it_end_utf8 last)
{
    it_in_utf8 src = first;

    unsigned char last_ccc = 0;
    type_codept c = 0;

    while (src != last)
    {
        src = utf8_iter(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (!stages_qc_yes_is_nfkc(c, &last_ccc))
            return norm_is_no_or_maybe;
    }
    return impl_norm_is_yes;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
uaix_static int impl_utf8_is_nfkd(it_in_utf8 first, it_end_utf8 last)
{
    it_in_utf8 src = first;

    unsigned char last_ccc = 0;
    type_codept c = 0;

    while (src != last)
    {
        src = utf8_iter(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (!stages_qc_yes_is_nfkd(c, &last_ccc))
            return norm_is_no_or_maybe;
    }
    return impl_norm_is_yes;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

// BEGIN: GENERATED UTF-16 FUNCTIONS
#ifndef UNI_ALGO_DOC_GENERATED_UTF16

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_utf16_nfc(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    do
    {
        while (uaix_likely(src != last))
        {
            src = utf16_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfc(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf16(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue; // Fast loop ends here
                }
            }
            if (norm_decomp_nfc(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0) // The end of the data if we are here without last_qc
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);
        norm_comp(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
        {
            if (buffer_ccc[i] != 255)
                dst = codepoint_to_utf16(buffer[i], dst);
        }

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_utf16_nfd(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    do
    {
        while (src != last)
        {
            src = utf16_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfd(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf16(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfd(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0)
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
            dst = codepoint_to_utf16(buffer[i], dst);

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_utf16_nfkc(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    do
    {
        while (src != last)
        {
            src = utf16_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfkc(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf16(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfkc(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0)
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);
        norm_comp(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
        {
            if (buffer_ccc[i] != 255)
                dst = codepoint_to_utf16(buffer[i], dst);
        }

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_utf16_nfkd(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    do
    {
        while (src != last)
        {
            src = utf16_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfkd(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf16(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue;
                }
            }
            if (norm_decomp_nfkd(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0)
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
            dst = codepoint_to_utf16(buffer[i], dst);

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UNACCENT

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16, typename it_out_utf16>
#endif
uaix_static size_t impl_utf16_unaccent(it_in_utf16 first, it_end_utf16 last, it_out_utf16 result)
{
    it_in_utf16 src = first;
    it_out_utf16 dst = result;

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];

    size_t size = 0, last_qc = 0, count_ns = 0;
    type_codept c = 0;

    do
    {
        while (src != last)
        {
            src = utf16_iter(src, last, &c, iter_replacement);
            if (uaix_likely(stages_qc_yes_ns_nfd(c, &count_ns)))
            {
                if (uaix_likely(size == 1))
                {
                    dst = codepoint_to_utf16(buffer[0], dst);
                    size = 0;
                }
                if (uaix_likely(size == 0))
                {
                    buffer[0] = c;
                    buffer_ccc[0] = 0;
                    size = 1;
                    continue;
                }
            }
            if (norm_decomp_unaccent(c, buffer, buffer_ccc, &size, &last_qc, &count_ns))
                break;
        }

        if (last_qc == 0)
            last_qc = size;

        norm_order(buffer, buffer_ccc, last_qc);
        norm_comp(buffer, buffer_ccc, last_qc);

        for (size_t i = 0; i < last_qc; ++i)
        {
            if (buffer_ccc[i] != 255)
                dst = codepoint_to_utf16(buffer[i], dst);
        }

        norm_buffer(buffer, buffer_ccc, &size, &last_qc);
    }
    while (size > 0);

    return (size_t)(dst - result);
}

#endif // UNI_ALGO_DISABLE_UNACCENT

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_utf16_is_nfc(it_in_utf16 first, it_end_utf16 last)
{
    it_in_utf16 src = first;

    unsigned char last_ccc = 0;
    type_codept c = 0;

    while (src != last)
    {
        src = utf16_iter(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (!stages_qc_yes_is_nfc(c, &last_ccc))
            return norm_is_no_or_maybe;
    }
    return impl_norm_is_yes;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_utf16_is_nfd(it_in_utf16 first, it_end_utf16 last)
{
    it_in_utf16 src = first;

    unsigned char last_ccc = 0;
    type_codept c = 0;

    while (src != last)
    {
        src = utf16_iter(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (!stages_qc_yes_is_nfd(c, &last_ccc))
            return norm_is_no_or_maybe;
    }
    return impl_norm_is_yes;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_utf16_is_nfkc(it_in_utf16 first, it_end_utf16 last)
{
    it_in_utf16 src = first;

    unsigned char last_ccc = 0;
    type_codept c = 0;

    while (src != last)
    {
        src = utf16_iter(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (!stages_qc_yes_is_nfkc(c, &last_ccc))
            return norm_is_no_or_maybe;
    }
    return impl_norm_is_yes;
}

#ifdef __cplusplus
template<typename it_in_utf16, typename it_end_utf16>
#endif
uaix_static int impl_utf16_is_nfkd(it_in_utf16 first, it_end_utf16 last)
{
    it_in_utf16 src = first;

    unsigned char last_ccc = 0;
    type_codept c = 0;

    while (src != last)
    {
        src = utf16_iter(src, last, &c, iter_error);
        if (c == iter_error)
            return impl_norm_is_ill_formed;
        if (!stages_qc_yes_is_nfkd(c, &last_ccc))
            return norm_is_no_or_maybe;
    }
    return impl_norm_is_yes;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // UNI_ALGO_DOC_GENERATED_UTF16
// END: GENERATED UTF-16 FUNCTIONS

// Functions for normalization iterators that use the same algorithms

struct impl_norm_iter_state
{
    size_t size;
    size_t last_qc;
    size_t count_ns;

    size_t pos; // Iterator position

    type_codept buffer[norm_buffer_size];
    unsigned char buffer_ccc[norm_buffer_size];
};

uaix_inline void impl_norm_iter_state_reset(struct impl_norm_iter_state* s)
{
    s->size = 0;
    s->last_qc = 0;
    s->count_ns = 0;
    s->pos = 0;
}

uaix_always_inline
uaix_static bool unstable_norm_iter_ready(struct impl_norm_iter_state* s)
{
    return s->pos == 0 ? false : true;
}

// TODO: move it to internal_stages.h and name it stages_safe_cp()?
// Bad idea leave it here or maybe even inline into functions that use it.
uaix_always_inline
uaix_static type_codept norm_safe_cp(type_codept c)
{
    if (c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF))
        c = 0xFFFD;
    return c;
}

uaix_always_inline
uaix_static bool norm_state_fast_1(struct impl_norm_iter_state* s, type_codept c)
{
    s->buffer[1] = c;
    s->buffer_ccc[1] = 0;
    s->size = 2;
    s->last_qc = 1;
    s->pos = 0;
    return true;
}

uaix_always_inline
uaix_static bool norm_state_fast_0(struct impl_norm_iter_state* s, type_codept c)
{
    s->buffer[0] = c;
    s->buffer_ccc[0] = 0;
    s->size = 1;
    return false;
}

uaix_always_inline
uaix_static bool unstable_norm_iter_nfc(struct impl_norm_iter_state* s, type_codept c)
{
    // Note that we cannot implement a fast loop inside the normalization iterators
    // but we can use the same idea to make them a bit faster.
    // TODO: we can emulate the fast loop from impl_utf8_nfc much better.
    // ---
    // Very usefull todo. Much better how? I don't remember what I meant by this.
    // I hate myself so much sometimes.

    c = norm_safe_cp(c);
    if (stages_qc_yes_ns_nfc(c, &s->count_ns))
    {
        if (s->size == 1)
            return norm_state_fast_1(s, c);
        if (s->size == 0)
            return norm_state_fast_0(s, c);
    }
    if (!norm_decomp_nfc(c, s->buffer, s->buffer_ccc, &s->size, &s->last_qc, &s->count_ns))
        return false;

    return true;
}

uaix_always_inline
uaix_static bool unstable_norm_iter_nfd(struct impl_norm_iter_state* s, type_codept c)
{
    c = norm_safe_cp(c);
    if (stages_qc_yes_ns_nfd(c, &s->count_ns))
    {
        if (s->size == 1)
            return norm_state_fast_1(s, c);
        if (s->size == 0)
            return norm_state_fast_0(s, c);
    }
    if (!norm_decomp_nfd(c, s->buffer, s->buffer_ccc, &s->size, &s->last_qc, &s->count_ns))
        return false;

    return true;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool unstable_norm_iter_nfkc(struct impl_norm_iter_state* s, type_codept c)
{
    c = norm_safe_cp(c);
    if (stages_qc_yes_ns_nfkc(c, &s->count_ns))
    {
        if (s->size == 1)
            return norm_state_fast_1(s, c);
        if (s->size == 0)
            return norm_state_fast_0(s, c);
    }
    if (!norm_decomp_nfkc(c, s->buffer, s->buffer_ccc, &s->size, &s->last_qc, &s->count_ns))
        return false;

    return true;
}

uaix_always_inline
uaix_static bool unstable_norm_iter_nfkd(struct impl_norm_iter_state* s, type_codept c)
{
    c = norm_safe_cp(c);
    if (stages_qc_yes_ns_nfkd(c, &s->count_ns))
    {
        if (s->size == 1)
            return norm_state_fast_1(s, c);
        if (s->size == 0)
            return norm_state_fast_0(s, c);
    }
    if (!norm_decomp_nfkd(c, s->buffer, s->buffer_ccc, &s->size, &s->last_qc, &s->count_ns))
        return false;

    return true;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

uaix_always_inline
uaix_static bool unstable_norm_iter_next_comp(struct impl_norm_iter_state* s, type_codept* codepoint)
{
    // The function must be used together with unstable_norm_iter_nfc or unstable_norm_iter_nfc
    // The function must be used together with impl_state_norm_nfc or impl_state_norm_nfkc (OLD)
    // TODO: names:
    // norm_iter_comp_nfc_nfkc: impl_norm_iter_comp_nfc, impl_norm_iter_comp_nfkc + impl_norm_iter_nfc etc.
    // norm_iter_comp_nfc_nfkc: impl_norm_iter_comp_nfc, impl_norm_iter_comp_nfkc + impl_norm_iter_decomp_nfc etc.
    // norm_iter_next_nfc_nfkc: impl_norm_iter_next_nfc, impl_norm_iter_next_nfkc + impl_norm_iter_nfc etc. (prob this is the best)

    if (s->size == 0) // The end of the data
        return false;

    if (s->last_qc == 0) // This is the last buffer
        s->last_qc = s->size;

    // Reorder and compose the buffer if this is the first iteration on this buffer
    if (s->pos == 0)
    {
        norm_order(s->buffer, s->buffer_ccc, s->last_qc);
        norm_comp(s->buffer, s->buffer_ccc, s->last_qc);
    }

    // Skip all CCC=255 and get the next code point
    bool found = false;
    for (; s->pos < s->last_qc; ++s->pos)
    {
        if (s->buffer_ccc[s->pos] != 255)
        {
            if (found) break;
            *codepoint = s->buffer[s->pos];
            found = true;
        }
    }

    // This is the last iteration on this buffer, request more data
    if (s->pos == s->last_qc)
    {
        norm_buffer(s->buffer, s->buffer_ccc, &s->size, &s->last_qc);
        s->pos = 0;
    }

    return true;
}

uaix_always_inline
uaix_static bool unstable_norm_iter_next_decomp(struct impl_norm_iter_state* s, type_codept* codepoint)
{
    // The function must be used together with unstable_norm_iter_nfd or unstable_norm_iter_nfkd
    // The function must be used together with impl_state_norm_nfd or impl_state_norm_nfkd (OLD)

    if (s->size == 0) // The end of the data
        return false;

    if (s->last_qc == 0) // This is the last buffer
        s->last_qc = s->size;

    // Reorder the buffer if this is the first iteration on this buffer
    if (s->pos == 0)
        norm_order(s->buffer, s->buffer_ccc, s->last_qc);

    // Get the next code point
    if (s->pos < s->last_qc)
    {
        *codepoint = s->buffer[s->pos];
        ++s->pos;
    }

    // This is the last iteration on this buffer, request more data
    if (s->pos == s->last_qc)
    {
        norm_buffer(s->buffer, s->buffer_ccc, &s->size, &s->last_qc);
        s->pos = 0;
    }

    return true;
}

UNI_ALGO_IMPL_NAMESPACE_END

#include <uni/internal/undefs.h>

#endif // IMPL_NORM_H_UAIX

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
