/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// If it fails a compiler messed up the encoding of this file
// It the compiler is MSVC then /utf-8 command line option must be used
static_assert(U'㋡' == 0x32E1);

void test_case_compare_collate()
{
    // Use Cyrilic letter Џ/џ (U+040F/U+045F), this letter is always in the incorrect order
    // when comparing code points but must be in the correct order when collation

    // UTF-8

    TESTX(uni::casesens::utf8_compare("Џ", "Ч") < 0);
    TESTX(uni::casesens::utf8_compare("џ", "ш") > 0);
    TESTX(uni::caseless::utf8_compare("Џ", "ш") > 0);
    TESTX(uni::caseless::utf8_compare("Џ", "џ") == 0);

    TESTX(uni::casesens::utf8_collate("Џ", "Ч") > 0);
    TESTX(uni::casesens::utf8_collate("џ", "ш") < 0);
    TESTX(uni::caseless::utf8_collate("Џ", "ш") < 0);
    TESTX(uni::caseless::utf8_collate("Џ", "џ") == 0);

    // UTF-16

    TESTX(uni::casesens::utf16_compare(u"Џ", u"Ч") < 0);
    TESTX(uni::casesens::utf16_compare(u"џ", u"ш") > 0);
    TESTX(uni::caseless::utf16_compare(u"Џ", u"ш") > 0);
    TESTX(uni::caseless::utf16_compare(u"Џ", u"џ") == 0);

    TESTX(uni::casesens::utf16_collate(u"Џ", u"Ч") > 0);
    TESTX(uni::casesens::utf16_collate(u"џ", u"ш") < 0);
    TESTX(uni::caseless::utf16_collate(u"Џ", u"ш") < 0);
    TESTX(uni::caseless::utf16_collate(u"Џ", u"џ") == 0);

    // Make sure long strings optimization doesn't break the functions

    TESTX(uni::casesens::utf8_compare(uni::utf32to8(U"\x10000"), uni::utf32to8(U"\x10001")) != 0);
    TESTX(uni::casesens::utf8_collate(uni::utf32to8(U"\x10000"), uni::utf32to8(U"\x10001")) != 0);

    TESTX(uni::casesens::utf16_compare(uni::utf32to16u(U"\x10000"), uni::utf32to16u(U"\x10001")) != 0);
    TESTX(uni::casesens::utf16_collate(uni::utf32to16u(U"\x10000"), uni::utf32to16u(U"\x10001")) != 0);
}

