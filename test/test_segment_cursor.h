/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr bool test_segment_cursor_grapheme()
{
    // "N,"+"Regional Indicator: AC+BE+W"+" GE"
    // Graphemes: "N|,|AC|BE|W| |G|E"
    std::string_view    str   =  "N,\U0001F1E6\U0001F1E8\U0001F1E7\U0001F1EA\U0001F1FC GE";
    std::u16string_view str16 = u"N,\U0001F1E6\U0001F1E8\U0001F1E7\U0001F1EA\U0001F1FC GE";

    // UTF-8

    auto range = una::ranges::grapheme::utf8_view{str};

    std::string result;
    for (std::size_t i = 0; i <= str.size(); ++i)
    {
        result += *range.cursor(str.begin() + static_cast<std::ptrdiff_t>(i));
        result += '\n';
    }

    TESTX(*range.cursor(str.begin()) == "N");
    TESTX(*range.cursor(str.end()) == "E");

    TESTX(result ==
        "N\n"
        "N\n"
        ",\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1FC\n"
        "\U0001F1FC\n"
        "\U0001F1FC\n"
        "\U0001F1FC\n"
        " \n"
        "G\n"
        "E\n");

    // UTF-16

    auto range16 = una::ranges::grapheme::utf16_view{str16};

    std::u16string result16;
    for (std::size_t i = 0; i <= str16.size(); ++i)
    {
        result16 += *range16.cursor(str16.begin() + static_cast<std::ptrdiff_t>(i));
        result16 += '\n';
    }

    TESTX(*range16.cursor(str16.begin()) == u"N");
    TESTX(*range16.cursor(str16.end()) == u"E");

    TESTX(result16 ==
        u"N\n"
        u"N\n"
        u",\n"
        u"\U0001F1E6\U0001F1E8\n"
        u"\U0001F1E6\U0001F1E8\n"
        u"\U0001F1E6\U0001F1E8\n"
        u"\U0001F1E6\U0001F1E8\n"
        u"\U0001F1E7\U0001F1EA\n"
        u"\U0001F1E7\U0001F1EA\n"
        u"\U0001F1E7\U0001F1EA\n"
        u"\U0001F1E7\U0001F1EA\n"
        u"\U0001F1FC\n"
        u"\U0001F1FC\n"
        u" \n"
        u"G\n"
        u"E\n");

    return true;
}

test_constexpr bool test_segment_cursor_word()
{
    // "N,"+"Regional Indicator: AC+BE+W"+" GE"
    // Words: "N|,|AC|BE|W| |GE"
    std::string_view    str   =  "N,\U0001F1E6\U0001F1E8\U0001F1E7\U0001F1EA\U0001F1FC GE";
    std::u16string_view str16 = u"N,\U0001F1E6\U0001F1E8\U0001F1E7\U0001F1EA\U0001F1FC GE";

    // UTF-8

    auto range = una::ranges::word::utf8_view{str};

    std::string result;
    for (std::size_t i = 0; i <= str.size(); ++i)
    {
        result += *range.cursor(str.begin() + static_cast<std::ptrdiff_t>(i));
        result += '\n';
    }

    TESTX(*range.cursor(str.begin()) == "N");
    TESTX(*range.cursor(str.end()) == "GE");

    TESTX(result ==
        "N\n"
        "N\n"
        ",\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E6\U0001F1E8\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1E7\U0001F1EA\n"
        "\U0001F1FC\n"
        "\U0001F1FC\n"
        "\U0001F1FC\n"
        "\U0001F1FC\n"
        " \n"
        "GE\n"
        "GE\n");

    // UTF-16

    auto range16 = una::ranges::word::utf16_view{str16};

    std::u16string result16;
    for (std::size_t i = 0; i <= str16.size(); ++i)
    {
        result16 += *range16.cursor(str16.begin() + static_cast<std::ptrdiff_t>(i));
        result16 += '\n';
    }

    TESTX(*range16.cursor(str16.begin()) == u"N");
    TESTX(*range16.cursor(str16.end()) == u"GE");

    TESTX(result16 ==
        u"N\n"
        u"N\n"
        u",\n"
        u"\U0001F1E6\U0001F1E8\n"
        u"\U0001F1E6\U0001F1E8\n"
        u"\U0001F1E6\U0001F1E8\n"
        u"\U0001F1E6\U0001F1E8\n"
        u"\U0001F1E7\U0001F1EA\n"
        u"\U0001F1E7\U0001F1EA\n"
        u"\U0001F1E7\U0001F1EA\n"
        u"\U0001F1E7\U0001F1EA\n"
        u"\U0001F1FC\n"
        u"\U0001F1FC\n"
        u" \n"
        u"GE\n"
        u"GE\n");

    return true;
}

test_constexpr bool test_segment_cursor_word_only()
{
    // "N,"+"Regional Indicator: AC+BE+W"+" GE"
    // Only words: "N|GE"
    std::string_view    str   =  "N,\U0001F1E6\U0001F1E8\U0001F1E7\U0001F1EA\U0001F1FC GE";
    std::u16string_view str16 = u"N,\U0001F1E6\U0001F1E8\U0001F1E7\U0001F1EA\U0001F1FC GE";

    // UTF-8

    auto range = una::ranges::word_only::utf8_view{str};

    std::string result;
    for (std::size_t i = 0; i <= str.size(); ++i)
    {
        result += *range.cursor(str.begin() + static_cast<std::ptrdiff_t>(i));
        result += '\n';
    }

    TESTX(*range.cursor(str.begin()) == "N");
    TESTX(*range.cursor(str.end()) == "GE");

    TESTX(result ==
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "GE\n"
        "GE\n");

    // UTF-16

    auto range16 = una::ranges::word_only::utf16_view{str16};

    std::u16string result16;
    for (std::size_t i = 0; i <= str16.size(); ++i)
    {
        result16 += *range16.cursor(str16.begin() + static_cast<std::ptrdiff_t>(i));
        result16 += '\n';
    }

    TESTX(*range16.cursor(str16.begin()) == u"N");
    TESTX(*range16.cursor(str16.end()) == u"GE");

    TESTX(result16 ==
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"GE\n"
        u"GE\n");

    return true;
}

