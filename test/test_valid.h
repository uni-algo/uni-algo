/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

void test_valid_utf8()
{
    uni::error error;

    TESTX(uni::is_valid_utf8("ABC"));
    TESTX(uni::is_valid_utf8("ABC", error) && !error);
    TESTX(uni::is_valid_utf8("\xD0\x90\xD0\x91\xD0\x92"));
    TESTX(uni::is_valid_utf8("\xD0\x90\xD0\x91\xD0\x92", error) && !error);
    TESTX(uni::is_valid_utf8("\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3"));
    TESTX(uni::is_valid_utf8("\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3", error) && !error);
    TESTX(uni::is_valid_utf8("\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82"));
    TESTX(uni::is_valid_utf8("\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82", error) && !error);

    TESTX(!uni::is_valid_utf8("\x80\x80\x80\x80\x80\x80\x80"));
    TESTX(!uni::is_valid_utf8("\x80\x80\x80\x80\x80\x80\x80", error) && error && error.pos() == 0);
    TESTX(!uni::is_valid_utf8("\xC2\xB6\x80"));
    TESTX(!uni::is_valid_utf8("\xC2\xB6\x80", error) && error && error.pos() == 2);
    TESTX(!uni::is_valid_utf8("\xE2\x98\x83\x80"));
    TESTX(!uni::is_valid_utf8("\xE2\x98\x83\x80", error) && error && error.pos() == 3);
    TESTX(!uni::is_valid_utf8("\xF0\x9F\x92\xA9\x80"));
    TESTX(!uni::is_valid_utf8("\xF0\x9F\x92\xA9\x80", error) && error && error.pos() == 4);
}

void test_valid_utf16()
{
    uni::error error;

    TESTX(uni::is_valid_utf16(u"ABC"));
    TESTX(uni::is_valid_utf16(u"ABC", error) && !error);
    TESTX(uni::is_valid_utf16(u"\xFF21\xFF22\xFF23"));
    TESTX(uni::is_valid_utf16(u"\xFF21\xFF22\xFF23", error) && !error);
    TESTX(uni::is_valid_utf16(u"\xD835\xDC00\xD835\xDC01\xD835\xDC02"));
    TESTX(uni::is_valid_utf16(u"\xD835\xDC00\xD835\xDC01\xD835\xDC02", error) && !error);

    TESTX(!uni::is_valid_utf16(u"\xDC00\xD835"));
    TESTX(!uni::is_valid_utf16(u"\xDC00\xD835", error) && error && error.pos() == 0);
    TESTX(!uni::is_valid_utf16(u"\x0041\x0042\xD800\x0043"));
    TESTX(!uni::is_valid_utf16(u"\x0041\x0042\xD800\x0043", error) && error && error.pos() == 2);
}

void test_valid_utf32()
{
    uni::error error;

    TESTX(uni::is_valid_utf32(U"ABC"));
    TESTX(uni::is_valid_utf32(U"ABC", error) && !error);
    TESTX(uni::is_valid_utf32(U"\x0000FF21\x0000FF22\x0000FF23"));
    TESTX(uni::is_valid_utf32(U"\x0000FF21\x0000FF22\x0000FF23", error) && !error);
    TESTX(uni::is_valid_utf32(U"\x0001D400\x0001D401\x0001D402"));
    TESTX(uni::is_valid_utf32(U"\x0001D400\x0001D401\x0001D402", error) && !error);

    TESTX(!uni::is_valid_utf32(U"\x00110000"));
    TESTX(!uni::is_valid_utf32(U"\x00110000", error) && error && error.pos() == 0);
    TESTX(!uni::is_valid_utf32(U"\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043"));
    TESTX(!uni::is_valid_utf32(U"\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043", error) && error && error.pos() == 1);
}

