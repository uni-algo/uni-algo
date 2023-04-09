/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// If it fails a compiler messed up the UTF-8 encoding of this file.
// If the compiler is MSVC then /utf-8 command line option must be used.
static_assert(U'㋡' == 0x32E1);

test_constexpr bool test_case_compare_collate()
{
    // Use Cyrilic letter Џ/џ (U+040F/U+045F), this letter is always in the incorrect order
    // when comparing code points but must be in the correct order when collation

    // UTF-8

    TESTX(una::casesens::compare_utf8("Џ", "Ч") < 0);
    TESTX(una::casesens::compare_utf8("џ", "ш") > 0);
    TESTX(una::caseless::compare_utf8("Џ", "ш") > 0);
    TESTX(una::caseless::compare_utf8("Џ", "џ") == 0);

    TESTX(una::casesens::collate_utf8("Џ", "Ч") > 0);
    TESTX(una::casesens::collate_utf8("џ", "ш") < 0);
    TESTX(una::caseless::collate_utf8("Џ", "ш") < 0);
    TESTX(una::caseless::collate_utf8("Џ", "џ") == 0);

    // UTF-16

    TESTX(una::casesens::compare_utf16(u"Џ", u"Ч") < 0);
    TESTX(una::casesens::compare_utf16(u"џ", u"ш") > 0);
    TESTX(una::caseless::compare_utf16(u"Џ", u"ш") > 0);
    TESTX(una::caseless::compare_utf16(u"Џ", u"џ") == 0);

    TESTX(una::casesens::collate_utf16(u"Џ", u"Ч") > 0);
    TESTX(una::casesens::collate_utf16(u"џ", u"ш") < 0);
    TESTX(una::caseless::collate_utf16(u"Џ", u"ш") < 0);
    TESTX(una::caseless::collate_utf16(u"Џ", u"џ") == 0);

    // Make sure long strings optimization doesn't break the functions

    TESTX(una::casesens::compare_utf8(una::utf32to8(U"\x10000"), una::utf32to8(U"\x10001")) != 0);
    TESTX(una::casesens::collate_utf8(una::utf32to8(U"\x10000"), una::utf32to8(U"\x10001")) != 0);

    TESTX(una::casesens::compare_utf16(una::utf32to16u(U"\x10000"), una::utf32to16u(U"\x10001")) != 0);
    TESTX(una::casesens::collate_utf16(una::utf32to16u(U"\x10000"), una::utf32to16u(U"\x10001")) != 0);

    return true;
}

test_constexpr bool test_case_find()
{
    una::found f;

    TESTX((f = una::casesens::find_utf8("バババババババババカバ", "バカ")) && f.pos() == 24 && f.end_pos() == 30);
    TESTX((f = una::caseless::find_utf8("бббббббббкбб", "ББК")) && f.pos() == 14 && f.end_pos() == 20);
    TESTX((f = una::caseless::find_utf8("bbbbbbbbbkbb", "BBK")) && f.pos() == 7 && f.end_pos() == 10);
    TESTX((f = una::casesens::find_utf8("バカ", "バカ")) && f.pos() == 0 && f.end_pos() == 6);
    TESTX((f = una::caseless::find_utf8("бака", "БАКА")) && f.pos() == 0 && f.end_pos() == 8);
    TESTX((f = una::caseless::find_utf8("baka", "BAKA")) && f.pos() == 0 && f.end_pos() == 4);
    TESTX(!una::casesens::find_utf8("バ", "バカ"));
    TESTX(!una::caseless::find_utf8("б", "БАКА"));
    TESTX(!una::caseless::find_utf8("b", "BAKA"));

    TESTX((f = una::casesens::find_utf16(u"バババババババババカバ", u"バカ")) && f.pos() == 8 && f.end_pos() == 10);
    TESTX((f = una::caseless::find_utf16(u"бббббббббкбб", u"ББК")) && f.pos() == 7 && f.end_pos() == 10);
    TESTX((f = una::caseless::find_utf16(u"bbbbbbbbbkbb", u"BBK")) && f.pos() == 7 && f.end_pos() == 10);
    TESTX((f = una::casesens::find_utf16(u"バカ", u"バカ")) && f.pos() == 0 && f.end_pos() == 2);
    TESTX((f = una::caseless::find_utf16(u"бака", u"БАКА")) && f.pos() == 0 && f.end_pos() == 4);
    TESTX((f = una::caseless::find_utf16(u"baka", u"BAKA")) && f.pos() == 0 && f.end_pos() == 4);
    TESTX(!una::casesens::find_utf16(u"バ", u"バカ"));
    TESTX(!una::caseless::find_utf16(u"б", u"БАКА"));
    TESTX(!una::caseless::find_utf16(u"b", u"BAKA"));

    // Empty strings

    TESTX((f = una::casesens::find_utf8("バカ", "")) && f.pos() == 0 && f.end_pos() == 0);
    TESTX((f = una::caseless::find_utf8("バカ", "")) && f.pos() == 0 && f.end_pos() == 0);
    TESTX((f = una::casesens::find_utf8("", "")) && f.pos() == 0 && f.end_pos() == 0);
    TESTX((f = una::caseless::find_utf8("", "")) && f.pos() == 0 && f.end_pos() == 0);
    TESTX(!una::casesens::find_utf8("", "バカ"));
    TESTX(!una::caseless::find_utf8("", "バカ"));

    TESTX((f = una::casesens::find_utf16(u"バカ", u"")) && f.pos() == 0 && f.end_pos() == 0);
    TESTX((f = una::caseless::find_utf16(u"バカ", u"")) && f.pos() == 0 && f.end_pos() == 0);
    TESTX((f = una::casesens::find_utf16(u"", u"")) && f.pos() == 0 && f.end_pos() == 0);
    TESTX((f = una::caseless::find_utf16(u"", u"")) && f.pos() == 0 && f.end_pos() == 0);
    TESTX(!una::casesens::find_utf16(u"", u"バカ"));
    TESTX(!una::caseless::find_utf16(u"", u"バカ"));

    return true;
}

