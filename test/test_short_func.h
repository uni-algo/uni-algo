/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Test that all short non-template functions are properly declared and compile.
// Note that this test doesn't test that the functions produce the correct result
// because if template functions in other tests work then these functions work too.

test_constexpr bool test_short_func_conv()
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

    TESTX(una::utf8to16u(str) == u16str);
    TESTX(una::utf8to32u(str) == u32str);

    TESTX(una::utf16to8(u16str) == str);
    TESTX(una::utf32to8(u32str) == str);

    TESTX(una::utf16to32u(u16str) == u32str);
    TESTX(una::utf32to16u(u32str) == u16str);

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    TESTX(una::utf8to16(str) == wstr);
    TESTX(una::utf16to8(wstr) == str);

    TESTX(una::utf16to32u(wstr) == u32str);
    TESTX(una::utf32to16(u32str) == wstr);
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
    TESTX(una::utf8to32(str) == wstr);
    TESTX(una::utf32to8(wstr) == str);

    TESTX(una::utf16to32(u16str) == wstr);
    TESTX(una::utf32to16u(wstr) == u16str);
#endif // WCHAR_MAX >= 0x7FFFFFFF

#ifdef __cpp_lib_char8_t
    TESTX(una::utf8to16u(u8str) == u16str);
    TESTX(una::utf8to32u(u8str) == u32str);

    TESTX(una::utf16to8u(u16str) == u8str);
    TESTX(una::utf32to8u(u32str) == u8str);
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    TESTX(una::utf8to16(u8str) == wstr);
    TESTX(una::utf16to8u(wstr) == u8str);
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
    TESTX(una::utf8to32(u8str) == wstr);
    TESTX(una::utf32to8u(wstr) == u8str);
#endif // WCHAR_MAX >= 0x7FFFFFFF
#endif // __cpp_lib_char8_t

//---------------------------------------------
// Repeat prev tests with una::strict namespace
//---------------------------------------------
    una::error error;

    TESTX(una::strict::utf8to16u(str, error) == u16str);
    TESTX(una::strict::utf8to32u(str, error) == u32str);

    TESTX(una::strict::utf16to8(u16str, error) == str);
    TESTX(una::strict::utf32to8(u32str, error) == str);

    TESTX(una::strict::utf16to32u(u16str, error) == u32str);
    TESTX(una::strict::utf32to16u(u32str, error) == u16str);

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    TESTX(una::strict::utf8to16(str, error) == wstr);
    TESTX(una::strict::utf16to8(wstr, error) == str);

    TESTX(una::strict::utf16to32u(wstr, error) == u32str);
    TESTX(una::strict::utf32to16(u32str, error) == wstr);
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
    TESTX(una::strict::utf8to32(str, error) == wstr);
    TESTX(una::strict::utf32to8(wstr, error) == str);

    TESTX(una::strict::utf16to32(u16str, error) == wstr);
    TESTX(una::strict::utf32to16u(wstr, error) == u16str);
#endif // WCHAR_MAX >= 0x7FFFFFFF

#ifdef __cpp_lib_char8_t
    TESTX(una::strict::utf8to16u(u8str, error) == u16str);
    TESTX(una::strict::utf8to32u(u8str, error) == u32str);

    TESTX(una::strict::utf16to8u(u16str, error) == u8str);
    TESTX(una::strict::utf32to8u(u32str, error) == u8str);
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    TESTX(una::strict::utf8to16(u8str, error) == wstr);
    TESTX(una::strict::utf16to8u(wstr, error) == u8str);
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
    TESTX(una::strict::utf8to32(u8str, error) == wstr);
    TESTX(una::strict::utf32to8u(wstr, error) == u8str);
#endif // WCHAR_MAX >= 0x7FFFFFFF
#endif // __cpp_lib_char8_t

//---------------------------------------------
// Validation functions
//---------------------------------------------

    TESTX(una::is_valid_utf8(str));
    TESTX(una::is_valid_utf8(str, error));
    TESTX(una::is_valid_utf16(u16str));
    TESTX(una::is_valid_utf16(u16str, error));
    TESTX(una::is_valid_utf32(u32str));
    TESTX(una::is_valid_utf32(u32str, error));

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
    TESTX(una::is_valid_utf16(wstr));
    TESTX(una::is_valid_utf16(wstr, error));
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
    TESTX(una::is_valid_utf32(wstr));
    TESTX(una::is_valid_utf32(wstr, error));
#endif // WCHAR_MAX >= 0x7FFFFFFF

