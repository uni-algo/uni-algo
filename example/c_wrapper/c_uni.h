/* C99 example wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#ifndef C_UNI_H
#define C_UNI_H

#include <stddef.h> // NULL, size_t
#include <stdbool.h> // bool, true, false

// Define types that must be used for strings
// These defines must not be used in a user code
#define UNI_CHAR16 wchar_t // Can be 16-bit or more signed/unsigned
#define UNI_CHAR32 int // Can be 32-bit or more signed/unsigned

// Case and/or Normalization modules can be disabled if needed.
// This excludes from compilation a part of Unicode data tables that are not needed.
// If both are disabled only converter functions will be available
// and all Unicode data tables will be excluded from compilation.
//#define UNI_ALGO_DISABLE_CASE
//#define UNI_ALGO_DISABLE_NORM

// Disable only collation functions (part of Case module)
//#define UNI_ALGO_DISABLE_COLLATION

// Disable only NFKC/NFKD normalization functions (part of Normalization module)
//#define UNI_ALGO_DISABLE_NFKC_NFKD

// Disable UTF-16 versions of case and normalization functions
//#define UNI_ALGO_DISABLE_UTF16

void uni_strfree(void* str);

size_t uni_strlen16(const UNI_CHAR16* str); // len in code units
size_t uni_strlen32(const UNI_CHAR32* str); // len in code units

UNI_CHAR16* uni_utf8to16(const char* str);
char* uni_utf16to8(const UNI_CHAR16* str);

UNI_CHAR32* uni_utf8to32(const char* str);
char* uni_utf32to8(const UNI_CHAR32* str);

UNI_CHAR32* uni_utf16to32(const UNI_CHAR16* str);
UNI_CHAR16* uni_utf32to16(const UNI_CHAR32* str);

#ifndef UNI_ALGO_DISABLE_CASE

char* uni_utf8_case_lower(const char* str);
char* uni_utf8_case_upper(const char* str);
char* uni_utf8_case_fold(const char* str);

#ifndef UNI_ALGO_DISABLE_UTF16
UNI_CHAR16* uni_utf16_case_lower(const UNI_CHAR16* str);
UNI_CHAR16* uni_utf16_case_upper(const UNI_CHAR16* str);
UNI_CHAR16* uni_utf16_case_fold(const UNI_CHAR16* str);
#endif // UNI_ALGO_DISABLE_UTF16

int uni_utf8_casesens_compare(const char* str1, const char* str2);
int uni_utf8_caseless_compare(const char* str1, const char* str2);

#ifndef UNI_ALGO_DISABLE_COLLATION
int uni_utf8_casesens_collate(const char* str1, const char* str2);
int uni_utf8_caseless_collate(const char* str1, const char* str2);
#endif // UNI_ALGO_DISABLE_COLLATION

size_t uni_utf8_casesens_search(const char* str1, const char* str2, size_t* pos, size_t* end);
size_t uni_utf8_caseless_search(const char* str1, const char* str2, size_t* pos, size_t* end);

#ifndef UNI_ALGO_DISABLE_UTF16

int uni_utf16_casesens_compare(const UNI_CHAR16* str1, const UNI_CHAR16* str2);
int uni_utf16_caseless_compare(const UNI_CHAR16* str1, const UNI_CHAR16* str2);

#ifndef UNI_ALGO_DISABLE_COLLATION
int uni_utf16_casesens_collate(const UNI_CHAR16* str1, const UNI_CHAR16* str2);
int uni_utf16_caseless_collate(const UNI_CHAR16* str1, const UNI_CHAR16* str2);
#endif // UNI_ALGO_DISABLE_COLLATION

size_t uni_utf16_casesens_search(const UNI_CHAR16* str1, const UNI_CHAR16* str2, size_t* pos, size_t* end);
size_t uni_utf16_caseless_search(const UNI_CHAR16* str1, const UNI_CHAR16* str2, size_t* pos, size_t* end);

#endif // UNI_ALGO_DISABLE_UTF16

// Not found value for search functions
extern const size_t uni_npos;

#endif // UNI_ALGO_DISABLE_CASE

#ifndef UNI_ALGO_DISABLE_NORM

char* uni_utf8_norm_nfc(const char* str);
char* uni_utf8_norm_nfd(const char* str);

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
char* uni_utf8_norm_nfkc(const char* str);
char* uni_utf8_norm_nfkd(const char* str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UTF16

UNI_CHAR16* uni_utf16_norm_nfc(const UNI_CHAR16* str);
UNI_CHAR16* uni_utf16_norm_nfd(const UNI_CHAR16* str);

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
UNI_CHAR16* uni_utf16_norm_nfkc(const UNI_CHAR16* str);
UNI_CHAR16* uni_utf16_norm_nfkd(const UNI_CHAR16* str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // UNI_ALGO_DISABLE_UTF16

bool uni_utf8_norm_is_nfc(const char* str);
bool uni_utf8_norm_is_nfd(const char* str);

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
bool uni_utf8_norm_is_nfkc(const char* str);
bool uni_utf8_norm_is_nfkd(const char* str);
#endif

#ifndef UNI_ALGO_DISABLE_UTF16

bool uni_utf16_norm_is_nfc(const UNI_CHAR16* str);
bool uni_utf16_norm_is_nfd(const UNI_CHAR16* str);

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
bool uni_utf16_norm_is_nfkc(const UNI_CHAR16* str);
bool uni_utf16_norm_is_nfkd(const UNI_CHAR16* str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // UNI_ALGO_DISABLE_UTF16

#endif // UNI_ALGO_DISABLE_NORM

#endif // C_UNI_H

#if 0
#ifndef UNI_C_H
#define UNI_C_H

void uni_freestr(void* str);

/* Types here must be the same that were defined in uni_c89.c */