test_constexpr bool test_case_ill_formed()
{
    // Ill-formed UTF must always produce replacement character U+FFFD
    // Check that all functions are consistent in this case

    una::found f;

    // UTF-8 (Use 0x80 and 0x81 they are lone tails)

    TESTX(una::casesens::compare_utf8("\x80", "\x81") == 0);
    TESTX(una::caseless::compare_utf8("\x80", "\x81") == 0);

    TESTX(una::casesens::collate_utf8("\x80", "\x81") == 0);
    TESTX(una::caseless::collate_utf8("\x80", "\x81") == 0);

    TESTX((f = una::casesens::find_utf8("abc\x80", "\x81")) && f.pos() == 3 && f.end_pos() == 4);
    TESTX((f = una::caseless::find_utf8("abc\x80", "\x81")) && f.pos() == 3 && f.end_pos() == 4);

    TESTX(una::cases::to_lowercase_utf8("\x80") == "\xEF\xBF\xBD");
    TESTX(una::cases::to_uppercase_utf8("\x80") == "\xEF\xBF\xBD");
    TESTX(una::cases::to_casefold_utf8("\x80") == "\xEF\xBF\xBD");

    // UTF-16 (Use 0xDC00 and 0xDC01 they are lone low surrogates)

    TESTX(una::casesens::compare_utf16(u"\xDC00", u"\xDC01") == 0);
    TESTX(una::caseless::compare_utf16(u"\xDC00", u"\xDC01") == 0);

    TESTX(una::casesens::collate_utf16(u"\xDC00", u"\xDC01") == 0);
    TESTX(una::caseless::collate_utf16(u"\xDC00", u"\xDC01") == 0);

    TESTX((f = una::casesens::find_utf16(u"abc\xDC00", u"\xDC01")) && f.pos() == 3 && f.end_pos() == 4);
    TESTX((f = una::caseless::find_utf16(u"abc\xDC00", u"\xDC01")) && f.pos() == 3 && f.end_pos() == 4);

    TESTX(una::cases::to_lowercase_utf16(u"\xDC00") == u"\xFFFD");
    TESTX(una::cases::to_uppercase_utf16(u"\xDC00") == u"\xFFFD");
    TESTX(una::cases::to_casefold_utf16(u"\xDC00") == u"\xFFFD");

    return true;
}

test_constexpr bool test_case_full_case()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    // Use ligature ﬁ (U+FB01) for testing

    una::found f;

    // UTF-8

    TESTX(una::caseless::compare_utf8("abcﬁ", "ABCFI") == 0);
    TESTX(una::caseless::compare_utf8("abcﬁ", "ABCF") > 0);
    TESTX(una::caseless::compare_utf8("abcﬁ", "ABCFX") < 0);

    TESTX(una::caseless::collate_utf8("abcﬁ", "ABCFI") == 0);
    TESTX(una::caseless::collate_utf8("abcﬁ", "ABCF") > 0);
    TESTX(una::caseless::collate_utf8("abcﬁ", "ABCFX") < 0);

    TESTX((f = una::caseless::find_utf8("abcﬁ", "FI")) && f.pos() == 3 && f.end_pos() == 6);
    TESTX((f = una::caseless::find_utf8("ABCFI", "ﬁ")) && f.pos() == 3 && f.end_pos() == 5);

    // Complicated cases
    TESTX(!una::caseless::find_utf8("f", "ﬁ"));
    TESTX(!una::caseless::find_utf8("ﬁ", "f"));
    TESTX(!una::caseless::find_utf8("ﬁ", "i"));
    TESTX(!una::caseless::find_utf8("abcﬁ", "f"));
    TESTX(!una::caseless::find_utf8("abcﬁ", "i"));
    TESTX((f = una::caseless::find_utf8("abcﬁf", "f")) && f.pos() == 6 && f.end_pos() == 7);
    TESTX((f = una::caseless::find_utf8("abcﬁ", "ﬁ")) && f.pos() == 3 && f.end_pos() == 6);

    // UTF-16

    TESTX(una::caseless::compare_utf16(u"abcﬁ", u"ABCFI") == 0);
    TESTX(una::caseless::compare_utf16(u"abcﬁ", u"ABCF") > 0);
    TESTX(una::caseless::compare_utf16(u"abcﬁ", u"ABCFX") < 0);

    TESTX(una::caseless::collate_utf16(u"abcﬁ", u"ABCFI") == 0);
    TESTX(una::caseless::collate_utf16(u"abcﬁ", u"ABCF") > 0);
    TESTX(una::caseless::collate_utf16(u"abcﬁ", u"ABCFX") < 0);

    TESTX((f = una::caseless::find_utf16(u"abcﬁ", u"FI")) && f.pos() == 3 && f.end_pos() == 4);
    TESTX((f = una::caseless::find_utf16(u"ABCFI", u"ﬁ")) && f.pos() == 3 && f.end_pos() == 5);

    // Complicated cases
    TESTX(!una::caseless::find_utf16(u"f", u"ﬁ"));
    TESTX(!una::caseless::find_utf16(u"ﬁ", u"f"));
    TESTX(!una::caseless::find_utf16(u"ﬁ", u"i"));
    TESTX(!una::caseless::find_utf16(u"abcﬁ", u"f"));
    TESTX(!una::caseless::find_utf16(u"abcﬁ", u"i"));
    TESTX((f = una::caseless::find_utf16(u"abcﬁf", u"f")) && f.pos() == 4 && f.end_pos() == 5);
    TESTX((f = una::caseless::find_utf16(u"abcﬁ", u"ﬁ")) && f.pos() == 3 && f.end_pos() == 4);

#endif // UNI_ALGO_DISABLE_FULL_CASE

    return true;
}