void test_case_search()
{
    uni::search s;

    TESTX((s = uni::casesens::utf8_search("バババババババババカバ", "バカ")) && s.pos() == 24 && s.end_pos() == 30);
    TESTX((s = uni::caseless::utf8_search("бббббббббкбб", "ББК")) && s.pos() == 14 && s.end_pos() == 20);
    TESTX((s = uni::caseless::utf8_search("bbbbbbbbbkbb", "BBK")) && s.pos() == 7 && s.end_pos() == 10);
    TESTX((s = uni::casesens::utf8_search("バカ", "バカ")) && s.pos() == 0 && s.end_pos() == 6);
    TESTX((s = uni::caseless::utf8_search("бака", "БАКА")) && s.pos() == 0 && s.end_pos() == 8);
    TESTX((s = uni::caseless::utf8_search("baka", "BAKA")) && s.pos() == 0 && s.end_pos() == 4);
    TESTX(!uni::casesens::utf8_search("バ", "バカ"));
    TESTX(!uni::caseless::utf8_search("б", "БАКА"));
    TESTX(!uni::caseless::utf8_search("b", "BAKA"));

    TESTX((s = uni::casesens::utf16_search(u"バババババババババカバ", u"バカ")) && s.pos() == 8 && s.end_pos() == 10);
    TESTX((s = uni::caseless::utf16_search(u"бббббббббкбб", u"ББК")) && s.pos() == 7 && s.end_pos() == 10);
    TESTX((s = uni::caseless::utf16_search(u"bbbbbbbbbkbb", u"BBK")) && s.pos() == 7 && s.end_pos() == 10);
    TESTX((s = uni::casesens::utf16_search(u"バカ", u"バカ")) && s.pos() == 0 && s.end_pos() == 2);
    TESTX((s = uni::caseless::utf16_search(u"бака", u"БАКА")) && s.pos() == 0 && s.end_pos() == 4);
    TESTX((s = uni::caseless::utf16_search(u"baka", u"BAKA")) && s.pos() == 0 && s.end_pos() == 4);
    TESTX(!uni::casesens::utf16_search(u"バ", u"バカ"));
    TESTX(!uni::caseless::utf16_search(u"б", u"БАКА"));
    TESTX(!uni::caseless::utf16_search(u"b", u"BAKA"));

    // Empty strings

    TESTX((s = uni::casesens::utf8_search("バカ", "")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = uni::caseless::utf8_search("バカ", "")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = uni::casesens::utf8_search("", "")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = uni::caseless::utf8_search("", "")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX(!uni::casesens::utf8_search("", "バカ"));
    TESTX(!uni::caseless::utf8_search("", "バカ"));

    TESTX((s = uni::casesens::utf16_search(u"バカ", u"")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = uni::caseless::utf16_search(u"バカ", u"")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = uni::casesens::utf16_search(u"", u"")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX((s = uni::caseless::utf16_search(u"", u"")) && s.pos() == 0 && s.end_pos() == 0);
    TESTX(!uni::casesens::utf16_search(u"", u"バカ"));
    TESTX(!uni::caseless::utf16_search(u"", u"バカ"));
}

void test_case_ill_formed()
{
    // Ill-formed UTF must always produce replacement character U+FFFD
    // Check that all functions are consistent in this case

    uni::search s;

    // UTF-8 (Use 0x80 and 0x81 they are lone tails)

    TESTX(uni::casesens::utf8_compare("\x80", "\x81") == 0);
    TESTX(uni::caseless::utf8_compare("\x80", "\x81") == 0);

    TESTX(uni::casesens::utf8_collate("\x80", "\x81") == 0);
    TESTX(uni::caseless::utf8_collate("\x80", "\x81") == 0);

    TESTX((s = uni::casesens::utf8_search("abc\x80", "\x81")) && s.pos() == 3 && s.end_pos() == 4);
    TESTX((s = uni::caseless::utf8_search("abc\x80", "\x81")) && s.pos() == 3 && s.end_pos() == 4);

    TESTX(uni::cases::utf8_lower("\x80") == "\xEF\xBF\xBD");
    TESTX(uni::cases::utf8_upper("\x80") == "\xEF\xBF\xBD");
    TESTX(uni::cases::utf8_fold("\x80") == "\xEF\xBF\xBD");

    // UTF-16 (Use 0xDC00 and 0xDC01 they are lone low surrogates)

    TESTX(uni::casesens::utf16_compare(u"\xDC00", u"\xDC01") == 0);
    TESTX(uni::caseless::utf16_compare(u"\xDC00", u"\xDC01") == 0);

    TESTX(uni::casesens::utf16_collate(u"\xDC00", u"\xDC01") == 0);
    TESTX(uni::caseless::utf16_collate(u"\xDC00", u"\xDC01") == 0);

    TESTX((s = uni::casesens::utf16_search(u"abc\xDC00", u"\xDC01")) && s.pos() == 3 && s.end_pos() == 4);
    TESTX((s = uni::caseless::utf16_search(u"abc\xDC00", u"\xDC01")) && s.pos() == 3 && s.end_pos() == 4);

    TESTX(uni::cases::utf16_lower(u"\xDC00") == u"\xFFFD");
    TESTX(uni::cases::utf16_upper(u"\xDC00") == u"\xFFFD");
    TESTX(uni::cases::utf16_fold(u"\xDC00") == u"\xFFFD");
}

void test_case_full_case()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    // Use ligature ﬁ (U+FB01) for testing

    uni::search s;

    // UTF-8

    TESTX(uni::caseless::utf8_compare("abcﬁ", "ABCFI") == 0);
    TESTX(uni::caseless::utf8_compare("abcﬁ", "ABCF") > 0);
    TESTX(uni::caseless::utf8_compare("abcﬁ", "ABCFX") < 0);

    TESTX(uni::caseless::utf8_collate("abcﬁ", "ABCFI") == 0);
    TESTX(uni::caseless::utf8_collate("abcﬁ", "ABCF") > 0);
    TESTX(uni::caseless::utf8_collate("abcﬁ", "ABCFX") < 0);

    TESTX((s = uni::caseless::utf8_search("abcﬁ", "FI")) && s.pos() == 3 && s.end_pos() == 6);
    TESTX((s = uni::caseless::utf8_search("ABCFI", "ﬁ")) && s.pos() == 3 && s.end_pos() == 5);

    // Complicated cases
    TESTX(!uni::caseless::utf8_search("f", "ﬁ"));
    TESTX(!uni::caseless::utf8_search("ﬁ", "f"));
    TESTX(!uni::caseless::utf8_search("ﬁ", "i"));
    TESTX(!uni::caseless::utf8_search("abcﬁ", "f"));
    TESTX(!uni::caseless::utf8_search("abcﬁ", "i"));
    TESTX((s = uni::caseless::utf8_search("abcﬁf", "f")) && s.pos() == 6 && s.end_pos() == 7);
    TESTX((s = uni::caseless::utf8_search("abcﬁ", "ﬁ")) && s.pos() == 3 && s.end_pos() == 6);

    // UTF-16

    TESTX(uni::caseless::utf16_compare(u"abcﬁ", u"ABCFI") == 0);
    TESTX(uni::caseless::utf16_compare(u"abcﬁ", u"ABCF") > 0);
    TESTX(uni::caseless::utf16_compare(u"abcﬁ", u"ABCFX") < 0);

    TESTX(uni::caseless::utf16_collate(u"abcﬁ", u"ABCFI") == 0);
    TESTX(uni::caseless::utf16_collate(u"abcﬁ", u"ABCF") > 0);
    TESTX(uni::caseless::utf16_collate(u"abcﬁ", u"ABCFX") < 0);

    TESTX((s = uni::caseless::utf16_search(u"abcﬁ", u"FI")) && s.pos() == 3 && s.end_pos() == 4);
    TESTX((s = uni::caseless::utf16_search(u"ABCFI", u"ﬁ")) && s.pos() == 3 && s.end_pos() == 5);

    // Complicated cases
    TESTX(!uni::caseless::utf16_search(u"f", u"ﬁ"));
    TESTX(!uni::caseless::utf16_search(u"ﬁ", u"f"));
    TESTX(!uni::caseless::utf16_search(u"ﬁ", u"i"));
    TESTX(!uni::caseless::utf16_search(u"abcﬁ", u"f"));
    TESTX(!uni::caseless::utf16_search(u"abcﬁ", u"i"));
    TESTX((s = uni::caseless::utf16_search(u"abcﬁf", u"f")) && s.pos() == 4 && s.end_pos() == 5);
    TESTX((s = uni::caseless::utf16_search(u"abcﬁ", u"ﬁ")) && s.pos() == 3 && s.end_pos() == 4);

#endif // UNI_ALGO_DISABLE_FULL_CASE
}

void test_case_upper_lower_fold()
{
    TESTX(uni::cases::utf8_upper("Test") == "TEST");
    TESTX(uni::cases::utf8_upper("два") == "ДВА");
    TESTX(uni::cases::utf8_fold("TEST") == "test");
    TESTX(uni::cases::utf8_fold("ДВА") == "два");
    TESTX(uni::cases::utf8_lower("TEST") == "test");
    TESTX(uni::cases::utf8_lower("ДВА") == "два");

    TESTX(uni::cases::utf16_upper(u"Test") == u"TEST");
    TESTX(uni::cases::utf16_upper(u"два") == u"ДВА");
    TESTX(uni::cases::utf16_fold(u"TEST") == u"test");
    TESTX(uni::cases::utf16_fold(u"ДВА") == u"два");
    TESTX(uni::cases::utf16_lower(u"TEST") == u"test");
    TESTX(uni::cases::utf16_lower(u"ДВА") == u"два");

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::utf8_upper("Straße") == "STRASSE");
    TESTX(uni::cases::utf8_upper("ﬁ ﬂ") == "FI FL");
    TESTX(uni::cases::utf8_fold("Straße") == "strasse");
    TESTX(uni::cases::utf8_fold("ﬁ ﬂ") == "fi fl");
    TESTX(uni::cases::utf8_lower("İ") == "i\xCC\x87");

    TESTX(uni::cases::utf8_upper("\xE1\xBE\x80") == "\xE1\xBC\x88\xCE\x99");
    TESTX(uni::cases::utf8_upper("\xE1\xBE\x88") == "\xE1\xBC\x88\xCE\x99");

    TESTX(uni::cases::utf16_upper(u"Straße") == u"STRASSE");
    TESTX(uni::cases::utf16_upper(u"ﬁ ﬂ") == u"FI FL");
    TESTX(uni::cases::utf16_fold(u"Straße") == u"strasse");
    TESTX(uni::cases::utf16_fold(u"ﬁ ﬂ") == u"fi fl");
    TESTX(uni::cases::utf16_lower(u"İ") == u"i\x0307");

    TESTX(uni::cases::utf16_upper(u"\x1F80") == u"\x1F08\x0399");
    TESTX(uni::cases::utf16_upper(u"\x1F88") == u"\x1F08\x0399");
#endif // UNI_ALGO_DISABLE_FULL_CASE
}

void test_case_final_sigma()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    // UTF-8

    TESTX(uni::cases::utf8_lower("Σ") == "σ");
    TESTX(uni::cases::utf8_lower(":::Σ:::") == ":::σ:::");
    TESTX(uni::cases::utf8_lower("F:::Σ:::") == "f:::ς:::");
    TESTX(uni::cases::utf8_lower("F:::Σ:::F") == "f:::σ:::f");
    TESTX(uni::cases::utf8_lower(":::Σ:::F") == ":::σ:::f");
    TESTX(uni::cases::utf8_lower("Σ:::F") == "σ:::f");
    TESTX(uni::cases::utf8_lower("F:::Σ") == "f:::ς");
    TESTX(uni::cases::utf8_lower("ΣF") == "σf");
    TESTX(uni::cases::utf8_lower("FΣ") == "fς");

    // UTF-16

    TESTX(uni::cases::utf16_lower(u"Σ") == u"σ");
    TESTX(uni::cases::utf16_lower(u":::Σ:::") == u":::σ:::");
    TESTX(uni::cases::utf16_lower(u"F:::Σ:::") == u"f:::ς:::");
    TESTX(uni::cases::utf16_lower(u"F:::Σ:::F") == u"f:::σ:::f");
    TESTX(uni::cases::utf16_lower(u":::Σ:::F") == u":::σ:::f");
    TESTX(uni::cases::utf16_lower(u"Σ:::F") == u"σ:::f");
    TESTX(uni::cases::utf16_lower(u"F:::Σ") == u"f:::ς");
    TESTX(uni::cases::utf16_lower(u"ΣF") == u"σf");
    TESTX(uni::cases::utf16_lower(u"FΣ") == u"fς");

    // Ill-formed UTF

    TESTX(uni::cases::utf8_lower("F:\x80::Σ:::") == "f:\xEF\xBF\xBD::σ:::");
    TESTX(uni::cases::utf8_lower("F:\x80::Σ::\x80:F") == "f:\xEF\xBF\xBD::σ::\xEF\xBF\xBD:f");
    TESTX(uni::cases::utf8_lower(":::Σ::\x80:F") == ":::σ::\xEF\xBF\xBD:f");

    TESTX(uni::cases::utf16_lower(u"F:\xDC00::Σ:::") == u"f:\xFFFD::σ:::");
    TESTX(uni::cases::utf16_lower(u"F:\xDC00::Σ::\xDC00:F") == u"f:\xFFFD::σ::\xFFFD:f");
    TESTX(uni::cases::utf16_lower(u":::Σ::\xDC00:F") == u":::σ::\xFFFD:f");

#endif /* UNI_ALGO_DISABLE_FULL_CASE */
}

void test_case_sort_key()
{
#ifdef UNI_ALGO_EXPERIMENTAL
    TESTX(uni::casesens::utf8_collate("Џ", "Ч") > 0 && uni::casesens::utf8_sortkey("Џ") > uni::casesens::utf8_sortkey("Ч"));
    TESTX(uni::casesens::utf8_collate("џ", "ш") < 0 && uni::casesens::utf8_sortkey("џ") < uni::casesens::utf8_sortkey("ш"));
    TESTX(uni::caseless::utf8_collate("Џ", "ш") < 0 && uni::caseless::utf8_sortkey("Џ") < uni::caseless::utf8_sortkey("ш"));
    TESTX(uni::caseless::utf8_collate("Џ", "џ") == 0 && uni::caseless::utf8_sortkey("Џ") == uni::caseless::utf8_sortkey("џ"));

    TESTX(uni::casesens::utf16_collate(u"Џ", u"Ч") > 0 && uni::casesens::utf16_sortkey(u"Џ") > uni::casesens::utf16_sortkey(u"Ч"));
    TESTX(uni::casesens::utf16_collate(u"џ", u"ш") < 0 && uni::casesens::utf16_sortkey(u"џ") < uni::casesens::utf16_sortkey(u"ш"));
    TESTX(uni::caseless::utf16_collate(u"Џ", u"ш") < 0 && uni::caseless::utf16_sortkey(u"Џ") < uni::caseless::utf16_sortkey(u"ш"));
    TESTX(uni::caseless::utf16_collate(u"Џ", u"џ") == 0 && uni::caseless::utf16_sortkey(u"Џ") == uni::caseless::utf16_sortkey(u"џ"));

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::caseless::utf8_collate("ﬁ", "FI") == 0 && uni::caseless::utf8_sortkey("ﬁ") == uni::caseless::utf8_sortkey("FI"));
    TESTX(uni::caseless::utf16_collate(u"ﬁ", u"FI") == 0 && uni::caseless::utf16_sortkey(u"ﬁ") == uni::caseless::utf16_sortkey(u"FI"));
    TESTX(uni::caseless::utf8_collate("\xCE\x90", "\xCE\xB9\xCC\x88\xCC\x81") == 0 && uni::caseless::utf8_sortkey("\xCE\x90") == uni::caseless::utf8_sortkey("\xCE\xB9\xCC\x88\xCC\x81"));
    TESTX(uni::caseless::utf16_collate(u"\x0390", u"\x03B9\x0308\x0301") == 0 && uni::caseless::utf16_sortkey(u"\x0390") == uni::caseless::utf16_sortkey(u"\x03B9\x0308\x0301"));
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_EXPERIMENTAL
}

