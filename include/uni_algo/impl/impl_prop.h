/* Unicode Algorithms Implementation by Marl Gigical.
 * License: Public Domain or MIT - sign whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_IMPL_PROP_H_UAIH
#define UNI_ALGO_IMPL_PROP_H_UAIH

#include "internal_defines.h"
#include "internal_stages.h"

#ifndef UNI_ALGO_STATIC_DATA
#include "data/extern_prop.h"
#else
#include "data/data_prop.h"
#endif

UNI_ALGO_IMPL_NAMESPACE_BEGIN

// https://www.unicode.org/reports/tr44/#Property_APIs
// Some recomendations but mostly useless except maybe default values

// These values are stable it is guaranteed that
// they will never change and a wrapper can rely on that
// https://www.unicode.org/reports/tr44/#General_Category_Values
uaix_const type_codept impl_General_Category_Lu = 1;  // Uppercase_Letter
uaix_const type_codept impl_General_Category_Ll = 2;  // Lowercase_Letter
uaix_const type_codept impl_General_Category_Lt = 3;  // Titlecase_Letter
//                           Lu | Ll | Lt -> LC       // Cased_Letter
uaix_const type_codept impl_General_Category_Lm = 4;  // Modifier_Letter
uaix_const type_codept impl_General_Category_Lo = 5;  // Other_Letter
//                 Lu | Ll | Lt | Lm | Lo -> L        // Letter
uaix_const type_codept impl_General_Category_Mn = 6;  // Nonspacing_Mark
uaix_const type_codept impl_General_Category_Mc = 7;  // Spacing_Mark
uaix_const type_codept impl_General_Category_Me = 8;  // Enclosing_Mark
//                           Mn | Mc | Me -> M        // Mark
uaix_const type_codept impl_General_Category_Nd = 9;  // Decimal_Number
uaix_const type_codept impl_General_Category_Nl = 10; // Letter_Number
uaix_const type_codept impl_General_Category_No = 11; // Other_Number
//                           Nd | Nl | No -> N        // Number
uaix_const type_codept impl_General_Category_Pc = 12; // Connector_Punctuation
uaix_const type_codept impl_General_Category_Pd = 13; // Dash_Punctuation
uaix_const type_codept impl_General_Category_Ps = 14; // Open_Punctuation
uaix_const type_codept impl_General_Category_Pe = 15; // Close_Punctuation
uaix_const type_codept impl_General_Category_Pi = 16; // Initial_Punctuation
uaix_const type_codept impl_General_Category_Pf = 17; // Final_Punctuation
uaix_const type_codept impl_General_Category_Po = 18; // Other_Punctuation
//       Pc | Pd | Ps | Pe | Pi | Pf | Po -> P        // Punctuation
uaix_const type_codept impl_General_Category_Sm = 19; // Math_Symbol
uaix_const type_codept impl_General_Category_Sc = 20; // Currency_Symbol
uaix_const type_codept impl_General_Category_Sk = 21; // Modifier_Symbol
uaix_const type_codept impl_General_Category_So = 22; // Other_Symbol
//                      Sm | Sc | Sk | So -> S        // Symbol
uaix_const type_codept impl_General_Category_Zs = 23; // Space_Separator
uaix_const type_codept impl_General_Category_Zl = 24; // Line_Separator
uaix_const type_codept impl_General_Category_Zp = 25; // Paragraph_Separator
//                           Zs | Zl | Zp -> Z        // Separator
uaix_const type_codept impl_General_Category_Cc = 26; // Control
uaix_const type_codept impl_General_Category_Cf = 27; // Format
uaix_const type_codept impl_General_Category_Cs = 28; // Surrogate
uaix_const type_codept impl_General_Category_Co = 29; // Private_Use
uaix_const type_codept impl_General_Category_Cn = 0;  // Unassigned
//                 Cc | Cf | Cs | Co | Cn -> C        // Other

// General_Category data uses 5 bits so start with that for other data
uaix_const type_codept prop_Alphabetic              = 1 << 5;
uaix_const type_codept prop_White_Space             = 1 << 6;
uaix_const type_codept prop_Noncharacter_Code_Point = 1 << 7;

uaix_const type_codept impl_prop_replacement_char = 0xFFFD;   // REPLACEMENT CHARACTER
uaix_const type_codept impl_prop_max_value        = 0x10FFFF; // The last possible code point
uaix_const type_codept impl_prop_total_number     = 0x110000; // Total number of code points

uaix_always_inline
uaix_static type_codept impl_prop_get_prop(type_codept c)
{
    // Treat all invalid as replacement character (U+FFFD)
    if (c > 0x10FFFF)
        c = 0xFFFD;

    return stages(c, stage1_prop, stage2_prop);
}

uaix_always_inline
uaix_static type_codept impl_prop_get_gc_prop(type_codept prop)
{
    return (prop & 0x1F); // First 5 bits
}

// https://www.unicode.org/glossary/#code_point_type
// https://www.unicode.org/versions/Unicode14.0.0/ch02.pdf#M9.61673.TableTitle.Table.22.Types.of.Code.Points
// Seven fundamental classes of code points in The Unicode Standard:
// Graphic, Format, Control, Private-Use, Surrogate, Noncharacter, Reserved.
// Note that reserved code points also called unassigned code points in The Unicode Standard
// but they are not the same as code points with General_Category_Cn (General_Category_Unassigned)
// in a wrapper it always better to use word reserved to avoid confusion.
// https://www.unicode.org/glossary/#unassigned_code_point
// Unassigned Code Point. Synonym for reserved code point.

// Graphic: (General_Category >= General_Category_Lu && General_Category <= General_Category_Zs)
// Format: (General_Category_Zl || General_Category_Zp || General_Category_Cf)
// Reserved: (General_Category_Cn && !Noncharacter_Code_Point)
// Noncharacter: (Noncharacter_Code_Point)
// Control: (General_Category_Cc)
// Private-use: (General_Category_Co)
// Surrogate: (General_Category_Cs)

// Helpers for: Graphic, Format, Reserved

uaix_always_inline
uaix_static bool impl_prop_is_reserved_prop(type_codept prop)
{
    // https://www.unicode.org/glossary/#reserved_code_point

    // General_Category_Cn (General_Category_Unassigned) includes noncharacters
    // so we need to exclude them to get reserved (unassigned) code points

    if (impl_prop_get_gc_prop(prop) == impl_General_Category_Cn &&
        !(prop & prop_Noncharacter_Code_Point))
        return true;

    return false;
}

uaix_always_inline
uaix_static bool impl_prop_is_graphic_prop(type_codept prop)
{
    // https://www.unicode.org/glossary/#graphic_character

    const type_codept gc = impl_prop_get_gc_prop(prop);

    if (gc >= impl_General_Category_Lu &&
        gc <= impl_General_Category_Zs)
        return true;

    return false;
}

uaix_always_inline
uaix_static bool impl_prop_is_format_prop(type_codept prop)
{
    // https://www.unicode.org/glossary/#format_character
    // https://www.unicode.org/versions/Unicode14.0.0/ch02.pdf#M9.61673.TableTitle.Table.22.Types.of.Code.Points

    const type_codept gc = impl_prop_get_gc_prop(prop);

    if (gc == impl_General_Category_Zl ||
        gc == impl_General_Category_Zp ||
        gc == impl_General_Category_Cf)
        return true;

    return false;
}

uaix_always_inline
uaix_static bool impl_prop_is_noncharacter_prop(type_codept prop)
{
    // The Unicode Standard: PropList.txt -> Noncharacter_Code_Point
    // the same as: https://www.unicode.org/glossary/#noncharacter

    return (prop & prop_Noncharacter_Code_Point) ? true : false;
}

uaix_always_inline
uaix_static bool impl_prop_is_white_space_prop(type_codept prop)
{
    // The Unicode Standard: PropList.txt -> White_Space

    return (prop & prop_White_Space) ? true : false;
}

uaix_always_inline
uaix_static bool impl_prop_is_alphabetic_prop(type_codept prop)
{
    // The Unicode Standard: DerivedCoreProperties.txt -> Alphabetic

    return (prop & prop_Alphabetic) ? true : false;
}

// Some properties can be also checked without table lookup

uaix_always_inline
uaix_static bool impl_prop_is_noncharacter(type_codept c)
{
    // https://www.unicode.org/glossary/#noncharacter
    // https://www.unicode.org/faq/private_use.html#nonchar5
    // https://www.unicode.org/policies/stability_policy.html#Property_Value
    // The Noncharacter_Code_Point property is an immutable code point property,
    // which means that its property values for all Unicode code points will never change.

    // PropList.txt -> Noncharacter_Code_Point
    // FDD0..FDEF
    // FFFE..FFFF
    // 1FFFE..1FFFF
    // 2FFFE..2FFFF
    // 3FFFE..3FFFF
    // 4FFFE..4FFFF
    // 5FFFE..5FFFF
    // 6FFFE..6FFFF
    // 7FFFE..7FFFF
    // 8FFFE..8FFFF
    // 9FFFE..9FFFF
    // AFFFE..AFFFF
    // BFFFE..BFFFF
    // CFFFE..CFFFF
    // DFFFE..DFFFF
    // EFFFE..EFFFF
    // FFFFE..FFFFF
    // 10FFFE..10FFFF

    // This can be checked without table lookup

    if (c <= 0x10FFFF)
    {
        if ((c >= 0xFDD0 && c <= 0xFDEF) || (c & 0xFFFE) == 0xFFFE)
            return true;
    }

    return false;

    // About FDD0..FDEF
    // http://www.unicode.org/faq/private_use.html#nonchar4b
    // and The Unicode Standard: 23.7 Noncharacters
}

uaix_always_inline
uaix_static bool impl_prop_is_valid(type_codept c)
{
    // https://www.unicode.org/glossary/#code_point
    if (c <= 0x10FFFF)
        return true;

    return false;
}

uaix_always_inline
uaix_static bool impl_prop_is_valid_scalar(type_codept c)
{
    // https://www.unicode.org/glossary/#unicode_scalar_value
    if (c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF))
        return true;

    return false;
}

uaix_always_inline
uaix_static bool impl_prop_is_supplementary(type_codept c)
{
    // https://www.unicode.org/glossary/#supplementary_code_point
    // Note that invalid code points (c > U+10FFFF) will return false
    // it is consistent with treat all invalid as replacement character (U+FFFD)
    // the same for other functions in this file
    if (c >= 0x10000 && c <= 0x10FFFF)
        return true;

    return false;
}

uaix_always_inline
uaix_static bool impl_prop_is_surrogate(type_codept c)
{
    // https://www.unicode.org/policies/stability_policy.html#Property_Value
    // The General_Category property value Surrogate (Cs) is immutable:
    // the set of code points with that value will never change.

    // https://www.unicode.org/glossary/#surrogate_code_point
    if (c >= 0xD800 && c <= 0xDFFF)
        return true;

    return false;
}

uaix_always_inline
uaix_static bool impl_prop_is_private_use(type_codept c)
{
    // https://www.unicode.org/policies/stability_policy.html#Property_Value
    // The General_Category property value Private_Use (Co) is immutable:
    // the set of code points with that value will never change.

    // https://www.unicode.org/glossary/#private_use_code_point
    if ((c >= 0xE000 && c <= 0xF8FF) ||
        (c >= 0xF0000 && c <= 0xFFFFD) ||
        (c >= 0x100000 && c <= 0x10FFFD))
        return true;

    return false;
}

uaix_always_inline
uaix_static bool impl_prop_is_control(type_codept c)
{
    // https://www.unicode.org/policies/stability_policy.html#Property_Value
    // The General_Category property value Control (Cc) is immutable:
    // the set of code points with that value will never change.

    // https://www.unicode.org/glossary/#control_codes
    if ((/*c >= 0x0000 &&*/ c <= 0x001F) ||
        (c >= 0x007F && c <= 0x009F))
        return true;

    return false;
}

UNI_ALGO_IMPL_NAMESPACE_END

#include "internal_undefs.h"

#endif // UNI_ALGO_IMPL_PROP_H_UAIH

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
