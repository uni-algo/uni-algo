/* C99 example wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

/* EXAMPLE
int main(void)
{
    char str8[] = "Naïve"; // Random UTF-8 string

    // Convert the string to UTF-16 and back to UTF-8
    wchar_t* str16_conv = uni_utf8to16(str8);
    char* str8_conv = uni_utf16to8(str16_conv);

    if (strcmp(str8, str8_conv) == 0)
        printf("OK1\n");

    uni_strfree(str16_conv);
    uni_strfree(str8_conv);

    char* str8_upper = uni_utf8_case_upper("Straße");
    printf("%s\n", str8_upper); // Prints STRASSE
    uni_strfree(str8_upper);

    if (uni_utf8_caseless_compare("naïve", "NAÏVE") == 0)
        printf("OK2\n");

    size_t pos = uni_npos;
    size_t end = uni_npos;
    if (uni_utf8_caseless_search("Find naïve string", "NAÏVE", &pos, &end) && pos == 5 && end == 11)
        printf("OK3\n");

    return 0;
}
*/

#include <limits.h> // UINT_MAX
#include <stdlib.h> // malloc, free
#include <string.h> // strlen

#include "c_uni.h"

// Define types that low-level will be using
// See impl/impl_types.h for details
#if UINT_MAX >= 0xFFFFFFFF
typedef unsigned int  type_codept; // Can be 32-bit or more unsigned only
#else
typedef unsigned long type_codept; // If unsigned int is 16-bit then use unsigned long
#endif
typedef char          type_char8; // Can be whatever
typedef UNI_CHAR16    type_char16; // Can be 16-bit or more signed/unsigned
typedef UNI_CHAR32    type_char32; // Can be 32-bit or more signed/unsigned

// This file must be included first
#include "impl/impl_types.h"

// We do not implement title case functions (part of Case module)
// and unaccent functions (part of Normalization module) in this example
// so we must disable these dependencies.
// See impl/doc/config_defines.txt and look for word DEPENDENCY
#define UNI_ALGO_DISABLE_BREAK_WORD
#define UNI_ALGO_DISABLE_PROP

// Include Convert module (doesn't need Unicode data tables)
#include "impl/impl_convert.h"

// Do not define it if you include the following Unicode data in a different c file
#define UNI_ALGO_STATIC_DATA

// Include Unicode data tables for Case and Normalization modules
#ifndef UNI_ALGO_DISABLE_CASE
#include "impl/impl_case_data.h"
#endif
#ifndef UNI_ALGO_DISABLE_NORM
#include "impl/impl_norm_data.h"
#endif

// Include Case and Normalization modules
#ifndef UNI_ALGO_DISABLE_CASE
#include "impl/impl_case.h"
#endif
#ifndef UNI_ALGO_DISABLE_NORM
#include "impl/impl_norm.h"
#endif

void uni_strfree(void* str)
{
    // Simple function to free a string
    free(str);
}

static size_t internal_strlen32(const type_char32* str)
{
    // C doesn't have functions to count code units in UTF-16 and UTF-32 but we need them

    const type_char32* s = str;
    while (*s)
        s++;
    return (size_t)(s - str);
}

static size_t internal_strlen16(const type_char16* str)
{
    const type_char16* s = str;
    while (*s)
        s++;
    return (size_t)(s - str);
}

static size_t internal_strlen(const type_char8* str)
{
    return strlen(str);
}

size_t uni_strlen16(const type_char16* str)
{
    return internal_strlen16(str);
}

size_t uni_strlen32(const type_char32* str)
{
    return internal_strlen32(str);
}

type_char16* uni_utf8to16(const type_char8* str)
{
    // In a real program you'll probably be using some struct for strings where
    // the lenght of the string is known so this strlen call can be omitted
    size_t len = internal_strlen(str);

    // Allocate memory for converted string and null terminator
    type_char16* dst = malloc((len * impl_x_utf8to16 + 1) * sizeof(type_char16));
    if (dst)
    {
        type_char16* result = NULL; // This is for realloc call below

        size_t end = impl_utf8to16(str, str + len, dst, NULL); // Convert
        dst[end] = '\0'; // Terminate the string

        // Reallocate the string to clean unused memory
        result = realloc(dst, (end + 1) * sizeof(type_char16));

        return result ? result : dst;
    }

    return NULL; // Out of memory or something like that
}

// Other functions can be implemented the same way

