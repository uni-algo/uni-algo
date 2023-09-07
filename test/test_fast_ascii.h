/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Test functions that use fast ASCII optimization
// with strings that can be divided by 4 with no remainder (strings below that start with 0)
// and just in case with a remainder (strings below that start with 1 and end with a remainder)
// to make sure the optimization doesn't break something.

test_constexpr bool test_fast_ascii_utf8to16()
{
    TESTX(una::utf8to16u("0123") == u"0123");
    TESTX(una::utf8to16u("01230123") == u"01230123");
    TESTX(una::utf8to16u("012301230123") == u"012301230123");

    TESTX(una::utf8to16u("123") == u"123");
    TESTX(una::utf8to16u("12301") == u"12301");
    TESTX(una::utf8to16u("1230123012") == u"1230123012");

    return true;
}

test_constexpr bool test_fast_ascii_utf8to32()
{
    TESTX(una::utf8to32u("0123") == U"0123");
    TESTX(una::utf8to32u("01230123") == U"01230123");
    TESTX(una::utf8to32u("012301230123") == U"012301230123");

    TESTX(una::utf8to32u("123") == U"123");
    TESTX(una::utf8to32u("12301") == U"12301");
    TESTX(una::utf8to32u("1230123012") == U"1230123012");

    return true;
}
