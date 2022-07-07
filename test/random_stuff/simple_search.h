/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// For reference
// Simple search algorithm

template<typename it_in_utf8>
size_t impl_utf8find(it_in_utf8 first1, it_in_utf8 last1, it_in_utf8 first2, it_in_utf8 last2)
{
    it_in_utf8 src1 = first1;
    it_in_utf8 src2 = first2;
    it_in_utf8 back = src1;

    if ((last1 - first1) < (last2 - first2))
        return (size_t)-1;

    for (; src1 <= last1 - (last2 - first2); ++src1)
    {
        for (src2 = first2, back = src1; src2 < last2; ++src2, ++back)
        {
            if (*back != *src2)
                break;
        }

        if (src2 == last2)
            return (size_t)(src1 - first1);
    }

    return (size_t)-1;
}
