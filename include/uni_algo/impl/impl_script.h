/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_IMPL_SCRIPT_H_UAIH
#define UNI_ALGO_IMPL_SCRIPT_H_UAIH

#include "internal_defines.h"
#include "internal_stages.h"

#ifndef UNI_ALGO_STATIC_DATA
#include "data/extern_script.h"
#else
#include "data/data_script.h"
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

uaix_always_inline
uaix_static type_codept impl_script_get_script(type_codept c)
{
    // https://www.unicode.org/reports/tr24/tr24-34.html#Script

    // Treat all invalid as replacement character (U+FFFD)
    if (c > 0x10FFFF)
        return 0x5A797979; // Zyyy (Common script)

    const size_t index = stages(c, stage1_script, stage2_script);

    if (index == 0)
        return 0x5A7A7A7A; // Zzzz (Unknown script)

    return stage3_script[index];
}

uaix_always_inline
uaix_static bool impl_script_has_script(type_codept c, type_codept script)
{
    // https://www.unicode.org/reports/tr24/tr24-34.html#Script_Extensions

    // Treat all invalid as replacement character (U+FFFD)
    if (c > 0x10FFFF)
        return (script == 0x5A797979) ? true : false; // Zyyy (Common script)

    // First try ScriptExtensions Unicode data
    size_t index = stages(c, stage1_script_ext, stage2_script_ext);

    if (index != 0)
    {
        // Linear search here, there are very few code points
        // with more than 5 scripts so no point to use binary search.
        const size_t size = stage3_script_ext[index];
        for (size_t i = 0; i < size; ++i)
        {
            if (script == stage3_script_ext[index + i + 1])
                return true;
        }
    }

    // Then try Scripts Unicode data
    index = stages(c, stage1_script, stage2_script);

    if (index == 0)
        return (script == 0x5A7A7A7A) ? true : false; // Zzzz (Unknown script)

    return (script == stage3_script[index]) ? true : false;
}

UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_IMPL_SCRIPT_H_UAIH

/* Public Domain Contract
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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO  THE  WARRANTIES  OF  MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE  SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT Contract
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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO  THE WARRANTIES OF  MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  TORT OR OTHERWISE, ARISING FROM,
 * OUT OF  OR  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
