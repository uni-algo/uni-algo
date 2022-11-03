/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

/* UTF-8 to UTF-16 conversion test using iterator function instead of inline everything into one function
 * Observations:
 * GCC performance is mostly the same ~5% slower (GCC 7.3.0 x64 -03), 10-20% slower (GCC 7.3.0 x86 -03)
 * MSVC performance is about 10% slower (VS2017 x64)
 *
 * Note that I did only basic tests, worse performance in some cases was enough for me to inline everything into a function
 */

namespace uni::detail
{

// The same as internal utf8iter function
#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8>
#endif
#ifdef __GNUC__
__attribute__((__always_inline__)) inline
#elif _MSC_VER
__forceinline
#endif
it_in_utf8 utf8iter_test(it_in_utf8 first, it_end_utf8 last, type_codept* codepoint, type_codept error)
{
    it_in_utf8 s = first;

    type_codept c = (*s & 0xFF), c2, c3, c4;

#ifdef __GNUC__
    if (__builtin_expect((c <= 0x7F),1))
#else
    if (c <= 0x7F)
#endif
    {
        *codepoint = c;
        return ++s;
    }
    else if (c >= 0xC2 && c <= 0xDF)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF))
        {
            c = ((c & 0x1F) << 6) + (c2 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c >= 0xE1 && c <= 0xEC)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
        {
            c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c >= 0xEE && c <= 0xEF)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
        {
            c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c == 0xE0)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0xA0 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
        {
            c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c == 0xED)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x9F) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF))
        {
            c = ((c & 0x0F) << 12) + ((c2 & 0x3F) << 6) + (c3 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c == 0xF0)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x90 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
            ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
        {
            c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c == 0xF4)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0x8F) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
            ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
        {
            c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else if (c >= 0xF1 && c <= 0xF3)
    {
        if (++s != last && ((c2 = (*s & 0xFF)) >= 0x80 && c2 <= 0xBF) &&
            ++s != last && ((c3 = (*s & 0xFF)) >= 0x80 && c3 <= 0xBF) &&
            ++s != last && ((c4 = (*s & 0xFF)) >= 0x80 && c4 <= 0xBF))
        {
            c = ((c & 0x07) << 18) + ((c2 & 0x3F) << 12) + ((c3 & 0x3F) << 6) + (c4 & 0x3F);
            *codepoint = c;
            return ++s;
        }
    }
    else
    {
        ++s;
    }

    *codepoint = error;

    return s;
}

#ifdef __cplusplus
template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf16>
#endif
size_t impl_utf8to16_test(it_in_utf8 first, it_end_utf8 last, it_out_utf16 result, size_t* error)
{
    it_in_utf8 src = first;
    it_in_utf8 prev = src;
    it_out_utf16 dst = result;
    type_codept c;

    while (src != last)
    {
        prev = src;
        src = utf8iter_test(src, last, &c, 0xFFFFFFFF);

        if (c == 0xFFFFFFFF)
        {
            if (error)
            {
                *error = (size_t)(prev - first);
                return (size_t)(dst - result);
            }

            *dst++ = (type_char16)0xFFFD;
        }
        else if (c <= 0xFFFF)
        {
            *dst++ = (type_char16)c;
        }
        else
        {
            *dst++ = (type_char16)(0xD7C0 + (c >> 10));
            *dst++ = (type_char16)(0xDC00 + (c & 0x3FF));
        }
    }

    return (size_t)(dst - result);
}

} // namespace uni::detail
