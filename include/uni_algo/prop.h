/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_PROP_H_UAIH
#define UNI_ALGO_PROP_H_UAIH

#ifdef UNI_ALGO_DISABLE_PROP
#error "Property module is disabled via define UNI_ALGO_DISABLE_PROP"
#endif

#include "config.h"
#include "internal/safe_layer.h"

#include "impl/impl_prop.h"

namespace una::codepoint {

// REPLACEMENT CHARACTER U+FFFD
inline constexpr char32_t replacement_char = detail::impl_prop_replacement_char;
// The last possible code point U+10FFFF
inline constexpr char32_t max_value = detail::impl_prop_max_value;
// Total number of code points 0x110000
inline constexpr char32_t total_number = detail::impl_prop_total_number;

enum class general_category : unsigned char {Cn = 0,
                                             Lu,Ll,Lt,Lm,Lo,
                                             Mn,Mc,Me,
                                             Nd,Nl,No,
                                             Pc,Pd,Ps,Pe,Pi,Pf,Po,
                                             Sm,Sc,Sk,So,
                                             Zs,Zl,Zp,
                                             Cc,Cf,Cs,Co};

class prop
{
    friend uaiw_constexpr general_category get_general_category(const prop& p) noexcept;

private:
    detail::type_codept data = 0;

public:
    uaiw_constexpr prop() = delete;
    uaiw_constexpr explicit prop(char32_t c) noexcept : data{detail::impl_prop_get_prop(c)} {}
    // TODO: We can cheat here to make the size of the class smaller because
    // we know that low-level uses only 8-bit for this property but I'm not sure it's worth it
    // data{static_cast<unsigned char>(detail::impl_prop_get_prop(c))}
    // and make data unsigned char
    // And No, I don't want to change low-level for this I want low-level always to return
    // property data in type_codept because everything related to code point uses it there.
    // --- Added later ---
    // Actually it might be better to redesign the low-level for this and use different types for properties there,
    // there are some parts of the low-level where it will make structs smaller too for example text segmentation.
    // Or it might be better to fix problems with structs with casts and keep property type always 32-bit for consistency.

    // https://www.unicode.org/reports/tr44/#General_Category_Values

