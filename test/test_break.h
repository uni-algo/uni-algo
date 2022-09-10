/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

void test_break_grapheme()
{
    std::ifstream input("GraphemeBreakTest.txt", std::ios::binary);
    TESTX(input.is_open());
    // Latest version:
    // ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/GraphemeBreakTest.txt
    // Other versions:
    // ftp://ftp.unicode.org/Public/12.0.0/ucd/auxiliary/GraphemeBreakTest.txt
    // ftp://ftp.unicode.org/Public/12.1.0/ucd/auxiliary/GraphemeBreakTest.txt
    // etc.

    size_t count_lines = 0;

    std::string line;
    while (std::getline(input, line))
    {
        count_lines++;

        if (line.empty() || line[0] == '#' || line[0] == '@')
            continue;

        std::u32string line32 = uni::utf8to32u(line);

        std::u32string string_without_breaks, string_with_breaks;

        for (size_t i = 0; i < line.size(); i++)
        {
            if (i == 0 || line32[i] == 0x00D7 || line32[i] == 0x00F7 || line32[i] == '#')
            {
                if (line32[i] == '#')
                    break;

                //if (line32[i] == 0x00D7)
                //	string_with_breaks += std::u32string(1, 0x00D7);
                if (line32[i] == 0x00F7)
                    string_with_breaks += std::u32string(1, 0x00F7);

                if (line32[i] == 0x00D7 || line32[i] == 0x00F7)
                    i++;

                uint32_t c = (uint32_t)strtoul(uni::utf32to8(line32.c_str()+i).c_str(), 0, 16);

                if (c != 0)
                {
                    string_without_breaks += std::u32string(1, c);
                    string_with_breaks += std::u32string(1, c);
                }
            }
        }

        // Test here

        // UTF-8
        {
            // FORWARD
            {
            std::string string_with_breaks_utf8 = uni::utf32to8(string_without_breaks);

            auto view = uni::ranges::grapheme::utf8_view{string_with_breaks_utf8};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = view.begin(); it != view.end(); ++it)
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf8.begin()));

            // Insert brakes
            std::size_t expand = 0;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                string_with_breaks_utf8.insert(vec[i] + expand, "\xC3\xB7");
                expand += 2;
            }
            string_with_breaks_utf8 += "\xC3\xB7";

            //std::u32string string_with_breaks_utf32 = uni::utf8to32u(string_with_breaks_utf8);

            TESTX(string_with_breaks_utf8 == uni::utf32to8(string_with_breaks));
            }
            // REVERSE
            {
            std::string string_with_breaks_utf8 = uni::utf32to8(string_without_breaks);

            auto view = uni::ranges::grapheme::utf8_view{string_with_breaks_utf8};

            // Collect reverse brakes
            std::vector<std::size_t> vec;
            for (auto it = view.end(); it != view.begin();)
            {
                --it;
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf8.begin()));
            }
            std::reverse(vec.begin(), vec.end());

            // Insert brakes
            std::size_t expand = 0;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                string_with_breaks_utf8.insert(vec[i] + expand, "\xC3\xB7");
                expand += 2;
            }
            string_with_breaks_utf8 += "\xC3\xB7";

            TESTX(string_with_breaks_utf8 == uni::utf32to8(string_with_breaks));
            }
        }
        // UTF-16
        {
            // FORWARD
            {
            std::u16string string_with_breaks_utf16 = uni::utf32to16u(string_without_breaks);

            auto view = uni::ranges::grapheme::utf16_view{string_with_breaks_utf16};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = view.begin(); it != view.end(); ++it)
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf16.begin()));

            // Insert brakes
            std::size_t expand = 0;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                string_with_breaks_utf16.insert(vec[i] + expand, u"\x00F7");
                expand += 1;
            }
            string_with_breaks_utf16 += u"\x00F7";

            TESTX(string_with_breaks_utf16 == uni::utf32to16u(string_with_breaks));
            }
            // REVERSE
            {
            std::u16string string_with_breaks_utf16 = uni::utf32to16u(string_without_breaks);

            auto view = uni::ranges::grapheme::utf16_view{string_with_breaks_utf16};

            // Collect reverse brakes
            std::vector<std::size_t> vec;
            for (auto it = view.end(); it != view.begin();)
            {
                --it;
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf16.begin()));
            }
            std::reverse(vec.begin(), vec.end());

            // Insert brakes
            std::size_t expand = 0;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                string_with_breaks_utf16.insert(vec[i] + expand, u"\x00F7");
                expand += 1;
            }
            string_with_breaks_utf16 += u"\x00F7";

            TESTX(string_with_breaks_utf16 == uni::utf32to16u(string_with_breaks));
            }
        }
    }
}