test_constexpr bool test_case_upper_lower_fold()
{
    TESTX(una::cases::to_uppercase_utf8("Test") == "TEST");
    TESTX(una::cases::to_uppercase_utf8("два") == "ДВА");
    TESTX(una::cases::to_casefold_utf8("TEST") == "test");
    TESTX(una::cases::to_casefold_utf8("ДВА") == "два");
    TESTX(una::cases::to_lowercase_utf8("TEST") == "test");
    TESTX(una::cases::to_lowercase_utf8("ДВА") == "два");

    TESTX(una::cases::to_uppercase_utf16(u"Test") == u"TEST");
    TESTX(una::cases::to_uppercase_utf16(u"два") == u"ДВА");
    TESTX(una::cases::to_casefold_utf16(u"TEST") == u"test");
    TESTX(una::cases::to_casefold_utf16(u"ДВА") == u"два");
    TESTX(una::cases::to_lowercase_utf16(u"TEST") == u"test");
    TESTX(una::cases::to_lowercase_utf16(u"ДВА") == u"два");

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_uppercase_utf8("Straße") == "STRASSE");
    TESTX(una::cases::to_uppercase_utf8("ﬁ ﬂ") == "FI FL");
    TESTX(una::cases::to_casefold_utf8("Straße") == "strasse");
    TESTX(una::cases::to_casefold_utf8("ﬁ ﬂ") == "fi fl");
    TESTX(una::cases::to_lowercase_utf8("İ") == "i\xCC\x87");

    TESTX(una::cases::to_uppercase_utf8("\xE1\xBE\x80") == "\xE1\xBC\x88\xCE\x99");
    TESTX(una::cases::to_uppercase_utf8("\xE1\xBE\x88") == "\xE1\xBC\x88\xCE\x99");

    TESTX(una::cases::to_uppercase_utf16(u"Straße") == u"STRASSE");
    TESTX(una::cases::to_uppercase_utf16(u"ﬁ ﬂ") == u"FI FL");
    TESTX(una::cases::to_casefold_utf16(u"Straße") == u"strasse");
    TESTX(una::cases::to_casefold_utf16(u"ﬁ ﬂ") == u"fi fl");
    TESTX(una::cases::to_lowercase_utf16(u"İ") == u"i\x0307");

    TESTX(una::cases::to_uppercase_utf16(u"\x1F80") == u"\x1F08\x0399");
    TESTX(una::cases::to_uppercase_utf16(u"\x1F88") == u"\x1F08\x0399");
#endif // UNI_ALGO_DISABLE_FULL_CASE

    return true;
}

test_constexpr bool test_case_final_sigma()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    // UTF-8

    TESTX(una::cases::to_lowercase_utf8("Σ") == "σ");
    TESTX(una::cases::to_lowercase_utf8(":::Σ:::") == ":::σ:::");
    TESTX(una::cases::to_lowercase_utf8("F:::Σ:::") == "f:::ς:::");
    TESTX(una::cases::to_lowercase_utf8("F:::Σ:::F") == "f:::σ:::f");
    TESTX(una::cases::to_lowercase_utf8(":::Σ:::F") == ":::σ:::f");
    TESTX(una::cases::to_lowercase_utf8("Σ:::F") == "σ:::f");
    TESTX(una::cases::to_lowercase_utf8("F:::Σ") == "f:::ς");
    TESTX(una::cases::to_lowercase_utf8("ΣF") == "σf");
    TESTX(una::cases::to_lowercase_utf8("FΣ") == "fς");

    // UTF-16

    TESTX(una::cases::to_lowercase_utf16(u"Σ") == u"σ");
    TESTX(una::cases::to_lowercase_utf16(u":::Σ:::") == u":::σ:::");
    TESTX(una::cases::to_lowercase_utf16(u"F:::Σ:::") == u"f:::ς:::");
    TESTX(una::cases::to_lowercase_utf16(u"F:::Σ:::F") == u"f:::σ:::f");
    TESTX(una::cases::to_lowercase_utf16(u":::Σ:::F") == u":::σ:::f");
    TESTX(una::cases::to_lowercase_utf16(u"Σ:::F") == u"σ:::f");
    TESTX(una::cases::to_lowercase_utf16(u"F:::Σ") == u"f:::ς");
    TESTX(una::cases::to_lowercase_utf16(u"ΣF") == u"σf");
    TESTX(una::cases::to_lowercase_utf16(u"FΣ") == u"fς");

    // Ill-formed UTF

    TESTX(una::cases::to_lowercase_utf8("F:\x80::Σ:::") == "f:\xEF\xBF\xBD::σ:::");
    TESTX(una::cases::to_lowercase_utf8("F:\x80::Σ::\x80:F") == "f:\xEF\xBF\xBD::σ::\xEF\xBF\xBD:f");
    TESTX(una::cases::to_lowercase_utf8(":::Σ::\x80:F") == ":::σ::\xEF\xBF\xBD:f");

    TESTX(una::cases::to_lowercase_utf16(u"F:\xDC00::Σ:::") == u"f:\xFFFD::σ:::");
    TESTX(una::cases::to_lowercase_utf16(u"F:\xDC00::Σ::\xDC00:F") == u"f:\xFFFD::σ::\xFFFD:f");
    TESTX(una::cases::to_lowercase_utf16(u":::Σ::\xDC00:F") == u":::σ::\xFFFD:f");

#endif /* UNI_ALGO_DISABLE_FULL_CASE */

    return true;
}