void test_case_like()
{
#ifdef UNI_ALGO_EXPERIMENTAL
    TESTX(uni::caseless::utf8_like("abc123xyz789", "%ABC%XYZ%"));
    TESTX(uni::caseless::utf8_like("abc123xyz", "%ABC%XYZ%"));
    TESTX(uni::caseless::utf8_like("abc123xyz", "%%%ABC%%%XYZ%%%"));

    TESTX(uni::caseless::utf8_like("abc123xyz", "ABC123XYZ"));
    TESTX(uni::caseless::utf8_like("abc123xyz", "_BC___XY_"));
    TESTX(uni::caseless::utf8_like("abc123xyz", "%A_C%X__%"));

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(!uni::caseless::utf8_like("ﬃ", "ﬁ"));
    TESTX(!uni::caseless::utf8_like("ﬃ", "%ﬁ%"));
    TESTX(!uni::caseless::utf8_like("ﬃ", "ﬁi"));
    TESTX(uni::caseless::utf8_like("ﬃ", "ffi"));
    TESTX(uni::caseless::utf8_like("ﬃ", "FFI"));
    TESTX(uni::caseless::utf8_like("ﬃ", "%FFI%"));
    TESTX(uni::caseless::utf8_like("abcﬃ", "%FFI"));
    TESTX(uni::caseless::utf8_like("ﬃabc", "FFI%"));
    TESTX(!uni::caseless::utf8_like("abcﬃxyz", "%FF%"));
    TESTX(!uni::caseless::utf8_like("abcﬃxyz", "%FF_%"));
    TESTX(uni::caseless::utf8_like("abcﬃxyz", "%FFI%"));

    TESTX(uni::caseless::utf8_like("abcﬃabc", "_B_%FFI%_B_"));
    TESTX(!uni::caseless::utf8_like("abcﬃabc", "_B__%FFI%_B_"));
    TESTX(!uni::caseless::utf8_like("ﬃ", "ff"));
#endif // UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::caseless::utf8_like("AB", "A%B"));
    TESTX(uni::caseless::utf8_like("AB", "%A%B"));
    TESTX(uni::caseless::utf8_like("AB", "%A%B%"));

    TESTX(uni::caseless::utf8_like("бббббббббкбб", "%ббк%"));
    TESTX(uni::caseless::utf8_like("бббббббббкбб", "%ББК%"));

    TESTX(uni::caseless::utf8_like("%%%%%%%%%%бк_бб", "%/%БК/_%", U'/'));

    TESTX(!uni::caseless::utf8_like("ABC", "ABC%", U'%'));
    TESTX(!uni::caseless::utf8_like("ABC", "ABC_", U'_'));

    // Empty strings

    TESTX(!uni::caseless::utf8_like("abc", ""));
    TESTX(uni::caseless::utf8_like("", ""));
#endif // UNI_ALGO_EXPERIMENTAL
}

