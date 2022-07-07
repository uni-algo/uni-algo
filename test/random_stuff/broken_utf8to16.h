/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// One of algorithms that was tested
// It almost works but it doesn't produce the same number of replacement characters as W3C in some cases
// and it needs at least forward iterator to produce replacement characters.
// It is here for a reference because it can be modified to a validate UTF-8 algorithm
// and if we ever need such algorithm we must test the performance of this algorithm
// and the current algorithm as validate UTF-8 algorithm. But note that for overlongs it always reads all code units
// so the behavior is a bit different than the current algorithm, it probably won't matter for validation but still.

// Note that all comments in this function are old and may be incorrect
template<typename it_in_utf8, typename it_out_utf16>
static size_t impl_utf8to16(it_in_utf8 first, it_in_utf8 last, it_out_utf16 result, size_t* error)
{
    // 2/3-byte sequence checks are swapped it makes 2 only a bit slower but good for 3

    it_in_utf8 s = first;
    it_in_utf8 prev = s;
    it_out_utf16 dst = result;

    while (s < last)
    {
        type_codept c = (*s & 0xFF), c2, c3, c4;
        prev = s; // Save previous position to be able to restart properly

        if (c <= 0x7F) // Fast path for ASCII
        {
            *dst++ = (type_char16)c;
            ++s;
            continue;
        }
        else if ((c & 0xF0) == 0xE0) // 1110xxxx
        {
            if (++s < last && ((c2 = (*s & 0xFF)) & 0xC0) == 0x80 && // 10xxxxxx
                ++s < last && ((c3 = (*s & 0xFF)) & 0xC0) == 0x80)   // 10xxxxxx
            {
                c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);

                if (c <= 0x7FF || (c >= 0xD800 && c <= 0xDFFF)) // Overlong or surrogate
                    c = 0xFFFFFFFF;
                else
                {
                    *dst++ = (type_char16)c;
                    ++s;
                    continue;
                }
            }
        }
        else if ((c & 0xE0) == 0xC0) // 110xxxxx
        {
            if (++s < last && ((c2 = (*s & 0xFF)) & 0xC0) == 0x80) // 10xxxxxx
            {
                c = ((c & 0x1F) << 6) + (c2 & 0x3F);

                if (c <= 0x7F) // Overlong
                    c = 0xFFFFFFFF;
                else
                {
                    *dst++ = (type_char16)c;
                    ++s;
                    continue;
                }
            }
        }
        else if ((c & 0xF8) == 0xF0) // 11110xxx
        {
            if (++s < last && ((c2 = (*s & 0xFF)) & 0xC0) == 0x80 && // 10xxxxxx
                ++s < last && ((c3 = (*s & 0xFF)) & 0xC0) == 0x80 && // 10xxxxxx
                ++s < last && ((c4 = (*s & 0xFF)) & 0xC0) == 0x80)   // 10xxxxxx
            {
                c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);

                if (c <= 0xFFFF || c > 0x10FFFF) // Overlong or out of range
                    c = 0xFFFFFFFF;
                else
                {
                    // Make a surrogate pair
                    *dst++ = (type_char16)(0xD7C0 + (c >> 10));
                    *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));
                    ++s;
                    continue;
                }
            }
        }

        if (error)
        {
            // *error points to the start of invalid sequence + 1
            *error = (size_t)(prev - first) + 1;
            return (size_t)(dst - result);
        }

        if (c == 0xFFFFFFFF) // Invalid sequence
        {
            // Multiple replacement chars for invalid to conform with W3C standard and ICU
            for (size_t i = 0; i <= (size_t)(s - prev); ++i)
                *dst++ = (type_char16)0xFFFD; // Replacement char U+FFFD

            ++s;
        }
        else // Truncated or lone non-ASCII (invalid) code unit
        {
            // One replacement char for truncated to conform with W3C standard and ICU
            *dst++ = (type_char16)0xFFFD; // Replacement char U+FFFD

            // If not truncated because the end of truncated can be the start of valid
            if (s - prev == 0)
                ++s;
        }
    }

    return (size_t)(dst - result);
}
