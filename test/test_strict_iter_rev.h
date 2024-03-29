/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr std::u16string iter_rev_utf8to16(std::string_view str, una::error& error)
{
    auto view = una::ranges::utf8_view<std::string_view, una::detail::impl_iter_error>{str};

    error.reset();

    std::u32string codepoints;
    for (auto it = view.end(); it != view.begin();)
    {
        --it;
        if (*it == una::detail::impl_iter_error)
        {
            error = una::error{una::error::code::ill_formed_utf, static_cast<std::size_t>(it.begin() - str.begin())};
            return std::u16string{};
        }
        codepoints.push_back(*it);
    }
    std::reverse(codepoints.begin(), codepoints.end());

    std::u16string result;
    std::back_insert_iterator output{result};

    for (auto c : codepoints)
        una::detail::impl_output_utf16(c, output);

    return result;
}

test_constexpr std::string iter_rev_utf16to8(std::u16string_view str, una::error& error)
{
    auto view = una::ranges::utf16_view<std::u16string_view, una::detail::impl_iter_error>{str};

    error.reset();

    std::u32string codepoints;
    for (auto it = view.end(); it != view.begin();)
    {
        --it;
        if (*it == una::detail::impl_iter_error)
        {
            error = una::error{una::error::code::ill_formed_utf, static_cast<std::size_t>(it.begin() - str.begin())};
            return std::string{};
        }
        codepoints.push_back(*it);
    }
    std::reverse(codepoints.begin(), codepoints.end());

    std::string result;
    std::back_insert_iterator output{result};

    for (auto c : codepoints)
        una::detail::impl_output_utf8(c, output);

    return result;
}