type_char8* uni_utf16to8(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    type_char8* dst = malloc((len * impl_x_utf16to8 + 1) * sizeof(type_char8));
    if (dst)
    {
        type_char8* result = NULL;

        size_t end = impl_utf16to8(str, str + len, dst, NULL);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char8));

        return result ? result : dst;
    }

    return NULL;
}

type_char32* uni_utf8to32(const type_char8* str)
{
    size_t len = internal_strlen(str);

    type_char32* dst = malloc((len * impl_x_utf8to32 + 1) * sizeof(type_char32));
    if (dst)
    {
        type_char32* result = NULL;

        size_t end = impl_utf8to32(str, str + len, dst, NULL);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char32));

        return result ? result : dst;
    }

    return NULL;
}

type_char8* uni_utf32to8(const type_char32* str)
{
    size_t len = internal_strlen32(str);

    type_char8* dst = malloc((len * impl_x_utf32to8 + 1) * sizeof(type_char8));
    if (dst)
    {
        type_char8* result = NULL;

        size_t end = impl_utf32to8(str, str + len, dst, NULL);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char8));

        return result ? result : dst;
    }

    return NULL;
}

type_char32* uni_utf16to32(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    type_char32* dst = malloc((len * impl_x_utf16to32 + 1) * sizeof(type_char32));
    if (dst)
    {
        type_char32* result = NULL;

        size_t end = impl_utf16to32(str, str + len, dst, NULL);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char32));

        return result ? result : dst;
    }

    return NULL;
}

type_char16* uni_utf32to16(const type_char32* str)
{
    size_t len = internal_strlen32(str);

    type_char16* dst = malloc((len * impl_x_utf32to16 + 1) * sizeof(type_char16));
    if (dst)
    {
        type_char16* result = NULL;

        size_t end = impl_utf32to16(str, str + len, dst, NULL);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char16));

        return result ? result : dst;
    }

    return NULL;
}

#ifndef UNI_ALGO_DISABLE_CASE

const size_t uni_npos = impl_npos;

// Case mapping functions can be implemented the same way as Unicode convertion functions

static type_char8* internal_utf8_casemap(const type_char8* str, int mode)
{
    size_t len = internal_strlen(str);

    type_char8* dst = malloc((len * impl_x_utf8_casemap + 1) * sizeof(type_char8));
    if (dst)
    {
        type_char8* result = NULL;

        size_t end = impl_utf8_casemap(str, str + len, dst, mode);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char8));

        return result ? result : dst;
    }

    return NULL;
}

#ifndef UNI_ALGO_DISABLE_UTF16
static type_char16* internal_utf16_casemap(const type_char16* str, int mode)
{
    size_t len = internal_strlen16(str);

    type_char16* dst = malloc((len * impl_x_utf16_casemap + 1) * sizeof(type_char16));
    if (dst)
    {
        type_char16* result = NULL;

        size_t end = impl_utf16_casemap(str, str + len, dst, mode);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char16));

        return result ? result : dst;
    }

    return NULL;
}
#endif

type_char8* uni_utf8_case_lower(const type_char8* str)
{
    return internal_utf8_casemap(str, impl_casemap_mode_lower);
}

type_char8* uni_utf8_case_upper(const type_char8* str)
{
    return internal_utf8_casemap(str, impl_casemap_mode_upper);
}

type_char8* uni_utf8_case_fold(const type_char8* str)
{
    return internal_utf8_casemap(str, impl_casemap_mode_fold);
}

#ifndef UNI_ALGO_DISABLE_UTF16
type_char16* uni_utf16_case_lower(const type_char16* str)
{
    return internal_utf16_casemap(str, impl_casemap_mode_lower);
}

type_char16* uni_utf16_case_upper(const type_char16* str)
{
    return internal_utf16_casemap(str, impl_casemap_mode_upper);
}

type_char16* uni_utf16_case_fold(const type_char16* str)
{
    return internal_utf16_casemap(str, impl_casemap_mode_fold);
}
#endif // UNI_ALGO_DISABLE_UTF16

// For compare, collate and search functions simple wrappers can be used

int uni_utf8_casesens_compare(const type_char8* str1, const type_char8* str2)
{
    size_t len1 = internal_strlen(str1);
    size_t len2 = internal_strlen(str2);

    return impl_utf8_compare(str1, str1 + len1, str2, str2 + len2, false);
}
int uni_utf8_caseless_compare(const type_char8* str1, const type_char8* str2)
{
    size_t len1 = internal_strlen(str1);
    size_t len2 = internal_strlen(str2);

    return impl_utf8_compare(str1, str1 + len1, str2, str2 + len2, true);
}

