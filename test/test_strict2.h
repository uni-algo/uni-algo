/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

void test2_strict_utf8to16()
{
    uni::error error;
    std::u16string str;

    str = uni::strict::utf8to16u("!!!ABC!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf8to16u("!!!\xD0\x90\xD0\x91\xD0\x92!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf8to16u("!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf8to16u("!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!", error);
    TESTX(!str.empty() && !error);

    str = uni::strict::utf8to16u("!!!\xC0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xE0\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF8\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xC1\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xE0\x81\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x80\x81\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF8\x80\x80\x81\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x80\x80\x81\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xE0\x82\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x80\x82\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF8\x80\x80\x82\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x80\x80\x82\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xE0\x9F\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x80\x9F\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF8\x80\x80\x9F\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x80\x80\x9F\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xF0\x80\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF8\x80\x80\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x80\x80\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xF0\x8F\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF8\x80\xBF\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x80\x8F\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xF8\x80\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x80\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xF8\x84\xBF\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x84\x8F\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xF4\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFB\xBF\xBF\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFD\xBF\xBF\xBF\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xED\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xED\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xED\xA0\xBD\xED\xB2\xA9!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xF8\x84\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFC\x80\x84\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x8D\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x8D\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x8D\xA0\xBD\xF0\x8D\xB2\xA9!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\x80\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\x80\x80\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\x80\x80\x80\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xC2\xB6\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = uni::strict::utf8to16u("!!!\xE2\x98\x83\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 6);
    str = uni::strict::utf8to16u("!!!\xF0\x9F\x92\xA9\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 7);
    str = uni::strict::utf8to16u("!!!\xFB\xBF\xBF\xBF\xBF\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFD\xBF\xBF\xBF\xBF\xBF\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xC2!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xE2!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xE2\x98!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x9F!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x9F\x92!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xF4\x90\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xF0\x80\xA0!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to16u("!!!\xFE!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFE\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to16u("!!!\xFF\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
}

void test2_strict_utf8to32()
{
    uni::error error;
    std::u32string str;

    str = uni::strict::utf8to32u("!!!ABC!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf8to32u("!!!\xD0\x90\xD0\x91\xD0\x92!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf8to32u("!!!\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf8to32u("!!!\xF0\x9D\x90\x80\xF0\x9D\x90\x81\xF0\x9D\x90\x82!!!", error);
    TESTX(!str.empty() && !error);

    str = uni::strict::utf8to32u("!!!\xC0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xE0\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF8\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xC1\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xE0\x81\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xF0\x80\x81\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF8\x80\x80\x81\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x80\x80\x81\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xE0\x82\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x80\x82\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF8\x80\x80\x82\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x80\x80\x82\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xE0\x9F\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x80\x9F\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF8\x80\x80\x9F\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x80\x80\x9F\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xF0\x80\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF8\x80\x80\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x80\x80\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xF0\x8F\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF8\x80\xBF\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x80\x8F\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xF8\x80\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x80\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xF8\x84\xBF\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x84\x8F\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xF4\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFB\xBF\xBF\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFD\xBF\xBF\xBF\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xED\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xED\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xED\xA0\xBD\xED\xB2\xA9!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xF8\x84\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFC\x80\x84\x90\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x8D\xA0\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x8D\xBF\xBF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x8D\xA0\xBD\xF0\x8D\xB2\xA9!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\x80\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\x80\x80\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\x80\x80\x80\x80\x80\x80\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xC2\xB6\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = uni::strict::utf8to32u("!!!\xE2\x98\x83\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 6);
    str = uni::strict::utf8to32u("!!!\xF0\x9F\x92\xA9\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 7);
    str = uni::strict::utf8to32u("!!!\xFB\xBF\xBF\xBF\xBF\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFD\xBF\xBF\xBF\xBF\xBF\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xC2!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xE2!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xE2\x98!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x9F!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x9F\x92!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xF4\x90\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xF0\x80\xA0!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf8to32u("!!!\xFE!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFE\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf8to32u("!!!\xFF\x80!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
}

void test2_strict_utf16to8()
{
    uni::error error;
    std::string str;

    str = uni::strict::utf16to8(u"!!!ABC!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf16to8(u"!!!\xFF21\xFF22\xFF23!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf16to8(u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!", error);
    TESTX(!str.empty() && !error);

    str = uni::strict::utf16to8(u"!!!\xD800!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf16to8(u"!!!\xD800\xDBFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf16to8(u"!!!\xD800\xD800\xDBFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf16to8(u"!!!\xD800\x0057!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf16to8(u"!!!\xD800\x0057\xDBFF\xFF37!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf16to8(u"!!!\xD800\x0057\xD800\xFF37\xDBFF\x0057!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf16to8(u"!!!\xDC00!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf16to8(u"!!!\xDC00\xDFFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf16to8(u"!!!\xDC00\xDC00\xDFFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf16to8(u"!!!\xDC00\xD835!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf16to8(u"!!!\xDC00\xD835\xDC01\xD835!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf16to8(u"!!!\xDC00\xD835\xDC01\xD835\xDC02\xD835!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf16to8(u"!!!\x0041\x0042\xD800\x0043!!!", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = uni::strict::utf16to8(u"!!!\x0041\x0042\xDC00\x0043!!!", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = uni::strict::utf16to8(u"!!!\x0041\x0042\xD800\xFF37\x0043!!!", error);
    TESTX(str.empty() && error && error.pos() == 5);
}

void test2_strict_utf32to8()
{
    uni::error error;
    std::string str;

    str = uni::strict::utf32to8(U"!!!ABC!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf32to8(U"!!!\x0000FF21\x0000FF22\x0000FF23!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf32to8(U"!!!\x0001D400\x0001D401\x0001D402!!!", error);
    TESTX(!str.empty() && !error);

    str = uni::strict::utf32to8(U"!!!\x0000D800!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf32to8(U"!!!\x0000D800\x0000DBFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf32to8(U"!!!\x0000DC00!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf32to8(U"!!!\x0000DC00\x0000DFFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf32to8(U"!!!\x0000D800\x0000DC00!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf32to8(U"!!!\x0000D800\x0000DC00\x0000DBFF\x0000DFFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf32to8(U"!!!\x00110000!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);
    str = uni::strict::utf32to8(U"!!!\x00110000\xFFFFFFFF!!!", error);
    TESTX(str.empty() && error && error.pos() == 3);

    str = uni::strict::utf32to8(U"!!!\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043!!!", error);
    TESTX(str.empty() && error && error.pos() == 4);
}

void test2_strict_utf16to32()
{
    uni::error error;
    std::u32string str;

    str = uni::strict::utf16to32u(u"!!!ABC!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf16to32u(u"!!!\xFF21\xFF22\xFF23!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf16to32u(u"!!!\xD835\xDC00\xD835\xDC01\xD835\xDC02!!!", error);
    TESTX(!str.empty() && !error);

    str = uni::strict::utf16to32u(u"!!!\x0041\x0042\xD800\x0043!!!", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = uni::strict::utf16to32u(u"!!!\x0041\x0042\xDC00\x0043!!!", error);
    TESTX(str.empty() && error && error.pos() == 5);
    str = uni::strict::utf16to32u(u"!!!\x0041\x0042\xD800\xFF37\x0043!!!", error);
    TESTX(str.empty() && error && error.pos() == 5);
}

void test2_strict_utf32to16()
{
    uni::error error;
    std::u16string str;

    str = uni::strict::utf32to16u(U"!!!ABC!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf32to16u(U"!!!\x0000FF21\x0000FF22\x0000FF23!!!", error);
    TESTX(!str.empty() && !error);
    str = uni::strict::utf32to16u(U"!!!\x0001D400\x0001D401\x0001D402!!!", error);
    TESTX(!str.empty() && !error);

    str = uni::strict::utf32to16u(U"!!!\x00000041\x0000D800\x0000DC00\x00000042\x00110000\x00000043!!!", error);
    TESTX(str.empty() && error && error.pos() == 4);
}