wchar_t* uni_utf8to16(const char* str);
char* uni_utf16to8(const wchar_t* str);

int* uni_utf8to32(const char* str);
char* uni_utf32to8(const int* str);

int* uni_utf16to32(const wchar_t* str);
wchar_t* uni_utf32to16(const int* str);

/* Types here must be the same that were defined in uni_c89.c */

char* uni_utf8lower(const char* str);
char* uni_utf8upper(const char* str);
wchar_t* uni_utf16lower(const wchar_t* str);
wchar_t* uni_utf16upper(const wchar_t* str);

int uni_utf8compare(const char* str1, const char* str2, int nocase);
int uni_utf8collate(const char* str1, const char* str2, int nocase);
size_t uni_utf8search(const char* str1, const char* str2, int nocase);
int uni_utf16compare(const wchar_t* str1, const wchar_t* str2, int nocase);
int uni_utf16collate(const wchar_t* str1, const wchar_t* str2, int nocase);
size_t uni_utf16search(const wchar_t* str1, const wchar_t* str2, int nocase);

/* Not found value for search functions */
extern const size_t uni_search_not_found;

/* Types here must be the same that were defined in uni_c89.c */

char* uni_utf8nfc(const char* str);
char* uni_utf8nfd(const char* str);
char* uni_utf8nfkc(const char* str);
char* uni_utf8nfkd(const char* str);
wchar_t* uni_utf16nfc(const wchar_t* str);
wchar_t* uni_utf16nfd(const wchar_t* str);
wchar_t* uni_utf16nfkc(const wchar_t* str);
wchar_t* uni_utf16nfkd(const wchar_t* str);

int uni_utf8isnfc(const char* str);
int uni_utf8isnfd(const char* str);
int uni_utf8isnfkc(const char* str);
int uni_utf8isnfkd(const char* str);
int uni_utf16isnfc(const wchar_t* str);
int uni_utf16isnfd(const wchar_t* str);
int uni_utf16isnfkc(const wchar_t* str);
int uni_utf16isnfkd(const wchar_t* str);

#endif /* UNI_C_H */
#endif
