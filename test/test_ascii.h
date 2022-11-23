/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

void test_ascii_prop()
{
    // NOTE:
    // ASCII functions know nothing about Unicode so they are not consistent with Unicode functions
    // for example ASCII: unx::codepoint::to_ascii_lowercase(0x110000) == 0x110000
    // but Unicode: uni::codepoint::to_simple_lowercase(0x110000) == 0xFFFD
    // this means Unicode function always forced to return valid code point but ASCII don't follow this rule.

    TESTX(unx::codepoint::is_ascii(U'w'));
    TESTX(!unx::codepoint::is_ascii(0x80));

    TESTX(unx::codepoint::is_ascii(0));
    TESTX(unx::codepoint::is_ascii(0));
    TESTX(!unx::codepoint::is_ascii(0xFFFD));
    TESTX(!unx::codepoint::is_ascii(0xFFFD));
    TESTX(!unx::codepoint::is_ascii(0x10FFFF));
    TESTX(!unx::codepoint::is_ascii(0x10FFFF));
    TESTX(!unx::codepoint::is_ascii(0x110000));
    TESTX(!unx::codepoint::is_ascii(0x110000));
    TESTX(!unx::codepoint::is_ascii(0xFFFFFFFF));
    TESTX(!unx::codepoint::is_ascii(0xFFFFFFFF));

    TESTX(unx::codepoint::is_ascii_lowercase(U'w'));
    TESTX(!unx::codepoint::is_ascii_lowercase(U'W'));
    TESTX(unx::codepoint::is_ascii_uppercase(U'W'));
    TESTX(!unx::codepoint::is_ascii_uppercase(U'w'));

    TESTX(!unx::codepoint::is_ascii_lowercase(0));
    TESTX(!unx::codepoint::is_ascii_uppercase(0));
    TESTX(!unx::codepoint::is_ascii_lowercase(0xFFFD));
    TESTX(!unx::codepoint::is_ascii_uppercase(0xFFFD));
    TESTX(!unx::codepoint::is_ascii_lowercase(0x10FFFF));
    TESTX(!unx::codepoint::is_ascii_uppercase(0x10FFFF));
    TESTX(!unx::codepoint::is_ascii_lowercase(0x110000));
    TESTX(!unx::codepoint::is_ascii_uppercase(0x110000));
    TESTX(!unx::codepoint::is_ascii_lowercase(0xFFFFFFFF));
    TESTX(!unx::codepoint::is_ascii_uppercase(0xFFFFFFFF));

    TESTX(unx::codepoint::to_ascii_lowercase(U'W') == U'w');
    TESTX(unx::codepoint::to_ascii_uppercase(U'w') == U'W');
    TESTX(unx::codepoint::to_ascii_lowercase(U':') == U':');
    TESTX(unx::codepoint::to_ascii_uppercase(U':') == U':');

    TESTX(unx::codepoint::to_ascii_lowercase(0) == 0);
    TESTX(unx::codepoint::to_ascii_uppercase(0) == 0);
    TESTX(unx::codepoint::to_ascii_lowercase(0xFFFD) == 0xFFFD);
    TESTX(unx::codepoint::to_ascii_uppercase(0xFFFD) == 0xFFFD);
    TESTX(unx::codepoint::to_ascii_lowercase(0x10FFFF) == 0x10FFFF);
    TESTX(unx::codepoint::to_ascii_uppercase(0x10FFFF) == 0x10FFFF);
    TESTX(unx::codepoint::to_ascii_lowercase(0x110000) == 0x110000);
    TESTX(unx::codepoint::to_ascii_uppercase(0x110000) == 0x110000);
    TESTX(unx::codepoint::to_ascii_lowercase(0xFFFFFFFF) == 0xFFFFFFFF);
    TESTX(unx::codepoint::to_ascii_uppercase(0xFFFFFFFF) == 0xFFFFFFFF);
}

