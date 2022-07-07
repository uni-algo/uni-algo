/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// This file just explains why there is almost no differency in performance between safe and unsafe UTF-8 algorithms.

// Two unsafe (they don't check for ill-formed sequences) utf8to16 functions implemented here.
// One with boundary checks and one without them.

// First function is faster than the current safe algorithm is about 0-5% and it really looks like a fluctuation.
// Second function is about 20-30% faster than the current.
// So it seems safety checks don't affect performance much and the real culprit is boundary check.
// My guess is branch predictor it can easy predict safety checks but it cannot predict boudary
// so safety checks can be "skipped" by branch predictor but every boudary check must be perfomed so they do affect performance.
// ---
// With random bytes test the difference between first function and the current algo is about 10-20%
// so it doesn't look like a fluctuation anymore. So it seems my guess is right and it is the branch predictor.
// Of course it doesn't matter for real UTF-8 text, there won't be any misspredictions almost always.
// ---
// TLDR there is no difference in performance between safe and unsafe UTF-8 functions
// so we don't need them or some define for that crap to achive a better performance.

// Unsafe with bound checks
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf16>
#endif
uaix_static size_t impl_utf8to16(it_in_utf8 first, it_end_utf8 last, it_out_utf16 result, size_t* error)
{
    // 2/3-byte sequence checks are swapped it makes 2 only a bit slower but good for 3

    it_in_utf8 s = first;
    it_out_utf16 dst = result;

    while (s != last)
    {
        type_codept c = (*s++ & 0xFF), c2, c3, c4;

        if (c <= 0x7F) // Fast path for ASCII
        {
            *dst++ = (type_char16)c;
        }
        else if ((c & 0xF0) == 0xE0) // 1110xxxx
        {
            if (s != last)
            {
                c2 = (*s++ & 0x3F); // 10xxxxxx
                if (s != last)
                {
                    c3 = (*s++ & 0x3F); // 10xxxxxx
                    c = ((c & 0x0F) << 12) + (c2 << 6) + c3;

                    *dst++ = (type_char16)c;
                }
            }
        }
        else if ((c & 0xE0) == 0xC0) // 110xxxxx
        {
            if (s != last)
            {
                c2 = (*s++ & 0x3F); // 10xxxxxx
                c = ((c & 0x1F) << 6) + c2;

                *dst++ = (type_char16)c;
            }
        }
        else if ((c & 0xF8) == 0xF0) // 11110xxx
        {
            if (s != last)
            {
                c2 = (*s++ & 0x3F); // 10xxxxxx
                if (s != last)
                {
                    c3 = (*s++ & 0x3F); // 10xxxxxx
                    if (s != last)
                    {
                        c4 = (*s++ & 0x3F); // 10xxxxxx
                        c = ((c & 0x07) << 18) + (c2 << 12) + (c3 << 6) + c4;

                        // Make a surrogate pair
                        *dst++ = (type_char16)(0xD7C0 + (c >> 10));
                        *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));
                    }
                }
            }
        }
    }

    return (size_t)(dst - result);
}

// Unsafe without bound checks.
// This is just pure garbage actually because without bounds checks it is not just unsafe it is unsecure.
// Paying security for performance is not just bad it is pure evil.
// And changing while (s != last) to (s < last) won't make it more secure it just hide the problem
// and it won't work with input iterators too anyway.
// Simply speaking we just cannot get rid of boudary checks even if we allow unsafe functions to exist.
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf16>
#endif
uaix_static size_t impl_utf8to16(it_in_utf8 first, it_end_utf8 last, it_out_utf16 result, size_t* error)
{
    // 2/3-byte sequence checks are swapped it makes 2 only a bit slower but good for 3

    it_in_utf8 s = first;
    it_out_utf16 dst = result;

    while (s != last)
    {
        type_codept c = (*s++ & 0xFF), c2, c3, c4;

        if (c <= 0x7F) // Fast path for ASCII
        {
            *dst++ = (type_char16)c;
        }
        else if ((c & 0xF0) == 0xE0) // 1110xxxx
        {
            c2 = (*s++ & 0x3F); // 10xxxxxx
            c3 = (*s++ & 0x3F); // 10xxxxxx
            c = ((c & 0x0F) << 12) + (c2 << 6) + c3;
        }
        else if ((c & 0xE0) == 0xC0) // 110xxxxx
        {
            c2 = (*s++ & 0x3F); // 10xxxxxx
            c = ((c & 0x1F) << 6) + c2;

            *dst++ = (type_char16)c;
        }
        else if ((c & 0xF8) == 0xF0) // 11110xxx
        {
            c2 = (*s++ & 0x3F); // 10xxxxxx
            c3 = (*s++ & 0x3F); // 10xxxxxx
            c4 = (*s++ & 0x3F); // 10xxxxxx
            c = ((c & 0x07) << 18) + (c2 << 12) + (c3 << 6) + c4;

            // Make a surrogate pair
            *dst++ = (type_char16)(0xD7C0 + (c >> 10));
            *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));
        }
    }

    return (size_t)(dst - result);
}