#ifdef __cpp_lib_char8_t
    TESTX(una::is_valid_utf8(u8str));
    TESTX(una::is_valid_utf8(u8str, error));
#endif // __cpp_lib_char8_t

    return true;
}

test_constexpr bool test_short_func_case()
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

    TESTX(una::cases::to_lowercase_utf8(str) == str);
    TESTX(una::cases::to_uppercase_utf8(str) == str);
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_lowercase_utf8(str, una::locale{}) == str);
    TESTX(una::cases::to_uppercase_utf8(str, una::locale{}) == str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_casefold_utf8(str) == str);
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_titlecase_utf8(str) == str);
    TESTX(una::cases::to_titlecase_utf8(str, una::locale{}) == str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_SEGMENT_WORD

    TESTX(una::casesens::compare_utf8(str, str) == 0);
    TESTX(una::caseless::compare_utf8(str, str) == 0);
#ifndef UNI_ALGO_DISABLE_COLLATE
    TESTX(una::casesens::collate_utf8(str, str) == 0);
    TESTX(una::caseless::collate_utf8(str, str) == 0);
#endif // UNI_ALGO_DISABLE_COLLATE
    TESTX(una::casesens::find_utf8(str, str));
    TESTX(una::caseless::find_utf8(str, str));

// --------------
// std::u16string
// --------------

    TESTX(una::cases::to_lowercase_utf16(u16str) == u16str);
    TESTX(una::cases::to_uppercase_utf16(u16str) == u16str);
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_lowercase_utf16(u16str, una::locale{}) == u16str);
    TESTX(una::cases::to_uppercase_utf16(u16str, una::locale{}) == u16str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_casefold_utf16(u16str) == u16str);
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_titlecase_utf16(u16str) == u16str);
    TESTX(una::cases::to_titlecase_utf16(u16str, una::locale{}) == u16str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_SEGMENT_WORD

    TESTX(una::casesens::compare_utf16(u16str, u16str) == 0);
    TESTX(una::caseless::compare_utf16(u16str, u16str) == 0);
#ifndef UNI_ALGO_DISABLE_COLLATE
    TESTX(una::casesens::collate_utf16(u16str, u16str) == 0);
    TESTX(una::caseless::collate_utf16(u16str, u16str) == 0);
#endif // UNI_ALGO_DISABLE_COLLATE
    TESTX(una::casesens::find_utf16(u16str, u16str));
    TESTX(una::caseless::find_utf16(u16str, u16str));

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
// ------------
// std::wstring
// ------------

    TESTX(una::cases::to_lowercase_utf16(wstr) == wstr);
    TESTX(una::cases::to_uppercase_utf16(wstr) == wstr);
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_lowercase_utf16(wstr, una::locale{}) == wstr);
    TESTX(una::cases::to_uppercase_utf16(wstr, una::locale{}) == wstr);
#endif // UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_casefold_utf16(wstr) == wstr);
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_titlecase_utf16(wstr) == wstr);
    TESTX(una::cases::to_titlecase_utf16(wstr, una::locale{}) == wstr);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_SEGMENT_WORD

    TESTX(una::casesens::compare_utf16(wstr, wstr) == 0);
    TESTX(una::caseless::compare_utf16(wstr, wstr) == 0);
#ifndef UNI_ALGO_DISABLE_COLLATE
    TESTX(una::casesens::collate_utf16(wstr, wstr) == 0);
    TESTX(una::caseless::collate_utf16(wstr, wstr) == 0);
#endif // UNI_ALGO_DISABLE_COLLATE
    TESTX(una::casesens::find_utf16(wstr, wstr));
    TESTX(una::caseless::find_utf16(wstr, wstr));

#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef __cpp_lib_char8_t
// -------------
// std::u8string
// -------------

    TESTX(una::cases::to_lowercase_utf8(u8str) == u8str);
    TESTX(una::cases::to_uppercase_utf8(u8str) == u8str);
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_lowercase_utf8(u8str, una::locale{}) == u8str);
    TESTX(una::cases::to_uppercase_utf8(u8str, una::locale{}) == u8str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_casefold_utf8(u8str) == u8str);
#ifndef UNI_ALGO_DISABLE_SEGMENT_WORD
#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(una::cases::to_titlecase_utf8(u8str) == u8str);
    TESTX(una::cases::to_titlecase_utf8(u8str, una::locale{}) == u8str);
#endif // UNI_ALGO_DISABLE_FULL_CASE
#endif // UNI_ALGO_DISABLE_SEGMENT_WORD

    TESTX(una::casesens::compare_utf8(u8str, u8str) == 0);
    TESTX(una::caseless::compare_utf8(u8str, u8str) == 0);
