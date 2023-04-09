/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

bool test_segment_grapheme()
{
    std::ifstream input("GraphemeBreakTest.txt", std::ios::binary);
    TESTX(input.is_open());
    // Latest version:
    // https://www.unicode.org/Public/UNIDATA/auxiliary/GraphemeBreakTest.txt
    // Other versions:
    // https://www.unicode.org/Public/12.0.0/ucd/auxiliary/GraphemeBreakTest.txt
    // https://www.unicode.org/Public/12.1.0/ucd/auxiliary/GraphemeBreakTest.txt
    // etc.

    std::string line;
    while (std::getline(input, line))
    {
        if (line.empty() || line[0] == '#' || line[0] == '@')
            continue;

        std::u32string line32 = una::utf8to32u(line);

        std::u32string string_without_breaks, string_with_breaks;

        for (size_t i = 0; i < line.size(); i++)
        {
            if (i == 0 || line32[i] == 0x00D7 || line32[i] == 0x00F7 || line32[i] == '#')
            {
                if (line32[i] == '#')
                    break;

                //if (line32[i] == 0x00D7)
                //	string_with_breaks += std::u32string{0x00D7};
                if (line32[i] == 0x00F7)
                    string_with_breaks += std::u32string{0x00F7};

                if (line32[i] == 0x00D7 || line32[i] == 0x00F7)
                    i++;

                uint32_t c = (uint32_t)strtoul(una::utf32to8(line32.c_str()+i).c_str(), 0, 16);

                if (c != 0)
                {
                    string_without_breaks += std::u32string{c};
                    string_with_breaks += std::u32string{c};
                }
            }
        }

        // Test here

        // UTF-8

        // FORWARD
        {
            std::string string_with_breaks_utf8 = una::utf32to8(string_without_breaks);

            auto view = una::ranges::grapheme::utf8_view{string_with_breaks_utf8};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = view.begin(); it != view.end(); ++it)
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf8.begin()));
            vec.push_back(string_with_breaks_utf8.size());

            // Insert brakes
            for (std::size_t i = 0, expand = 0; i < vec.size(); ++i, expand += 2)
                string_with_breaks_utf8.insert(vec[i] + expand, "\xC3\xB7");

            TESTX(string_with_breaks_utf8 == una::utf32to8(string_with_breaks));
        }
        // REVERSE
        {
            std::string string_with_breaks_utf8 = una::utf32to8(string_without_breaks);

            auto view = una::ranges::grapheme::utf8_view{string_with_breaks_utf8};

            // Collect reverse brakes
            std::vector<std::size_t> vec;
            vec.push_back(string_with_breaks_utf8.size());
            for (auto it = view.end(); it != view.begin();)
            {
                --it;
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf8.begin()));
            }
            std::reverse(vec.begin(), vec.end());

            // Insert brakes
            for (std::size_t i = 0, expand = 0; i < vec.size(); ++i, expand += 2)
                string_with_breaks_utf8.insert(vec[i] + expand, "\xC3\xB7");

            TESTX(string_with_breaks_utf8 == una::utf32to8(string_with_breaks));
        }

        // UTF-16

        // FORWARD
        {
            std::u16string string_with_breaks_utf16 = una::utf32to16u(string_without_breaks);

            auto view = una::ranges::grapheme::utf16_view{string_with_breaks_utf16};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = view.begin(); it != view.end(); ++it)
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf16.begin()));
            vec.push_back(string_with_breaks_utf16.size());

            // Insert brakes

            // NOTE: Use (... 1, 0x00F7) instead of (... u"\x00F7") here and below to dodge GCC 12 bug
            // For details see commit: 4acb1d447e883855fb3af691247780b6c8f6e30d

            for (std::size_t i = 0, expand = 0; i < vec.size(); ++i, expand += 1)
                string_with_breaks_utf16.insert(vec[i] + expand, 1, 0x00F7);

            TESTX(string_with_breaks_utf16 == una::utf32to16u(string_with_breaks));
        }
        // REVERSE
        {
            std::u16string string_with_breaks_utf16 = una::utf32to16u(string_without_breaks);

            auto view = una::ranges::grapheme::utf16_view{string_with_breaks_utf16};

            // Collect reverse brakes
            std::vector<std::size_t> vec;
            vec.push_back(string_with_breaks_utf16.size());
            for (auto it = view.end(); it != view.begin();)
            {
                --it;
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf16.begin()));
            }
            std::reverse(vec.begin(), vec.end());

            // Insert brakes
            for (std::size_t i = 0, expand = 0; i < vec.size(); ++i, expand += 1)
                string_with_breaks_utf16.insert(vec[i] + expand, 1, 0x00F7);

            TESTX(string_with_breaks_utf16 == una::utf32to16u(string_with_breaks));
        }
    }

    return true;
}