test_constexpr bool test_case_sort_key()
{
#ifdef UNI_ALGO_EXPERIMENTAL
    TESTX(una::casesens::collate_utf8("Џ", "Ч") > 0 && una::casesens::sortkey_utf8("Џ") > una::casesens::sortkey_utf8("Ч"));
    TESTX(una::casesens::collate_utf8("џ", "ш") < 0 && una::casesens::sortkey_utf8("џ") < una::casesens::sortkey_utf8("ш"));
    TESTX(una::caseless::collate_utf8("Џ", "ш") < 0 && una::caseless::sortkey_utf8("Џ") < una::caseless::sortkey_utf8("ш"));
    TESTX(una::caseless::collate_utf8("Џ", "џ") == 0 && una::caseless::sortkey_utf8("Џ") == una::caseless::sortkey_utf8("џ"));

    TESTX(una::casesens::collate_utf16(u"Џ", u"Ч") > 0 && una::casesens::sortkey_utf16(u"Џ") > una::casesens::sortkey_utf16(u"Ч"));
    TESTX(una::casesens::collate_utf16(u"џ", u"ш") < 0 && una::casesens::sortkey_utf16(u"џ") < una::casesens::sortkey_utf16(u"ш"));
    TESTX(una::caseless::collate_utf16(u"Џ", u"ш") < 0 && una::caseless::sortkey_utf16(u"Џ") < una::caseless::sortkey_utf16(u"ш"));
    TESTX(una::caseless::collate_utf16(u"Џ", u"џ") == 0 && una::caseless::sortkey_utf16(u"Џ") == una::caseless::sortkey_utf16(u"џ"));

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::caseless::collate_utf8("ﬁ", "FI") == 0 && una::caseless::sortkey_utf8("ﬁ") == una::caseless::sortkey_utf8("FI"));
    TESTX(una::caseless::collate_utf16(u"ﬁ", u"FI") == 0 && una::caseless::sortkey_utf16(u"ﬁ") == una::caseless::sortkey_utf16(u"FI"));
    TESTX(una::caseless::collate_utf8("\xCE\x90", "\xCE\xB9\xCC\x88\xCC\x81") == 0 &&
          una::caseless::sortkey_utf8("\xCE\x90") == una::caseless::sortkey_utf8("\xCE\xB9\xCC\x88\xCC\x81"));
    TESTX(una::caseless::collate_utf16(u"\x0390", u"\x03B9\x0308\x0301") == 0 &&
          una::caseless::sortkey_utf16(u"\x0390") == una::caseless::sortkey_utf16(u"\x03B9\x0308\x0301"));
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_EXPERIMENTAL

    return true;
}

test_constexpr bool test_case_like()
{
#ifdef UNI_ALGO_EXPERIMENTAL
    TESTX(una::caseless::like_utf8("abc123xyz789", "%ABC%XYZ%"));
    TESTX(una::caseless::like_utf8("abc123xyz", "%ABC%XYZ%"));
    TESTX(una::caseless::like_utf8("abc123xyz", "%%%ABC%%%XYZ%%%"));

    TESTX(una::caseless::like_utf8("abc123xyz", "ABC123XYZ"));
    TESTX(una::caseless::like_utf8("abc123xyz", "_BC___XY_"));
    TESTX(una::caseless::like_utf8("abc123xyz", "%A_C%X__%"));

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(!una::caseless::like_utf8("ﬃ", "ﬁ"));
    TESTX(!una::caseless::like_utf8("ﬃ", "%ﬁ%"));
    TESTX(!una::caseless::like_utf8("ﬃ", "ﬁi"));
    TESTX(una::caseless::like_utf8("ﬃ", "ffi"));
    TESTX(una::caseless::like_utf8("ﬃ", "FFI"));
    TESTX(una::caseless::like_utf8("ﬃ", "%FFI%"));
    TESTX(una::caseless::like_utf8("abcﬃ", "%FFI"));
    TESTX(una::caseless::like_utf8("ﬃabc", "FFI%"));
    TESTX(!una::caseless::like_utf8("abcﬃxyz", "%FF%"));
    TESTX(!una::caseless::like_utf8("abcﬃxyz", "%FF_%"));
    TESTX(una::caseless::like_utf8("abcﬃxyz", "%FFI%"));

    TESTX(una::caseless::like_utf8("abcﬃabc", "_B_%FFI%_B_"));
    TESTX(!una::caseless::like_utf8("abcﬃabc", "_B__%FFI%_B_"));
    TESTX(!una::caseless::like_utf8("ﬃ", "ff"));
#endif // UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::caseless::like_utf8("AB", "A%B"));
    TESTX(una::caseless::like_utf8("AB", "%A%B"));
    TESTX(una::caseless::like_utf8("AB", "%A%B%"));

    TESTX(una::caseless::like_utf8("бббббббббкбб", "%ббк%"));
    TESTX(una::caseless::like_utf8("бббббббббкбб", "%ББК%"));

    TESTX(una::caseless::like_utf8("%%%%%%%%%%бк_бб", "%/%БК/_%", U'/'));

    TESTX(!una::caseless::like_utf8("ABC", "ABC%", U'%'));
    TESTX(!una::caseless::like_utf8("ABC", "ABC_", U'_'));

    // Empty strings

    TESTX(!una::caseless::like_utf8("abc", ""));
    TESTX(una::caseless::like_utf8("", ""));
#endif // UNI_ALGO_EXPERIMENTAL

    return true;
}

