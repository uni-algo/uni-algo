/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr bool test2_lenient_utf8to16()
{
    TESTX(una::utf8to16u("!!!ABC!!!") == u"!!!ABC!!!");
    TESTX(una::utf8to16u("!!!\xD0\x90\xD0\x91\xD0\x92!!!") == u"!!!\x0410\x0411\x0412!!!");
    TESTX(una::utf8to16u("!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!") == u"!!!\xFF21\xFF22\xFF23!!!");
    TESTX(una::utf8to16u("!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!") == u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!");

    TESTX(una::utf8to16u("!!!\xC0\x80!!!") == u"!!!\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xE0\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x80\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF8\x80\x80\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x80\x80\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xC1\xBF!!!") == u"!!!\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xE0\x81\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x80\x81\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF8\x80\x80\x81\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x80\x80\x81\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xE0\x82\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x80\x82\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF8\x80\x80\x82\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x80\x80\x82\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xE0\x9F\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x80\x9F\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF8\x80\x80\x9F\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x80\x80\x9F\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xF0\x80\xA0\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF8\x80\x80\xA0\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x80\x80\xA0\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xF0\x8F\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF8\x80\xBF\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x80\x8F\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xF8\x80\x90\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x80\x90\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xF8\x84\xBF\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x84\x8F\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xF4\x90\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFB\xBF\xBF\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFD\xBF\xBF\xBF\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xED\xA0\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xED\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xED\xA0\xBD\xED\xB2\xA9!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xF8\x84\x90\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFC\x80\x84\x90\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x8D\xA0\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x8D\xBF\xBF!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x8D\xA0\xBD\xF0\x8D\xB2\xA9!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\x80!!!") == u"!!!\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\x80\x80!!!") == u"!!!\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\x80\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\x80\x80\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\x80\x80\x80\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\x80\x80\x80\x80\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\x80\x80\x80\x80\x80\x80\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xC2\xB6\x80!!!") == u"!!!\x00B6\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xE2\x98\x83\x80!!!") == u"!!!\x2603\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x9F\x92\xA9\x80!!!") == u"!!!\xD83D\xDCA9\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFB\xBF\xBF\xBF\xBF\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFD\xBF\xBF\xBF\xBF\xBF\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xC2!!!") == u"!!!\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xE2!!!") == u"!!!\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xE2\x98!!!") == u"!!!\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0!!!") == u"!!!\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x9F!!!") == u"!!!\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x9F\x92!!!") == u"!!!\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xF4\x90\x80!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xF0\x80\xA0!!!") == u"!!!\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to16u("!!!\xFE!!!") == u"!!!\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFE\x80!!!") == u"!!!\xFFFD\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFF!!!") == u"!!!\xFFFD!!!");
    TESTX(una::utf8to16u("!!!\xFF\x80!!!") == u"!!!\xFFFD\xFFFD!!!");

    return true;
}

