/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Almost useless but probably can be used for some tests

template<typename it_in_utf8, typename it_out_utf16>
size_t impl_utf8to16(it_in_utf8 first, it_in_utf8 last, it_out_utf16 result, size_t* error)
{
    // W3C algorithm implemented word by word: https://encoding.spec.whatwg.org/#utf-8-decoder
    // It looks unreadable but it passes all tests.
    // And of course it is slow.

    // Note that we return a bit different *error value in different branches
    // this is crucial for correct *error value behaviour.

    it_in_utf8 src = first;
    it_out_utf16 dst = result;

    size_t bytes_seen = 0;
    size_t bytes_needed = 0;
    type_codept lower_boundary = 0x80;
    type_codept upper_boundary = 0xBF;
    type_codept codepoint = 0;

    while (src != last) // 2.
    {
        type_codept byte = (*src & 0xFF);

        if (bytes_needed == 0) // 3.
        {
            if (byte <= 0x7F)
            {
                *dst++ = (type_char16)byte;
                ++src;
                continue;
            }
            else if (byte >= 0xC2 && byte <= 0xDF)
            {
                bytes_needed = 1;
                codepoint = (byte & 0x1F);
            }
            else if (byte >= 0xE0 && byte <= 0xEF)
            {
                if (byte == 0xE0) lower_boundary = 0xA0;
                if (byte == 0xED) upper_boundary = 0x9F;
                bytes_needed = 2;
                codepoint = (byte & 0xF);
            }
            else if (byte >= 0xF0 && byte <= 0xF4)
            {
                if (byte == 0xF0) lower_boundary = 0x90;
                if (byte == 0xF4) upper_boundary = 0x8F;
                bytes_needed = 3;
                codepoint = (byte & 0x7);
            }
            else
            {
                if (error)
                {
                    *error = (size_t)(src - first) - bytes_seen + 1;
                    return (size_t)(dst - result);
                }

                *dst++ = (type_char16)0xFFFD;
            }
        }
        else
        {
            if (!(byte >= lower_boundary && byte <= upper_boundary)) // 4.
            {
                if (error)
                {
                    *error = (size_t)(src - first) - bytes_seen;
                    return (size_t)(dst - result);
                }

                *dst++ = (type_char16)0xFFFD;

                codepoint = 0;
                bytes_needed = 0;
                bytes_seen = 0;
                lower_boundary = 0x80;
                upper_boundary = 0xBF;

                // 4.2. "Prepend byte to stream."
                // That means reread the same byte. In other words - restart.
                // They should write it more clear at least.
                continue;
            }

            // 5.
            lower_boundary = 0x80;
            upper_boundary = 0xBF;

            // 6.
            codepoint = (codepoint << 6) | (byte & 0x3F);

            // 7.
            ++bytes_seen;

            if (bytes_seen == bytes_needed) // 8.
            {
                // 9. and 11.
                // And convert codepoint to UTF-16
                if (codepoint <= 0xFFFF)
                {
                    *dst++ = (type_char16)codepoint;
                }
                else // Make a surrogate pair
                {
                    *dst++ = (type_char16)(0xD7C0 + (codepoint >> 10));
                    *dst++ = (type_char16)(0xDC00 + (codepoint & 0x3FF));
                }

                // 10.
                codepoint = 0;
                bytes_needed = 0;
                bytes_seen = 0;
            }
        }

        ++src;
    }

    if (bytes_needed != 0) // 1.
    {
        if (error)
        {
            *error = (size_t)(src - first) - bytes_seen;
            return (size_t)(dst - result);
        }

        *dst++ = (type_char16)0xFFFD;
    }

    return (size_t)(dst - result);
}