test_constexpr bool test_case_locale_lt()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    una::locale locale("lt");

    // SpecialCasing.txt

    // # Lithuanian

    // # Lithuanian retains the dot in a lowercase i when followed by accents.

    // # Remove DOT ABOVE (U+0307) after "i" with upper or titlecase

    TESTX(una::cases::to_uppercase_utf8("i\xCC\x87", locale) == "I");
    TESTX(una::cases::to_uppercase_utf8("i\xCC\x87w\xCC\x87i\xCC\x87", locale) == "IW\xCC\x87I");

    TESTX(una::cases::to_uppercase_utf16(u"i\x0307", locale) == u"I");
    TESTX(una::cases::to_uppercase_utf16(u"i\x0307w\x0307i\x0307", locale) == u"IW\x0307I");

    // # Introduce an explicit dot above when lowercasing capital I's and J's
    // # whenever there are more accents above.
    // # (of the accents used in Lithuanian: grave, acute, tilde above, and ogonek)

    // UTF-8

    TESTX(una::cases::to_lowercase_utf8("\x49", locale) == "\x69");
    TESTX(una::cases::to_lowercase_utf8("\x4A", locale) == "\x6A");
    TESTX(una::cases::to_lowercase_utf8("\xC4\xAE", locale) == "\xC4\xAF");
    // If more accents above introduce U+0307
    TESTX(una::cases::to_lowercase_utf8("\x49\xCC\x80", locale) == "\x69\xCC\x87\xCC\x80");
    TESTX(una::cases::to_lowercase_utf8("\x4A\xCC\x80", locale) == "\x6A\xCC\x87\xCC\x80");
    TESTX(una::cases::to_lowercase_utf8("\xC4\xAE\xCC\x80", locale) == "\xC4\xAF\xCC\x87\xCC\x80");
    // Intervine with U+0315
    TESTX(una::cases::to_lowercase_utf8("\x49\xCC\x95\xCC\x80", locale) == "\x69\xCC\x87\xCC\x95\xCC\x80");
    TESTX(una::cases::to_lowercase_utf8("\x4A\xCC\x95\xCC\x80", locale) == "\x6A\xCC\x87\xCC\x95\xCC\x80");
    TESTX(una::cases::to_lowercase_utf8("\xC4\xAE\xCC\x95\xCC\x80", locale) == "\xC4\xAF\xCC\x87\xCC\x95\xCC\x80");
    // Intervine with W
    TESTX(una::cases::to_lowercase_utf8("\x49W\xCC\x80", locale) == "\x69w\xCC\x80");
    TESTX(una::cases::to_lowercase_utf8("\x4AW\xCC\x80", locale) == "\x6Aw\xCC\x80");
    TESTX(una::cases::to_lowercase_utf8("\xC4\xAEW\xCC\x80", locale) == "\xC4\xAFw\xCC\x80");

    TESTX(una::cases::to_lowercase_utf8("\xC3\x8C", locale) == "\x69\xCC\x87\xCC\x80");
    TESTX(una::cases::to_lowercase_utf8("\xC3\x8D", locale) == "\x69\xCC\x87\xCC\x81");
    TESTX(una::cases::to_lowercase_utf8("\xC4\xA8", locale) == "\x69\xCC\x87\xCC\x83");

    // UTF-16

    TESTX(una::cases::to_lowercase_utf16(u"\x0049", locale) == u"\x0069");
    TESTX(una::cases::to_lowercase_utf16(u"\x004A", locale) == u"\x006A");
    TESTX(una::cases::to_lowercase_utf16(u"\x012E", locale) == u"\x012F");
    // If more accents above introduce U+0307
    TESTX(una::cases::to_lowercase_utf16(u"\x0049\x0300", locale) == u"\x0069\x0307\x0300");
    TESTX(una::cases::to_lowercase_utf16(u"\x004A\x0300", locale) == u"\x006A\x0307\x0300");
    TESTX(una::cases::to_lowercase_utf16(u"\x012E\x0300", locale) == u"\x012F\x0307\x0300");
    // Intervine with U+0315
    TESTX(una::cases::to_lowercase_utf16(u"\x0049\x0315\x0300", locale) == u"\x0069\x0307\x0315\x0300");
    TESTX(una::cases::to_lowercase_utf16(u"\x004A\x0315\x0300", locale) == u"\x006A\x0307\x0315\x0300");
    TESTX(una::cases::to_lowercase_utf16(u"\x012E\x0315\x0300", locale) == u"\x012F\x0307\x0315\x0300");
    // Intervine with W
    TESTX(una::cases::to_lowercase_utf16(u"\x0049W\x0300", locale) == u"\x0069w\x0300");
    TESTX(una::cases::to_lowercase_utf16(u"\x004AW\x0300", locale) == u"\x006Aw\x0300");
    TESTX(una::cases::to_lowercase_utf16(u"\x012EW\x0300", locale) == u"\x012Fw\x0300");

    TESTX(una::cases::to_lowercase_utf16(u"\x00CC", locale) == u"\x0069\x0307\x0300");
    TESTX(una::cases::to_lowercase_utf16(u"\x00CD", locale) == u"\x0069\x0307\x0301");
    TESTX(una::cases::to_lowercase_utf16(u"\x0128", locale) == u"\x0069\x0307\x0303");

#endif // UNI_ALGO_DISABLE_FULL_CASE

    return true;
}

test_constexpr bool test_case_locale_tr_az()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    una::locale locale("tr");

    // SpecialCasing.txt

    // # Turkish and Azeri

    // # I and i-dotless; I-dot and i are case pairs in Turkish and Azeri
    // # The following rules handle those cases.

    TESTX(una::cases::to_lowercase_utf8("İ", locale) == "i");
    TESTX(una::cases::to_lowercase_utf8("\xC4\xB0", locale) == "\x69");

    TESTX(una::cases::to_lowercase_utf16(u"İ", locale) == u"i");
    TESTX(una::cases::to_lowercase_utf16(u"\x0130", locale) == u"\x0069");

    // # When lowercasing, remove dot_above in the sequence I + dot_above, which will turn into i.
    // # This matches the behavior of the canonically equivalent I-dot_above

    TESTX(una::cases::to_lowercase_utf8("I\xCC\x87", locale) == "i");

    TESTX(una::cases::to_lowercase_utf16(u"I\x0307", locale) == u"i");

    // # When lowercasing, unless an I is before a dot_above, it turns into a dotless i.

    TESTX(una::cases::to_lowercase_utf8("I", locale) == "ı");
    TESTX(una::cases::to_lowercase_utf8("\x49", locale) == "\xC4\xB1");

    TESTX(una::cases::to_lowercase_utf16(u"I", locale) == u"ı");
    TESTX(una::cases::to_lowercase_utf16(u"\x0049", locale) == u"\x0131");

    // # When uppercasing, i turns into a dotted capital I

    TESTX(una::cases::to_uppercase_utf8("i", locale) == "İ");
    TESTX(una::cases::to_uppercase_utf8("\x69", locale) == "\xC4\xB0");

    TESTX(una::cases::to_uppercase_utf16(u"i", locale) == u"İ");
    TESTX(una::cases::to_uppercase_utf16(u"\x0069", locale) == u"\x0130");