bool test_segment_word()
{
    std::ifstream input("WordBreakTest.txt", std::ios::binary);
    TESTX(input.is_open());
    // Latest version:
    // ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/WordBreakTest.txt
    // Other versions:
    // ftp://ftp.unicode.org/Public/12.0.0/ucd/auxiliary/WordBreakTest.txt
    // ftp://ftp.unicode.org/Public/12.1.0/ucd/auxiliary/WordBreakTest.txt
    // etc.

    std::string line;
    while (std::getline(input, line))
    {
        if (line.empty() || line[0] == '#' || line[0] == '@')
            continue;

        std::u32string line32 = una::utf8to32u(line);

        std::u32string string_without_breaks, string_with_breaks;

        for (size_t i = 0; i < line.size(); i++)
        {
            if (i == 0 || line32[i] == 0x00D7 || line32[i] == 0x00F7 || line32[i] == '#')
            {
                if (line32[i] == '#')
                    break;

                //if (line32[i] == 0x00D7)
                //	string_with_breaks += std::u32string{0x00D7};
                if (line32[i] == 0x00F7)
                    string_with_breaks += std::u32string{0x00F7};

                if (line32[i] == 0x00D7 || line32[i] == 0x00F7)
                    i++;

                uint32_t c = (uint32_t)strtoul(una::utf32to8(line32.c_str()+i).c_str(), 0, 16);

                if (c != 0)
                {
                    string_without_breaks += std::u32string{c};
                    string_with_breaks += std::u32string{c};
                }
            }
        }

        // Test here

        // UTF-8

        // FORWARD
        {
            std::string string_with_breaks_utf8 = una::utf32to8(string_without_breaks);

            auto view = una::ranges::word::utf8_view{string_with_breaks_utf8};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = view.begin(); it != view.end(); ++it)
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf8.begin()));
            vec.push_back(string_with_breaks_utf8.size());

            // Insert brakes
            for (std::size_t i = 0, expand = 0; i < vec.size(); ++i, expand += 2)
                string_with_breaks_utf8.insert(vec[i] + expand, "\xC3\xB7");

            TESTX(string_with_breaks_utf8 == una::utf32to8(string_with_breaks));
        }
        // REVERSE
        {
            std::string string_with_breaks_utf8 = una::utf32to8(string_without_breaks);

            auto view = una::ranges::word::utf8_view{string_with_breaks_utf8};

            // Collect reverse brakes
            std::vector<std::size_t> vec;
            vec.push_back(string_with_breaks_utf8.size());
            for (auto it = view.end(); it != view.begin();)
            {
                --it;
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf8.begin()));
            }
            std::reverse(vec.begin(), vec.end());

            // Insert brakes
            for (std::size_t i = 0, expand = 0; i < vec.size(); ++i, expand += 2)
                string_with_breaks_utf8.insert(vec[i] + expand, "\xC3\xB7");

            TESTX(string_with_breaks_utf8 == una::utf32to8(string_with_breaks));
        }

        // UTF-16

        // FORWARD
        {
            std::u16string string_with_breaks_utf16 = una::utf32to16u(string_without_breaks);

            auto view = una::ranges::word::utf16_view{string_with_breaks_utf16};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = view.begin(); it != view.end(); ++it)
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf16.begin()));
            vec.push_back(string_with_breaks_utf16.size());

            // Insert brakes
            for (std::size_t i = 0, expand = 0; i < vec.size(); ++i, expand += 1)
                string_with_breaks_utf16.insert(vec[i] + expand, 1, 0x00F7);

            TESTX(string_with_breaks_utf16 == una::utf32to16u(string_with_breaks));
        }
        // REVERSE
        {
            std::u16string string_with_breaks_utf16 = una::utf32to16u(string_without_breaks);

            auto view = una::ranges::word::utf16_view{string_with_breaks_utf16};

            // Collect reverse brakes
            std::vector<std::size_t> vec;
            vec.push_back(string_with_breaks_utf16.size());
            for (auto it = view.end(); it != view.begin();)
            {
                --it;
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf16.begin()));
            }
            std::reverse(vec.begin(), vec.end());

            // Insert brakes
            for (std::size_t i = 0, expand = 0; i < vec.size(); ++i, expand += 1)
                string_with_breaks_utf16.insert(vec[i] + expand, 1, 0x00F7);

            TESTX(string_with_breaks_utf16 == una::utf32to16u(string_with_breaks));
        }
    }

    return true;
}

