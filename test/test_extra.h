/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

void test_overflow()
{
    // Even though this usage is incorrect all overflows must be consistent

    // Reminder: to make this test fail make all low-level types = unsigned long long
    // and for example remove "& 0xFFFFFFFF" in impl_utf32to8 or impl_utf32to16 in impl_convert.h
    // or "& 0xFFFF" in impl_utf16to8 or impl_utf16to32 (will fail even without making all unsigned long long)

    std::basic_string<unsigned long long> str8(1, 0xFFull + 1 + 0x5A);
    std::basic_string<unsigned long long> str16(1, 0xFFFFull + 1 + 0x5A);
    std::basic_string<unsigned long long> str32(1, 0xFFFFFFFFull + 1 + 0x5A);

    TESTX((uni::utf8to16<unsigned long long, char16_t>(str8)[0] == 0x5A));
    TESTX((uni::utf8to32<unsigned long long, char32_t>(str8)[0] == 0x5A));
    TESTX((uni::utf16to8<unsigned long long, char>(str16)[0] == 0x5A));
    TESTX((uni::utf32to8<unsigned long long, char>(str32)[0] == 0x5A));
    TESTX((uni::utf16to32<unsigned long long, char32_t>(str16)[0] == 0x5A));
    TESTX((uni::utf32to16<unsigned long long, char16_t>(str32)[0] == 0x5A));

    // Iterators

    // Reminder: to make this test fail for example remove "& 0xFFFF" in utf16_iter in impl_iterator.h

    TESTX((*uni::iter::utf8{str8.cbegin(), str8.cend()} == 0x5A));
    TESTX((*uni::iter::utf16{str16.cbegin(), str16.cend()} == 0x5A));
}

void test_alter_value()
{
    // Make sure that low-level casts don't alter values
    // This test just explains why all low-level types must be unsigned in C++
    // See: impl_types.h (at the end of the file)

    // Reminder: to make this test fail make impl_char8 = char instead of unsigned char

    // TODO: add tests for impl_char16 and impl_char32 here too for consistency

    std::basic_string<unsigned long long> str8;

    str8 = uni::utf16to8<char16_t, unsigned long long>(u"\x0410");
    TESTX(str8[0] == 0xD0 && str8[1] == 0x90);

    str8 = uni::utf32to8<char32_t, unsigned long long>(U"\x0410");
    TESTX(str8[0] == 0xD0 && str8[1] == 0x90);

    str8 = uni::utf16to8<char16_t, unsigned long long>(u"\xD800"); // ill-formed
    TESTX(str8[0] == 0xEF && str8[1] == 0xBF && str8[2] == 0xBD);

    str8 = uni::utf32to8<char32_t, unsigned long long>(U"\xD800"); // ill-formed
    TESTX(str8[0] == 0xEF && str8[1] == 0xBF && str8[2] == 0xBD);

    // Iterators

    str8.resize(4); // Reuse str8, size 4 is enough for any code point

    uni::iter::output::utf8{str8.begin()} = 0x0410;
    TESTX(str8[0] == 0xD0 && str8[1] == 0x90);

    uni::iter::output::utf8{str8.begin()} = 0xD800;
    TESTX(str8[0] == 0xEF && str8[1] == 0xBF && str8[2] == 0xBD);
}
