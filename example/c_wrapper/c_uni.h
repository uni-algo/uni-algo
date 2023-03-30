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
// If both are disabled only conversion functions will be available
// and all Unicode data tables will be excluded from compilation.
//#define UNI_ALGO_DISABLE_CASE
//#define UNI_ALGO_DISABLE_NORM

// Disable only collation functions (part of Case module)
//#define UNI_ALGO_DISABLE_COLLATION

// Disable only NFKC/NFKD normalization functions (part of Normalization module)
//#define UNI_ALGO_DISABLE_NFKC_NFKD

// Disable UTF-16 versions of case and normalization functions
//#define UNI_ALGO_DISABLE_UTF16

void una_strfree(void* str);

size_t una_strlen16(const UNI_CHAR16* str); // len in code units
size_t una_strlen32(const UNI_CHAR32* str); // len in code units

UNI_CHAR16* una_utf8to16(const char* str);
char* una_utf16to8(const UNI_CHAR16* str);

UNI_CHAR32* una_utf8to32(const char* str);
char* una_utf32to8(const UNI_CHAR32* str);

UNI_CHAR32* una_utf16to32(const UNI_CHAR16* str);
UNI_CHAR16* una_utf32to16(const UNI_CHAR32* str);

#ifndef UNI_ALGO_DISABLE_CASE

char* una_to_lowercase_utf8(const char* str);
char* una_to_uppercase_utf8(const char* str);
char* una_to_casefold_utf8(const char* str);

#ifndef UNI_ALGO_DISABLE_UTF16
UNI_CHAR16* una_to_lowercase_utf16(const UNI_CHAR16* str);
UNI_CHAR16* una_to_uppercase_utf16(const UNI_CHAR16* str);
UNI_CHAR16* una_to_casefold_utf16(const UNI_CHAR16* str);
#endif // UNI_ALGO_DISABLE_UTF16

int una_casesens_compare_utf8(const char* str1, const char* str2);
int una_caseless_compare_utf8(const char* str1, const char* str2);

#ifndef UNI_ALGO_DISABLE_COLLATION
int una_casesens_collate_utf8(const char* str1, const char* str2);
int una_caseless_collate_utf8(const char* str1, const char* str2);
#endif // UNI_ALGO_DISABLE_COLLATION

bool una_casesens_find_utf8(const char* str1, const char* str2, size_t* found_pos, size_t* found_end);
bool una_caseless_find_utf8(const char* str1, const char* str2, size_t* found_pos, size_t* found_end);

#ifndef UNI_ALGO_DISABLE_UTF16

int una_casesens_compare_utf16(const UNI_CHAR16* str1, const UNI_CHAR16* str2);
int una_caseless_compare_utf16(const UNI_CHAR16* str1, const UNI_CHAR16* str2);

#ifndef UNI_ALGO_DISABLE_COLLATION
int una_casesens_collate_utf16(const UNI_CHAR16* str1, const UNI_CHAR16* str2);
int una_caseless_collate_utf16(const UNI_CHAR16* str1, const UNI_CHAR16* str2);
#endif // UNI_ALGO_DISABLE_COLLATION

bool una_casesens_find_utf16(const UNI_CHAR16* str1, const UNI_CHAR16* str2, size_t* found_pos, size_t* found_end);
bool una_caseless_find_utf16(const UNI_CHAR16* str1, const UNI_CHAR16* str2, size_t* found_pos, size_t* found_end);

#endif // UNI_ALGO_DISABLE_UTF16

// Not found value for find functions
extern const size_t una_npos;

#endif // UNI_ALGO_DISABLE_CASE

#ifndef UNI_ALGO_DISABLE_NORM

char* una_norm_to_nfc_utf8(const char* str);
char* una_norm_to_nfd_utf8(const char* str);

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
char* una_norm_to_nfkc_utf8(const char* str);
char* una_norm_to_nfkd_utf8(const char* str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UTF16

UNI_CHAR16* una_norm_to_nfc_utf16(const UNI_CHAR16* str);
UNI_CHAR16* una_norm_to_nfd_utf16(const UNI_CHAR16* str);

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
UNI_CHAR16* una_norm_to_nfkc_utf16(const UNI_CHAR16* str);
UNI_CHAR16* una_norm_to_nfkd_utf16(const UNI_CHAR16* str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // UNI_ALGO_DISABLE_UTF16

bool una_norm_is_nfc_utf8(const char* str);
bool una_norm_is_nfd_utf8(const char* str);

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
bool una_norm_is_nfkc_utf8(const char* str);
bool una_norm_is_nfkd_utf8(const char* str);
#endif

#ifndef UNI_ALGO_DISABLE_UTF16

bool una_norm_is_nfc_utf16(const UNI_CHAR16* str);
bool una_norm_is_nfd_utf16(const UNI_CHAR16* str);

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
bool una_norm_is_nfkc_utf16(const UNI_CHAR16* str);
bool una_norm_is_nfkd_utf16(const UNI_CHAR16* str);
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // UNI_ALGO_DISABLE_UTF16

#endif // UNI_ALGO_DISABLE_NORM

#endif // C_UNI_H
