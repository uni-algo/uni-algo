/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr bool test_valid_utf8()
{
    una::error error;

    TESTX(una::is_valid_utf8("ABC"));
    TESTX(una::is_valid_utf8("ABC", error) && !error);
    TESTX(una::is_valid_utf8("\xD0\x90\xD0\x91\xD0\x92"));
    TESTX(una::is_valid_utf8("\xD0\x90\xD0\x91\xD0\x92", error) && !error);
    TESTX(una::is_valid_utf8("\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3"));
    TESTX(una::is_valid_utf8("\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3", error) && !error);
    TESTX(una::is_valid_utf8("\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82"));
    TESTX(una::is_valid_utf8("\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82", error) && !error);

    TESTX(!una::is_valid_utf8("\x80\x80\x80\x80\x80\x80\x80"));
    TESTX(!una::is_valid_utf8("\x80\x80\x80\x80\x80\x80\x80", error) && error && error.pos() == 0);
    TESTX(!una::is_valid_utf8("\xC2\xB6\x80"));
    TESTX(!una::is_valid_utf8("\xC2\xB6\x80", error) && error && error.pos() == 2);
    TESTX(!una::is_valid_utf8("\xE2\x98\x83\x80"));
    TESTX(!una::is_valid_utf8("\xE2\x98\x83\x80", error) && error && error.pos() == 3);
    TESTX(!una::is_valid_utf8("\xF0\x9F\x92\xA9\x80"));
    TESTX(!una::is_valid_utf8("\xF0\x9F\x92\xA9\x80", error) && error && error.pos() == 4);

    return true;
}

test_constexpr bool test_valid_utf16()
{
    una::error error;

    TESTX(una::is_valid_utf16(u"ABC"));
    TESTX(una::is_valid_utf16(u"ABC", error) && !error);
    TESTX(una::is_valid_utf16(u"\xFF21\xFF22\xFF23"));
    TESTX(una::is_valid_utf16(u"\xFF21\xFF22\xFF23", error) && !error);
    TESTX(una::is_valid_utf16(u"\xD835\xDC00\xD835\xDC01\xD835\xDC02"));
    TESTX(una::is_valid_utf16(u"\xD835\xDC00\xD835\xDC01\xD835\xDC02", error) && !error);

    TESTX(!una::is_valid_utf16(u"\xDC00\xD835"));
    TESTX(!una::is_valid_utf16(u"\xDC00\xD835", error) && error && error.pos() == 0);
    TESTX(!una::is_valid_utf16(u"\x0041\x0042\xD800\x0043"));
    TESTX(!una::is_valid_utf16(u"\x0041\x0042\xD800\x0043", error) && error && error.pos() == 2);

    return true;
}

test_constexpr bool test_valid_utf32()
{
    una::error error;

    TESTX(una::is_valid_utf32(U"ABC"));
    TESTX(una::is_valid_utf32(U"ABC", error) && !error);
    TESTX(una::is_valid_utf32(U"\x0000FF21\x0000FF22\x0000FF23"));
    TESTX(una::is_valid_utf32(U"\x0000FF21\x0000FF22\x0000FF23", error) && !error);
    TESTX(una::is_valid_utf32(U"\x0001D400\x0001D401\x0001D402"));
    TESTX(una::is_valid_utf32(U"\x0001D400\x0001D401\x0001D402", error) && !error);

    TESTX(!una::is_valid_utf32(U"\x00110000"));
    TESTX(!una::is_valid_utf32(U"\x00110000", error) && error && error.pos() == 0);
    TESTX(!una::is_valid_utf32(U"\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043"));
    TESTX(!una::is_valid_utf32(U"\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043", error) && error && error.pos() == 1);

    return true;
}

test_constexpr bool test2_valid_utf8()
{
    una::error error;

    TESTX(una::is_valid_utf8("!!!ABC!!!"));
    TESTX(una::is_valid_utf8("!!!ABC!!!", error) && !error);
    TESTX(una::is_valid_utf8("!!!\xD0\x90\xD0\x91\xD0\x92!!!"));
    TESTX(una::is_valid_utf8("!!!\xD0\x90\xD0\x91\xD0\x92!!!", error) && !error);
    TESTX(una::is_valid_utf8("!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!"));
    TESTX(una::is_valid_utf8("!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!", error) && !error);
    TESTX(una::is_valid_utf8("!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!"));
    TESTX(una::is_valid_utf8("!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!", error) && !error);

    TESTX(!una::is_valid_utf8("!!!\x80\x80\x80\x80\x80\x80\x80!!!"));
    TESTX(!una::is_valid_utf8("!!!\x80\x80\x80\x80\x80\x80\x80!!!", error) && error && error.pos() == 3);
    TESTX(!una::is_valid_utf8("!!!\xC2\xB6\x80!!!"));
    TESTX(!una::is_valid_utf8("!!!\xC2\xB6\x80!!!", error) && error && error.pos() == 5);
    TESTX(!una::is_valid_utf8("!!!\xE2\x98\x83\x80!!!"));
    TESTX(!una::is_valid_utf8("!!!\xE2\x98\x83\x80!!!", error) && error && error.pos() == 6);
    TESTX(!una::is_valid_utf8("!!!\xF0\x9F\x92\xA9\x80!!!"));
    TESTX(!una::is_valid_utf8("!!!\xF0\x9F\x92\xA9\x80!!!", error) && error && error.pos() == 7);

    return true;
}

test_constexpr bool test2_valid_utf16()
{
    una::error error;

    TESTX(una::is_valid_utf16(u"!!!ABC!!!"));
    TESTX(una::is_valid_utf16(u"!!!ABC!!!", error) && !error);
    TESTX(una::is_valid_utf16(u"!!!\xFF21\xFF22\xFF23!!!"));
    TESTX(una::is_valid_utf16(u"!!!\xFF21\xFF22\xFF23!!!", error) && !error);
    TESTX(una::is_valid_utf16(u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!"));
    TESTX(una::is_valid_utf16(u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!", error) && !error);

    TESTX(!una::is_valid_utf16(u"!!!\xDC00\xD835!!!"));
    TESTX(!una::is_valid_utf16(u"!!!\xDC00\xD835!!!", error) && error && error.pos() == 3);
    TESTX(!una::is_valid_utf16(u"!!!\x0041\x0042\xD800\x0043!!!"));
    TESTX(!una::is_valid_utf16(u"!!!\x0041\x0042\xD800\x0043!!!", error) && error && error.pos() == 5);

    return true;
}

test_constexpr bool test2_valid_utf32()
{
    una::error error;

    TESTX(una::is_valid_utf32(U"!!!ABC!!!"));
    TESTX(una::is_valid_utf32(U"!!!ABC!!!", error) && !error);
    TESTX(una::is_valid_utf32(U"!!!\x0000FF21\x0000FF22\x0000FF23!!!"));
    TESTX(una::is_valid_utf32(U"!!!\x0000FF21\x0000FF22\x0000FF23!!!", error) && !error);
    TESTX(una::is_valid_utf32(U"!!!\x0001D400\x0001D401\x0001D402!!!"));
    TESTX(una::is_valid_utf32(U"!!!\x0001D400\x0001D401\x0001D402!!!", error) && !error);

    TESTX(!una::is_valid_utf32(U"!!!\x00110000!!!"));
    TESTX(!una::is_valid_utf32(U"!!!\x00110000", error) && error && error.pos() == 3);
    TESTX(!una::is_valid_utf32(U"!!!\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043!!!"));
    TESTX(!una::is_valid_utf32(U"!!!\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043!!!", error) && error && error.pos() == 4);

    return true;
}