test_constexpr std::size_t test_segment_count_words(std::string_view str)
{
    auto view = una::ranges::word::utf8_view{str};

    return static_cast<std::size_t>(std::distance(view.begin(), view.end()));
}

test_constexpr std::size_t test_segment_count_only_words(std::string_view str)
{
    auto view = una::ranges::word_only::utf8_view{str};

    return static_cast<std::size_t>(std::distance(view.begin(), view.end()));
}

test_constexpr std::size_t test_segment_count_words16(std::u16string_view str)
{
    auto view = una::ranges::word::utf16_view{str};

    return static_cast<std::size_t>(std::distance(view.begin(), view.end()));
}

test_constexpr std::size_t test_segment_count_only_words16(std::u16string_view str)
{
    auto view = una::ranges::word_only::utf16_view{str};

    return static_cast<std::size_t>(std::distance(view.begin(), view.end()));
}

test_constexpr bool test_segment_word_corner_cases()
{
    // Word indicator must work properly in all the cases.
    // The behavior is the same as in ICU.
    // Extend + ALetter + Extend
    TESTX(test_segment_count_words("\xCC\x82\x77\xCC\x82") == 2);
    TESTX(test_segment_count_only_words("\xCC\x82\x77\xCC\x82") == 1);
    TESTX(test_segment_count_words16(u"\x0302\x0077\x0302") == 2);
    TESTX(test_segment_count_only_words16(u"\x0302\x0077\x0302") == 1);
    // Hebrew_Letter + Single_Quote
    TESTX(test_segment_count_words("\xD7\x90\x27") == 1);
    TESTX(test_segment_count_only_words("\xD7\x90\x27") == 1);
    TESTX(test_segment_count_words16(u"\x05D0\x0027") == 1);
    TESTX(test_segment_count_only_words16(u"\x05D0\x0027") == 1);
    // AHLetter + ExtendNumLet
    TESTX(test_segment_count_words("\x77\x5F") == 1);
    TESTX(test_segment_count_only_words("\x77\x5F") == 1);
    TESTX(test_segment_count_words16(u"\x0077\x005F") == 1);
    TESTX(test_segment_count_only_words16(u"\x0077\x005F") == 1);
    // ExtendNumLet + AHLetter
    TESTX(test_segment_count_words("\x5F\x77") == 1);
    TESTX(test_segment_count_only_words("\x5F\x77") == 1);
    TESTX(test_segment_count_words16(u"\x005F\x0077") == 1);
    TESTX(test_segment_count_only_words16(u"\x005F\x0077") == 1);

    // Properly handle corner cases with Extended_Pictographic in word properties.
    // Extended_Pictographic intersects with ALetter with some code points in word properties.
    // Note that Extended_Pictographic never intersects with something in grapheme properties.
    // The behavior is the same as in ICU.
    TESTX(test_segment_count_words("\xE2\x80\x8D\xE2\x84\xB9\x77") == 1);
    TESTX(test_segment_count_words("\xE2\x84\xB9\xE2\x80\x8D\x77") == 1);
    TESTX(test_segment_count_words16(u"\x200D\x2139\x0077") == 1);
    TESTX(test_segment_count_words16(u"\x2139\x200D\x0077") == 1);
    TESTX(test_segment_count_words("\xE2\x80\x8D\xE2\x84\xB9") == 1);
    TESTX(test_segment_count_words("\xE2\x84\xB9\xE2\x80\x8D") == 1);
    TESTX(test_segment_count_words16(u"\x200D\x2139") == 1);
    TESTX(test_segment_count_words16(u"\x2139\x200D") == 1);

    // Repeat previous tests with count_only_words just in case.
    // If a word contains Extended_Pictographic it promotes to emoji and must be skipped.
    TESTX(test_segment_count_only_words("\xE2\x80\x8D\xE2\x84\xB9\x77") == 0);
    TESTX(test_segment_count_only_words("\xE2\x84\xB9\xE2\x80\x8D\x77") == 0);
    TESTX(test_segment_count_only_words16(u"\x200D\x2139\x0077") == 0);
    TESTX(test_segment_count_only_words16(u"\x2139\x200D\x0077") == 0);
    TESTX(test_segment_count_only_words("\xE2\x80\x8D\xE2\x84\xB9") == 0);
    TESTX(test_segment_count_only_words("\xE2\x84\xB9\xE2\x80\x8D") == 0);
    TESTX(test_segment_count_only_words16(u"\x200D\x2139") == 0);
    TESTX(test_segment_count_only_words16(u"\x2139\x200D") == 0);

    return true;
}