void test_case_locale_lt()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    uni::locale locale("lt");

    // SpecialCasing.txt

    // # Lithuanian

    // # Lithuanian retains the dot in a lowercase i when followed by accents.

    // # Remove DOT ABOVE (U+0307) after "i" with upper or titlecase

    TESTX(uni::cases::utf8_upper("i\xCC\x87", locale) == "I");
    TESTX(uni::cases::utf8_upper("i\xCC\x87w\xCC\x87i\xCC\x87", locale) == "IW\xCC\x87I");

    TESTX(uni::cases::utf16_upper(u"i\x0307", locale) == u"I");
    TESTX(uni::cases::utf16_upper(u"i\x0307w\x0307i\x0307", locale) == u"IW\x0307I");

    // # Introduce an explicit dot above when lowercasing capital I's and J's
    // # whenever there are more accents above.
    // # (of the accents used in Lithuanian: grave, acute, tilde above, and ogonek)

    // UTF-8

    TESTX(uni::cases::utf8_lower("\x49", locale) == "\x69");
    TESTX(uni::cases::utf8_lower("\x4A", locale) == "\x6A");
    TESTX(uni::cases::utf8_lower("\xC4\xAE", locale) == "\xC4\xAF");
    // If more accents above introduce U+0307
    TESTX(uni::cases::utf8_lower("\x49\xCC\x80", locale) == "\x69\xCC\x87\xCC\x80");
    TESTX(uni::cases::utf8_lower("\x4A\xCC\x80", locale) == "\x6A\xCC\x87\xCC\x80");
    TESTX(uni::cases::utf8_lower("\xC4\xAE\xCC\x80", locale) == "\xC4\xAF\xCC\x87\xCC\x80");
    // Intervine with U+0315
    TESTX(uni::cases::utf8_lower("\x49\xCC\x95\xCC\x80", locale) == "\x69\xCC\x87\xCC\x95\xCC\x80");
    TESTX(uni::cases::utf8_lower("\x4A\xCC\x95\xCC\x80", locale) == "\x6A\xCC\x87\xCC\x95\xCC\x80");
    TESTX(uni::cases::utf8_lower("\xC4\xAE\xCC\x95\xCC\x80", locale) == "\xC4\xAF\xCC\x87\xCC\x95\xCC\x80");
    // Intervine with W
    TESTX(uni::cases::utf8_lower("\x49W\xCC\x80", locale) == "\x69w\xCC\x80");
    TESTX(uni::cases::utf8_lower("\x4AW\xCC\x80", locale) == "\x6Aw\xCC\x80");
    TESTX(uni::cases::utf8_lower("\xC4\xAEW\xCC\x80", locale) == "\xC4\xAFw\xCC\x80");

    TESTX(uni::cases::utf8_lower("\xC3\x8C", locale) == "\x69\xCC\x87\xCC\x80");
    TESTX(uni::cases::utf8_lower("\xC3\x8D", locale) == "\x69\xCC\x87\xCC\x81");
    TESTX(uni::cases::utf8_lower("\xC4\xA8", locale) == "\x69\xCC\x87\xCC\x83");

    // UTF-16

    TESTX(uni::cases::utf16_lower(u"\x0049", locale) == u"\x0069");
    TESTX(uni::cases::utf16_lower(u"\x004A", locale) == u"\x006A");
    TESTX(uni::cases::utf16_lower(u"\x012E", locale) == u"\x012F");
    // If more accents above introduce U+0307
    TESTX(uni::cases::utf16_lower(u"\x0049\x0300", locale) == u"\x0069\x0307\x0300");
    TESTX(uni::cases::utf16_lower(u"\x004A\x0300", locale) == u"\x006A\x0307\x0300");
    TESTX(uni::cases::utf16_lower(u"\x012E\x0300", locale) == u"\x012F\x0307\x0300");
    // Intervine with U+0315
    TESTX(uni::cases::utf16_lower(u"\x0049\x0315\x0300", locale) == u"\x0069\x0307\x0315\x0300");
    TESTX(uni::cases::utf16_lower(u"\x004A\x0315\x0300", locale) == u"\x006A\x0307\x0315\x0300");
    TESTX(uni::cases::utf16_lower(u"\x012E\x0315\x0300", locale) == u"\x012F\x0307\x0315\x0300");
    // Intervine with W
    TESTX(uni::cases::utf16_lower(u"\x0049W\x0300", locale) == u"\x0069w\x0300");
    TESTX(uni::cases::utf16_lower(u"\x004AW\x0300", locale) == u"\x006Aw\x0300");
    TESTX(uni::cases::utf16_lower(u"\x012EW\x0300", locale) == u"\x012Fw\x0300");

    TESTX(uni::cases::utf16_lower(u"\x00CC", locale) == u"\x0069\x0307\x0300");
    TESTX(uni::cases::utf16_lower(u"\x00CD", locale) == u"\x0069\x0307\x0301");
    TESTX(uni::cases::utf16_lower(u"\x0128", locale) == u"\x0069\x0307\x0303");