    uaiw_constexpr bool General_Category_Lu() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Lu; }
    uaiw_constexpr bool General_Category_Ll() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Ll; }
    uaiw_constexpr bool General_Category_Lt() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Lt; }
    uaiw_constexpr bool General_Category_Lm() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Lm; }
    uaiw_constexpr bool General_Category_Lo() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Lo; }

    uaiw_constexpr bool General_Category_Mn() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Mn; }
    uaiw_constexpr bool General_Category_Mc() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Mc; }
    uaiw_constexpr bool General_Category_Me() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Me; }

    uaiw_constexpr bool General_Category_Nd() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Nd; }
    uaiw_constexpr bool General_Category_Nl() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Nl; }
    uaiw_constexpr bool General_Category_No() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_No; }

    uaiw_constexpr bool General_Category_Pc() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Pc; }
    uaiw_constexpr bool General_Category_Pd() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Pd; }
    uaiw_constexpr bool General_Category_Ps() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Ps; }
    uaiw_constexpr bool General_Category_Pe() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Pe; }
    uaiw_constexpr bool General_Category_Pi() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Pi; }
    uaiw_constexpr bool General_Category_Pf() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Pf; }
    uaiw_constexpr bool General_Category_Po() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Po; }

    uaiw_constexpr bool General_Category_Sm() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Sm; }
    uaiw_constexpr bool General_Category_Sc() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Sc; }
    uaiw_constexpr bool General_Category_Sk() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Sk; }
    uaiw_constexpr bool General_Category_So() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_So; }

    uaiw_constexpr bool General_Category_Zs() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Zs; }
    uaiw_constexpr bool General_Category_Zl() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Zl; }
    uaiw_constexpr bool General_Category_Zp() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Zp; }

    uaiw_constexpr bool General_Category_Cc() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Cc; }
    uaiw_constexpr bool General_Category_Cf() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Cf; }
    uaiw_constexpr bool General_Category_Cs() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Cs; }
    uaiw_constexpr bool General_Category_Co() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Co; }
    uaiw_constexpr bool General_Category_Cn() const noexcept { return detail::impl_prop_get_gc_prop(data) == detail::impl_General_Category_Cn; }

    uaiw_constexpr bool General_Category_LC() const noexcept
    {
        const detail::type_codept gc = detail::impl_prop_get_gc_prop(data);
        return gc >= detail::impl_General_Category_Lu && gc <= detail::impl_General_Category_Lt;
    }
    uaiw_constexpr bool General_Category_L() const noexcept
    {
        const detail::type_codept gc = detail::impl_prop_get_gc_prop(data);
        return gc >= detail::impl_General_Category_Lu && gc <= detail::impl_General_Category_Lo;
    }
    uaiw_constexpr bool General_Category_M() const noexcept
    {
        const detail::type_codept gc = detail::impl_prop_get_gc_prop(data);
        return gc >= detail::impl_General_Category_Mn && gc <= detail::impl_General_Category_Me;
    }
    uaiw_constexpr bool General_Category_N() const noexcept
    {
        const detail::type_codept gc = detail::impl_prop_get_gc_prop(data);
        return gc >= detail::impl_General_Category_Nd && gc <= detail::impl_General_Category_No;
    }
    uaiw_constexpr bool General_Category_P() const noexcept
    {
        const detail::type_codept gc = detail::impl_prop_get_gc_prop(data);
        return gc >= detail::impl_General_Category_Pc && gc <= detail::impl_General_Category_Po;
    }
    uaiw_constexpr bool General_Category_S() const noexcept
    {
        const detail::type_codept gc = detail::impl_prop_get_gc_prop(data);
        return gc >= detail::impl_General_Category_Sm && gc <= detail::impl_General_Category_So;
    }
    uaiw_constexpr bool General_Category_Z() const noexcept
    {
        const detail::type_codept gc = detail::impl_prop_get_gc_prop(data);
        return gc >= detail::impl_General_Category_Zs && gc <= detail::impl_General_Category_Zp;
    }
    uaiw_constexpr bool General_Category_C() const noexcept
    {
        const detail::type_codept gc = detail::impl_prop_get_gc_prop(data);
        return (gc >= detail::impl_General_Category_Cc && gc <= detail::impl_General_Category_Co) ||
                gc == detail::impl_General_Category_Cn;
    }

    // https://www.unicode.org/glossary/#code_point_type
    // Seven fundamental classes of code points in The Unicode Standard:
    // Graphic, Format, Control, Private-Use, Surrogate, Noncharacter, Reserved.

    uaiw_constexpr bool Noncharacter_Code_Point() const noexcept
    {
        // https://www.unicode.org/glossary/#noncharacter
        return detail::impl_prop_is_noncharacter_prop(data);
    }
    uaiw_constexpr bool Surrogate() const noexcept
    {
        // https://www.unicode.org/glossary/#surrogate_code_point
        return General_Category_Cs();
    }
    uaiw_constexpr bool Private_Use() const noexcept
    {
        // https://www.unicode.org/glossary/#private_use_code_point
        return General_Category_Co();
    }
    uaiw_constexpr bool Control() const noexcept
    {
        // https://www.unicode.org/glossary/#control_codes
        return General_Category_Cc();
    }
    uaiw_constexpr bool Graphic() const noexcept
    {
        // https://www.unicode.org/glossary/#graphic_character
        return detail::impl_prop_is_graphic_prop(data);
    }
    uaiw_constexpr bool Format() const noexcept
    {
        // https://www.unicode.org/glossary/#format_character
        return detail::impl_prop_is_format_prop(data);
    }
    uaiw_constexpr bool Reserved() const noexcept
    {
        // https://www.unicode.org/glossary/#reserved_code_point
        return detail::impl_prop_is_reserved_prop(data);
    }

    // Other properties

    uaiw_constexpr bool White_Space() const noexcept
    {
        // The Unicode Standard: PropList.txt -> White_Space
        return detail::impl_prop_is_white_space_prop(data);
    }
    uaiw_constexpr bool Alphabetic() const noexcept
    {
        // The Unicode Standard: DerivedCoreProperties.txt -> Alphabetic
        return detail::impl_prop_is_alphabetic_prop(data);
    }
    uaiw_constexpr bool Numeric() const noexcept
    {
        // Code points with General_Category=Number (Nd | Nl | No)
        return General_Category_N();
    }

