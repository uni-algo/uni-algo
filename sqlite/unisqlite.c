/* SQLite Extension for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

// How to use (3 ways):
// 1. The easiest way is to compile this file as a part of
// your application then include unisqlite.h and call
// uni_sqlite3_init() before a first SQLite function call.
// 2. You can compile this file together with sqlite-amalgamation
// into SQLite library and then use it the same way as 1.
// 3. You can compile dynamic extension using this file
// with UNI_SQLITE_DYNAMIC_EXTENSION define and then load it
// with SQLite function: sqlite3_load_extension.
// Note that the name of the dynamic extension must be unisqlite.dll/unisqlite.so/libunisqlite.so

// Use the define to compile dynamic extension
//#define UNI_SQLITE_DYNAMIC_EXTENSION

// Normalization can be disabled to drastically reduce the size of Unicode data
//#define UNI_ALGO_IMPL_DISABLE_NORM
//#define UNI_ALGO_IMPL_DISABLE_NFKC_NFKD

#include "../src/impl/impl_unicode_version.h"

// Collation names
#if UNI_ALGO_UNICODE_VERSION != 14000000
#error "Fix collation name"
#endif
#define UNI_SQLITE_COLLATION "UNI_1400"
#define UNI_SQLITE_COLLATION_NOCASE "UNI_1400_NOCASE"

// ----------------
// uni_sqlite3_init
// ----------------

// uni_sqlite3_init function with sqlite3_auto_extension call
// must be declared before #include "sqlite3ext.h" etc.
// or we will get segmentation fault because of how SQLite handles it,
// so just forward declare everything that we need for the function.
#ifndef UNI_SQLITE_DYNAMIC_EXTENSION
struct sqlite3;
struct sqlite3_api_routines;
#ifdef SQLITE_API
SQLITE_API
#endif
int sqlite3_auto_extension(void(*)(void));

// Forward declaration for SQLite extension entry point
#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_unisqlite_init(struct sqlite3* db, char** err_msg, const struct sqlite3_api_routines* papi);

// Export uni_sqlite3_init too so the file can be compiled together with sqlite-amalgamation
#ifdef _WIN32
__declspec(dllexport)
#endif
void uni_sqlite3_init()
{
    sqlite3_auto_extension((void(*)(void))(sqlite3_unisqlite_init));
}
#endif // UNI_SQLITE_DYNAMIC_EXTENSION

// --------------
// Extension code
// --------------

// https://www.sqlite.org/loadext.html
// This must be included first
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1

#include <stddef.h> // NULL, size_t
#include <stdbool.h> // bool, true, false
#include <limits.h> // UINT_MAX

// Define types that internal implementation will be using
#if UINT_MAX >= 0xFFFFFFFF
typedef unsigned int   type_codept; // Can be 32-bit or more unsigned only
#else
typedef unsigned long  type_codept; // If unsigned int is 16-bit then use unsigned long
#endif
typedef char           type_char8; // Can be whatever (but SQLite uses char for UTF-8 so do we)
// We don't implement UTF-16 and UTF-32 functions but still need to define
// the types for low-level to work, the types doesn't matter.
typedef unsigned short type_char16; // Can be 16-bit or more signed/unsigned
typedef type_codept    type_char32; // Can be 32-bit or more signed/unsigned

// Like function requeries experimental define
#define UNI_ALGO_EXPERIMENTAL

// TODO: title case
#define UNI_ALGO_DISABLE_BREAK_WORD

// Include impl_types.h always first then data and then implementation.
#include "../src/impl/impl_types.h"
#include "../src/impl/impl_case_data.h"
#include "../src/impl/impl_case.h"
#ifndef UNI_ALGO_IMPL_DISABLE_NORM
#include "../src/impl/impl_norm_data.h"
#include "../src/impl/impl_norm.h"
#endif // UNI_ALGO_IMPL_DISABLE_NORM

static void sqlite3_utf8_like(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    size_t len1 = (size_t)sqlite3_value_bytes(argv[1]);
    size_t len2 = (size_t)sqlite3_value_bytes(argv[0]);

    const char* str1 = (const char*)sqlite3_value_text(argv[1]);
    const char* str2 = (const char*)sqlite3_value_text(argv[0]);

    if (!str1 || !str2)
    {
        sqlite3_result_null(context);
        return;
    }

    type_codept escape = 0;

    if (argc == 3)
    {
        // Note that we support only ASCII for ESCAPE char to simplify the code
        const char *escape_str = (const char*)sqlite3_value_text(argv[2]);
        escape = escape_str ? escape_str[0] : 0;
        if (!escape_str || sqlite3_value_bytes(argv[2]) != 1 ||
            escape == 0 || escape > 0x7F)
        {
            sqlite3_result_null(context);
            return;
        }
    }

    bool ret = impl_utf8_like(str1, str1 + len1, str2, str2 + len2, true, '%', '_', escape);

    sqlite3_result_int(context, ret ? 1 : 0);
}

static const char* internal_utf8_casemap(const char* str, size_t len, int mode, size_t* end)
{
    char* dst = sqlite3_malloc64(len * impl_x_utf8_casemap * sizeof(char));
    if (dst)
    {
        *end = impl_utf8_casemap(str, str + len, dst, mode);
        // TODO: not sure if realloc is needed maybe SQLite will free the string right away
        // anyway so we do the extra realloc call for no reason
        char* result = sqlite3_realloc64(dst, *end * sizeof(char));
        return (const char*)(result ? result : dst);
    }
    return NULL;
}

#ifndef UNI_ALGO_IMPL_DISABLE_NORM

static const char* internal_utf8_nfc(const char* str, size_t len, size_t* end)
{
    char* dst = sqlite3_malloc64(len * impl_x_utf8_nfc * sizeof(char));
    if (dst)
    {
        *end = impl_utf8_nfc(str, str + len, dst);
        char* result = sqlite3_realloc64(dst, *end * sizeof(char));
        return (const char*)(result ? result : dst);
    }
    return NULL;
}

static const char* internal_utf8_nfd(const char* str, size_t len, size_t* end)
{
    char* dst = sqlite3_malloc64(len * impl_x_utf8_nfd * sizeof(char));
    if (dst)
    {
        *end = impl_utf8_nfd(str, str + len, dst);
        char* result = sqlite3_realloc64(dst, *end * sizeof(char));
        return (const char*)(result ? result : dst);
    }
    return NULL;
}

#ifndef UNI_ALGO_IMPL_DISABLE_NFKC_NFKD

static const char* internal_utf8_nfkc(const char* str, size_t len, size_t* end)
{
    char* dst = sqlite3_malloc64(len * impl_x_utf8_nfkc * sizeof(char));
    if (dst)
    {
        *end = impl_utf8_nfkc(str, str + len, dst);
        char* result = sqlite3_realloc64(dst, *end * sizeof(char));
        return (const char*)(result ? result : dst);
    }
    return NULL;
}

static const char* internal_utf8_nfkd(const char* str, size_t len, size_t* end)
{
    char* dst = sqlite3_malloc64(len * impl_x_utf8_nfkd * sizeof(char));
    if (dst)
    {
        *end = impl_utf8_nfkd(str, str + len, dst);
        char* result = sqlite3_realloc64(dst, *end * sizeof(char));
        return (const char*)(result ? result : dst);
    }
    return NULL;
}

#endif // UNI_ALGO_IMPL_DISABLE_NFKC_NFKD

static const char* internal_utf8_unaccent(const char* str, size_t len, size_t* end)
{
    char* dst = sqlite3_malloc64(len * impl_x_utf8_unaccent * sizeof(char));
    if (dst)
    {
        *end = impl_utf8_unaccent(str, str + len, dst);
        char* result = sqlite3_realloc64(dst, *end * sizeof(char));
        return (const char*)(result ? result : dst);
    }
    return NULL;
}

#endif // UNI_ALGO_IMPL_DISABLE_NORM

static void sqlite3_utf8_upper(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    (void)argc;
    size_t len = (size_t)sqlite3_value_bytes(argv[0]);
    const char* str = (const char*)sqlite3_value_text(argv[0]);
    if (!str)
    {
        sqlite3_result_null(context);
        return;
    }

    size_t out_len = 0;
    const char* result = internal_utf8_casemap(str, len, impl_casemap_mode_upper, &out_len);
    sqlite3_result_text64(context, result, out_len, sqlite3_free, SQLITE_UTF8);
}

static void sqlite3_utf8_lower(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    (void)argc;
    size_t len = (size_t)sqlite3_value_bytes(argv[0]);
    const char* str = (const char*)sqlite3_value_text(argv[0]);
    if (!str)
    {
        sqlite3_result_null(context);
        return;
    }

    size_t out_len = 0;
    const char* result = internal_utf8_casemap(str, len, impl_casemap_mode_lower, &out_len);
    sqlite3_result_text64(context, result, out_len, sqlite3_free, SQLITE_UTF8);
}

static void sqlite3_utf8_fold(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    (void)argc;
    size_t len = (size_t)sqlite3_value_bytes(argv[0]);
    const char* str = (const char*)sqlite3_value_text(argv[0]);
    if (!str)
    {
        sqlite3_result_null(context);
        return;
    }

    size_t out_len = 0;
    const char* result = internal_utf8_casemap(str, len, impl_casemap_mode_fold, &out_len);
    sqlite3_result_text64(context, result, out_len, sqlite3_free, SQLITE_UTF8);
}

#ifndef UNI_ALGO_IMPL_DISABLE_NORM

static void sqlite3_utf8_nfc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    (void)argc;
    size_t len = (size_t)sqlite3_value_bytes(argv[0]);
    const char* str = (const char*)sqlite3_value_text(argv[0]);
    if (!str)
    {
        sqlite3_result_null(context);
        return;
    }

    size_t out_len = 0;
    const char* result = internal_utf8_nfc(str, len, &out_len);
    sqlite3_result_text64(context, result, out_len, sqlite3_free, SQLITE_UTF8);
}

static void sqlite3_utf8_nfd(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    (void)argc;
    size_t len = (size_t)sqlite3_value_bytes(argv[0]);
    const char* str = (const char*)sqlite3_value_text(argv[0]);
    if (!str)
    {
        sqlite3_result_null(context);
        return;
    }

    size_t out_len = 0;
    const char* result = internal_utf8_nfd(str, len, &out_len);
    sqlite3_result_text64(context, result, out_len, sqlite3_free, SQLITE_UTF8);
}

#ifndef UNI_ALGO_IMPL_DISABLE_NFKC_NFKD

static void sqlite3_utf8_nfkc(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    (void)argc;
    size_t len = (size_t)sqlite3_value_bytes(argv[0]);
    const char* str = (const char*)sqlite3_value_text(argv[0]);
    if (!str)
    {
        sqlite3_result_null(context);
        return;
    }

    size_t out_len = 0;
    const char* result = internal_utf8_nfkc(str, len, &out_len);
    sqlite3_result_text64(context, result, out_len, sqlite3_free, SQLITE_UTF8);
}

static void sqlite3_utf8_nfkd(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    (void)argc;
    size_t len = (size_t)sqlite3_value_bytes(argv[0]);
    const char* str = (const char*)sqlite3_value_text(argv[0]);
    if (!str)
    {
        sqlite3_result_null(context);
        return;
    }

    size_t out_len = 0;
    const char* result = internal_utf8_nfkd(str, len, &out_len);
    sqlite3_result_text64(context, result, out_len, sqlite3_free, SQLITE_UTF8);
}

#endif // UNI_ALGO_IMPL_DISABLE_NFKC_NFKD

static void sqlite3_utf8_unaccent(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    (void)argc;
    size_t len = (size_t)sqlite3_value_bytes(argv[0]);
    const char* str = (const char*)sqlite3_value_text(argv[0]);
    if (!str)
    {
        sqlite3_result_null(context);
        return;
    }

    size_t out_len = 0;
    const char* result = internal_utf8_unaccent(str, len, &out_len);
    sqlite3_result_text64(context, result, out_len, sqlite3_free, SQLITE_UTF8);
}

#endif // UNI_ALGO_IMPL_DISABLE_NORM

static int sqlite3_utf8_collate(void* context, int len1, const void* str1, int len2, const void* str2)
{
    (void)context;
    const char* s1 = (const char*)str1;
    const char* s2 = (const char*)str2;
    return impl_utf8_collate(s1, s1 + (size_t)len1, s2, s2 + (size_t)len2, false);
}

static int sqlite3_utf8_collate_nocase(void* context, int len1, const void* str1, int len2, const void* str2)
{
    (void)context;
    const char* s1 = (const char*)str1;
    const char* s2 = (const char*)str2;
    return impl_utf8_collate(s1, s1 + (size_t)len1, s2, s2 + (size_t)len2, true);
}

// SQLite extension entry point.
// Note that SQLite uses strict rules for the name of the function
// so it can auto detect the entry point name if it is not specified.
// Use sqlite3.exe with .load command to check that it works properly.
// With the current name the extension name must be unisqlite.dll/unisqlite.so/libunisqlite.so
#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_unisqlite_init(struct sqlite3* db, char** err_msg, const struct sqlite3_api_routines* papi)
{
    (void)err_msg;
    SQLITE_EXTENSION_INIT2(papi);

    int rc = SQLITE_OK;

    rc = sqlite3_create_function(db, "like", 2, SQLITE_UTF8, NULL, sqlite3_utf8_like, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
    rc = sqlite3_create_function(db, "like", 3, SQLITE_UTF8, NULL, sqlite3_utf8_like, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
    rc = sqlite3_create_function(db, "upper", 1, SQLITE_UTF8, NULL, sqlite3_utf8_upper, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
    rc = sqlite3_create_function(db, "lower", 1, SQLITE_UTF8, NULL, sqlite3_utf8_lower, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
    rc = sqlite3_create_function(db, "fold", 1, SQLITE_UTF8, NULL, sqlite3_utf8_fold, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
#ifndef UNI_ALGO_IMPL_DISABLE_NORM
    rc = sqlite3_create_function(db, "nfc", 1, SQLITE_UTF8, NULL, sqlite3_utf8_nfc, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
    rc = sqlite3_create_function(db, "nfd", 1, SQLITE_UTF8, NULL, sqlite3_utf8_nfd, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
#ifndef UNI_ALGO_IMPL_DISABLE_NFKC_NFKD
    rc = sqlite3_create_function(db, "nfkc", 1, SQLITE_UTF8, NULL, sqlite3_utf8_nfkc, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
    rc = sqlite3_create_function(db, "nfkd", 1, SQLITE_UTF8, NULL, sqlite3_utf8_nfkd, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
#endif // UNI_ALGO_IMPL_DISABLE_NFKC_NFKD
    rc = sqlite3_create_function(db, "unaccent", 1, SQLITE_UTF8, NULL, sqlite3_utf8_unaccent, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
#endif // UNI_ALGO_IMPL_DISABLE_NORM
    rc = sqlite3_create_collation(db, UNI_SQLITE_COLLATION, SQLITE_UTF8, NULL, sqlite3_utf8_collate);
    if (rc != SQLITE_OK) return rc;
    rc = sqlite3_create_collation(db, UNI_SQLITE_COLLATION_NOCASE, SQLITE_UTF8, NULL, sqlite3_utf8_collate_nocase);
    if (rc != SQLITE_OK) return rc;

    return rc;
}

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