#endif // UNI_ALGO_DISABLE_FULL_CASE
}

void test_case_locale_tr_az()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    uni::locale locale("tr");

    // SpecialCasing.txt

    // # Turkish and Azeri

    // # I and i-dotless; I-dot and i are case pairs in Turkish and Azeri
    // # The following rules handle those cases.

    TESTX(uni::cases::utf8_lower("İ", locale) == "i");
    TESTX(uni::cases::utf8_lower("\xC4\xB0", locale) == "\x69");

    TESTX(uni::cases::utf16_lower(u"İ", locale) == u"i");
    TESTX(uni::cases::utf16_lower(u"\x0130", locale) == u"\x0069");

    // # When lowercasing, remove dot_above in the sequence I + dot_above, which will turn into i.
    // # This matches the behavior of the canonically equivalent I-dot_above

    TESTX(uni::cases::utf8_lower("I\xCC\x87", locale) == "i");

    TESTX(uni::cases::utf16_lower(u"I\x0307", locale) == u"i");

    // # When lowercasing, unless an I is before a dot_above, it turns into a dotless i.

    TESTX(uni::cases::utf8_lower("I", locale) == "ı");
    TESTX(uni::cases::utf8_lower("\x49", locale) == "\xC4\xB1");

    TESTX(uni::cases::utf16_lower(u"I", locale) == u"ı");
    TESTX(uni::cases::utf16_lower(u"\x0049", locale) == u"\x0131");

    // # When uppercasing, i turns into a dotted capital I

    TESTX(uni::cases::utf8_upper("i", locale) == "İ");
    TESTX(uni::cases::utf8_upper("\x69", locale) == "\xC4\xB0");

    TESTX(uni::cases::utf16_upper(u"i", locale) == u"İ");
    TESTX(uni::cases::utf16_upper(u"\x0069", locale) == u"\x0130");

#endif // UNI_ALGO_DISABLE_FULL_CASE
}