test_constexpr bool test_segment_word_prop()
{
    // FORWARD

    // UTF-8

    std::string result;
    std::size_t count_words = 0;
    std::size_t count_punct = 0;
    std::size_t count_space = 0;
    std::string_view sv = "Tes't. 123,5 7test,test7 \xE3\x83\x90\xE3\x82\xAB \xE6\xA8\xB1 \xF0\x9F\x98\xBA"
        "\xF0\x9F\x8F\xB4\xF3\xA0\x81\xA7\xF3\xA0\x81\xA2\xF3\xA0\x81\xB7\xF3\xA0\x81\xAC\xF3\xA0\x81\xB3\xF3\xA0\x81\xBF"; // Flag: Wales
    auto view = una::ranges::word::utf8_view{sv};
    for (auto it = view.begin(); it != view.end(); ++it)
    {
        if (it.is_emoji())
            result += "Emoji : " + std::string{*it} + '\n';
        if (it.is_word_letter())
            result += "Letter: " + std::string{*it} + '\n';
        if (it.is_word_number())
            result += "Number: " + std::string{*it} + '\n';
        if (it.is_word_kana())
            result += "Kana  : " + std::string{*it} + '\n';
        if (it.is_word_ideographic())
            result += "Ideo  : " + std::string{*it} + '\n';
        //if (it.is_punctuation())
        //    result += "Punct: " + std::string{*it} + '\n';
        if (it.is_word())        count_words++;
        if (it.is_punctuation()) count_punct++;
        if (it.is_segspace())    count_space++;
    }
    TESTX(result == "Letter: Tes't\n"
                    "Number: 123,5\n"
                    "Letter: 7test\n"
                    "Letter: test7\n"
                    "Kana  : \xE3\x83\x90\xE3\x82\xAB\n" // Katakana: Baka
                    "Ideo  : \xE6\xA8\xB1\n" // Kanji: Sakura
                    "Emoji : \xF0\x9F\x98\xBA\n" // Emoji: Smiling Cat Face with Open Mouth
                    "Emoji : \xF0\x9F\x8F\xB4\xF3\xA0\x81\xA7\xF3\xA0\x81\xA2\xF3\xA0\x81\xB7\xF3\xA0\x81\xAC\xF3\xA0\x81\xB3\xF3\xA0\x81\xBF\n"); // Flag: Wales
    TESTX(count_words == 6);
    TESTX(count_punct == 2);
    TESTX(count_space == 5);
    //std::cout << result << '\n';

    // UTF-16

    std::u16string result16;
    std::size_t count_words16 = 0;
    std::size_t count_punct16 = 0;
    std::size_t count_space16 = 0;
    std::u16string_view sv16 = u"Tes't. 123,5 7test,test7 \x30D0\x30AB \x6A31 \xD83D\xDE3A"
        u"\xD83C\xDFF4\xDB40\xDC67\xDB40\xDC62\xDB40\xDC77\xDB40\xDC6C\xDB40\xDC73\xDB40\xDC7F"; // Flag: Wales
    auto view16 = una::ranges::word::utf16_view{sv16};
    for (auto it = view16.begin(); it != view16.end(); ++it)
    {
        if (it.is_emoji())
            result16 += u"Emoji : " + std::u16string{*it} + u'\n';
        if (it.is_word_letter())
            result16 += u"Letter: " + std::u16string{*it} + u'\n';
        if (it.is_word_number())
            result16 += u"Number: " + std::u16string{*it} + u'\n';
        if (it.is_word_kana())
            result16 += u"Kana  : " + std::u16string{*it} + u'\n';
        if (it.is_word_ideographic())
            result16 += u"Ideo  : " + std::u16string{*it} + u'\n';
        //if (it.is_punctuation())
        //    result16 += u"Punct: " + std::u16string{*it} + u'\n';
        if (it.is_word())        count_words16++;
        if (it.is_punctuation()) count_punct16++;
        if (it.is_segspace())    count_space16++;
    }
    TESTX(result16 == u"Letter: Tes't\n"
                      u"Number: 123,5\n"
                      u"Letter: 7test\n"
                      u"Letter: test7\n"
                      u"Kana  : \x30D0\x30AB\n" // Katakana: Baka
                      u"Ideo  : \x6A31\n" // Kanji: Sakura
                      u"Emoji : \xD83D\xDE3A\n" // Emoji: Smiling Cat Face with Open Mouth
                      u"Emoji : \xD83C\xDFF4\xDB40\xDC67\xDB40\xDC62\xDB40\xDC77\xDB40\xDC6C\xDB40\xDC73\xDB40\xDC7F\n"); // Flag: Wales
    TESTX(count_words16 == 6);
    TESTX(count_punct16 == 2);
    TESTX(count_space16 == 5);

    // REVERSE

    // UTF-8

    result.clear();
    count_words = 0;
    count_punct = 0;
    count_space = 0;
    for (auto it = view.end(); it != view.begin();)
    {
        --it;

        if (it.is_emoji())
            result += "Emoji : " + std::string{*it} + '\n';
        if (it.is_word_letter())
            result += "Letter: " + std::string{*it} + '\n';
        if (it.is_word_number())
            result += "Number: " + std::string{*it} + '\n';
        if (it.is_word_kana())
            result += "Kana  : " + std::string{*it} + '\n';
        if (it.is_word_ideographic())
            result += "Ideo  : " + std::string{*it} + '\n';
        //if (it.is_punctuation())
        //    result += "Punct: " + std::string{*it} + '\n';
        if (it.is_word())        count_words++;
        if (it.is_punctuation()) count_punct++;
        if (it.is_segspace())    count_space++;
    }
    TESTX(result == "Emoji : \xF0\x9F\x8F\xB4\xF3\xA0\x81\xA7\xF3\xA0\x81\xA2\xF3\xA0\x81\xB7\xF3\xA0\x81\xAC\xF3\xA0\x81\xB3\xF3\xA0\x81\xBF\n" // Flag: Wales
                    "Emoji : \xF0\x9F\x98\xBA\n" // Emoji: Smiling Cat Face with Open Mouth
                    "Ideo  : \xE6\xA8\xB1\n" // Kanji: Sakura
                    "Kana  : \xE3\x83\x90\xE3\x82\xAB\n" // Katakana: Baka
                    "Letter: test7\n"
                    "Letter: 7test\n"
                    "Number: 123,5\n"
                    "Letter: Tes't\n");
    TESTX(count_words == 6);
    TESTX(count_punct == 2);
    TESTX(count_space == 5);
    //std::cout << result << '\n';

    // UTF-16

    result16.clear();
    count_words16 = 0;
    count_punct16 = 0;
    count_space16 = 0;
    for (auto it = view16.end(); it != view16.begin();)
    {
        --it;

        if (it.is_emoji())
            result16 += u"Emoji : " + std::u16string{*it} + u'\n';
        if (it.is_word_letter())
            result16 += u"Letter: " + std::u16string{*it} + u'\n';
        if (it.is_word_number())
            result16 += u"Number: " + std::u16string{*it} + u'\n';
        if (it.is_word_kana())
            result16 += u"Kana  : " + std::u16string{*it} + u'\n';
        if (it.is_word_ideographic())
            result16 += u"Ideo  : " + std::u16string{*it} + u'\n';
        //if (it.is_punctuation())
        //    result16 += u"Punct: " + std::u16string{*it} + u'\n';
        if (it.is_word())        count_words16++;
        if (it.is_punctuation()) count_punct16++;
        if (it.is_segspace())    count_space16++;
    }
    TESTX(result16 == u"Emoji : \xD83C\xDFF4\xDB40\xDC67\xDB40\xDC62\xDB40\xDC77\xDB40\xDC6C\xDB40\xDC73\xDB40\xDC7F\n" // Flag: Wales
                      u"Emoji : \xD83D\xDE3A\n" // Emoji: Smiling Cat Face with Open Mouth
                      u"Ideo  : \x6A31\n" // Kanji: Sakura
                      u"Kana  : \x30D0\x30AB\n" // Katakana: Baka
                      u"Letter: test7\n"
                      u"Letter: 7test\n"
                      u"Number: 123,5\n"
                      u"Letter: Tes't\n");
    TESTX(count_words16 == 6);
    TESTX(count_punct16 == 2);
    TESTX(count_space16 == 5);

    // TEST word_only view here too

    // FORWARD

    result.clear();
    for (std::string_view s : sv | una::views::word_only::utf8) { result += s; result += '|'; }
    TESTX(result == "Tes't|123,5|7test|test7|\xE3\x83\x90\xE3\x82\xAB|\xE6\xA8\xB1|");

    result16.clear();
    for (std::u16string_view s : sv16 | una::views::word_only::utf16) { result16 += s; result16 += u'|'; }
    TESTX(result16 == u"Tes't|123,5|7test|test7|\x30D0\x30AB|\x6A31|");

    // REVERSE

    result.clear();
    for (std::string_view s : sv | una::views::word_only::utf8 | una::views::reverse) { result += s; result += '|'; }
    TESTX(result == "\xE6\xA8\xB1|\xE3\x83\x90\xE3\x82\xAB|test7|7test|123,5|Tes't|");

    result16.clear();
    for (std::u16string_view s : sv16 | una::views::word_only::utf16 | una::views::reverse) { result16 += s; result16 += u'|'; }
    TESTX(result16 == u"\x6A31|\x30D0\x30AB|test7|7test|123,5|Tes't|");

    return true;
}