#endif // UNI_ALGO_DISABLE_FULL_CASE

    return true;
}

test_constexpr bool test_case_locale_el()
{
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    una::locale locale("el");

    // http://site.icu-project.org/design/case/greek-upper

    TESTX(una::cases::to_uppercase_utf8("άδικος, κείμενο, ίριδα", locale) == "ΑΔΙΚΟΣ, ΚΕΙΜΕΝΟ, ΙΡΙΔΑ");
    TESTX(una::cases::to_uppercase_utf8("Πατάτα", locale) == "ΠΑΤΑΤΑ");
    TESTX(una::cases::to_uppercase_utf8("Αέρας, Μυστήριο, Ωραίο", locale) == "ΑΕΡΑΣ, ΜΥΣΤΗΡΙΟ, ΩΡΑΙΟ");
    TESTX(una::cases::to_uppercase_utf8("Μαΐου, Πόρος, Ρύθμιση", locale) == "ΜΑΪΟΥ, ΠΟΡΟΣ, ΡΥΘΜΙΣΗ");
    TESTX(una::cases::to_uppercase_utf8("ΰ, Τηρώ, Μάιος", locale) == "Ϋ, ΤΗΡΩ, ΜΑΪΟΣ");
    TESTX(una::cases::to_uppercase_utf8("άυλος", locale) == "ΑΫΛΟΣ");
    TESTX(una::cases::to_uppercase_utf8("ΑΫΛΟΣ", locale) == "ΑΫΛΟΣ");
    TESTX(una::cases::to_uppercase_utf8("Άκλιτα ρήματα ή άκλιτες μετοχές", locale) == "ΑΚΛΙΤΑ ΡΗΜΑΤΑ Ή ΑΚΛΙΤΕΣ ΜΕΤΟΧΕΣ"); // Η Ή Ή
    TESTX(una::cases::to_uppercase_utf8("Επειδή η αναγνώριση της αξιοπρέπειας", locale) == "ΕΠΕΙΔΗ Η ΑΝΑΓΝΩΡΙΣΗ ΤΗΣ ΑΞΙΟΠΡΕΠΕΙΑΣ");
    TESTX(una::cases::to_uppercase_utf8("νομικού ή διεθνούς", locale) == "ΝΟΜΙΚΟΥ Ή ΔΙΕΘΝΟΥΣ"); // Η Ή Ή
    TESTX(una::cases::to_uppercase_utf8("Ἐπειδὴ ἡ ἀναγνώριση", locale) == "ΕΠΕΙΔΗ Η ΑΝΑΓΝΩΡΙΣΗ");
    TESTX(una::cases::to_uppercase_utf8("νομικοῦ ἢ διεθνοῦς", locale) == "ΝΟΜΙΚΟΥ Ή ΔΙΕΘΝΟΥΣ"); // Η Ή Ή
    TESTX(una::cases::to_uppercase_utf8("Νέο, Δημιουργία", locale) == "ΝΕΟ, ΔΗΜΙΟΥΡΓΙΑ");
    TESTX(una::cases::to_uppercase_utf8("Ελάτε να φάτε τα καλύτερα παϊδάκια!", locale) == "ΕΛΑΤΕ ΝΑ ΦΑΤΕ ΤΑ ΚΑΛΥΤΕΡΑ ΠΑΪΔΑΚΙΑ!");
    TESTX(una::cases::to_uppercase_utf8("Μαΐου, τρόλεϊ", locale) == "ΜΑΪΟΥ, ΤΡΟΛΕΪ");
    TESTX(una::cases::to_uppercase_utf8("Το ένα ή το άλλο.", locale) == "ΤΟ ΕΝΑ Ή ΤΟ ΑΛΛΟ."); // Η Ή Ή
    TESTX(una::cases::to_uppercase_utf8("ρωμέικα", locale) == "ΡΩΜΕΪΚΑ");

    TESTX(una::cases::to_uppercase_utf8("άZυλος", locale) == "ΑZΥΛΟΣ");

    TESTX(una::cases::to_uppercase_utf16(u"άδικος, κείμενο, ίριδα", locale) == u"ΑΔΙΚΟΣ, ΚΕΙΜΕΝΟ, ΙΡΙΔΑ");
    TESTX(una::cases::to_uppercase_utf16(u"Πατάτα", locale) == u"ΠΑΤΑΤΑ");
    TESTX(una::cases::to_uppercase_utf16(u"Αέρας, Μυστήριο, Ωραίο", locale) == u"ΑΕΡΑΣ, ΜΥΣΤΗΡΙΟ, ΩΡΑΙΟ");
    TESTX(una::cases::to_uppercase_utf16(u"Μαΐου, Πόρος, Ρύθμιση", locale) == u"ΜΑΪΟΥ, ΠΟΡΟΣ, ΡΥΘΜΙΣΗ");
    TESTX(una::cases::to_uppercase_utf16(u"ΰ, Τηρώ, Μάιος", locale) == u"Ϋ, ΤΗΡΩ, ΜΑΪΟΣ");
    TESTX(una::cases::to_uppercase_utf16(u"άυλος", locale) == u"ΑΫΛΟΣ");
    TESTX(una::cases::to_uppercase_utf16(u"ΑΫΛΟΣ", locale) == u"ΑΫΛΟΣ");
    TESTX(una::cases::to_uppercase_utf16(u"Άκλιτα ρήματα ή άκλιτες μετοχές", locale) == u"ΑΚΛΙΤΑ ΡΗΜΑΤΑ Ή ΑΚΛΙΤΕΣ ΜΕΤΟΧΕΣ"); // Η Ή Ή
    TESTX(una::cases::to_uppercase_utf16(u"Επειδή η αναγνώριση της αξιοπρέπειας", locale) == u"ΕΠΕΙΔΗ Η ΑΝΑΓΝΩΡΙΣΗ ΤΗΣ ΑΞΙΟΠΡΕΠΕΙΑΣ");
    TESTX(una::cases::to_uppercase_utf16(u"νομικού ή διεθνούς", locale) == u"ΝΟΜΙΚΟΥ Ή ΔΙΕΘΝΟΥΣ"); // Η Ή Ή
    TESTX(una::cases::to_uppercase_utf16(u"Ἐπειδὴ ἡ ἀναγνώριση", locale) == u"ΕΠΕΙΔΗ Η ΑΝΑΓΝΩΡΙΣΗ");
    TESTX(una::cases::to_uppercase_utf16(u"νομικοῦ ἢ διεθνοῦς", locale) == u"ΝΟΜΙΚΟΥ Ή ΔΙΕΘΝΟΥΣ"); // Η Ή Ή
    TESTX(una::cases::to_uppercase_utf16(u"Νέο, Δημιουργία", locale) == u"ΝΕΟ, ΔΗΜΙΟΥΡΓΙΑ");
    TESTX(una::cases::to_uppercase_utf16(u"Ελάτε να φάτε τα καλύτερα παϊδάκια!", locale) == u"ΕΛΑΤΕ ΝΑ ΦΑΤΕ ΤΑ ΚΑΛΥΤΕΡΑ ΠΑΪΔΑΚΙΑ!");
    TESTX(una::cases::to_uppercase_utf16(u"Μαΐου, τρόλεϊ", locale) == u"ΜΑΪΟΥ, ΤΡΟΛΕΪ");
    TESTX(una::cases::to_uppercase_utf16(u"Το ένα ή το άλλο.", locale) == u"ΤΟ ΕΝΑ Ή ΤΟ ΑΛΛΟ."); // Η Ή Ή
    TESTX(una::cases::to_uppercase_utf16(u"ρωμέικα", locale) == u"ΡΩΜΕΪΚΑ");

    TESTX(una::cases::to_uppercase_utf16(u"άZυλος", locale) == u"ΑZΥΛΟΣ");

#endif // UNI_ALGO_DISABLE_FULL_CASE

    return true;
}