test_constexpr bool test2_segment_cursor_grapheme()
{
    // "N,"+"Emoji: Extended_Pictographic+Extend*3+ZWJ+Extended_Pictographic"+" GE"
    // Graphemes: "N|,|Emoji| |G|E"
    std::string_view    str   =  "N,\u2122\u0300\u0300\u0300\u200D\u2122 GE";
    std::u16string_view str16 = u"N,\u2122\u0300\u0300\u0300\u200D\u2122 GE";

    // UTF-8

    auto range = una::ranges::grapheme::utf8_view{str};

    std::string result;
    for (std::size_t i = 0; i <= str.size(); ++i)
    {
        result += *range.cursor(str.begin() + static_cast<std::ptrdiff_t>(i));
        result += '\n';
    }

    TESTX(*range.cursor(str.begin()) == "N");
    TESTX(*range.cursor(str.end()) == "E");

    TESTX(result ==
        "N\n"
        "N\n"
        ",\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        " \n"
        "G\n"
        "E\n");

    // UTF-16

    auto range16 = una::ranges::grapheme::utf16_view{str16};

    std::u16string result16;
    for (std::size_t i = 0; i <= str16.size(); ++i)
    {
        result16 += *range16.cursor(str16.begin() + static_cast<std::ptrdiff_t>(i));
        result16 += '\n';
    }

    TESTX(*range16.cursor(str16.begin()) == u"N");
    TESTX(*range16.cursor(str16.end()) == u"E");

    TESTX(result16 ==
        u"N\n"
        u"N\n"
        u",\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u" \n"
        u"G\n"
        u"E\n");

    return true;
}

test_constexpr bool test2_segment_cursor_word()
{
    // "N,"+"Emoji: Extended_Pictographic+Extend*3+ZWJ+Extended_Pictographic"+" GE"
    // Words: "N|,|Emoji| |GE"
    std::string_view    str   =  "N,\u2122\u0300\u0300\u0300\u200D\u2122 GE";
    std::u16string_view str16 = u"N,\u2122\u0300\u0300\u0300\u200D\u2122 GE";

    // UTF-8

    auto range = una::ranges::word::utf8_view{str};

    std::string result;
    for (std::size_t i = 0; i <= str.size(); ++i)
    {
        result += *range.cursor(str.begin() + static_cast<std::ptrdiff_t>(i));
        result += '\n';
    }

    TESTX(*range.cursor(str.begin()) == "N");
    TESTX(*range.cursor(str.end()) == "GE");

    TESTX(result ==
        "N\n"
        "N\n"
        ",\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        "\u2122\u0300\u0300\u0300\u200D\u2122\n"
        " \n"
        "GE\n"
        "GE\n");

    // UTF-16

    auto range16 = una::ranges::word::utf16_view{str16};

    std::u16string result16;
    for (std::size_t i = 0; i <= str16.size(); ++i)
    {
        result16 += *range16.cursor(str16.begin() + static_cast<std::ptrdiff_t>(i));
        result16 += '\n';
    }

    TESTX(*range16.cursor(str16.begin()) == u"N");
    TESTX(*range16.cursor(str16.end()) == u"GE");

    TESTX(result16 ==
        u"N\n"
        u"N\n"
        u",\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u"\u2122\u0300\u0300\u0300\u200D\u2122\n"
        u" \n"
        u"GE\n"
        u"GE\n");

    return true;
}

test_constexpr bool test2_segment_cursor_word_only()
{
    // "N,"+"Emoji: Extended_Pictographic+Extend*3+ZWJ+Extended_Pictographic"+" GE"
    // Only words: "N|GE"
    std::string_view    str   =  "N,\u2122\u0300\u0300\u0300\u200D\u2122 GE";
    std::u16string_view str16 = u"N,\u2122\u0300\u0300\u0300\u200D\u2122 GE";

    // UTF-8

    auto range = una::ranges::word_only::utf8_view{str};

    std::string result;
    for (std::size_t i = 0; i <= str.size(); ++i)
    {
        result += *range.cursor(str.begin() + static_cast<std::ptrdiff_t>(i));
        result += '\n';
    }

    TESTX(*range.cursor(str.begin()) == "N");
    TESTX(*range.cursor(str.end()) == "GE");

    TESTX(result ==
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "N\n"
        "GE\n"
        "GE\n");

    // UTF-16

    auto range16 = una::ranges::word_only::utf16_view{str16};

    std::u16string result16;
    for (std::size_t i = 0; i <= str16.size(); ++i)
    {
        result16 += *range16.cursor(str16.begin() + static_cast<std::ptrdiff_t>(i));
        result16 += '\n';
    }

    TESTX(*range16.cursor(str16.begin()) == u"N");
    TESTX(*range16.cursor(str16.end()) == u"GE");

    TESTX(result16 ==
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"N\n"
        u"GE\n"
        u"GE\n");

    return true;
}
