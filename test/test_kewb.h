/* Test for Unicode Algorithms Implementation.
 * See notice at the end of this file. */

/* This is branchless algorithm from:
 * https://github.com/BobSteagall/utf_utils
 * that uses DFA (Deterministic Finite Automaton)
 * Check this too: https://www.youtube.com/watch?v=5FQ87-Ecb-A
 *
 * We use it for a performance comparison because it's the fastest branchless algorithm out there.
 * The performance of the algorithm is mostly the same as our algorithm that uses branches.
 * It is the same for 2-bytes sequences but slower for 3/4-bytes sequences
 * and ASCII (1-byte) is obviously the same because it just a simple fast loop in both algorithms.
 *
 * Note that we don't test SSE-based optimization for ASCII from the original algorithm
 * because it has nothing to do with the algorithm itself and can be added to any algorithm.
 *
 * The test function produces exactly the same result as our functions:
 * 1. The same number of replacement characters
 * 2. *error always points to the start of ill-formed sequence
 * so it passes all tests, only the algorithm is different.
 * That guaranties that we actually testing only the algorithm performance.
 *
 * I did try to optimize it: use struct with the same aligment like it was in the original algorithm
 * instead of arrays, rearrage some branches, etc. but of course I didn't achieve anything,
 * the actual algorithm is like 10 lines of code, it's not that hard for a compiler to optimize it properly.
 *
 * Performance notes:
 * Be aware that the perfomance in some cases can be a bit better in dumb tests but the algorithm wastes L1d cache for
 * data tables it'll lead to more cache misses at some point so in a real program the performance will be almost always worse.
 * The real advantage of the algorithm is that UTF-8 iterator func is pretty small around the same size as UTF16 iterator func.
 * That means in general there is no point to use branchless algorithm instead of an algorithm with branches
 * it is always will be slower. But in some cases small UTF-8 iterator may be usefull in the future.
 *
 * There is another old implementation of branchless UTF-8 algorithm exists:
 * https://bjoern.hoehrmann.de/utf-8/decoder/dfa/
 * It is always slower than this algorithm.
 * So I just want to mention here that I did test it.
 *
 * Disclaimer: Please don't use it as is, even though it passes tests it is just a test function.
 */