test_constexpr bool test_case_title()
{
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD

    TESTX(una::cases::to_titlecase_utf8("t") == "T");
    TESTX(una::cases::to_titlecase_utf8("tEsT") == "Test");
    TESTX(una::cases::to_titlecase_utf8("tEsT tEST TesT") == "Test Test Test");
    TESTX(una::cases::to_titlecase_utf8(" tEsT tEST TesT ") == " Test Test Test ");
    TESTX(una::cases::to_titlecase_utf8("  tEsT  tEST  TesT  ") == "  Test  Test  Test  ");

    TESTX(una::cases::to_titlecase_utf8("_t_") == "_T_");
    TESTX(una::cases::to_titlecase_utf8("_tEsT_") == "_Test_");
    TESTX(una::cases::to_titlecase_utf8("_tEsT_ _tEST_ _TesT_") == "_Test_ _Test_ _Test_");
    TESTX(una::cases::to_titlecase_utf8(" _tEsT_ _tEST_ _TesT_ ") == " _Test_ _Test_ _Test_ ");
    TESTX(una::cases::to_titlecase_utf8("  __tEsT__  __tEST__  __TesT__  ") == "  __Test__  __Test__  __Test__  ");

    TESTX(una::cases::to_titlecase_utf16(u"t") == u"T");
    TESTX(una::cases::to_titlecase_utf16(u"tEsT") == u"Test");
    TESTX(una::cases::to_titlecase_utf16(u"tEsT tEST TesT") == u"Test Test Test");
    TESTX(una::cases::to_titlecase_utf16(u" tEsT tEST TesT ") == u" Test Test Test ");
    TESTX(una::cases::to_titlecase_utf16(u"  tEsT  tEST  TesT  ") == u"  Test  Test  Test  ");

    TESTX(una::cases::to_titlecase_utf16(u"_t_") == u"_T_");
    TESTX(una::cases::to_titlecase_utf16(u"_tEsT_") == u"_Test_");
    TESTX(una::cases::to_titlecase_utf16(u"_tEsT_ _tEST_ _TesT_") == u"_Test_ _Test_ _Test_");
    TESTX(una::cases::to_titlecase_utf16(u" _tEsT_ _tEST_ _TesT_ ") == u" _Test_ _Test_ _Test_ ");
    TESTX(una::cases::to_titlecase_utf16(u"  __tEsT__  __tEST__  __TesT__  ") == u"  __Test__  __Test__  __Test__  ");

#ifndef UNI_ALGO_DISABLE_FULL_CASE

    TESTX(una::cases::to_titlecase_utf8("ﬃ") == "Ffi");
    TESTX(una::cases::to_titlecase_utf8("ﬃ ﬁ ﬂ") == "Ffi Fi Fl");
    TESTX(una::cases::to_titlecase_utf8("_ﬃ_ _ﬁ_ _ﬂ_") == "_Ffi_ _Fi_ _Fl_");

    TESTX(una::cases::to_titlecase_utf16(u"ﬃ") == u"Ffi");
    TESTX(una::cases::to_titlecase_utf16(u"ﬃ ﬁ ﬂ") == u"Ffi Fi Fl");
    TESTX(una::cases::to_titlecase_utf16(u"_ﬃ_ _ﬁ_ _ﬂ_") == u"_Ffi_ _Fi_ _Fl_");

#endif // UNI_ALGO_DISABLE_FULL_CASE

    // Found a strange behaviour in ICU when compared results.
    // ICU never title cases U+0345 but upper cases it just fine to U+0399.
    // Don't know why, it must be title cased according to title case algorithm,
    // maybe it just by design because U+0345 is pretty complicated code point.
    // It doesn't actually matter much because a word can't start
    // with such code point just want to mention it here.

    TESTX(una::cases::to_titlecase_utf8("\xCD\x85\xCD\x85\xCD\x85") == "\xCE\x99\xCD\x85\xCD\x85");

    TESTX(una::cases::to_titlecase_utf16(u"\x0345\x0345\x0345") == u"\x0399\x0345\x0345");

#endif // UNI_ALGO_DISABLE_SEGMENT_WORD

    return true;
}

