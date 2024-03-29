/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr bool test_overflow()
{
    // Even though this usage is incorrect all overflows must be consistent

    // REMINDER: to make this test fail make all low-level types = unsigned long long
    // and for example remove "& 0xFFFFFFFF" in impl_utf32to8 or impl_utf32to16 in impl_conv.h
    // or "& 0xFFFF" in impl_utf16to8 or impl_utf16to32 (will fail even without making all unsigned long long)

    std::basic_string<unsigned long long> str8(1, 0xFFull + 1 + 0x5A);
    std::basic_string<unsigned long long> str16(1, 0xFFFFull + 1 + 0x5A);
    std::basic_string<unsigned long long> str32(1, 0xFFFFFFFFull + 1 + 0x5A);

    TESTX((una::utf8to16<unsigned long long, char16_t>(str8)[0] == 0x5A));
    TESTX((una::utf8to32<unsigned long long, char32_t>(str8)[0] == 0x5A));
    TESTX((una::utf16to8<unsigned long long, char>(str16)[0] == 0x5A));
    TESTX((una::utf32to8<unsigned long long, char>(str32)[0] == 0x5A));
    TESTX((una::utf16to32<unsigned long long, char32_t>(str16)[0] == 0x5A));
    TESTX((una::utf32to16<unsigned long long, char16_t>(str32)[0] == 0x5A));

    // Ranges

    // REMINDER: to make this test fail for example remove "& 0xFFFF" in utf16_iter in impl_iter.h

    TESTX(*una::ranges::utf8_view{str8}.begin() == 0x5A);
    TESTX(*una::ranges::utf16_view{str16}.begin() == 0x5A);

    return true;
}

test_constexpr bool test_alter_value()
{
    // Make sure that low-level casts don't alter values
    // This test just explains why all low-level types must be unsigned in C++
    // See: impl_types.h (at the end of the file)

    // REMINDER: to make this test fail make type_char8 = char instead of unsigned char
    // or type_char16 = short instead of char16_t UTF-32 test is here only for consistency

    std::basic_string<unsigned long long> str8;

    str8 = una::utf16to8<char16_t, unsigned long long>(u"\x0410");
    TESTX(str8.size() == 2 && str8[0] == 0xD0 && str8[1] == 0x90);

    str8 = una::utf32to8<char32_t, unsigned long long>(U"\x0410");
    TESTX(str8.size() == 2 && str8[0] == 0xD0 && str8[1] == 0x90);

    str8 = una::utf16to8<char16_t, unsigned long long>(u"\xD800"); // ill-formed
    TESTX(str8.size() == 3 && str8[0] == 0xEF && str8[1] == 0xBF && str8[2] == 0xBD);

    str8 = una::utf32to8<char32_t, unsigned long long>(U"\xD800"); // ill-formed
    TESTX(str8.size() == 3 && str8[0] == 0xEF && str8[1] == 0xBF && str8[2] == 0xBD);

    std::basic_string<unsigned long long> str16;

    str16 = una::utf8to16<char, unsigned long long>("\xD0\x90");
    TESTX(str16.size() == 1 && str16[0] == 0x0410);

    str16 = una::utf32to16<char32_t, unsigned long long>(U"\x0410");
    TESTX(str16.size() == 1 && str16[0] == 0x0410);

    str16 = una::utf8to16<char, unsigned long long>("\x80"); // ill-formed
    TESTX(str16.size() == 1 && str16[0] == 0xFFFD);

    str16 = una::utf32to16<char32_t, unsigned long long>(U"\xD800"); // ill-formed
    TESTX(str16.size() == 1 && str16[0] == 0xFFFD);

    std::basic_string<unsigned long long> str32;

    str32 = una::utf8to32<char, unsigned long long>("\xD0\x90");
    TESTX(str32.size() == 1 && str32[0] == 0x0410);

    str32 = una::utf16to32<char16_t, unsigned long long>(u"\x0410");
    TESTX(str32.size() == 1 && str32[0] == 0x0410);

    str32 = una::utf8to32<char, unsigned long long>("\x80"); // ill-formed
    TESTX(str32.size() == 1 && str32[0] == 0xFFFD);

    str32 = una::utf16to32<char16_t, unsigned long long>(u"\xD800"); // ill-formed
    TESTX(str32.size() == 1 && str32[0] == 0xFFFD);

    // Ranges

    str8 = una::ranges::to_utf8<decltype(str8)>(std::u32string{0x0410});
    TESTX(str8.size() == 2 && str8[0] == 0xD0 && str8[1] == 0x90);

    str8 = una::ranges::to_utf8<decltype(str8)>(std::u32string{0xD800}); // ill-formed
    TESTX(str8.size() == 3 && str8[0] == 0xEF && str8[1] == 0xBF && str8[2] == 0xBD);

    str16 = una::ranges::to_utf16<decltype(str16)>(std::u32string{0x0410});
    TESTX(str16.size() == 1 && str16[0] == 0x0410);

    str16 = una::ranges::to_utf16<decltype(str16)>(std::u32string{0xD800}); // ill-formed
    TESTX(str16.size() == 1 && str16[0] == 0xFFFD);

    return true;
}
