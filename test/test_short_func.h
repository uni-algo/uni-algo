/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Test that all short non-template functions are properly declared and compile.
// Note that this test doesn't test that the functions produce the correct result
// because if template functions in other tests work then these functions work too.

void test_short_func_convert()
{
    std::string str = "123";
    std::u16string u16str = u"123";
    std::u32string u32str = U"123";
#if WCHAR_MAX >= 0x7FFF // if not 8-bit wchar_t
    std::wstring wstr = L"123";
#endif // WCHAR_MAX >= 0x7FFF
#ifdef __cpp_lib_char8_t
    std::u8string u8str = u8"123";
#endif // __cpp_lib_char8_t

    TESTX(uni::utf16to32u(u16str) == u32str);
    TESTX(uni::utf32to16u(u32str) == u16str);

    TESTX(uni::utf8to16u(str) == u16str);
    TESTX(uni::utf8to32u(str) == u32str);
    TESTX(uni::utf16to8(u16str) == str);
    TESTX(uni::utf32to8(u32str) == str);

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    TESTX(uni::utf16to8(wstr) == str);
    TESTX(uni::utf8to16(str) == wstr);

    TESTX(uni::utf32to16(u32str) == wstr);
    TESTX(uni::utf16to32u(wstr) == u32str);
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
    TESTX(uni::utf32to8(wstr) == str);
    TESTX(uni::utf8to32(str) == wstr);

    TESTX(uni::utf32to16u(wstr) == u16str);
    TESTX(uni::utf16to32(u16str) == wstr);
#endif // WCHAR_MAX >= 0x7FFFFFFF

#ifdef __cpp_lib_char8_t
    TESTX(uni::utf8to16u(u8str) == u16str);
    TESTX(uni::utf8to32u(u8str) == u32str);
    TESTX(uni::utf16to8u(u16str) == u8str);
    TESTX(uni::utf32to8u(u32str) == u8str);
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    TESTX(uni::utf16to8u(wstr) == u8str);
    TESTX(uni::utf8to16(u8str) == wstr);
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
    TESTX(uni::utf32to8u(wstr) == u8str);
    TESTX(uni::utf8to32(u8str) == wstr);
#endif // WCHAR_MAX >= 0x7FFFFFFF
#endif // __cpp_lib_char8_t
}

void test_short_func_case()
{
    std::string str = "123";
    std::u16string u16str = u"123";
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    std::wstring wstr = L"123";
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#ifdef __cpp_lib_char8_t
    std::u8string u8str = u8"123";
#endif // __cpp_lib_char8_t

// -----------
// std::string
// -----------

    TESTX(uni::cases::utf8_lower(str) == str);
    TESTX(uni::cases::utf8_upper(str) == str);
    TESTX(uni::cases::utf8_lower(str, uni::locale{}) == str);
    TESTX(uni::cases::utf8_upper(str, uni::locale{}) == str);
    TESTX(uni::cases::utf8_fold(str) == str);
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::utf8_title(str) == str);
    TESTX(uni::cases::utf8_title(str, uni::locale{}) == str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD

    TESTX(uni::casesens::utf8_compare(str, str) == 0);
    TESTX(uni::caseless::utf8_compare(str, str) == 0);
#ifndef UNI_ALGO_DISABLE_COLLATE
    TESTX(uni::casesens::utf8_collate(str, str) == 0);
    TESTX(uni::caseless::utf8_collate(str, str) == 0);
#endif // UNI_ALGO_DISABLE_COLLATE
    TESTX(uni::casesens::utf8_search(str, str));
    TESTX(uni::caseless::utf8_search(str, str));

// --------------
// std::u16string
// --------------

    TESTX(uni::cases::utf16_lower(u16str) == u16str);
    TESTX(uni::cases::utf16_upper(u16str) == u16str);
    TESTX(uni::cases::utf16_lower(u16str, uni::locale{}) == u16str);
    TESTX(uni::cases::utf16_upper(u16str, uni::locale{}) == u16str);
    TESTX(uni::cases::utf16_fold(u16str) == u16str);
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::utf16_title(u16str) == u16str);
    TESTX(uni::cases::utf16_title(u16str, uni::locale{}) == u16str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD

    TESTX(uni::casesens::utf16_compare(u16str, u16str) == 0);
    TESTX(uni::caseless::utf16_compare(u16str, u16str) == 0);
#ifndef UNI_ALGO_DISABLE_COLLATE
    TESTX(uni::casesens::utf16_collate(u16str, u16str) == 0);
    TESTX(uni::caseless::utf16_collate(u16str, u16str) == 0);
#endif // UNI_ALGO_DISABLE_COLLATE
    TESTX(uni::casesens::utf16_search(u16str, u16str));
    TESTX(uni::caseless::utf16_search(u16str, u16str));

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
// ------------
// std::wstring
// ------------

    TESTX(uni::cases::utf16_lower(wstr) == wstr);
    TESTX(uni::cases::utf16_upper(wstr) == wstr);
    TESTX(uni::cases::utf16_lower(wstr, uni::locale{}) == wstr);
    TESTX(uni::cases::utf16_upper(wstr, uni::locale{}) == wstr);
    TESTX(uni::cases::utf16_fold(wstr) == wstr);
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::utf16_title(wstr) == wstr);
    TESTX(uni::cases::utf16_title(wstr, uni::locale{}) == wstr);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD

    TESTX(uni::casesens::utf16_compare(wstr, wstr) == 0);
    TESTX(uni::caseless::utf16_compare(wstr, wstr) == 0);