void test_ascii_upper_lower()
{
    TESTX(unx::cases::to_uppercase_ascii("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
                                      == "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
    TESTX(unx::cases::to_lowercase_ascii("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
                                      == "0123456789abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
}

void test_ascii_search()
{
    uni::search s;

    TESTX((s = unx::caseless::search_ascii("bbbbbbbbbkbb", "BBK")) && s.pos() == 7 && s.end_pos() == 10);
    TESTX((s = unx::caseless::search_ascii("bbk", "BBK")) && s.pos() == 0 && s.end_pos() == 3);
    TESTX(!unx::caseless::search_ascii("b", "BBK"));

    TESTX((s = unx::casesens::search_ascii("bbbbbbbbbkbb", "bbk")) && s.pos() == 7 && s.end_pos() == 10);
    TESTX((s = unx::casesens::search_ascii("bbk", "bbk")) && s.pos() == 0 && s.end_pos() == 3);
    TESTX(!unx::casesens::search_ascii("b", "bbk"));

    // Empty strings

    TESTX((s = unx::casesens::search_ascii("bbk", "")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = unx::caseless::search_ascii("bbk", "")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = unx::casesens::search_ascii("", "")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = unx::caseless::search_ascii("", "")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX(!unx::casesens::search_ascii("", "bbk"));
    TESTX(!unx::caseless::search_ascii("", "bbk"));
}

void test_ascii_trim()
{
    TESTX(unx::trim_ascii("   123   ") == "123");
    TESTX(unx::trim_start_ascii("   123   ") == "123   ");
    TESTX(unx::trim_end_ascii("   123   ") == "   123");

    TESTX(unx::trim_ascii("\t\r\n\f\v 123 \t\r\n\f\v") == "123");
    TESTX(unx::trim_start_ascii("\t\r\n\f\v 123 \t\r\n\f\v") == "123 \t\r\n\f\v");
    TESTX(unx::trim_end_ascii("\t\r\n\f\v 123 \t\r\n\f\v") == "\t\r\n\f\v 123");

    TESTX(unx::trim_ascii("") == "");
    TESTX(unx::trim_start_ascii("") == "");
    TESTX(unx::trim_end_ascii("") == "");

    // std::u16string_view

    TESTX(unx::trim_ascii<char16_t>(u"   123   ") == u"123");
    TESTX(unx::trim_start_ascii<char16_t>(u"   123   ") == u"123   ");
    TESTX(unx::trim_end_ascii<char16_t>(u"   123   ") == u"   123");

    TESTX(unx::trim_ascii<char16_t>(u"\t\r\n\f\v 123 \t\r\n\f\v") == u"123");
    TESTX(unx::trim_start_ascii<char16_t>(u"\t\r\n\f\v 123 \t\r\n\f\v") == u"123 \t\r\n\f\v");
    TESTX(unx::trim_end_ascii<char16_t>(u"\t\r\n\f\v 123 \t\r\n\f\v") == u"\t\r\n\f\v 123");

    TESTX(unx::trim_ascii<char16_t>(u"") == u"");
    TESTX(unx::trim_start_ascii<char16_t>(u"") == u"");
    TESTX(unx::trim_end_ascii<char16_t>(u"") == u"");
}

void test_ascii_valid()
{
    TESTX(unx::is_valid_ascii(""));
    TESTX(unx::is_valid_ascii("123"));
    TESTX(unx::is_valid_ascii("ABC"));
    TESTX(!unx::is_valid_ascii("\x80"));
    TESTX(!unx::is_valid_ascii("123 \x80"));
    TESTX(!unx::is_valid_ascii("\x80 123"));

    // std::u16string_view

    TESTX(unx::is_valid_ascii<char16_t>(u""));
    TESTX(unx::is_valid_ascii<char16_t>(u"123"));
    TESTX(unx::is_valid_ascii<char16_t>(u"ABC"));
    TESTX(!unx::is_valid_ascii<char16_t>(u"\x80"));
    TESTX(!unx::is_valid_ascii<char16_t>(u"123 \x80"));
    TESTX(!unx::is_valid_ascii<char16_t>(u"\x80 123"));
}

void test_ascii_short_func()
{
    std::string str = "123";

    TESTX(unx::is_valid_ascii(str));

    TESTX(unx::cases::to_lowercase_ascii(str) == str);
    TESTX(unx::cases::to_uppercase_ascii(str) == str);

    TESTX(unx::casesens::compare_ascii(str, str) == 0);
    TESTX(unx::caseless::compare_ascii(str, str) == 0);
    TESTX(unx::casesens::collate_ascii(str, str) == 0);
    TESTX(unx::caseless::collate_ascii(str, str) == 0);
    TESTX(unx::casesens::search_ascii(str, str));
    TESTX(unx::caseless::search_ascii(str, str));

    TESTX(unx::trim_ascii(str) == str);
    TESTX(unx::trim_start_ascii(str) == str);
    TESTX(unx::trim_end_ascii(str) == str);
}

void test_ascii_collate()
{
    std::string str_ascii;
    for(char32_t c = 0; c <= 0x7F; c++)
        str_ascii.push_back(static_cast<char>(c));

    // Shuffle
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(str_ascii.begin(), str_ascii.end(), std::mt19937{static_cast<std::mt19937::result_type>(seed)});

    std::string test_ascii = str_ascii;
    std::string test_utf8 = str_ascii;
    std::string test_utf16 = str_ascii;

    // Binary sort for reference (for debugging)
    //std::sort(test_ascii.begin(), test_ascii.end());
    //std::sort(test_utf8.begin(), test_utf8.end());
    //std::sort(test_utf16.begin(), test_utf16.end());

    // Test all collate functions to make sure the result is consistent

    std::sort(test_ascii.begin(), test_ascii.end(), [](char a, char b) {
        return unx::casesens::collate_ascii<char>(std::string{a}, std::string{b}) < 0;
    });

    std::sort(test_utf8.begin(), test_utf8.end(), [](char a, char b) {
        return uni::casesens::collate_utf8<char>(std::string{a}, std::string{b}) < 0;
    });

    std::sort(test_utf16.begin(), test_utf16.end(), [](char a, char b) {
        return uni::casesens::collate_utf16<char16_t>(std::u16string{static_cast<char16_t>(a)}, std::u16string{static_cast<char16_t>(b)}) < 0;
    });

    std::string test_group_ascii = test_ascii;
    std::string test_group_utf8 = test_utf8;
    std::string test_group_utf16 = test_utf16;

    auto it_ascii = std::unique(test_group_ascii.begin(), test_group_ascii.end(), [](char a, char b) {
        return unx::caseless::collate_ascii<char>(std::string{a}, std::string{b}) == 0;
    });
    test_group_ascii.erase(it_ascii, test_group_ascii.end());

    auto it_utf8 = std::unique(test_group_utf8.begin(), test_group_utf8.end(), [](char a, char b) {
        return uni::caseless::collate_utf8<char>(std::string{a}, std::string{b}) == 0;
    });
    test_group_utf8.erase(it_utf8, test_group_utf8.end());

    auto it_utf16 = std::unique(test_group_utf16.begin(), test_group_utf16.end(), [](char a, char b) {
        return uni::caseless::collate_utf16<char16_t>(std::u16string{static_cast<char16_t>(a)}, std::u16string{static_cast<char16_t>(b)}) == 0;
    });
    test_group_utf16.erase(it_utf16, test_group_utf16.end());

    auto func_make_readable = [](const std::string& str) -> std::string
    {
        std::string result;

        std::for_each(str.cbegin(), str.cend(), [&result](char c) {
            if (c == 0x00)
                result += ' ';
            else if (c <= 0x1F || c == 0x7F || c == '"' || c == '\\')
            {
                char hex[16] = {};
                std::snprintf(hex, 16, "\\x%02X", c);
                result += hex;
            }
            else
                result += std::string{c};
        });

        return result;
    };

    std::string result_ascii = func_make_readable(test_ascii);
    std::string result_utf8 = func_make_readable(test_utf8);
    std::string result_utf16 = func_make_readable(test_utf16);

    std::string result_group_ascii = func_make_readable(test_group_ascii);
    std::string result_group_utf8 = func_make_readable(test_group_utf8);
    std::string result_group_utf16 = func_make_readable(test_group_utf16);

    std::string valid_result =
    " \x01\x02\x03\x04\x05\x06\x07\x08\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x7F\x09\x0A\x0B\x0C\x0D"
    " !\x22#%&'()*+,-./:;<=>?@[\x5C]^_`{|}~$0123456789aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";

    valid_result = func_make_readable(valid_result);

    // For reference valid result with binary sort
    // " \x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
    // " !\x22#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\x5C]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7F";

    std::string valid_result_group =
    " \x01\x02\x03\x04\x05\x06\x07\x08\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x7F\x09\x0A\x0B\x0C\x0D"
    " !\x22#%&'()*+,-./:;<=>?@[\x5C]^_`{|}~$0123456789abcdefghijklmnopqrstuvwxyz";

    valid_result_group = func_make_readable(valid_result_group);

    TESTX(valid_result == result_ascii);
    TESTX(valid_result == result_utf8);
    TESTX(valid_result == result_utf16);

    TESTX(valid_result_group == result_group_ascii);
    TESTX(valid_result_group == result_group_utf8);
    TESTX(valid_result_group == result_group_utf16);
}

#if 0
// This is the generator that what used to generate small ascii collate tables from DUCET data (stage1_order/stage2_order)
void generator_ascii_collate()
{
    std::u32string str;
    std::u32string str_result;

    for(char32_t c = 0; c <= 0x7F; c++)
        str.push_back(uni::detail::stages_order(c));

    str_result.resize(str.size());

    for(char32_t c = 0; c <= 0x7F; c++)
    {
        char32_t v = 0xFFFFFFFF;
        char32_t index = 0;
        for(char32_t i = 0; i <= 0x7F; i++)
        {
            if (str[i] < v)
            {
                v = str[i];
                index = i;
            }
        }

        str[index] = 0xFFFFFFFF;
        str_result[index] = c+1;
    }

    // For caseless table
    /*for (char32_t c = 0; c <= 0x7F; c++)
    {
        if (c >= 'A' && c <= 'Z')
            str2[c] = str2[c ^ 32];
    }*/

    for (char32_t c: str_result)
    {
        if (c < 10)
            std::cout << "  " << c << ", ";
        else if (c < 100)
            std::cout << " " << c << ", ";
        else
            std::cout << c << ", ";
    }
}
#endif