test_constexpr bool test_segment_bidi()
{
    // Skip constexpr test because uses std::list
#ifndef TEST_MODE_CONSTEXPR

    std::string_view sv = "Tes't. 123,5 7test,test7 \xE3\x83\x90\xE3\x82\xAB \xE6\xA8\xB1 \xF0\x9F\x98\xBA";
    std::u16string_view sv16 = u"Tes't. 123,5 7test,test7 \x30D0\x30AB \x6A31 \xD83D\xDE3A";

    // Use std::list for the bidirectional range test
    std::list<char> list = {sv.begin(), sv.end()};
    std::list<char16_t> list16 = {sv16.begin(), sv16.end()};

    std::string result;
    std::u16string result16;

    // ---------
    // TEST word
    // ---------

    auto view = una::ranges::word::utf8_view{list};
    auto view16 = una::ranges::word::utf16_view{list16};

    // FORWARD

    result.clear();
    for (auto it = view.begin(); it != view.end(); ++it) { result += std::string{it.begin(), it.end()}; result += '|'; }
    TESTX(result == "Tes't|.| |123,5| |7test|,|test7| |\xE3\x83\x90\xE3\x82\xAB| |\xE6\xA8\xB1| |\xF0\x9F\x98\xBA|");

    result16.clear();
    for (auto it = view16.begin(); it != view16.end(); ++it) { result16 += std::u16string{it.begin(), it.end()}; result16 += u'|'; }
    TESTX(result16 == u"Tes't|.| |123,5| |7test|,|test7| |\x30D0\x30AB| |\x6A31| |\xD83D\xDE3A|");

    // REVERSE

    result.clear();
    for (auto it = view.end(); it != view.begin();) { --it; result += std::string{it.begin(), it.end()}; result += '|'; }
    TESTX(result == "\xF0\x9F\x98\xBA| |\xE6\xA8\xB1| |\xE3\x83\x90\xE3\x82\xAB| |test7|,|7test| |123,5| |.|Tes't|");

    result16.clear();
    for (auto it = view16.end(); it != view16.begin();) { --it; result16 += std::u16string{it.begin(), it.end()}; result16 += u'|'; }
    TESTX(result16 == u"\xD83D\xDE3A| |\x6A31| |\x30D0\x30AB| |test7|,|7test| |123,5| |.|Tes't|");

    // --------------
    // TEST word_only
    // --------------

    auto vwwo = una::ranges::word_only::utf8_view{list};
    auto vwwo16 = una::ranges::word_only::utf16_view{list16};

    // FORWARD

    result.clear();
    for (auto it = vwwo.begin(); it != vwwo.end(); ++it) { result += std::string{it.begin(), it.end()}; result += '|'; }
    TESTX(result == "Tes't|123,5|7test|test7|\xE3\x83\x90\xE3\x82\xAB|\xE6\xA8\xB1|");

    result16.clear();
    for (auto it = vwwo16.begin(); it != vwwo16.end(); ++it) { result16 += std::u16string{it.begin(), it.end()}; result16 += u'|'; }
    TESTX(result16 == u"Tes't|123,5|7test|test7|\x30D0\x30AB|\x6A31|");

    // REVERSE

    result.clear();
    for (auto it = vwwo.end(); it != vwwo.begin();) { --it; result += std::string{it.begin(), it.end()}; result += '|'; }
    TESTX(result == "\xE6\xA8\xB1|\xE3\x83\x90\xE3\x82\xAB|test7|7test|123,5|Tes't|");

    result16.clear();
    for (auto it = vwwo16.end(); it != vwwo16.begin();) { --it; result16 += std::u16string{it.begin(), it.end()}; result16 += u'|'; }
    TESTX(result16 == u"\x6A31|\x30D0\x30AB|test7|7test|123,5|Tes't|");

    // -------------
    // TEST grapheme
    // -------------

    sv = "Tes't. \xE3\x83\x90\xE3\x82\xAB \xE6\xA8\xB1 \xF0\x9F\x98\xBA";
    sv16 = u"Tes't. \x30D0\x30AB \x6A31 \xD83D\xDE3A";
    list = {sv.begin(), sv.end()};
    list16 = {sv16.begin(), sv16.end()};
    auto vwgp = una::ranges::grapheme::utf8_view{list};
    auto vwgp16 = una::ranges::grapheme::utf16_view{list16};

    // FORWARD

    result.clear();
    for (auto it = vwgp.begin(); it != vwgp.end(); ++it) { result += std::string{it.begin(), it.end()}; result += '|'; }
    TESTX(result == "T|e|s|'|t|.| |\xE3\x83\x90|\xE3\x82\xAB| |\xE6\xA8\xB1| |\xF0\x9F\x98\xBA|");

    result16.clear();
    for (auto it = vwgp16.begin(); it != vwgp16.end(); ++it) { result16 += std::u16string{it.begin(), it.end()}; result16 += u'|'; }
    TESTX(result16 == u"T|e|s|'|t|.| |\x30D0|\x30AB| |\x6A31| |\xD83D\xDE3A|");

    // REVERSE

    result.clear();
    for (auto it = vwgp.end(); it != vwgp.begin();) { --it; result += std::string{it.begin(), it.end()}; result += '|'; }
    TESTX(result == "\xF0\x9F\x98\xBA| |\xE6\xA8\xB1| |\xE3\x82\xAB|\xE3\x83\x90| |.|t|'|s|e|T|");

    result16.clear();
    for (auto it = vwgp16.end(); it != vwgp16.begin();) { --it; result16 += std::u16string{it.begin(), it.end()}; result16 += u'|'; }
    TESTX(result16 == u"\xD83D\xDE3A| |\x6A31| |\x30AB|\x30D0| |.|t|'|s|e|T|");

#endif // TEST_MODE_CONSTEXPR

    return true;
}