test_constexpr bool test_case_title_locale()
{
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE

    TESTX(una::cases::to_titlecase_utf8("ijtest ijTeSt ﬃtEsT", una::locale("nl")) == "IJtest IJtest Ffitest");
    TESTX(una::cases::to_titlecase_utf8("ijtest ijTeSt ﬃtEsT", una::locale("tr")) == "İjtest İjtest Ffitest");
    TESTX(una::cases::to_titlecase_utf8("ijtest ijTeSt ﬃtEsT", una::locale("lt")) == "Ijtest Ijtest Ffitest"); // The same as by default

    TESTX(una::cases::to_titlecase_utf16(u"ijtest ijTeSt ﬃtEsT", una::locale("nl")) == u"IJtest IJtest Ffitest");
    TESTX(una::cases::to_titlecase_utf16(u"ijtest ijTeSt ﬃtEsT", una::locale("tr")) == u"İjtest İjtest Ffitest");
    TESTX(una::cases::to_titlecase_utf16(u"ijtest ijTeSt ﬃtEsT", una::locale("lt")) == u"Ijtest Ijtest Ffitest"); // The same as by default

    // Test for very stupid mistake
    TESTX(una::cases::to_titlecase_utf8(std::string_view{"ij", 1}, una::locale("nl")) == "I");
    TESTX(una::cases::to_titlecase_utf16(std::u16string_view{u"ij", 1}, una::locale("nl")) == u"I");

    // Strange corner case
    // # Lithuanian # Remove DOT ABOVE after "i" with upper or titlecase
    // rule from SpecialCasing.txt interfere with toTitlecase(X) rules from The Unicode Standard
    // so it is not handled. The behaviour is the same as in ICU.

    TESTX(una::cases::to_titlecase_utf8("i\xCC\x87tEsT", una::locale("lt")) == "I\xCC\x87test");

    TESTX(una::cases::to_titlecase_utf16(u"i\x0307tEsT", una::locale("lt")) == u"I\x0307test");

#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_SEGMENT_WORD

    return true;
}

// Implement random algorithms using find function

template<typename T>
test_constexpr bool caseless_ends_with(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    // Find always returns true for empty string2 so we need this to avoid endless loop
    if (string2.empty())
        return true;

    std::size_t start = 0;
    una::found found;
    while (true)
    {
        if constexpr (std::is_same_v<T, char>)
            found = una::caseless::find_utf8(string1.substr(start), string2);
        else if constexpr (std::is_same_v<T, char16_t>)
            found = una::caseless::find_utf16(string1.substr(start), string2);
        if (!found)
            break;
        if (start + found.end_pos() == string1.size())
            return true;
        start += found.end_pos();
    }

    return false;
}

template<typename T>
test_constexpr bool caseless_starts_with(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    una::found found;

    if constexpr (std::is_same_v<T, char>)
        found = una::caseless::find_utf8(string1, string2);
    else if constexpr (std::is_same_v<T, char16_t>)
        found = una::caseless::find_utf16(string1, string2);

    return found && found.pos() == 0;
}

// NOTE: constexpr std::vector is broken in GCC with -D_GLIBCXX_DEBUG
#if !(defined(TEST_MODE_CONSTEXPR) && defined(_GLIBCXX_DEBUG))
template<typename T>
test_constexpr std::vector<std::pair<std::size_t, std::size_t>>
caseless_find_all(std::basic_string_view<T> string1, std::basic_string_view<T> string2)
{
    std::vector<std::pair<std::size_t, std::size_t>> result;

    if (string2.empty())
        return result;

    std::size_t start = 0;
    una::found found;
    while (true)
    {
        if constexpr (std::is_same_v<T, char>)
            found = una::caseless::find_utf8(string1.substr(start), string2);
        else if constexpr (std::is_same_v<T, char16_t>)
            found = una::caseless::find_utf16(string1.substr(start), string2);
        if (!found)
            break;
        result.push_back({start + found.pos(), start + found.end_pos()});
        start += found.end_pos();
    }

    return result;
}
#endif

// Test them

test_constexpr bool test_case_find_ex()
{
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

    // NOTE: See comment in caseless_find_all function above for details
#if !(defined(TEST_MODE_CONSTEXPR) && defined(_GLIBCXX_DEBUG))
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

    // NOTE: See comment in caseless_find_all function above for details
#if !(defined(TEST_MODE_CONSTEXPR) && defined(_GLIBCXX_DEBUG))
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

    // TEST std::string::erase

    una::found found;

    std::string str8 = "ABCﬁXYZ";
    found = una::caseless::find_utf8(str8, "FI");
    str8.erase(found.pos(), found.end_pos() - found.pos());
    TESTX(str8 == "ABCXYZ");

    std::u16string str16 = u"ABCﬁXYZ";
    found = una::caseless::find_utf16(str16, u"FI");
    str16.erase(found.pos(), found.end_pos() - found.pos());
    TESTX(str16 == u"ABCXYZ");

    return true;
}