test_constexpr bool test2_lenient_utf8to32()
{
    TESTX(una::utf8to32u("!!!ABC!!!") == U"!!!ABC!!!");
    TESTX(una::utf8to32u("!!!\xD0\x90\xD0\x91\xD0\x92!!!") == U"!!!\x0410\x0411\x0412!!!");
    TESTX(una::utf8to32u("!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!") == U"!!!\xFF21\xFF22\xFF23!!!");
    TESTX(una::utf8to32u("!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!") == U"!!!\x0001D400\x0001D401\x0001D402!!!");

    TESTX(una::utf8to32u("!!!\xC0\x80!!!") == U"!!!\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xE0\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x80\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF8\x80\x80\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x80\x80\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xC1\xBF!!!") == U"!!!\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xE0\x81\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x80\x81\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF8\x80\x80\x81\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x80\x80\x81\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xE0\x82\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x80\x82\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF8\x80\x80\x82\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x80\x80\x82\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xE0\x9F\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x80\x9F\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF8\x80\x80\x9F\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x80\x80\x9F\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xF0\x80\xA0\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF8\x80\x80\xA0\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x80\x80\xA0\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xF0\x8F\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF8\x80\xBF\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x80\x8F\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xF8\x80\x90\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x80\x90\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xF8\x84\xBF\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x84\x8F\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xF4\x90\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFB\xBF\xBF\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFD\xBF\xBF\xBF\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xED\xA0\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xED\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xED\xA0\xBD\xED\xB2\xA9!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xF8\x84\x90\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFC\x80\x84\x90\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x8D\xA0\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x8D\xBF\xBF!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x8D\xA0\xBD\xF0\x8D\xB2\xA9!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\x80!!!") == U"!!!\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\x80\x80!!!") == U"!!!\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\x80\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\x80\x80\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\x80\x80\x80\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\x80\x80\x80\x80\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\x80\x80\x80\x80\x80\x80\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xC2\xB6\x80!!!") == U"!!!\x00B6\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xE2\x98\x83\x80!!!") == U"!!!\x2603\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x9F\x92\xA9\x80!!!") == U"!!!\x0001F4A9\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFB\xBF\xBF\xBF\xBF\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFD\xBF\xBF\xBF\xBF\xBF\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xC2!!!") == U"!!!\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xE2!!!") == U"!!!\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xE2\x98!!!") == U"!!!\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0!!!") == U"!!!\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x9F!!!") == U"!!!\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x9F\x92!!!") == U"!!!\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xF4\x90\x80!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xF0\x80\xA0!!!") == U"!!!\xFFFD\xFFFD\xFFFD!!!");

    TESTX(una::utf8to32u("!!!\xFE!!!") == U"!!!\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFE\x80!!!") == U"!!!\xFFFD\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFF!!!") == U"!!!\xFFFD!!!");
    TESTX(una::utf8to32u("!!!\xFF\x80!!!") == U"!!!\xFFFD\xFFFD!!!");

    return true;
}