#ifndef UNI_ALGO_DISABLE_COLLATE
    TESTX(una::casesens::collate_utf8(u8str, u8str) == 0);
    TESTX(una::caseless::collate_utf8(u8str, u8str) == 0);
#endif // UNI_ALGO_DISABLE_COLLATE
    TESTX(una::casesens::find_utf8(u8str, u8str));
    TESTX(una::caseless::find_utf8(u8str, u8str));

#endif // __cpp_lib_char8_t

    return true;
}

test_constexpr bool test_short_func_norm()
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

    TESTX(una::norm::to_nfc_utf8(str) == str);
    TESTX(una::norm::to_nfd_utf8(str) == str);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::to_nfkc_utf8(str) == str);
    TESTX(una::norm::to_nfkd_utf8(str) == str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
    TESTX(una::norm::to_unaccent_utf8(str) == str);
#endif // UNI_ALGO_DISABLE_PROP

    TESTX(una::norm::is_nfc_utf8(str));
    TESTX(una::norm::is_nfd_utf8(str));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::is_nfkc_utf8(str));
    TESTX(una::norm::is_nfkd_utf8(str));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

    una::error error;

    TESTX(una::norm::is_nfc_utf8(str, error));
    TESTX(una::norm::is_nfd_utf8(str, error));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::is_nfkc_utf8(str, error));
    TESTX(una::norm::is_nfkd_utf8(str, error));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

// --------------
// std::u16string
// --------------

    TESTX(una::norm::to_nfc_utf16(u16str) == u16str);
    TESTX(una::norm::to_nfd_utf16(u16str) == u16str);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::to_nfkc_utf16(u16str) == u16str);
    TESTX(una::norm::to_nfkd_utf16(u16str) == u16str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
    TESTX(una::norm::to_unaccent_utf16(u16str) == u16str);
#endif // UNI_ALGO_DISABLE_PROP

    TESTX(una::norm::is_nfc_utf16(u16str));
    TESTX(una::norm::is_nfd_utf16(u16str));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::is_nfkc_utf16(u16str));
    TESTX(una::norm::is_nfkd_utf16(u16str));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

    TESTX(una::norm::is_nfc_utf16(u16str, error));
    TESTX(una::norm::is_nfd_utf16(u16str, error));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::is_nfkc_utf16(u16str, error));
    TESTX(una::norm::is_nfkd_utf16(u16str, error));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
// ------------
// std::wstring
// ------------

    TESTX(una::norm::to_nfc_utf16(wstr) == wstr);
    TESTX(una::norm::to_nfd_utf16(wstr) == wstr);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::to_nfkc_utf16(wstr) == wstr);
    TESTX(una::norm::to_nfkd_utf16(wstr) == wstr);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
    TESTX(una::norm::to_unaccent_utf16(wstr) == wstr);
#endif // UNI_ALGO_DISABLE_PROP

    TESTX(una::norm::is_nfc_utf16(wstr));
    TESTX(una::norm::is_nfd_utf16(wstr));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::is_nfkc_utf16(wstr));
    TESTX(una::norm::is_nfkd_utf16(wstr));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

    TESTX(una::norm::is_nfc_utf16(wstr, error));
    TESTX(una::norm::is_nfd_utf16(wstr, error));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::is_nfkc_utf16(wstr, error));
    TESTX(una::norm::is_nfkd_utf16(wstr, error));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef __cpp_lib_char8_t
// -------------
// std::u8string
// -------------

    TESTX(una::norm::to_nfc_utf8(u8str) == u8str);
    TESTX(una::norm::to_nfd_utf8(u8str) == u8str);
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::to_nfkc_utf8(u8str) == u8str);
    TESTX(una::norm::to_nfkd_utf8(u8str) == u8str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
    TESTX(una::norm::to_unaccent_utf8(u8str) == u8str);
#endif // UNI_ALGO_DISABLE_PROP

    TESTX(una::norm::is_nfc_utf8(u8str));
    TESTX(una::norm::is_nfd_utf8(u8str));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::is_nfkc_utf8(u8str));
    TESTX(una::norm::is_nfkd_utf8(u8str));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

    TESTX(una::norm::is_nfc_utf8(u8str, error));
    TESTX(una::norm::is_nfd_utf8(u8str, error));
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    TESTX(una::norm::is_nfkc_utf8(u8str, error));
    TESTX(una::norm::is_nfkd_utf8(u8str, error));
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // __cpp_lib_char8_t

    return true;
}