void test_break_word()
{
    std::ifstream input("WordBreakTest.txt", std::ios::binary);
    TESTX(input.is_open());
    // Latest version:
    // ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/WordBreakTest.txt
    // Other versions:
    // ftp://ftp.unicode.org/Public/12.0.0/ucd/auxiliary/WordBreakTest.txt
    // ftp://ftp.unicode.org/Public/12.1.0/ucd/auxiliary/WordBreakTest.txt
    // etc.

    size_t count_lines = 0;

    std::string line;
    while (std::getline(input, line))
    {
        count_lines++;

        if (line.empty() || line[0] == '#' || line[0] == '@')
            continue;

        std::u32string line32 = uni::utf8to32u(line);

        std::u32string string_without_breaks, string_with_breaks;

        for (size_t i = 0; i < line.size(); i++)
        {
            if (i == 0 || line32[i] == 0x00D7 || line32[i] == 0x00F7 || line32[i] == '#')
            {
                if (line32[i] == '#')
                    break;

                //if (line32[i] == 0x00D7)
                //	string_with_breaks += std::u32string(1, 0x00D7);
                if (line32[i] == 0x00F7)
                    string_with_breaks += std::u32string(1, 0x00F7);

                if (line32[i] == 0x00D7 || line32[i] == 0x00F7)
                    i++;

                uint32_t c = (uint32_t)strtoul(uni::utf32to8(line32.c_str()+i).c_str(), 0, 16);

                if (c != 0)
                {
                    string_without_breaks += std::u32string(1, c);
                    string_with_breaks += std::u32string(1, c);
                }
            }
        }

        // Test here

        // UTF-8
        {
            std::string string_with_breaks_utf8 = uni::utf32to8(string_without_breaks);

            auto view = uni::ranges::word::utf8_view{string_with_breaks_utf8};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = view.begin(); it != view.end(); ++it)
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf8.begin()));

            // Insert brakes
            std::size_t expand = 0;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                string_with_breaks_utf8.insert(vec[i] + expand, "\xC3\xB7");
                expand += 2;
            }
            string_with_breaks_utf8 += "\xC3\xB7";

            TESTX(string_with_breaks_utf8 == uni::utf32to8(string_with_breaks));
        }

        // UTF-16
        {
            std::u16string string_with_breaks_utf16 = uni::utf32to16u(string_without_breaks);

            auto view = uni::ranges::word::utf16_view{string_with_breaks_utf16};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = view.begin(); it != view.end(); ++it)
                vec.push_back(static_cast<std::size_t>(it.begin() - string_with_breaks_utf16.begin()));

            // Insert brakes
            std::size_t expand = 0;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                string_with_breaks_utf16.insert(vec[i] + expand, u"\x00F7");
                expand += 1;
            }
            string_with_breaks_utf16 += u"\x00F7";

            TESTX(string_with_breaks_utf16 == uni::utf32to16u(string_with_breaks));
        }
    }
}

std::size_t test_break_count_words(std::string_view str)
{
    auto view = uni::ranges::word::utf8_view{str};

    return static_cast<std::size_t>(std::distance(view.begin(), view.end()));
}

std::size_t test_break_count_only_words(std::string_view str)
{
    auto view = uni::ranges::word_only::utf8_view{str};

    return static_cast<std::size_t>(std::distance(view.begin(), view.end()));
}

std::size_t test_break_count_words16(std::u16string_view str)
{
    auto view = uni::ranges::word::utf16_view{str};

    return static_cast<std::size_t>(std::distance(view.begin(), view.end()));
}

std::size_t test_break_count_only_words16(std::u16string_view str)
{
    auto view = uni::ranges::word_only::utf16_view{str};

    return static_cast<std::size_t>(std::distance(view.begin(), view.end()));
}