void test_case_locale_el()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    uni::locale locale("el");

    // http://site.icu-project.org/design/case/greek-upper

    TESTX(uni::cases::utf8_upper("άδικος, κείμενο, ίριδα", locale) == "ΑΔΙΚΟΣ, ΚΕΙΜΕΝΟ, ΙΡΙΔΑ");
    TESTX(uni::cases::utf8_upper("Πατάτα", locale) == "ΠΑΤΑΤΑ");
    TESTX(uni::cases::utf8_upper("Αέρας, Μυστήριο, Ωραίο", locale) == "ΑΕΡΑΣ, ΜΥΣΤΗΡΙΟ, ΩΡΑΙΟ");
    TESTX(uni::cases::utf8_upper("Μαΐου, Πόρος, Ρύθμιση", locale) == "ΜΑΪΟΥ, ΠΟΡΟΣ, ΡΥΘΜΙΣΗ");
    TESTX(uni::cases::utf8_upper("ΰ, Τηρώ, Μάιος", locale) == "Ϋ, ΤΗΡΩ, ΜΑΪΟΣ");
    TESTX(uni::cases::utf8_upper("άυλος", locale) == "ΑΫΛΟΣ");
    TESTX(uni::cases::utf8_upper("ΑΫΛΟΣ", locale) == "ΑΫΛΟΣ");
    TESTX(uni::cases::utf8_upper("Άκλιτα ρήματα ή άκλιτες μετοχές", locale) == "ΑΚΛΙΤΑ ΡΗΜΑΤΑ Ή ΑΚΛΙΤΕΣ ΜΕΤΟΧΕΣ"); // Η Ή Ή
    TESTX(uni::cases::utf8_upper("Επειδή η αναγνώριση της αξιοπρέπειας", locale) == "ΕΠΕΙΔΗ Η ΑΝΑΓΝΩΡΙΣΗ ΤΗΣ ΑΞΙΟΠΡΕΠΕΙΑΣ");
    TESTX(uni::cases::utf8_upper("νομικού ή διεθνούς", locale) == "ΝΟΜΙΚΟΥ Ή ΔΙΕΘΝΟΥΣ"); // Η Ή Ή
    TESTX(uni::cases::utf8_upper("Ἐπειδὴ ἡ ἀναγνώριση", locale) == "ΕΠΕΙΔΗ Η ΑΝΑΓΝΩΡΙΣΗ");
    TESTX(uni::cases::utf8_upper("νομικοῦ ἢ διεθνοῦς", locale) == "ΝΟΜΙΚΟΥ Ή ΔΙΕΘΝΟΥΣ"); // Η Ή Ή
    TESTX(uni::cases::utf8_upper("Νέο, Δημιουργία", locale) == "ΝΕΟ, ΔΗΜΙΟΥΡΓΙΑ");
    TESTX(uni::cases::utf8_upper("Ελάτε να φάτε τα καλύτερα παϊδάκια!", locale) == "ΕΛΑΤΕ ΝΑ ΦΑΤΕ ΤΑ ΚΑΛΥΤΕΡΑ ΠΑΪΔΑΚΙΑ!");
    TESTX(uni::cases::utf8_upper("Μαΐου, τρόλεϊ", locale) == "ΜΑΪΟΥ, ΤΡΟΛΕΪ");
    TESTX(uni::cases::utf8_upper("Το ένα ή το άλλο.", locale) == "ΤΟ ΕΝΑ Ή ΤΟ ΑΛΛΟ."); // Η Ή Ή
    TESTX(uni::cases::utf8_upper("ρωμέικα", locale) == "ΡΩΜΕΪΚΑ");

    TESTX(uni::cases::utf8_upper("άZυλος", locale) == "ΑZΥΛΟΣ");

    TESTX(uni::cases::utf16_upper(u"άδικος, κείμενο, ίριδα", locale) == u"ΑΔΙΚΟΣ, ΚΕΙΜΕΝΟ, ΙΡΙΔΑ");
    TESTX(uni::cases::utf16_upper(u"Πατάτα", locale) == u"ΠΑΤΑΤΑ");
    TESTX(uni::cases::utf16_upper(u"Αέρας, Μυστήριο, Ωραίο", locale) == u"ΑΕΡΑΣ, ΜΥΣΤΗΡΙΟ, ΩΡΑΙΟ");
    TESTX(uni::cases::utf16_upper(u"Μαΐου, Πόρος, Ρύθμιση", locale) == u"ΜΑΪΟΥ, ΠΟΡΟΣ, ΡΥΘΜΙΣΗ");
    TESTX(uni::cases::utf16_upper(u"ΰ, Τηρώ, Μάιος", locale) == u"Ϋ, ΤΗΡΩ, ΜΑΪΟΣ");
    TESTX(uni::cases::utf16_upper(u"άυλος", locale) == u"ΑΫΛΟΣ");
    TESTX(uni::cases::utf16_upper(u"ΑΫΛΟΣ", locale) == u"ΑΫΛΟΣ");
    TESTX(uni::cases::utf16_upper(u"Άκλιτα ρήματα ή άκλιτες μετοχές", locale) == u"ΑΚΛΙΤΑ ΡΗΜΑΤΑ Ή ΑΚΛΙΤΕΣ ΜΕΤΟΧΕΣ"); // Η Ή Ή
    TESTX(uni::cases::utf16_upper(u"Επειδή η αναγνώριση της αξιοπρέπειας", locale) == u"ΕΠΕΙΔΗ Η ΑΝΑΓΝΩΡΙΣΗ ΤΗΣ ΑΞΙΟΠΡΕΠΕΙΑΣ");
    TESTX(uni::cases::utf16_upper(u"νομικού ή διεθνούς", locale) == u"ΝΟΜΙΚΟΥ Ή ΔΙΕΘΝΟΥΣ"); // Η Ή Ή
    TESTX(uni::cases::utf16_upper(u"Ἐπειδὴ ἡ ἀναγνώριση", locale) == u"ΕΠΕΙΔΗ Η ΑΝΑΓΝΩΡΙΣΗ");
    TESTX(uni::cases::utf16_upper(u"νομικοῦ ἢ διεθνοῦς", locale) == u"ΝΟΜΙΚΟΥ Ή ΔΙΕΘΝΟΥΣ"); // Η Ή Ή
    TESTX(uni::cases::utf16_upper(u"Νέο, Δημιουργία", locale) == u"ΝΕΟ, ΔΗΜΙΟΥΡΓΙΑ");
    TESTX(uni::cases::utf16_upper(u"Ελάτε να φάτε τα καλύτερα παϊδάκια!", locale) == u"ΕΛΑΤΕ ΝΑ ΦΑΤΕ ΤΑ ΚΑΛΥΤΕΡΑ ΠΑΪΔΑΚΙΑ!");
    TESTX(uni::cases::utf16_upper(u"Μαΐου, τρόλεϊ", locale) == u"ΜΑΪΟΥ, ΤΡΟΛΕΪ");
    TESTX(uni::cases::utf16_upper(u"Το ένα ή το άλλο.", locale) == u"ΤΟ ΕΝΑ Ή ΤΟ ΑΛΛΟ."); // Η Ή Ή
    TESTX(uni::cases::utf16_upper(u"ρωμέικα", locale) == u"ΡΩΜΕΪΚΑ");

    TESTX(uni::cases::utf16_upper(u"άZυλος", locale) == u"ΑZΥΛΟΣ");

#endif // UNI_ALGO_DISABLE_FULL_CASE
}