namespace uni::detail
{

template<typename it_in_utf8, typename it_end_utf8, typename it_out_utf16>
size_t kewb_utf8to16(it_in_utf8 first, it_end_utf8 last, it_out_utf16 result, size_t* error)
{
    static const unsigned char firstunit[256][2] = {
        {0x00,  0}, {0x01,  0}, {0x02,  0}, {0x03,  0}, {0x04,  0}, {0x05,  0}, {0x06,  0}, {0x07,  0},
        {0x08,  0}, {0x09,  0}, {0x0A,  0}, {0x0B,  0}, {0x0C,  0}, {0x0D,  0}, {0x0E,  0}, {0x0F,  0},
        {0x10,  0}, {0x11,  0}, {0x12,  0}, {0x13,  0}, {0x14,  0}, {0x15,  0}, {0x16,  0}, {0x17,  0},
        {0x18,  0}, {0x19,  0}, {0x1A,  0}, {0x1B,  0}, {0x1C,  0}, {0x1D,  0}, {0x1E,  0}, {0x1F,  0},
        {0x20,  0}, {0x21,  0}, {0x22,  0}, {0x23,  0}, {0x24,  0}, {0x25,  0}, {0x26,  0}, {0x27,  0},
        {0x28,  0}, {0x29,  0}, {0x2A,  0}, {0x2B,  0}, {0x2C,  0}, {0x2D,  0}, {0x2E,  0}, {0x2F,  0},
        {0x30,  0}, {0x31,  0}, {0x32,  0}, {0x33,  0}, {0x34,  0}, {0x35,  0}, {0x36,  0}, {0x37,  0},
        {0x38,  0}, {0x39,  0}, {0x3A,  0}, {0x3B,  0}, {0x3C,  0}, {0x3D,  0}, {0x3E,  0}, {0x3F,  0},
        {0x40,  0}, {0x41,  0}, {0x42,  0}, {0x43,  0}, {0x44,  0}, {0x45,  0}, {0x46,  0}, {0x47,  0},
        {0x48,  0}, {0x49,  0}, {0x4A,  0}, {0x4B,  0}, {0x4C,  0}, {0x4D,  0}, {0x4E,  0}, {0x4F,  0},
        {0x50,  0}, {0x51,  0}, {0x52,  0}, {0x53,  0}, {0x54,  0}, {0x55,  0}, {0x56,  0}, {0x57,  0},
        {0x58,  0}, {0x59,  0}, {0x5A,  0}, {0x5B,  0}, {0x5C,  0}, {0x5D,  0}, {0x5E,  0}, {0x5F,  0},
        {0x60,  0}, {0x61,  0}, {0x62,  0}, {0x63,  0}, {0x64,  0}, {0x65,  0}, {0x66,  0}, {0x67,  0},
        {0x68,  0}, {0x69,  0}, {0x6A,  0}, {0x6B,  0}, {0x6C,  0}, {0x6D,  0}, {0x6E,  0}, {0x6F,  0},
        {0x70,  0}, {0x71,  0}, {0x72,  0}, {0x73,  0}, {0x74,  0}, {0x75,  0}, {0x76,  0}, {0x77,  0},
        {0x78,  0}, {0x79,  0}, {0x7A,  0}, {0x7B,  0}, {0x7C,  0}, {0x7D,  0}, {0x7E,  0}, {0x7F,  0},
        {0x00, 12}, {0x01, 12}, {0x02, 12}, {0x03, 12}, {0x04, 12}, {0x05, 12}, {0x06, 12}, {0x07, 12},
        {0x08, 12}, {0x09, 12}, {0x0A, 12}, {0x0B, 12}, {0x0C, 12}, {0x0D, 12}, {0x0E, 12}, {0x0F, 12},
        {0x10, 12}, {0x11, 12}, {0x12, 12}, {0x13, 12}, {0x14, 12}, {0x15, 12}, {0x16, 12}, {0x17, 12},
        {0x18, 12}, {0x19, 12}, {0x1A, 12}, {0x1B, 12}, {0x1C, 12}, {0x1D, 12}, {0x1E, 12}, {0x1F, 12},
        {0x20, 12}, {0x21, 12}, {0x22, 12}, {0x23, 12}, {0x24, 12}, {0x25, 12}, {0x26, 12}, {0x27, 12},
        {0x28, 12}, {0x29, 12}, {0x2A, 12}, {0x2B, 12}, {0x2C, 12}, {0x2D, 12}, {0x2E, 12}, {0x2F, 12},
        {0x30, 12}, {0x31, 12}, {0x32, 12}, {0x33, 12}, {0x34, 12}, {0x35, 12}, {0x36, 12}, {0x37, 12},
        {0x38, 12}, {0x39, 12}, {0x3A, 12}, {0x3B, 12}, {0x3C, 12}, {0x3D, 12}, {0x3E, 12}, {0x3F, 12},
        {0xC0, 12}, {0xC1, 12}, {0x02, 24}, {0x03, 24}, {0x04, 24}, {0x05, 24}, {0x06, 24}, {0x07, 24},
        {0x08, 24}, {0x09, 24}, {0x0A, 24}, {0x0B, 24}, {0x0C, 24}, {0x0D, 24}, {0x0E, 24}, {0x0F, 24},
        {0x10, 24}, {0x11, 24}, {0x12, 24}, {0x13, 24}, {0x14, 24}, {0x15, 24}, {0x16, 24}, {0x17, 24},
        {0x18, 24}, {0x19, 24}, {0x1A, 24}, {0x1B, 24}, {0x1C, 24}, {0x1D, 24}, {0x1E, 24}, {0x1F, 24},
        {0x00, 60}, {0x01, 36}, {0x02, 36}, {0x03, 36}, {0x04, 36}, {0x05, 36}, {0x06, 36}, {0x07, 36},
        {0x08, 36}, {0x09, 36}, {0x0A, 36}, {0x0B, 36}, {0x0C, 36}, {0x0D, 72}, {0x0E, 36}, {0x0F, 36},
        {0x00, 84}, {0x01, 48}, {0x02, 48}, {0x03, 48}, {0x04, 96}, {0xF5, 12}, {0xF6, 12}, {0xF7, 12},
        {0xF8, 12}, {0xF9, 12}, {0xFA, 12}, {0xFB, 12}, {0xFC, 12}, {0xFD, 12}, {0xFE, 12}, {0xFF, 12},
    };

    static const unsigned char categories[256] = {
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
        4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
        4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
        0,  0,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
        5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
        6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  7,  7,
        9, 10, 10, 10, 11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    };

    static const unsigned char transitions[108] = {
        12,  0, 12, 12, 12, 24, 60, 36, 72, 84, 48, 96,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        12, 12,  0,  0,  0, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 24, 24, 24, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 36, 36, 36, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 24, 24, 12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 36, 36, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    };

    it_in_utf8 src = first;
    it_out_utf16 dst = result;

    while (src != last)
    {
        it_in_utf8 prev = src;
        char32_t c = (*src++ & 0xFF);

        /* It seems __builtin_expect doesn't do anything here but I'll leave it to make sure the fast loop works properly */
        /* Anyway we use it in our functions too because GCC sometimes messes up our fast loop so it's better for consistency */
#ifdef __GNUC__
        if (__builtin_expect(!!(c <= 0x7F),1))
#else
        if (c <= 0x7F)
#endif
        {
            *dst++ = (char16_t)c;
        }
        else
        {
            char32_t ret = firstunit[c][0];
            unsigned char state = firstunit[c][1];

            while (state != 0)
            {
                if (src != last)
                {
                    char32_t cc = (*src & 0xFF);
                    ret = (ret << 6) | (cc & 0x3F);
                    state = transitions[state + categories[cc]];
                    if (state == 12)
                    {
                        ret = 0xFFFFFFFF;
                        break;
                    }
                    ++src;
                }
                else
                {
                    ret = 0xFFFFFFFF;
                    break;
                }
            }

            if (ret == 0xFFFFFFFF)
            {
                if (error)
                {
                    *error = (size_t)(prev - first);
                    return (size_t)(dst - result);
                }

                *dst++ = (char16_t)0xFFFD;
            }
            else if (ret <= 0xFFFF)
            {
                *dst++ = (char16_t)ret;
            }
            else
            {
                *dst++ = (char16_t)(0xD7C0 + (ret >> 10));
                *dst++ = (char16_t)(0xDC00 + (ret & 0x3FF));
            }
        }
    }

    return (size_t)(dst - result);
}

} // namespace uni::detail

/* Copyright (c) 2018 Robert N. Steagall and KEWB Computing
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal with the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *     and the following disclaimers.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *     conditions and the following disclaimers in the documentation and/or other materials provided
 *     with the distribution.
 *  3. Neither the names of the copyright holder, nor the names of its contributors may be used
 *     to endorse or promote products derived from this Software without specific prior written
 *     permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 */
