/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Convert module test

void test_convert_and_iter_convert()
{
    // UTF-32 code unit (code point)
    for (char32_t c = 0; c <= 0x110000; ++c)
    {
        std::u32string str32 = std::u32string(1, c);

        std::string str8 = uni::utf32to8<char32_t, char>(str32);
        std::u16string str16 = uni::utf32to16<char32_t, char16_t>(str32);

        if ((c < 0xD800 || c > 0xDFFF) && c <= 0x10FFFF)
        {
            TESTX((str8 == uni::utf16to8<char16_t, char>(str16)));
            TESTX((str16 == uni::utf8to16<char, char16_t>(str8)));
            TESTX((str32 == uni::utf8to32<char, char32_t>(str8)));
            TESTX((str32 == uni::utf16to32<char16_t, char32_t>(str16)));
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

        std::string str8 = uni::utf16to8<char16_t, char>(str16);
        std::u32string str32 = uni::utf16to32<char16_t, char32_t>(str16);

        if (c < 0xD800 || c > 0xDFFF)
        {
            TESTX((str8 == uni::utf32to8<char32_t, char>(str32)));
            TESTX((str32 == uni::utf8to32<char, char32_t>(str8)));
            TESTX((str16 == uni::utf8to16<char, char16_t>(str8)));
            TESTX((str16 == uni::utf32to16<char32_t, char16_t>(str32)));
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

        std::basic_string<type> str8 = uni::utf32to8<type, type>(str32);
        std::basic_string<type> str16 = uni::utf32to16<type, type>(str32);

        if ((c < 0xD800 || c > 0xDFFF) && c <= 0x10FFFF)
        {
            TESTX((str8 == uni::utf16to8<type, type>(str16)));
            TESTX((str16 == uni::utf8to16<type, type>(str8)));
            TESTX((str32 == uni::utf8to32<type, type>(str8)));
            TESTX((str32 == uni::utf16to32<type, type>(str16)));
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

        std::basic_string<type> str8 = uni::utf16to8<type, type>(str16);
        std::basic_string<type> str32 = uni::utf16to32<type, type>(str16);

        if (c < 0xD800 || c > 0xDFFF)
        {
            TESTX((str8 == uni::utf32to8<type, type>(str32)));
            TESTX((str32 == uni::utf8to32<type, type>(str8)));
            TESTX((str16 == uni::utf8to16<type, type>(str8)));
            TESTX((str16 == uni::utf32to16<type, type>(str32)));
        }
        else
        {
            TESTX((str8 == std::basic_string<type>{0xEF, 0xBF, 0xBD}));
            TESTX((str32 == std::basic_string<type>{0xFFFD}));
        }
    }
}

// Iterator module test

// Implement all convert functions using iterators

template<typename UTF8, typename UTF16>
std::basic_string<UTF16> utf8to16(std::basic_string_view<UTF8> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    uni::iter::utf8 begin{source.cbegin(), source.cend()};
    std::basic_string<UTF16> result;
    uni::iter::output::utf16 out{std::back_inserter(result)};
    for (auto it = begin; it != uni::sentinel; ++it)
        out = *it;
    return result;
}
template<typename UTF16, typename UTF8>
std::basic_string<UTF8> utf16to8(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    uni::iter::utf16 begin{source.cbegin(), source.cend()};
    std::basic_string<UTF8> result;
    uni::iter::output::utf8 out{std::back_inserter(result)};
    for (auto it = begin; it != uni::sentinel; ++it)
        out = *it;
    return result;
}
template<typename UTF8, typename UTF32>
std::basic_string<UTF32> utf8to32(std::basic_string_view<UTF8> source)
{
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    uni::iter::utf8 begin{source.cbegin(), source.cend()};
    std::basic_string<UTF32> result;
    for (auto it = begin; it != uni::sentinel; ++it)
        result.push_back(*it);
    return result;
}
template<typename UTF32, typename UTF8>
std::basic_string<UTF8> utf32to8(std::basic_string_view<UTF32> source)
{
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    std::basic_string<UTF8> result;
    uni::iter::output::utf8 out{std::back_inserter(result)};
    for (auto it = source.cbegin(); it != source.cend(); ++it)
        out = static_cast<char32_t>(*it);
    return result;
}
template<typename UTF16, typename UTF32>
std::basic_string<UTF32> utf16to32(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    uni::iter::utf16 begin{source.cbegin(), source.cend()};
    std::basic_string<UTF32> result;
    for (auto it = begin; it != uni::sentinel; ++it)
        result.push_back(*it);
    return result;
}
template<typename UTF32, typename UTF16>
std::basic_string<UTF16> utf32to16(std::basic_string_view<UTF32> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    std::basic_string<UTF16> result;
    uni::iter::output::utf16 out{std::back_inserter(result)};
    for (auto it = source.cbegin(); it != source.cend(); ++it)
        out = static_cast<char32_t>(*it);
    return result;
}

// Redo the previous test using these convert functions

void test_convert_and_iter_iter()
{
    // UTF-32 code unit (code point)
    for (char32_t c = 0; c <= 0x110000; ++c)
    {
        std::u32string str32 = std::u32string(1, c);

        std::string str8 = utf32to8<char32_t, char>(str32);
        std::u16string str16 = utf32to16<char32_t, char16_t>(str32);

        if ((c < 0xD800 || c > 0xDFFF) && c <= 0x10FFFF)
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

        if (c < 0xD800 || c > 0xDFFF)
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

        if ((c < 0xD800 || c > 0xDFFF) && c <= 0x10FFFF)
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

        if (c < 0xD800 || c > 0xDFFF)
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
}