void test_case_title()
{
#ifndef UNI_ALGO_DISABLE_BREAK_WORD

    TESTX(uni::cases::utf8_title("t") == "T");
    TESTX(uni::cases::utf8_title("tEsT") == "Test");
    TESTX(uni::cases::utf8_title("tEsT tEST TesT") == "Test Test Test");
    TESTX(uni::cases::utf8_title(" tEsT tEST TesT ") == " Test Test Test ");
    TESTX(uni::cases::utf8_title("  tEsT  tEST  TesT  ") == "  Test  Test  Test  ");

    TESTX(uni::cases::utf8_title("_t_") == "_T_");
    TESTX(uni::cases::utf8_title("_tEsT_") == "_Test_");
    TESTX(uni::cases::utf8_title("_tEsT_ _tEST_ _TesT_") == "_Test_ _Test_ _Test_");
    TESTX(uni::cases::utf8_title(" _tEsT_ _tEST_ _TesT_ ") == " _Test_ _Test_ _Test_ ");
    TESTX(uni::cases::utf8_title("  __tEsT__  __tEST__  __TesT__  ") == "  __Test__  __Test__  __Test__  ");

    TESTX(uni::cases::utf16_title(u"t") == u"T");
    TESTX(uni::cases::utf16_title(u"tEsT") == u"Test");
    TESTX(uni::cases::utf16_title(u"tEsT tEST TesT") == u"Test Test Test");
    TESTX(uni::cases::utf16_title(u" tEsT tEST TesT ") == u" Test Test Test ");
    TESTX(uni::cases::utf16_title(u"  tEsT  tEST  TesT  ") == u"  Test  Test  Test  ");

    TESTX(uni::cases::utf16_title(u"_t_") == u"_T_");
    TESTX(uni::cases::utf16_title(u"_tEsT_") == u"_Test_");
    TESTX(uni::cases::utf16_title(u"_tEsT_ _tEST_ _TesT_") == u"_Test_ _Test_ _Test_");
    TESTX(uni::cases::utf16_title(u" _tEsT_ _tEST_ _TesT_ ") == u" _Test_ _Test_ _Test_ ");
    TESTX(uni::cases::utf16_title(u"  __tEsT__  __tEST__  __TesT__  ") == u"  __Test__  __Test__  __Test__  ");

#ifndef UNI_ALGO_DISABLE_FULL_CASE

    TESTX(uni::cases::utf8_title("ﬃ") == "Ffi");
    TESTX(uni::cases::utf8_title("ﬃ ﬁ ﬂ") == "Ffi Fi Fl");
    TESTX(uni::cases::utf8_title("_ﬃ_ _ﬁ_ _ﬂ_") == "_Ffi_ _Fi_ _Fl_");

    TESTX(uni::cases::utf16_title(u"ﬃ") == u"Ffi");
    TESTX(uni::cases::utf16_title(u"ﬃ ﬁ ﬂ") == u"Ffi Fi Fl");
    TESTX(uni::cases::utf16_title(u"_ﬃ_ _ﬁ_ _ﬂ_") == u"_Ffi_ _Fi_ _Fl_");

#endif // UNI_ALGO_DISABLE_FULL_CASE

    // Found a strange behaviour in ICU when compared results.
    // ICU never title cases U+0345 but upper cases it just fine to U+0399.
    // Don't know why, it must be title cased according to title case algorithm,
    // maybe it just by design because U+0345 is pretty complicated code point.
    // It doesn't actually matter much because a word can't start
    // with such code point just want to mention it here.

    TESTX(uni::cases::utf8_title("\xCD\x85\xCD\x85\xCD\x85") == "\xCE\x99\xCD\x85\xCD\x85");

    TESTX(uni::cases::utf16_title(u"\x0345\x0345\x0345") == u"\x0399\x0345\x0345");

#endif // UNI_ALGO_DISABLE_BREAK_WORD
}

void test_case_title_locale()
{
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    TESTX(uni::cases::utf8_title("ijtest ijTeSt ﬃtEsT", uni::locale("nl")) == "IJtest IJtest Ffitest");
    TESTX(uni::cases::utf8_title("ijtest ijTeSt ﬃtEsT", uni::locale("tr")) == "İjtest İjtest Ffitest");
    TESTX(uni::cases::utf8_title("ijtest ijTeSt ﬃtEsT", uni::locale("lt")) == "Ijtest Ijtest Ffitest"); // The same as by default

    TESTX(uni::cases::utf16_title(u"ijtest ijTeSt ﬃtEsT", uni::locale("nl")) == u"IJtest IJtest Ffitest");
    TESTX(uni::cases::utf16_title(u"ijtest ijTeSt ﬃtEsT", uni::locale("tr")) == u"İjtest İjtest Ffitest");
    TESTX(uni::cases::utf16_title(u"ijtest ijTeSt ﬃtEsT", uni::locale("lt")) == u"Ijtest Ijtest Ffitest"); // The same as by default

    // Test for very stupid mistake
    TESTX(uni::cases::utf8_title(std::string_view{"ij", 1}, uni::locale("nl")) == "I");
    TESTX(uni::cases::utf16_title(std::u16string_view{u"ij", 1}, uni::locale("nl")) == u"I");

    // Strange corner case
    // # Lithuanian # Remove DOT ABOVE after "i" with upper or titlecase
    // rule from SpecialCasing.txt interfere with toTitlecase(X) rules from Unicode Standard
    // so it is not handled. The behaviour is the same as in ICU.

    TESTX(uni::cases::utf8_title("i\xCC\x87tEsT", uni::locale("lt")) == "I\xCC\x87test");

    TESTX(uni::cases::utf16_title(u"i\x0307tEsT", uni::locale("lt")) == u"I\x0307test");

#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD
}

// Implement random algorithms using search function

template<typename T>
bool caseless_ends_with(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    // Search always returns true for empty string2 so we need this to avoid endless loop
    if (string2.empty())
        return true;

    std::size_t start = 0;
    uni::search found;
    while (true)
    {
        if constexpr (std::is_same_v<T, char>)
            found = uni::caseless::utf8_search(string1.substr(start), string2);
        else if constexpr (std::is_same_v<T, char16_t>)
            found = uni::caseless::utf16_search(string1.substr(start), string2);
        if (!found)
            break;
        if (start + found.end_pos() == string1.size())
            return true;
        start += found.end_pos();
    }

    return false;
}

template<typename T>
bool caseless_starts_with(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    uni::search found;

    if constexpr (std::is_same_v<T, char>)
        found = uni::caseless::utf8_search(string1, string2);
    else if constexpr (std::is_same_v<T, char16_t>)
        found = uni::caseless::utf16_search(string1, string2);

    return found && found.pos() == 0;
}

// TODO: FIXME:
// This function doesn't compile on macOS Xcode with very strange template error
// so disable it for now the test is not that important anyway.
#if 0
template<typename T>
std::vector<std::pair<std::size_t, std::size_t>>
caseless_find_all(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    std::vector<std::pair<std::size_t, std::size_t>> result;

    if (string2.empty())
        return result;

    std::size_t start = 0;
    uni::search found;
    while (true)
    {
        if constexpr (std::is_same_v<T, char>)
            found = uni::caseless::utf8_search(string1.substr(start), string2);
        else if constexpr (std::is_same_v<T, char16_t>)
            found = uni::caseless::utf16_search(string1.substr(start), string2);
        if (!found)
            break;
        result.push_back({start + found.pos(), start + found.end_pos()});
        start += found.end_pos();
    }

    return result;
}
#endif

// Test them