#ifndef UNI_ALGO_DISABLE_COLLATE
    TESTX(uni::casesens::utf16_collate(wstr, wstr) == 0);
    TESTX(uni::caseless::utf16_collate(wstr, wstr) == 0);
#endif // UNI_ALGO_DISABLE_COLLATE
    TESTX(uni::casesens::utf16_search(wstr, wstr));
    TESTX(uni::caseless::utf16_search(wstr, wstr));

#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef __cpp_lib_char8_t
// -------------
// std::u8string
// -------------

    TESTX(uni::cases::utf8_lower(u8str) == u8str);
    TESTX(uni::cases::utf8_upper(u8str) == u8str);
    TESTX(uni::cases::utf8_lower(u8str, uni::locale{}) == u8str);
    TESTX(uni::cases::utf8_upper(u8str, uni::locale{}) == u8str);
    TESTX(uni::cases::utf8_fold(u8str) == u8str);
#ifndef UNI_ALGO_DISABLE_BREAK_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::cases::utf8_title(u8str) == u8str);
    TESTX(uni::cases::utf8_title(u8str, uni::locale{}) == u8str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_BREAK_WORD

    TESTX(uni::casesens::utf8_compare(u8str, u8str) == 0);
    TESTX(uni::caseless::utf8_compare(u8str, u8str) == 0);
#ifndef UNI_ALGO_DISABLE_COLLATE
    TESTX(uni::casesens::utf8_collate(u8str, u8str) == 0);
    TESTX(uni::caseless::utf8_collate(u8str, u8str) == 0);
#endif // UNI_ALGO_DISABLE_COLLATE
    TESTX(uni::casesens::utf8_search(u8str, u8str));
    TESTX(uni::caseless::utf8_search(u8str, u8str));

#endif // __cpp_lib_char8_t
}

void test_short_func_norm()
{
    std::string str = "123";
    std::u16string u16str = u"123";
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    std::wstring wstr = L"123";
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF
#ifdef __cpp_lib_char8_t
    std::u8string u8str = u8"123";
#endif // __cpp_lib_char8_t

// -----------
// std::string
// -----------

    TESTX(uni::norm::utf8_nfc(str) == str);
    TESTX(uni::norm::utf8_nfd(str) == str);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::utf8_nfkc(str) == str);
    TESTX(uni::norm::utf8_nfkd(str) == str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_UNACCENT
    TESTX(uni::norm::utf8_unaccent(str) == str);
#endif // UNI_ALGO_DISABLE_UNACCENT

    TESTX(uni::norm::utf8_is_nfc(str));
    TESTX(uni::norm::utf8_is_nfd(str));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::utf8_is_nfkc(str));
    TESTX(uni::norm::utf8_is_nfkd(str));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

// --------------
// std::u16string
// --------------

    TESTX(uni::norm::utf16_nfc(u16str) == u16str);
    TESTX(uni::norm::utf16_nfd(u16str) == u16str);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::utf16_nfkc(u16str) == u16str);
    TESTX(uni::norm::utf16_nfkd(u16str) == u16str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_UNACCENT
    TESTX(uni::norm::utf16_unaccent(u16str) == u16str);
#endif // UNI_ALGO_DISABLE_UNACCENT

    TESTX(uni::norm::utf16_is_nfc(u16str));
    TESTX(uni::norm::utf16_is_nfd(u16str));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::utf16_is_nfkc(u16str));
    TESTX(uni::norm::utf16_is_nfkd(u16str));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
// ------------
// std::wstring
// ------------

    TESTX(uni::norm::utf16_nfc(wstr) == wstr);
    TESTX(uni::norm::utf16_nfd(wstr) == wstr);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::utf16_nfkc(wstr) == wstr);
    TESTX(uni::norm::utf16_nfkd(wstr) == wstr);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_UNACCENT
    TESTX(uni::norm::utf16_unaccent(wstr) == wstr);
#endif // UNI_ALGO_DISABLE_UNACCENT

    TESTX(uni::norm::utf16_is_nfc(wstr));
    TESTX(uni::norm::utf16_is_nfd(wstr));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::utf16_is_nfkc(wstr));
    TESTX(uni::norm::utf16_is_nfkd(wstr));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef __cpp_lib_char8_t
// -------------
// std::u8string
// -------------

    TESTX(uni::norm::utf8_nfc(u8str) == u8str);
    TESTX(uni::norm::utf8_nfd(u8str) == u8str);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::utf8_nfkc(u8str) == u8str);
    TESTX(uni::norm::utf8_nfkd(u8str) == u8str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_UNACCENT
    TESTX(uni::norm::utf8_unaccent(u8str) == u8str);
#endif // UNI_ALGO_DISABLE_UNACCENT

    TESTX(uni::norm::utf8_is_nfc(u8str));
    TESTX(uni::norm::utf8_is_nfd(u8str));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(uni::norm::utf8_is_nfkc(u8str));
    TESTX(uni::norm::utf8_is_nfkd(u8str));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // __cpp_lib_char8_t
}