test_constexpr bool test_strict_iter_rev_utf8to16()
{
    una::error error;
    std::u16string str;

    str = iter_rev_utf8to16("ABC", error);
    TESTX(!str.empty() && !error);
    str = iter_rev_utf8to16("\xD0\x90\xD0\x91\xD0\x92", error);
    TESTX(!str.empty() && !error);
    str = iter_rev_utf8to16("\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3", error);
    TESTX(!str.empty() && !error);
    str = iter_rev_utf8to16("\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82", error);
    TESTX(!str.empty() && !error);

    str = iter_rev_utf8to16("\xC0\x80", error);
    TESTX(str.empty() && error && error.pos() == 1);
    str = iter_rev_utf8to16("\xE0\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf8to16("\xF0\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xF8\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xC1\xBF", error);
    TESTX(str.empty() && error && error.pos() == 1);
    str = iter_rev_utf8to16("\xE0\x81\xBF", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf8to16("\xF0\x80\x81\xBF", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xF8\x80\x80\x81\xBF", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x80\x80\x81\xBF", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xE0\x82\x80", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf8to16("\xF0\x80\x82\x80", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xF8\x80\x80\x82\x80", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x80\x80\x82\x80", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xE0\x9F\xBF", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf8to16("\xF0\x80\x9F\xBF", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xF8\x80\x80\x9F\xBF", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x80\x80\x9F\xBF", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xF0\x80\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xF8\x80\x80\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x80\x80\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xF0\x8F\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xF8\x80\xBF\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x80\x8F\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xF8\x80\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x80\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xF8\x84\xBF\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x84\x8F\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xF4\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xFB\xBF\xBF\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFD\xBF\xBF\xBF\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = iter_rev_utf8to16("\xED\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf8to16("\xED\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf8to16("\xED\xA0\xBD\xED\xB2\xA9", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf8to16("\xF8\x84\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\xFC\x80\x84\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = iter_rev_utf8to16("\xF0\x8D\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xF0\x8D\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\xF0\x8D\xA0\xBD\xF0\x8D\xB2\xA9", error);
    TESTX(str.empty() && error && error.pos() == 7);

    str = iter_rev_utf8to16("\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf8to16("\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 1);
    str = iter_rev_utf8to16("\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf8to16("\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf8to16("\x80\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = iter_rev_utf8to16("\x80\x80\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = iter_rev_utf8to16("\x80\x80\x80\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 6);
    str = iter_rev_utf8to16("\xC2\xB6\x80", error);
    TESTX(str.empty() && error && error.pos() == 2); //
    str = iter_rev_utf8to16("\xE2\x98\x83\x80", error);
    TESTX(str.empty() && error && error.pos() == 3); //
    str = iter_rev_utf8to16("\xF0\x9F\x92\xA9\x80", error);
    TESTX(str.empty() && error && error.pos() == 4); //
    str = iter_rev_utf8to16("\xFB\xBF\xBF\xBF\xBF\x80", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = iter_rev_utf8to16("\xFD\xBF\xBF\xBF\xBF\xBF\x80", error);
    TESTX(str.empty() && error && error.pos() == 6);

    str = iter_rev_utf8to16("\xC2", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf8to16("\xE2", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf8to16("\xE2\x98", error);
    TESTX(str.empty() && error && error.pos() == 0); //
    str = iter_rev_utf8to16("\xF0", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf8to16("\xF0\x9F", error);
    TESTX(str.empty() && error && error.pos() == 0); //
    str = iter_rev_utf8to16("\xF0\x9F\x92", error);
    TESTX(str.empty() && error && error.pos() == 0); //

    str = iter_rev_utf8to16("\xF4\x90\x80", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf8to16("\xF0\x80\xA0", error);
    TESTX(str.empty() && error && error.pos() == 2);

    str = iter_rev_utf8to16("\xFE", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf8to16("\xFE\x80", error);
    TESTX(str.empty() && error && error.pos() == 1);
    str = iter_rev_utf8to16("\xFF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf8to16("\xFF\x80", error);
    TESTX(str.empty() && error && error.pos() == 1);

    return true;
}

test_constexpr bool test_strict_iter_rev_utf16to8()
{
    una::error error;
    std::string str;

    str = iter_rev_utf16to8(u"ABC", error);
    TESTX(!str.empty() && !error);
    str = iter_rev_utf16to8(u"\xFF21\xFF22\xFF23", error);
    TESTX(!str.empty() && !error);
    str = iter_rev_utf16to8(u"\xD835\xDC00\xD835\xDC01\xD835\xDC02", error);
    TESTX(!str.empty() && !error);

    str = iter_rev_utf16to8(u"\xD800", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf16to8(u"\xD800\xDBFF", error);
    TESTX(str.empty() && error && error.pos() == 1);
    str = iter_rev_utf16to8(u"\xD800\xD800\xDBFF", error);
    TESTX(str.empty() && error && error.pos() == 2);

    str = iter_rev_utf16to8(u"\xD800\x0057", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf16to8(u"\xD800\x0057\xDBFF\xFF37", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = iter_rev_utf16to8(u"\xD800\x0057\xD800\xFF37\xDBFF\x0057", error);
    TESTX(str.empty() && error && error.pos() == 4);

    str = iter_rev_utf16to8(u"\xDC00", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = iter_rev_utf16to8(u"\xDC00\xDFFF", error);
    TESTX(str.empty() && error && error.pos() == 1);
    str = iter_rev_utf16to8(u"\xDC00\xDC00\xDFFF", error);
    TESTX(str.empty() && error && error.pos() == 2);

    str = iter_rev_utf16to8(u"\xDC00\xD835", error);
    TESTX(str.empty() && error && error.pos() == 1);
    str = iter_rev_utf16to8(u"\xDC00\xD835\xDC01\xD835", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = iter_rev_utf16to8(u"\xDC00\xD835\xDC01\xD835\xDC02\xD835", error);
    TESTX(str.empty() && error && error.pos() == 5);

    str = iter_rev_utf16to8(u"\x0041\x0042\xD800\x0043", error);
    TESTX(str.empty() && error && error.pos() == 2); //
    str = iter_rev_utf16to8(u"\x0041\x0042\xDC00\x0043", error);
    TESTX(str.empty() && error && error.pos() == 2); //
    str = iter_rev_utf16to8(u"\x0041\x0042\xD800\xFF37\x0043", error);
    TESTX(str.empty() && error && error.pos() == 2); //

    return true;
}