void test_case_search_ex()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    // UTF-8

    TESTX(caseless_ends_with<char>("ABCﬁ", "FI"));
    TESTX(!caseless_ends_with<char>("ABCﬁ", "I"));
    TESTX(!caseless_ends_with<char>("ABCﬁ", "ABC"));
    TESTX(caseless_ends_with<char>("ﬁABCﬁ", "FI"));
    TESTX(!caseless_ends_with<char>("ﬁABCﬁ", "I"));
    TESTX(caseless_ends_with<char>("ABCﬃ", "FFI"));
    TESTX(!caseless_ends_with<char>("ABCﬃ", "FI"));
    TESTX(!caseless_ends_with<char>("ABCﬃ", "ABC"));
    TESTX(caseless_ends_with<char>("ﬃABCﬃ", "FFI"));
    TESTX(!caseless_ends_with<char>("ﬃABCﬃ", "FI"));
    TESTX(caseless_ends_with<char>("", ""));
    TESTX(caseless_ends_with<char>("ABC", ""));
    TESTX(!caseless_ends_with<char>("", "ABC"));

    TESTX(caseless_starts_with<char>("ﬁABC", "FI"));
    TESTX(!caseless_starts_with<char>("ﬁABC", "F"));
    TESTX(!caseless_starts_with<char>("ﬁABC", "ABC"));
    TESTX(caseless_starts_with<char>("ﬁABCﬁ", "FI"));
    TESTX(!caseless_starts_with<char>("ﬁABCﬁ", "F"));
    TESTX(caseless_starts_with<char>("ﬃABC", "FFI"));
    TESTX(!caseless_starts_with<char>("ﬃABC", "FF"));
    TESTX(!caseless_starts_with<char>("ﬃABC", "ABC"));
    TESTX(caseless_starts_with<char>("ﬃABCﬃ", "FFI"));
    TESTX(!caseless_starts_with<char>("ﬃABCﬃ", "FF"));
    TESTX(caseless_starts_with<char>("", ""));
    TESTX(caseless_starts_with<char>("ABC", ""));
    TESTX(!caseless_starts_with<char>("", "ABC"));
#if 0
    auto vec = caseless_find_all<char>("ﬁABCﬁ", "FI");
    TESTX(vec.size() == 2);
    TESTX(vec[0].first == 0 && vec[0].second == 3);
    TESTX(vec[1].first == 6 && vec[1].second == 9);
    vec = caseless_find_all<char>("ﬁﬁ", "FI");
    TESTX(vec.size() == 2);
    TESTX(vec[0].first == 0 && vec[0].second == 3);
    TESTX(vec[1].first == 3 && vec[1].second == 6);
    vec = caseless_find_all<char>("", "");
    TESTX(vec.size() == 0);
#endif
    // UTF-16

    TESTX(caseless_ends_with<char16_t>(u"ABCﬁ", u"FI"));
    TESTX(!caseless_ends_with<char16_t>(u"ABCﬁ", u"I"));
    TESTX(!caseless_ends_with<char16_t>(u"ABCﬁ", u"ABC"));
    TESTX(caseless_ends_with<char16_t>(u"ﬁABCﬁ", u"FI"));
    TESTX(!caseless_ends_with<char16_t>(u"ﬁABCﬁ", u"I"));
    TESTX(caseless_ends_with<char16_t>(u"ABCﬃ", u"FFI"));
    TESTX(!caseless_ends_with<char16_t>(u"ABCﬃ", u"FI"));
    TESTX(!caseless_ends_with<char16_t>(u"ABCﬃ", u"ABC"));
    TESTX(caseless_ends_with<char16_t>(u"ﬃABCﬃ", u"FFI"));
    TESTX(!caseless_ends_with<char16_t>(u"ﬃABCﬃ", u"FI"));
    TESTX(caseless_ends_with<char16_t>(u"", u""));
    TESTX(caseless_ends_with<char16_t>(u"ABC", u""));
    TESTX(!caseless_ends_with<char16_t>(u"", u"ABC"));

    TESTX(caseless_starts_with<char16_t>(u"ﬁABC", u"FI"));
    TESTX(!caseless_starts_with<char16_t>(u"ﬁABC", u"F"));
    TESTX(!caseless_starts_with<char16_t>(u"ﬁABC", u"ABC"));
    TESTX(caseless_starts_with<char16_t>(u"ﬁABCﬁ", u"FI"));
    TESTX(!caseless_starts_with<char16_t>(u"ﬁABCﬁ", u"F"));
    TESTX(caseless_starts_with<char16_t>(u"ﬃABC", u"FFI"));
    TESTX(!caseless_starts_with<char16_t>(u"ﬃABC", u"FF"));
    TESTX(!caseless_starts_with<char16_t>(u"ﬃABC", u"ABC"));
    TESTX(caseless_starts_with<char16_t>(u"ﬃABCﬃ", u"FFI"));
    TESTX(!caseless_starts_with<char16_t>(u"ﬃABCﬃ", u"FF"));
    TESTX(caseless_starts_with<char16_t>(u"", u""));
    TESTX(caseless_starts_with<char16_t>(u"ABC", u""));
    TESTX(!caseless_starts_with<char16_t>(u"", u"ABC"));
#if 0
    vec = caseless_find_all<char16_t>(u"ﬁABCﬁ", u"FI");
    TESTX(vec.size() == 2);
    TESTX(vec[0].first == 0 && vec[0].second == 1);
    TESTX(vec[1].first == 4 && vec[1].second == 5);
    vec = caseless_find_all<char16_t>(u"ﬁﬁ", u"FI");
    TESTX(vec.size() == 2);
    TESTX(vec[0].first == 0 && vec[0].second == 1);
    TESTX(vec[1].first == 1 && vec[1].second == 2);
    vec = caseless_find_all<char16_t>(u"", u"");
    TESTX(vec.size() == 0);
#endif
    //

    uni::search s;

    std::string str8 = "ABCﬁXYZ";
    s = uni::caseless::utf8_search(str8, "FI");
    str8.erase(s.pos(), s.end_pos() - s.pos());
    TESTX(str8 == "ABCXYZ");

    std::u16string str16 = u"ABCﬁXYZ";
    s = uni::caseless::utf16_search(str16, u"FI");
    str16.erase(s.pos(), s.end_pos() - s.pos());
    TESTX(str16 == u"ABCXYZ");

#endif // UNI_ALGO_DISABLE_FULL_CASE
}
