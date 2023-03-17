/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr std::u16string test_iter_utf8to16strict(std::string_view str, una::error& error)
{
    auto view = una::ranges::utf8_view<std::string_view, una::detail::impl_iter_error>{str};

    error.reset();

    std::u16string result;
    std::back_insert_iterator output{result};

    for (auto it = view.begin(); it != view.end(); ++it)
    {
        if (*it == una::detail::impl_iter_error)
        {
            error = una::error{una::error::code::ill_formed_utf, static_cast<std::size_t>(it.begin() - str.begin())};
            return std::u16string{};
        }

        una::detail::impl_output_utf16(*it, output);
    }
    return result;
}

test_constexpr std::string test_iter_utf16to8strict(std::u16string_view str, una::error& error)
{
    auto view = una::ranges::utf16_view<std::u16string_view, una::detail::impl_iter_error>{str};

    error.reset();

    std::string result;
    std::back_insert_iterator output{result};

    for (auto it = view.begin(); it != view.end(); ++it)
    {
        if (*it == una::detail::impl_iter_error)
        {
            error = una::error{una::error::code::ill_formed_utf, static_cast<std::size_t>(it.begin() - str.begin())};
            return std::string{};
        }

        una::detail::impl_output_utf8(*it, output);
    }

    return result;
}

test_constexpr bool test_strict_iter_utf8to16()
{
    una::error error;
    std::u16string str;

    str = test_iter_utf8to16strict("ABC", error);
    TESTX(!str.empty() && !error);
    str = test_iter_utf8to16strict("\xD0\x90\xD0\x91\xD0\x92", error);
    TESTX(!str.empty() && !error);
    str = test_iter_utf8to16strict("\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3", error);
    TESTX(!str.empty() && !error);
    str = test_iter_utf8to16strict("\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82", error);
    TESTX(!str.empty() && !error);

    str = test_iter_utf8to16strict("\xC0\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xE0\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF8\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xC1\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xE0\x81\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x80\x81\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF8\x80\x80\x81\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x80\x80\x81\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xE0\x82\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x80\x82\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF8\x80\x80\x82\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x80\x80\x82\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xE0\x9F\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x80\x9F\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF8\x80\x80\x9F\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x80\x80\x9F\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xF0\x80\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF8\x80\x80\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x80\x80\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xF0\x8F\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF8\x80\xBF\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x80\x8F\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xF8\x80\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x80\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xF8\x84\xBF\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x84\x8F\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xF4\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFB\xBF\xBF\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFD\xBF\xBF\xBF\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xED\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xED\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xED\xA0\xBD\xED\xB2\xA9", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xF8\x84\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFC\x80\x84\x90\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x8D\xA0\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x8D\xBF\xBF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x8D\xA0\xBD\xF0\x8D\xB2\xA9", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\x80\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\x80\x80\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\x80\x80\x80\x80\x80\x80\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xC2\xB6\x80", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = test_iter_utf8to16strict("\xE2\x98\x83\x80", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = test_iter_utf8to16strict("\xF0\x9F\x92\xA9\x80", error);
    TESTX(str.empty() && error && error.pos() == 4);
    str = test_iter_utf8to16strict("\xFB\xBF\xBF\xBF\xBF\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFD\xBF\xBF\xBF\xBF\xBF\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xC2", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xE2", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xE2\x98", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x9F", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x9F\x92", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xF4\x90\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xF0\x80\xA0", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf8to16strict("\xFE", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFE\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf8to16strict("\xFF\x80", error);
    TESTX(str.empty() && error && error.pos() == 0);

    return true;
}

test_constexpr bool test_strict_iter_utf16to8()
{
    una::error error;
    std::string str;

    str = test_iter_utf16to8strict(u"ABC", error);
    TESTX(!str.empty() && !error);
    str = test_iter_utf16to8strict(u"\xFF21\xFF22\xFF23", error);
    TESTX(!str.empty() && !error);
    str = test_iter_utf16to8strict(u"\xD835\xDC00\xD835\xDC01\xD835\xDC02", error);
    TESTX(!str.empty() && !error);

    str = test_iter_utf16to8strict(u"\xD800", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf16to8strict(u"\xD800\xDBFF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf16to8strict(u"\xD800\xD800\xDBFF", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf16to8strict(u"\xD800\x0057", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf16to8strict(u"\xD800\x0057\xDBFF\xFF37", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf16to8strict(u"\xD800\x0057\xD800\xFF37\xDBFF\x0057", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf16to8strict(u"\xDC00", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf16to8strict(u"\xDC00\xDFFF", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf16to8strict(u"\xDC00\xDC00\xDFFF", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf16to8strict(u"\xDC00\xD835", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf16to8strict(u"\xDC00\xD835\xDC01\xD835", error);
    TESTX(str.empty() && error && error.pos() == 0);
    str = test_iter_utf16to8strict(u"\xDC00\xD835\xDC01\xD835\xDC02\xD835", error);
    TESTX(str.empty() && error && error.pos() == 0);

    str = test_iter_utf16to8strict(u"\x0041\x0042\xD800\x0043", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = test_iter_utf16to8strict(u"\x0041\x0042\xDC00\x0043", error);
    TESTX(str.empty() && error && error.pos() == 2);
    str = test_iter_utf16to8strict(u"\x0041\x0042\xD800\xFF37\x0043", error);
    TESTX(str.empty() && error && error.pos() == 2);

    return true;
}