test_constexpr bool test2_lenient_utf16to8()
{
    TESTX(una::utf16to8(u"!!!ABC!!!") == "!!!ABC!!!");
    TESTX(una::utf16to8(u"!!!\xFF21\xFF22\xFF23!!!") == "!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!");
    TESTX(una::utf16to8(u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!") == "!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!");

    TESTX(una::utf16to8(u"!!!\xD800!!!") == "!!!\xEF\xBF\xBD!!!");
    TESTX(una::utf16to8(u"!!!\xD800\xDBFF!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD!!!");
    TESTX(una::utf16to8(u"!!!\xD800\xD800\xDBFF!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD!!!");

    TESTX(una::utf16to8(u"!!!\xD800\x0057!!!") == "!!!\xEF\xBF\xBD\x57!!!");
    TESTX(una::utf16to8(u"!!!\xD800\x0057\xDBFF\xFF37!!!") == "!!!\xEF\xBF\xBD\x57\xEF\xBF\xBD\xEF\xBC\xB7!!!");
    TESTX(una::utf16to8(u"!!!\xD800\x0057\xD800\xFF37\xDBFF\x0057!!!") == "!!!\xEF\xBF\xBD\x57\xEF\xBF\xBD\xEF\xBC\xB7\xEF\xBF\xBD\x57!!!");

    TESTX(una::utf16to8(u"!!!\xDC00!!!") == "!!!\xEF\xBF\xBD!!!");
    TESTX(una::utf16to8(u"!!!\xDC00\xDFFF!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD!!!");
    TESTX(una::utf16to8(u"!!!\xDC00\xDC00\xDFFF!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD!!!");

    TESTX(una::utf16to8(u"!!!\xDC00\xD835!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD!!!");
    TESTX(una::utf16to8(u"!!!\xDC00\xD835\xDC01\xD835!!!") == "!!!\xEF\xBF\xBD\xF0\x9D\x90\x81\xEF\xBF\xBD!!!");
    TESTX(una::utf16to8(u"!!!\xDC00\xD835\xDC01\xD835\xDC02\xD835!!!") == "!!!\xEF\xBF\xBD\xF0\x9D\x90\x81\xF0\x9D\x90\x82\xEF\xBF\xBD!!!");

    TESTX(una::utf16to8(u"!!!\x0041\x0042\xD800\x0043!!!") == "!!!\x41\x42\xEF\xBF\xBD\x43!!!");
    TESTX(una::utf16to8(u"!!!\x0041\x0042\xDC00\x0043!!!") == "!!!\x41\x42\xEF\xBF\xBD\x43!!!");
    TESTX(una::utf16to8(u"!!!\x0041\x0042\xD800\xFF37\x0043!!!") == "!!!\x41\x42\xEF\xBF\xBD\xEF\xBC\xB7\x43!!!");

    return true;
}

test_constexpr bool test2_lenient_utf32to8()
{
    TESTX(una::utf32to8(U"!!!ABC!!!") == "!!!ABC!!!");
    TESTX(una::utf32to8(U"!!!\x0000FF21\x0000FF22\x0000FF23!!!") == "!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!");
    TESTX(una::utf32to8(U"!!!\x0001D400\x0001D401\x0001D402!!!") == "!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!");

    TESTX(una::utf32to8(U"!!!\x0000D800!!!") == "!!!\xEF\xBF\xBD!!!");
    TESTX(una::utf32to8(U"!!!\x0000D800\x0000DBFF!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD!!!");

    TESTX(una::utf32to8(U"!!!\x0000DC00!!!") == "!!!\xEF\xBF\xBD!!!");
    TESTX(una::utf32to8(U"!!!\x0000DC00\x0000DFFF!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD!!!");

    TESTX(una::utf32to8(U"!!!\x0000D800\x0000DC00!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD!!!");
    TESTX(una::utf32to8(U"!!!\x0000D800\x0000DC00\x0000DBFF\x0000DFFF!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD\xEF\xBF\xBD!!!");

    TESTX(una::utf32to8(U"!!!\x00110000!!!") == "!!!\xEF\xBF\xBD!!!");
    TESTX(una::utf32to8(U"!!!\x00110000\xFFFFFFFF!!!") == "!!!\xEF\xBF\xBD\xEF\xBF\xBD!!!");

    TESTX(una::utf32to8(U"!!!\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043!!!") == "!!!\x41\xEF\xBF\xBD\xEF\xBF\xBD\x42\xEF\xBF\xBD\x43!!!");

    return true;
}

test_constexpr bool test2_lenient_utf16to32()
{
    TESTX(una::utf16to32u(u"!!!ABC!!!") == U"!!!ABC!!!");
    TESTX(una::utf16to32u(u"!!!\xFF21\xFF22\xFF23!!!") == U"!!!\x0000FF21\x0000FF22\x0000FF23!!!");
    TESTX(una::utf16to32u(u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!") == U"!!!\x0001D400\x0001D401\x0001D402!!!");

    TESTX(una::utf16to32u(u"!!!\x0041\x0042\xD800\x0043!!!") == U"!!!\x00000041\x00000042\x0000FFFD\x00000043!!!");
    TESTX(una::utf16to32u(u"!!!\x0041\x0042\xDC00\x0043!!!") == U"!!!\x00000041\x00000042\x0000FFFD\x00000043!!!");
    TESTX(una::utf16to32u(u"!!!\x0041\x0042\xD800\xFF37\x0043!!!") == U"!!!\x00000041\x00000042\x0000FFFD\x0000FF37\x00000043!!!");

    return true;
}

test_constexpr bool test2_lenient_utf32to16()
{
    TESTX(una::utf32to16u(U"!!!ABC!!!") == u"!!!ABC!!!");
    TESTX(una::utf32to16u(U"!!!\x0000FF21\x0000FF22\x0000FF23!!!") == u"!!!\xFF21\xFF22\xFF23!!!");
    TESTX(una::utf32to16u(U"!!!\x0001D400\x0001D401\x0001D402!!!") == u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!");

    TESTX(una::utf32to16u(U"!!!\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043!!!") == u"!!!\x0041\xFFFD\xFFFD\x0042\xFFFD\x0043!!!");

    return true;
}
