/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// If it fails a compiler messed up the encoding of this file
// It the compiler is MSVC then /utf-8 command line option must be used
static_assert(U'㋡' == 0x32E1);

void test_norm_unaccent()
{
    TESTX(uni::norm::utf8_unaccent("Naïve") == "Naive");
    TESTX(uni::norm::utf8_unaccent("Voilà, Pâte, Dîner, Côté, Goûter") == "Voila, Pate, Diner, Cote, Gouter");
    TESTX(uni::norm::utf8_unaccent("İstanbul") == "Istanbul");
    TESTX(uni::norm::utf8_unaccent("Μαΐου, τρόλεϊ") == "Μαιου, τρολει");
    TESTX(uni::norm::utf8_unaccent("Το ένα ή το άλλο.") == "Το ενα η το αλλο.");

    TESTX(uni::norm::utf16_unaccent(u"Naïve") == u"Naive");
    TESTX(uni::norm::utf16_unaccent(u"Voilà, Pâte, Dîner, Côté, Goûter") == u"Voila, Pate, Diner, Cote, Gouter");
    TESTX(uni::norm::utf16_unaccent(u"İstanbul") == u"Istanbul");
    TESTX(uni::norm::utf16_unaccent(u"Μαΐου, τρόλεϊ") == u"Μαιου, τρολει");
    TESTX(uni::norm::utf16_unaccent(u"Το ένα ή το άλλο.") == u"Το ενα η το αλλο.");
}