void test2_valid_utf8()
{
    uni::error error;

    TESTX(uni::is_valid_utf8("!!!ABC!!!"));
    TESTX(uni::is_valid_utf8("!!!ABC!!!", error) && !error);
    TESTX(uni::is_valid_utf8("!!!\xD0\x90\xD0\x91\xD0\x92!!!"));
    TESTX(uni::is_valid_utf8("!!!\xD0\x90\xD0\x91\xD0\x92!!!", error) && !error);
    TESTX(uni::is_valid_utf8("!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!"));
    TESTX(uni::is_valid_utf8("!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!", error) && !error);
    TESTX(uni::is_valid_utf8("!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!"));
    TESTX(uni::is_valid_utf8("!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!", error) && !error);

    TESTX(!uni::is_valid_utf8("!!!\x80\x80\x80\x80\x80\x80\x80!!!"));
    TESTX(!uni::is_valid_utf8("!!!\x80\x80\x80\x80\x80\x80\x80!!!", error) && error && error.pos() == 3);
    TESTX(!uni::is_valid_utf8("!!!\xC2\xB6\x80!!!"));
    TESTX(!uni::is_valid_utf8("!!!\xC2\xB6\x80!!!", error) && error && error.pos() == 5);
    TESTX(!uni::is_valid_utf8("!!!\xE2\x98\x83\x80!!!"));
    TESTX(!uni::is_valid_utf8("!!!\xE2\x98\x83\x80!!!", error) && error && error.pos() == 6);
    TESTX(!uni::is_valid_utf8("!!!\xF0\x9F\x92\xA9\x80!!!"));
    TESTX(!uni::is_valid_utf8("!!!\xF0\x9F\x92\xA9\x80!!!", error) && error && error.pos() == 7);
}

void test2_valid_utf16()
{
    uni::error error;

    TESTX(uni::is_valid_utf16(u"!!!ABC!!!"));
    TESTX(uni::is_valid_utf16(u"!!!ABC!!!", error) && !error);
    TESTX(uni::is_valid_utf16(u"!!!\xFF21\xFF22\xFF23!!!"));
    TESTX(uni::is_valid_utf16(u"!!!\xFF21\xFF22\xFF23!!!", error) && !error);
    TESTX(uni::is_valid_utf16(u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!"));
    TESTX(uni::is_valid_utf16(u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!", error) && !error);

    TESTX(!uni::is_valid_utf16(u"!!!\xDC00\xD835!!!"));
    TESTX(!uni::is_valid_utf16(u"!!!\xDC00\xD835!!!", error) && error && error.pos() == 3);
    TESTX(!uni::is_valid_utf16(u"!!!\x0041\x0042\xD800\x0043!!!"));
    TESTX(!uni::is_valid_utf16(u"!!!\x0041\x0042\xD800\x0043!!!", error) && error && error.pos() == 5);
}

void test2_valid_utf32()
{
    uni::error error;

    TESTX(uni::is_valid_utf32(U"!!!ABC!!!"));
    TESTX(uni::is_valid_utf32(U"!!!ABC!!!", error) && !error);
    TESTX(uni::is_valid_utf32(U"!!!\x0000FF21\x0000FF22\x0000FF23!!!"));
    TESTX(uni::is_valid_utf32(U"!!!\x0000FF21\x0000FF22\x0000FF23!!!", error) && !error);
    TESTX(uni::is_valid_utf32(U"!!!\x0001D400\x0001D401\x0001D402!!!"));
    TESTX(uni::is_valid_utf32(U"!!!\x0001D400\x0001D401\x0001D402!!!", error) && !error);

    TESTX(!uni::is_valid_utf32(U"!!!\x00110000!!!"));
    TESTX(!uni::is_valid_utf32(U"!!!\x00110000", error) && error && error.pos() == 3);
    TESTX(!uni::is_valid_utf32(U"!!!\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043!!!"));
    TESTX(!uni::is_valid_utf32(U"!!!\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043!!!", error) && error && error.pos() == 4);
}
