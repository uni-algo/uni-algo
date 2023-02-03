/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Conversion module test

test_constexpr bool test_conv_and_iter_conv()
{
    // UTF-32 code unit (code point)
    for (char32_t c = 0; c <= 0x110000; ++c)
    {
        std::u32string str32 = std::u32string(1, c);

        std::string str8 = una::utf32to8<char32_t, char>(str32);
        std::u16string str16 = una::utf32to16<char32_t, char16_t>(str32);

        if (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF))
        {
            TESTX((str8 == una::utf16to8<char16_t, char>(str16)));
            TESTX((str16 == una::utf8to16<char, char16_t>(str8)));
            TESTX((str32 == una::utf8to32<char, char32_t>(str8)));
            TESTX((str32 == una::utf16to32<char16_t, char32_t>(str16)));
        }
        else
        {
            TESTX(str8 == "\xEF\xBF\xBD");
            TESTX(str16 == u"\xFFFD");
        }
    }

    // UTF-16 code unit
    for (char32_t c = 0; c <= 0xFFFF; ++c)
    {
        std::u16string str16 = std::u16string(1, static_cast<char16_t>(c));

        std::string str8 = una::utf16to8<char16_t, char>(str16);
        std::u32string str32 = una::utf16to32<char16_t, char32_t>(str16);

        if (!(c >= 0xD800 && c <= 0xDFFF))
        {
            TESTX((str8 == una::utf32to8<char32_t, char>(str32)));
            TESTX((str32 == una::utf8to32<char, char32_t>(str8)));
            TESTX((str16 == una::utf8to16<char, char16_t>(str8)));
            TESTX((str16 == una::utf32to16<char32_t, char16_t>(str32)));
        }
        else
        {
            TESTX(str8 == "\xEF\xBF\xBD");
            TESTX(str32 == U"\xFFFD");
        }
    }

    // The same but all types are long long

    using type = long long;

    // UTF-32 code unit (code point)
    for (char32_t c = 0; c <= 0x110000; ++c)
    {
        std::basic_string<type> str32 = std::basic_string<type>(1, c);

        std::basic_string<type> str8 = una::utf32to8<type, type>(str32);
        std::basic_string<type> str16 = una::utf32to16<type, type>(str32);

        if (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF))
        {
            TESTX((str8 == una::utf16to8<type, type>(str16)));
            TESTX((str16 == una::utf8to16<type, type>(str8)));
            TESTX((str32 == una::utf8to32<type, type>(str8)));
            TESTX((str32 == una::utf16to32<type, type>(str16)));
        }
        else
        {
            TESTX((str8 == std::basic_string<type>{0xEF, 0xBF, 0xBD}));
            TESTX((str16 == std::basic_string<type>{0xFFFD}));
        }
    }

    // UTF-16 code unit
    for (char32_t c = 0; c <= 0xFFFF; ++c)
    {
        std::basic_string<type> str16 = std::basic_string<type>(1, static_cast<type>(c));

        std::basic_string<type> str8 = una::utf16to8<type, type>(str16);
        std::basic_string<type> str32 = una::utf16to32<type, type>(str16);

        if (!(c >= 0xD800 && c <= 0xDFFF))
        {
            TESTX((str8 == una::utf32to8<type, type>(str32)));
            TESTX((str32 == una::utf8to32<type, type>(str8)));
            TESTX((str16 == una::utf8to16<type, type>(str8)));
            TESTX((str16 == una::utf32to16<type, type>(str32)));
        }
        else
        {
            TESTX((str8 == std::basic_string<type>{0xEF, 0xBF, 0xBD}));
            TESTX((str32 == std::basic_string<type>{0xFFFD}));
        }
    }

    return true;
}

// Iterator module test

// Implement all conversion functions using ranges