#ifndef UNI_ALGO_DISABLE_COLLATION
int uni_utf8_casesens_collate(const type_char8* str1, const type_char8* str2)
{
    size_t len1 = internal_strlen(str1);
    size_t len2 = internal_strlen(str2);

    return impl_utf8_collate(str1, str1 + len1, str2, str2 + len2, false);
}
int uni_utf8_caseless_collate(const type_char8* str1, const type_char8* str2)
{
    size_t len1 = internal_strlen(str1);
    size_t len2 = internal_strlen(str2);

    return impl_utf8_collate(str1, str1 + len1, str2, str2 + len2, true);
}
#endif

size_t uni_utf8_casesens_search(const type_char8* str1, const type_char8* str2, size_t* pos, size_t *end)
{
    size_t len1 = internal_strlen(str1);
    size_t len2 = internal_strlen(str2);

    // Always set position to impl_npos before use a low-level function
    *pos = impl_npos;
    *end = impl_npos;

    return impl_utf8_search(str1, str1 + len1, str2, str2 + len2, false, pos, end);
}
size_t uni_utf8_caseless_search(const type_char8* str1, const type_char8* str2, size_t* pos, size_t *end)
{
    size_t len1 = internal_strlen(str1);
    size_t len2 = internal_strlen(str2);

    *pos = impl_npos;
    *end = impl_npos;

    return impl_utf8_search(str1, str1 + len1, str2, str2 + len2, true, pos, end);
}

#ifndef UNI_ALGO_DISABLE_UTF16
int uni_utf16_casesens_compare(const type_char16* str1, const type_char16* str2)
{
    size_t len1 = internal_strlen16(str1);
    size_t len2 = internal_strlen16(str2);

    return impl_utf16_compare(str1, str1 + len1, str2, str2 + len2, false);
}
int uni_utf16_caseless_compare(const type_char16* str1, const type_char16* str2)
{
    size_t len1 = internal_strlen16(str1);
    size_t len2 = internal_strlen16(str2);

    return impl_utf16_compare(str1, str1 + len1, str2, str2 + len2, true);
}

#ifndef UNI_ALGO_DISABLE_COLLATION
int uni_utf16_casesens_collate(const type_char16* str1, const type_char16* str2)
{
    size_t len1 = internal_strlen16(str1);
    size_t len2 = internal_strlen16(str2);

    return impl_utf16_collate(str1, str1 + len1, str2, str2 + len2, false);
}
int uni_utf16_caseless_collate(const type_char16* str1, const type_char16* str2)
{
    size_t len1 = internal_strlen16(str1);
    size_t len2 = internal_strlen16(str2);

    return impl_utf16_collate(str1, str1 + len1, str2, str2 + len2, true);
}
#endif

size_t uni_utf16_casesens_search(const type_char16* str1, const type_char16* str2, size_t* pos, size_t *end)
{
    size_t len1 = internal_strlen16(str1);
    size_t len2 = internal_strlen16(str2);

    *pos = impl_npos;
    *end = impl_npos;

    return impl_utf16_search(str1, str1 + len1, str2, str2 + len2, false, pos, end);
}
size_t uni_utf16_caseless_search(const type_char16* str1, const type_char16* str2, size_t* pos, size_t *end)
{
    size_t len1 = internal_strlen16(str1);
    size_t len2 = internal_strlen16(str2);

    *pos = impl_npos;
    *end = impl_npos;

    return impl_utf16_search(str1, str1 + len1, str2, str2 + len2, true, pos, end);
}
#endif // UNI_ALGO_DISABLE_UTF16
#endif // UNI_ALGO_DISABLE_CASE

#ifndef UNI_ALGO_DISABLE_NORM

// Normalization functions can be implemented the same way as Unicode convertion functions

type_char8* uni_utf8_norm_nfc(const type_char8* str)
{
    size_t len = internal_strlen(str);

    type_char8* dst = malloc((len * impl_x_utf8_nfc + 1) * sizeof(type_char8));
    if (dst)
    {
        type_char8* result = NULL;

        size_t end = impl_utf8_nfc(str, str + len, dst);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char8));

        return result ? result : dst;
    }

    return NULL;
}