#ifdef UNI_ALGO_EXPERIMENTAL
    // This might be usefull for tests
    //uaiw_constexpr unsigned char General_Category() const noexcept { return static_cast<unsigned char>(detail::impl_prop_get_gc_prop(data)); }
#endif
};

inline uaiw_constexpr general_category get_general_category(char32_t c) noexcept
{
    return static_cast<general_category>(detail::impl_prop_get_gc_prop(detail::impl_prop_get_prop(c)));
}

inline uaiw_constexpr general_category get_general_category(const prop& p) noexcept
{
    return static_cast<general_category>(detail::impl_prop_get_gc_prop(p.data));
}

inline uaiw_constexpr bool is_alphabetic(char32_t c) noexcept
{
    return prop{c}.Alphabetic();
}

inline uaiw_constexpr bool is_alphabetic(const prop& p) noexcept
{
    return p.Alphabetic();
}

inline uaiw_constexpr bool is_numeric(char32_t c) noexcept
{
    return prop{c}.Numeric();
}

inline uaiw_constexpr bool is_numeric(const prop& p) noexcept
{
    return p.Numeric();
}

inline uaiw_constexpr bool is_alphanumeric(char32_t c) noexcept
{
    const prop p{c};

    return p.Alphabetic() || p.Numeric();
}

inline uaiw_constexpr bool is_alphanumeric(const prop& p) noexcept
{
    return p.Alphabetic() || p.Numeric();
}

inline uaiw_constexpr bool is_whitespace(char32_t c) noexcept
{
    return prop{c}.White_Space();
}

inline uaiw_constexpr bool is_whitespace(const prop& p) noexcept
{
    return p.White_Space();
}

inline uaiw_constexpr bool is_reserved(char32_t c) noexcept
{
    // https://www.unicode.org/glossary/#reserved_code_point
    return prop{c}.Reserved();
}

inline uaiw_constexpr bool is_reserved(const prop& p) noexcept
{
    // https://www.unicode.org/glossary/#reserved_code_point
    return p.Reserved();
}

inline uaiw_constexpr bool is_valid(char32_t c) noexcept
{
    // https://www.unicode.org/glossary/#code_point
    return detail::impl_prop_is_valid(c);
}

inline uaiw_constexpr bool is_valid_scalar(char32_t c) noexcept
{
    // https://www.unicode.org/glossary/#unicode_scalar_value
    return detail::impl_prop_is_valid_scalar(c);
}

inline uaiw_constexpr bool is_supplementary(char32_t c) noexcept
{
    // https://www.unicode.org/glossary/#supplementary_code_point
    return detail::impl_prop_is_supplementary(c);
}

inline uaiw_constexpr bool is_noncharacter(char32_t c) noexcept
{
    // https://www.unicode.org/glossary/#noncharacter
    return detail::impl_prop_is_noncharacter(c);
}

inline uaiw_constexpr bool is_noncharacter(const prop& p) noexcept
{
    // https://www.unicode.org/glossary/#noncharacter
    return p.Noncharacter_Code_Point();
}

inline uaiw_constexpr bool is_surrogate(char32_t c) noexcept
{
    // https://www.unicode.org/glossary/#surrogate_code_point
    return detail::impl_prop_is_surrogate(c);
}

inline uaiw_constexpr bool is_surrogate(const prop& p) noexcept
{
    // https://www.unicode.org/glossary/#surrogate_code_point
    return p.Surrogate();
}

inline uaiw_constexpr bool is_private_use(char32_t c) noexcept
{
    // https://www.unicode.org/glossary/#private_use_code_point
    return detail::impl_prop_is_private_use(c);
}

inline uaiw_constexpr bool is_private_use(const prop& p) noexcept
{
    // https://www.unicode.org/glossary/#private_use_code_point
    return p.Private_Use();
}

inline uaiw_constexpr bool is_control(char32_t c) noexcept
{
    // https://www.unicode.org/glossary/#control_codes
    return detail::impl_prop_is_control(c);
}

inline uaiw_constexpr bool is_control(const prop& p) noexcept
{
    // https://www.unicode.org/glossary/#control_codes
    return p.Control();
}

} // namespace una::codepoint

#endif // UNI_ALGO_PROP_H_UAIH

/* Public Domain License:
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
 * MIT License:
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