template<typename UTF8, typename UTF16>
test_constexpr std::basic_string<UTF16> utf8to16(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return source | una::views::utf8 | una::ranges::to_utf16<std::basic_string<UTF16>>();
}
template<typename UTF16, typename UTF8>
test_constexpr std::basic_string<UTF8> utf16to8(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return source | una::views::utf16 | una::ranges::to_utf8<std::basic_string<UTF8>>();
}
template<typename UTF8, typename UTF32>
test_constexpr std::basic_string<UTF32> utf8to32(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    auto view = source | una::views::utf8;

    return std::basic_string<UTF32>{view.begin(), view.end()};
}
template<typename UTF32, typename UTF8>
test_constexpr std::basic_string<UTF8> utf32to8(std::basic_string_view<UTF32> source)
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return source | una::ranges::to_utf8<std::basic_string<UTF8>>();
}
template<typename UTF16, typename UTF32>
test_constexpr std::basic_string<UTF32> utf16to32(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    auto view = source | una::views::utf16;

    return std::basic_string<UTF32>{view.begin(), view.end()};
}
template<typename UTF32, typename UTF16>
test_constexpr std::basic_string<UTF16> utf32to16(std::basic_string_view<UTF32> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return source | una::ranges::to_utf16<std::basic_string<UTF16>>();
}

// Redo the previous test using these conversion functions

test_constexpr bool test_conv_and_iter_iter()
{
    // UTF-32 code unit (code point)
    for (char32_t c = 0; c <= 0x110000; ++c)
    {
        std::u32string str32 = std::u32string(1, c);

        std::string str8 = utf32to8<char32_t, char>(str32);
        std::u16string str16 = utf32to16<char32_t, char16_t>(str32);

        if (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF))
        {
            TESTX((str8 == utf16to8<char16_t, char>(str16)));
            TESTX((str16 == utf8to16<char, char16_t>(str8)));
            TESTX((str32 == utf8to32<char, char32_t>(str8)));
            TESTX((str32 == utf16to32<char16_t, char32_t>(str16)));
        }
        else
        {
            TESTX(str8 == "\xEF\xBF\xBD");
            TESTX(str16 == u"\xFFFD");
        }
    }

    // UTF-16 code unit
    for (char32_t c = 0; c <= 0xFFFF; ++c)
    {
        std::u16string str16 = std::u16string(1, static_cast<char16_t>(c));

        std::string str8 = utf16to8<char16_t, char>(str16);
        std::u32string str32 = utf16to32<char16_t, char32_t>(str16);

        if (!(c >= 0xD800 && c <= 0xDFFF))
        {
            TESTX((str8 == utf32to8<char32_t, char>(str32)));
            TESTX((str32 == utf8to32<char, char32_t>(str8)));
            TESTX((str16 == utf8to16<char, char16_t>(str8)));
            TESTX((str16 == utf32to16<char32_t, char16_t>(str32)));
        }
        else
        {
            TESTX(str8 == "\xEF\xBF\xBD");
            TESTX(str32 == U"\xFFFD");
        }
    }

    // The same but all types are long long

    using type = long long;

    // UTF-32 code unit (code point)
    for (char32_t c = 0; c <= 0x110000; ++c)
    {
        std::basic_string<type> str32 = std::basic_string<type>(1, c);

        std::basic_string<type> str8 = utf32to8<type, type>(str32);
        std::basic_string<type> str16 = utf32to16<type, type>(str32);

        if (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF))
        {
            TESTX((str8 == utf16to8<type, type>(str16)));
            TESTX((str16 == utf8to16<type, type>(str8)));
            TESTX((str32 == utf8to32<type, type>(str8)));
            TESTX((str32 == utf16to32<type, type>(str16)));
        }
        else
        {
            TESTX((str8 == std::basic_string<type>{0xEF, 0xBF, 0xBD}));
            TESTX((str16 == std::basic_string<type>{0xFFFD}));
        }
    }

    // UTF-16 code unit
    for (char32_t c = 0; c <= 0xFFFF; ++c)
    {
        std::basic_string<type> str16 = std::basic_string<type>(1, static_cast<type>(c));

        std::basic_string<type> str8 = utf16to8<type, type>(str16);
        std::basic_string<type> str32 = utf16to32<type, type>(str16);

        if (!(c >= 0xD800 && c <= 0xDFFF))
        {
            TESTX((str8 == utf32to8<type, type>(str32)));
            TESTX((str32 == utf8to32<type, type>(str8)));
            TESTX((str16 == utf8to16<type, type>(str8)));
            TESTX((str16 == utf32to16<type, type>(str32)));
        }
        else
        {
            TESTX((str8 == std::basic_string<type>{0xEF, 0xBF, 0xBD}));
            TESTX((str32 == std::basic_string<type>{0xFFFD}));
        }
    }

    return true;
}