type_char8* uni_utf8_norm_nfd(const type_char8* str)
{
    size_t len = internal_strlen(str);

    type_char8* dst = malloc((len * impl_x_utf8_nfd + 1) * sizeof(type_char8));
    if (dst)
    {
        type_char8* result = NULL;

        size_t end = impl_utf8_nfd(str, str + len, dst);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char8));

        return result ? result : dst;
    }

    return NULL;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
type_char8* uni_utf8_norm_nfkc(const type_char8* str)
{
    size_t len = internal_strlen(str);

    type_char8* dst = malloc((len * impl_x_utf8_nfkc + 1) * sizeof(type_char8));
    if (dst)
    {
        type_char8* result = NULL;

        size_t end = impl_utf8_nfkc(str, str + len, dst);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char8));

        return result ? result : dst;
    }

    return NULL;
}

type_char8* uni_utf8_norm_nfkd(const type_char8* str)
{
    size_t len = internal_strlen(str);

    type_char8* dst = malloc((len * impl_x_utf8_nfkd + 1) * sizeof(type_char8));
    if (dst)
    {
        type_char8* result = NULL;

        size_t end = impl_utf8_nfkd(str, str + len, dst);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char8));

        return result ? result : dst;
    }

    return NULL;
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UTF16
type_char16* uni_utf16_norm_nfc(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    type_char16* dst = malloc((len * impl_x_utf16_nfc + 1) * sizeof(type_char16));
    if (dst)
    {
        type_char16* result = NULL;

        size_t end = impl_utf16_nfc(str, str + len, dst);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char16));

        return result ? result : dst;
    }

    return NULL;
}

type_char16* uni_utf16_norm_nfd(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    type_char16* dst = malloc((len * impl_x_utf16_nfd + 1) * sizeof(type_char16));
    if (dst)
    {
        type_char16* result = NULL;

        size_t end = impl_utf16_nfd(str, str + len, dst);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char16));

        return result ? result : dst;
    }

    return NULL;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
type_char16* uni_utf16_norm_nfkc(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    type_char16* dst = malloc((len * impl_x_utf16_nfkc + 1) * sizeof(type_char16));
    if (dst)
    {
        type_char16* result = NULL;

        size_t end = impl_utf16_nfkc(str, str + len, dst);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char16));

        return result ? result : dst;
    }

    return NULL;
}

type_char16* uni_utf16_norm_nfkd(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    type_char16* dst = malloc((len * impl_x_utf16_nfkd + 1) * sizeof(type_char16));
    if (dst)
    {
        type_char16* result = NULL;

        size_t end = impl_utf16_nfkd(str, str + len, dst);
        dst[end] = '\0';

        result = realloc(dst, (end + 1) * sizeof(type_char16));

        return result ? result : dst;
    }

    return NULL;
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#endif // UNI_ALGO_DISABLE_UTF16

// For normalization detection functions simple wrappers can be used

bool uni_utf8_norm_is_nfc(const type_char8* str)
{
    size_t len = internal_strlen(str);

    return impl_utf8_is_nfc(str, str + len) == impl_norm_is_yes;
}

bool uni_utf8_norm_is_nfd(const type_char8* str)
{
    size_t len = internal_strlen(str);

    return impl_utf8_is_nfd(str, str + len) == impl_norm_is_yes;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
bool uni_utf8_norm_is_nfkc(const type_char8* str)
{
    size_t len = internal_strlen(str);

    return impl_utf8_is_nfkc(str, str + len) == impl_norm_is_yes;
}

bool uni_utf8_norm_is_nfkd(const type_char8* str)
{
    size_t len = internal_strlen(str);

    return impl_utf8_is_nfkd(str, str + len) == impl_norm_is_yes;
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UTF16
bool uni_utf16_norm_is_nfc(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    return impl_utf16_is_nfc(str, str + len) == impl_norm_is_yes;
}

bool uni_utf16_norm_is_nfd(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    return impl_utf16_is_nfd(str, str + len) == impl_norm_is_yes;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
bool uni_utf16_norm_is_nfkc(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    return impl_utf16_is_nfkc(str, str + len) == impl_norm_is_yes;
}

bool uni_utf16_norm_is_nfkd(const type_char16* str)
{
    size_t len = internal_strlen16(str);

    return impl_utf16_is_nfkd(str, str + len) == impl_norm_is_yes;
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#endif // UNI_ALGO_DISABLE_UTF16
#endif // UNI_ALGO_DISABLE_NORM

/* Public Domain License
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