void test_break_word_corner_cases()
{
    // Word indicator must work properly in all the cases.
    // The behavior is the same as in ICU.
    // Extend + ALetter + Extend
    TESTX(test_break_count_words("\xCC\x82\x77\xCC\x82") == 2);
    TESTX(test_break_count_only_words("\xCC\x82\x77\xCC\x82") == 1);
    TESTX(test_break_count_words16(u"\x0302\x0077\x0302") == 2);
    TESTX(test_break_count_only_words16(u"\x0302\x0077\x0302") == 1);
    // Hebrew_Letter + Single_Quote
    TESTX(test_break_count_words("\xD7\x90\x27") == 1);
    TESTX(test_break_count_only_words("\xD7\x90\x27") == 1);
    TESTX(test_break_count_words16(u"\x05D0\x0027") == 1);
    TESTX(test_break_count_only_words16(u"\x05D0\x0027") == 1);
    // AHLetter + ExtendNumLet
    TESTX(test_break_count_words("\x77\x5F") == 1);
    TESTX(test_break_count_only_words("\x77\x5F") == 1);
    TESTX(test_break_count_words16(u"\x0077\x005F") == 1);
    TESTX(test_break_count_only_words16(u"\x0077\x005F") == 1);
    // ExtendNumLet + AHLetter
    TESTX(test_break_count_words("\x5F\x77") == 1);
    TESTX(test_break_count_only_words("\x5F\x77") == 1);
    TESTX(test_break_count_words16(u"\x005F\x0077") == 1);
    TESTX(test_break_count_only_words16(u"\x005F\x0077") == 1);

    // Properly handle corner cases with Extended_Pictographic in word properties.
    // Extended_Pictographic intersects with ALetter with some code points in word properties.
    // Note that Extended_Pictographic never intersects with something in grapheme properties.
    // The behavior is the same as in ICU.
    TESTX(test_break_count_words("\xE2\x80\x8D\xE2\x84\xB9\x77") == 1);
    TESTX(test_break_count_words("\xE2\x84\xB9\xE2\x80\x8D\x77") == 1);
    TESTX(test_break_count_words16(u"\x200D\x2139\x0077") == 1);
    TESTX(test_break_count_words16(u"\x2139\x200D\x0077") == 1);
    TESTX(test_break_count_words("\xE2\x80\x8D\xE2\x84\xB9") == 1);
    TESTX(test_break_count_words("\xE2\x84\xB9\xE2\x80\x8D") == 1);
    TESTX(test_break_count_words16(u"\x200D\x2139") == 1);
    TESTX(test_break_count_words16(u"\x2139\x200D") == 1);

    // Repeat previous tests with count_only_words just in case.
    TESTX(test_break_count_only_words("\xE2\x80\x8D\xE2\x84\xB9\x77") == 0);
    TESTX(test_break_count_only_words("\xE2\x84\xB9\xE2\x80\x8D\x77") == 0);
    TESTX(test_break_count_only_words16(u"\x200D\x2139\x0077") == 0);
    TESTX(test_break_count_only_words16(u"\x2139\x200D\x0077") == 0);
    TESTX(test_break_count_only_words("\xE2\x80\x8D\xE2\x84\xB9") == 0);
    TESTX(test_break_count_only_words("\xE2\x84\xB9\xE2\x80\x8D") == 0);
    TESTX(test_break_count_only_words16(u"\x200D\x2139") == 0);
    TESTX(test_break_count_only_words16(u"\x2139\x200D") == 0);
}

void test_break_word_prop()
{
    std::string result;
    std::size_t count_words = 0;
    //std::size_t count_punct = 0;
    auto view = uni::ranges::word::utf8_view{std::string_view{"Tes't. 123,5 7test,test7 \xE3\x83\x90\xE3\x82\xAB \xE6\xA8\xB1 \xF0\x9F\x98\xBA"
        "\xF0\x9F\x8F\xB4\xF3\xA0\x81\xA7\xF3\xA0\x81\xA2\xF3\xA0\x81\xB7\xF3\xA0\x81\xAC\xF3\xA0\x81\xB3\xF3\xA0\x81\xBF"}}; // Flag: Wales
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
        if (it.is_word())        ++count_words;
        //if (it.is_punctuation()) ++count_punct;
    }
    TESTX(result == "Letter: Tes't\n"
                    "Number: 123,5\n"
                    "Letter: 7test\n"
                    "Letter: test7\n"
                    "Kana  : \xE3\x83\x90\xE3\x82\xAB\n" // Baka Katakana
                    "Ideo  : \xE6\xA8\xB1\n" // Sakura Kanji
                    "Emoji : \xF0\x9F\x98\xBA\n" // Smiling Cat Face with Open Mouth Emoji
                    "Emoji : \xF0\x9F\x8F\xB4\xF3\xA0\x81\xA7\xF3\xA0\x81\xA2\xF3\xA0\x81\xB7\xF3\xA0\x81\xAC\xF3\xA0\x81\xB3\xF3\xA0\x81\xBF\n"); // Flag: Wales
    TESTX(count_words == 6);
    //TESTX(count_punct == 7);
    //std::cout << result << '\n';

    std::u16string result16;
    std::size_t count_words16 = 0;
    //count_punct16 = 0;
    auto view16 = uni::ranges::word::utf16_view{std::u16string_view{u"Tes't. 123,5 7test,test7 \x30D0\x30AB \x6A31 \xD83D\xDE3A"
        u"\xD83C\xDFF4\xDB40\xDC67\xDB40\xDC62\xDB40\xDC77\xDB40\xDC6C\xDB40\xDC73\xDB40\xDC7F"}}; // Flag: Wales
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
        if (it.is_word())        ++count_words16;
        //if (it.is_punctuation()) ++count_punct16;
    }
    TESTX(result16 == u"Letter: Tes't\n"
                      u"Number: 123,5\n"
                      u"Letter: 7test\n"
                      u"Letter: test7\n"
                      u"Kana  : \x30D0\x30AB\n" // Baka Katakana
                      u"Ideo  : \x6A31\n" // Sakura Kanji
                      u"Emoji : \xD83D\xDE3A\n" // Smiling Cat Face with Open Mouth Emoji
                      u"Emoji : \xD83C\xDFF4\xDB40\xDC67\xDB40\xDC62\xDB40\xDC77\xDB40\xDC6C\xDB40\xDC73\xDB40\xDC7F\n"); // Flag: Wales
    TESTX(count_words16 == 6);
    //TESTX(count_punct16 == 7);
}
