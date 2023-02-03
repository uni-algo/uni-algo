/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// If it fails a compiler messed up the UTF-8 encoding of this file.
// If the compiler is MSVC then /utf-8 command line option must be used.
static_assert(U'㋡' == 0x32E1);

test_constexpr bool test_norm_unaccent()
{
    TESTX(una::norm::to_unaccent_utf8("Naïve") == "Naive");
    TESTX(una::norm::to_unaccent_utf8("Voilà, Pâte, Dîner, Côté, Goûter") == "Voila, Pate, Diner, Cote, Gouter");
    TESTX(una::norm::to_unaccent_utf8("İstanbul") == "Istanbul");
    TESTX(una::norm::to_unaccent_utf8("Μαΐου, τρόλεϊ") == "Μαιου, τρολει");
    TESTX(una::norm::to_unaccent_utf8("Το ένα ή το άλλο.") == "Το ενα η το αλλο.");

    TESTX(una::norm::to_unaccent_utf16(u"Naïve") == u"Naive");
    TESTX(una::norm::to_unaccent_utf16(u"Voilà, Pâte, Dîner, Côté, Goûter") == u"Voila, Pate, Diner, Cote, Gouter");
    TESTX(una::norm::to_unaccent_utf16(u"İstanbul") == u"Istanbul");
    TESTX(una::norm::to_unaccent_utf16(u"Μαΐου, τρόλεϊ") == u"Μαιου, τρολει");
    TESTX(una::norm::to_unaccent_utf16(u"Το ένα ή το άλλο.") == u"Το ενα η το αλλο.");

    return true;
}
