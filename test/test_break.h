/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <fstream>
#include <vector>

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
            std::string string_with_breaks_utf8 = uni::utf32to8(string_without_breaks);

            uni::breaks::grapheme::utf8 it_begin{string_with_breaks_utf8.cbegin(), string_with_breaks_utf8.cend()};
            uni::breaks::grapheme::utf8 it_end{string_with_breaks_utf8.cend(), string_with_breaks_utf8.cend()};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = it_begin; it != it_end; ++it)
                vec.push_back(static_cast<std::size_t>(it - it_begin));

            // Insert brakes
            std::size_t expand = 0;
            for (std::size_t i = 0; i < vec.size(); ++i)
            {
                string_with_breaks_utf8.insert(vec[i] + expand, "\xC3\xB7");
                expand += 2;
            }
            string_with_breaks_utf8 += "\xc3\xb7";

            //std::u32string string_with_breaks_utf32 = uni::utf8to32u(string_with_breaks_utf8);

            TESTX(string_with_breaks_utf8 == uni::utf32to8(string_with_breaks));
        }
        // UTF-16
        {
            std::u16string string_with_breaks_utf16 = uni::utf32to16u(string_without_breaks);

            uni::breaks::grapheme::utf16 it_begin{string_with_breaks_utf16.cbegin(), string_with_breaks_utf16.cend()};
            uni::breaks::grapheme::utf16 it_end{string_with_breaks_utf16.cend(), string_with_breaks_utf16.cend()};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = it_begin; it != it_end; ++it)
                vec.push_back(static_cast<std::size_t>(it - it_begin));

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

            uni::breaks::word::utf8 it_begin{string_with_breaks_utf8.cbegin(), string_with_breaks_utf8.cend()};
            uni::breaks::word::utf8 it_end{string_with_breaks_utf8.cend(), string_with_breaks_utf8.cend()};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = it_begin; it != it_end; ++it)
                vec.push_back(static_cast<std::size_t>(it - it_begin));

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

            uni::breaks::word::utf16 it_begin{string_with_breaks_utf16.cbegin(), string_with_breaks_utf16.cend()};
            uni::breaks::word::utf16 it_end{string_with_breaks_utf16.cend(), string_with_breaks_utf16.cend()};

            // Collect brakes
            std::vector<std::size_t> vec;
            for (auto it = it_begin; it != it_end; ++it)
                vec.push_back(static_cast<std::size_t>(it - it_begin));

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
    uni::breaks::word::utf8 it_begin{str.cbegin(), str.cend()};
    uni::breaks::word::utf8 it_end{str.cend(), str.cend()};

    return static_cast<std::size_t>(std::distance(it_begin, it_end));
}

std::size_t test_break_count_only_words(std::string_view str)
{
    uni::breaks::word::utf8 it_begin{str.cbegin(), str.cend()};
    uni::breaks::word::utf8 it_end{str.cend(), str.cend()};

    std::size_t count = 0;
    for (auto it = it_begin; it != it_end;)
    {
        ++it;
        if (it.word_on_left()) ++count;
    }
    return count;
}

std::size_t test_break_count_words16(std::u16string_view str)
{
    uni::breaks::word::utf16 it_begin{str.cbegin(), str.cend()};
    uni::breaks::word::utf16 it_end{str.cend(), str.cend()};

    return static_cast<std::size_t>(std::distance(it_begin, it_end));
}

std::size_t test_break_count_only_words16(std::u16string_view str)
{
    uni::breaks::word::utf16 it_begin{str.cbegin(), str.cend()};
    uni::breaks::word::utf16 it_end{str.cend(), str.cend()};

    std::size_t count = 0;
    for (auto it = it_begin; it != it_end;)
    {
        ++it;
        if (it.word_on_left()) ++count;
    }
    return count;
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
    // TODO: uncomment all of them when it will be fixed in impl_break.h
    TESTX(test_break_count_words("\x5F\x77") == 1);
    //TESTX(test_break_count_only_words("\x5F\x77") == 1);
    TESTX(test_break_count_words16(u"\x005F\x0077") == 1);
    //TESTX(test_break_count_only_words16(u"\x005F\x0077") == 1);

    // Uncomment all of them when TODO: in generator_break_word will be fixed.
    // Properly handle corner cases with Extended_Pictographic in word properties.
    // Extended_Pictographic intersects with ALetter in some code points in word properties.
    // Note that Extended_Pictographic never intersects with something in grapheme properties.
    // The behavior is the same as in ICU.
    //TESTX(test_break_count_words("\xE2\x80\x8D\xE2\x84\xB9\x77") == 1);
    TESTX(test_break_count_words("\xE2\x84\xB9\xE2\x80\x8D\x77") == 1);
    //TESTX(test_break_count_words16(u"\x200D\x2139\x0077") == 1);
    TESTX(test_break_count_words16(u"\x2139\x200D\x0077") == 1);
    //TESTX(test_break_count_words("\xE2\x80\x8D\xE2\x84\xB9") == 1);
    TESTX(test_break_count_words("\xE2\x84\xB9\xE2\x80\x8D") == 1);
    //TESTX(test_break_count_words16(u"\x200D\x2139") == 1);
    TESTX(test_break_count_words16(u"\x2139\x200D") == 1);

    // Repeat previous tests with count_only_words just in case.
    // Note that they all work just by a coincidence TODO: remove the note when it will be fixed.
    // The behavior is the same as in ICU.
    TESTX(test_break_count_only_words("\xE2\x80\x8D\xE2\x84\xB9\x77") == 1);
    TESTX(test_break_count_only_words("\xE2\x84\xB9\xE2\x80\x8D\x77") == 1);
    TESTX(test_break_count_only_words16(u"\x200D\x2139\x0077") == 1);
    TESTX(test_break_count_only_words16(u"\x2139\x200D\x0077") == 1);
    TESTX(test_break_count_only_words("\xE2\x80\x8D\xE2\x84\xB9") == 1);
    TESTX(test_break_count_only_words("\xE2\x84\xB9\xE2\x80\x8D") == 1);
    TESTX(test_break_count_only_words16(u"\x200D\x2139") == 1);
    TESTX(test_break_count_only_words16(u"\x2139\x200D") == 1);
}
