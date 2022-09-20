/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

void test_prop()
{
    std::size_t count_surrogate      = 0;
    std::size_t count_control        = 0;
    std::size_t count_noncharacter   = 0;
    std::size_t count_private_use    = 0;
    std::size_t count_supplementary  = 0;
    std::size_t count_invalid        = 0;
    std::size_t count_invalid_scalar = 0;

    for (char32_t c = 0; c <= 0x110002; ++c) // 0x110002 - 0x10FFFF == 3 means test 3 invalid code points
    {
        if (c == 0x110002)
            c = 0xFFFFFFFF;

        // Cross test functions that use Unicode data table and don't use it
        TESTX(uni::codepoint::is_surrogate(c)    == uni::codepoint::prop{c}.Surrogate());
        TESTX(uni::codepoint::is_private_use(c)  == uni::codepoint::prop{c}.Private_Use());
        TESTX(uni::codepoint::is_control(c)      == uni::codepoint::prop{c}.Control());
        TESTX(uni::codepoint::is_noncharacter(c) == uni::codepoint::prop{c}.Noncharacter_Code_Point());
        TESTX(uni::codepoint::is_reserved(c)     == uni::codepoint::prop{c}.Reserved());

        TESTX(uni::codepoint::is_surrogate(c)    == uni::codepoint::is_surrogate(uni::codepoint::prop{c}));
        TESTX(uni::codepoint::is_private_use(c)  == uni::codepoint::is_private_use(uni::codepoint::prop{c}));
        TESTX(uni::codepoint::is_control(c)      == uni::codepoint::is_control(uni::codepoint::prop{c}));
        TESTX(uni::codepoint::is_noncharacter(c) == uni::codepoint::is_noncharacter(uni::codepoint::prop{c}));
        TESTX(uni::codepoint::is_reserved(c)     == uni::codepoint::is_reserved(uni::codepoint::prop{c}));

        // Count stable code points
        // https://www.unicode.org/policies/stability_policy.html#Property_Value
        // X property is an immutable code point property,
        // which means that its property values for all Unicode code points will never change.
        if (uni::codepoint::is_surrogate(c))     count_surrogate++;
        if (uni::codepoint::is_private_use(c))   count_private_use++;
        if (uni::codepoint::is_control(c))       count_control++;
        if (uni::codepoint::is_noncharacter(c))  count_noncharacter++;
        if (uni::codepoint::is_supplementary(c)) count_supplementary++;
        if (!uni::codepoint::is_valid(c))        count_invalid++;
        if (!uni::codepoint::is_valid_scalar(c)) count_invalid_scalar++;

        // Other functions
        TESTX(uni::codepoint::is_alphabetic(c) == uni::codepoint::prop{c}.Alphabetic());
        TESTX(uni::codepoint::is_numeric(c)    == uni::codepoint::prop{c}.Numeric());
        TESTX(uni::codepoint::is_whitespace(c) == uni::codepoint::prop{c}.White_Space());

        TESTX(uni::codepoint::prop{c}.Surrogate()   == uni::codepoint::prop{c}.General_Category_Cs());
        TESTX(uni::codepoint::prop{c}.Private_Use() == uni::codepoint::prop{c}.General_Category_Co());
        TESTX(uni::codepoint::prop{c}.Control()     == uni::codepoint::prop{c}.General_Category_Cc());

        if (uni::codepoint::prop{c}.Alphabetic() ||
            uni::codepoint::prop{c}.Numeric())
            TESTX(uni::codepoint::is_alphanumeric(c));
        else
            TESTX(!uni::codepoint::is_alphanumeric(c));

        if (uni::codepoint::prop{c}.General_Category_Zl() ||
            uni::codepoint::prop{c}.General_Category_Zp() ||
            uni::codepoint::prop{c}.General_Category_Cf())
            TESTX(uni::codepoint::prop{c}.Format());
        else
            TESTX(!uni::codepoint::prop{c}.Format());

        if (uni::codepoint::prop{c}.General_Category_Cn() &&
            !uni::codepoint::prop{c}.Noncharacter_Code_Point())
            TESTX(uni::codepoint::prop{c}.Reserved());
        else
            TESTX(!uni::codepoint::prop{c}.Reserved());

        if (uni::codepoint::prop{c}.General_Category_Zl() ||
            uni::codepoint::prop{c}.General_Category_Zp() ||
            uni::codepoint::prop{c}.General_Category_Cc() ||
            uni::codepoint::prop{c}.General_Category_Cf() ||
            uni::codepoint::prop{c}.General_Category_Cs() ||
            uni::codepoint::prop{c}.General_Category_Co() ||
            uni::codepoint::prop{c}.General_Category_Cn())
            TESTX(!uni::codepoint::prop{c}.Graphic());
        else
            TESTX(uni::codepoint::prop{c}.Graphic());

        if (uni::codepoint::is_valid(c) && !uni::codepoint::is_surrogate(c))
            TESTX(uni::codepoint::is_valid_scalar(c));
        else
            TESTX(!uni::codepoint::is_valid_scalar(c));

        if (c > 0xFFFF && c <= 0x10FFFF)
            TESTX(uni::codepoint::is_supplementary(c));
        else
            TESTX(!uni::codepoint::is_supplementary(c));

        if (c >= 0xD800 && c <= 0xDFFF)
            TESTX(uni::codepoint::is_surrogate(c));
        else
            TESTX(!uni::codepoint::is_surrogate(c));

        if (c == 0xFFFFFFFF)
            break;
    }

    // Must be the same in all Unicode versions
    // https://en.wikipedia.org/wiki/Unicode_character_property#General_Category
    // https://en.wikipedia.org/w/index.php?title=Unicode_character_property&oldid=1066524639#General_Category
    TESTX(count_surrogate      == 2048);
    TESTX(count_private_use    == 137468);
    TESTX(count_control        == 65);
    TESTX(count_noncharacter   == 66);
    TESTX(count_supplementary  == 0x100000);

    TESTX(count_invalid        == 3);
    TESTX(count_invalid_scalar == count_surrogate + count_invalid);

    static_assert(uni::codepoint::max_value == 0x10FFFF);
    static_assert(uni::codepoint::total_number == 0x110000);
    static_assert(uni::codepoint::replacement_char == 0xFFFD);

    TESTX(uni::codepoint::is_valid(0));
    TESTX(uni::codepoint::is_valid(0xD800));
    TESTX(uni::codepoint::is_valid(0xDBFF));
    TESTX(uni::codepoint::is_valid(0xDC00));
    TESTX(uni::codepoint::is_valid(0xDFFF));
    TESTX(uni::codepoint::is_valid(0xFFFD));
    TESTX(uni::codepoint::is_valid(0xFFFE));
    TESTX(uni::codepoint::is_valid(0xFFFF));
    TESTX(uni::codepoint::is_valid(0x10FFFF));
    TESTX(!uni::codepoint::is_valid(0x110000));
    TESTX(!uni::codepoint::is_valid(0xFFFFFFFF));

    TESTX(uni::codepoint::is_valid_scalar(0));
    TESTX(!uni::codepoint::is_valid_scalar(0xD800));
    TESTX(!uni::codepoint::is_valid_scalar(0xDBFF));
    TESTX(!uni::codepoint::is_valid_scalar(0xDC00));
    TESTX(!uni::codepoint::is_valid_scalar(0xDFFF));
    TESTX(uni::codepoint::is_valid_scalar(0xFFFD));
    TESTX(uni::codepoint::is_valid_scalar(0xFFFE));
    TESTX(uni::codepoint::is_valid_scalar(0xFFFF));
    TESTX(uni::codepoint::is_valid_scalar(0x10FFFF));
    TESTX(!uni::codepoint::is_valid_scalar(0x110000));
    TESTX(!uni::codepoint::is_valid_scalar(0xFFFFFFFF));

    TESTX(uni::codepoint::is_alphabetic(U'Q'));
    TESTX(!uni::codepoint::is_alphabetic(U'1'));
    TESTX(!uni::codepoint::is_alphabetic(0));
    TESTX(!uni::codepoint::is_alphabetic(0x110000));
    TESTX(!uni::codepoint::is_alphabetic(0xFFFFFFFF));

    TESTX(uni::codepoint::is_alphabetic(uni::codepoint::prop{U'Q'}));
    TESTX(!uni::codepoint::is_alphabetic(uni::codepoint::prop{U'1'}));
    TESTX(!uni::codepoint::is_alphabetic(uni::codepoint::prop{0}));
    TESTX(!uni::codepoint::is_alphabetic(uni::codepoint::prop{0x110000}));
    TESTX(!uni::codepoint::is_alphabetic(uni::codepoint::prop{0xFFFFFFFF}));

    TESTX(!uni::codepoint::is_numeric(U'R'));
    TESTX(uni::codepoint::is_numeric(U'2'));
    TESTX(!uni::codepoint::is_numeric(0));
    TESTX(!uni::codepoint::is_numeric(0x110000));
    TESTX(!uni::codepoint::is_numeric(0xFFFFFFFF));

    TESTX(!uni::codepoint::is_numeric(uni::codepoint::prop{U'R'}));
    TESTX(uni::codepoint::is_numeric(uni::codepoint::prop{U'2'}));
    TESTX(!uni::codepoint::is_numeric(uni::codepoint::prop{0}));
    TESTX(!uni::codepoint::is_numeric(uni::codepoint::prop{0x110000}));
    TESTX(!uni::codepoint::is_numeric(uni::codepoint::prop{0xFFFFFFFF}));

    TESTX(uni::codepoint::is_alphanumeric(U'S'));
    TESTX(uni::codepoint::is_alphanumeric(U'3'));
    TESTX(!uni::codepoint::is_alphanumeric(0));
    TESTX(!uni::codepoint::is_alphanumeric(0x110000));
    TESTX(!uni::codepoint::is_alphanumeric(0xFFFFFFFF));

    TESTX(uni::codepoint::is_alphanumeric(uni::codepoint::prop{U'S'}));
    TESTX(uni::codepoint::is_alphanumeric(uni::codepoint::prop{U'3'}));
    TESTX(!uni::codepoint::is_alphanumeric(uni::codepoint::prop{0}));
    TESTX(!uni::codepoint::is_alphanumeric(uni::codepoint::prop{0x110000}));
    TESTX(!uni::codepoint::is_alphanumeric(uni::codepoint::prop{0xFFFFFFFF}));

    TESTX(uni::codepoint::is_whitespace(U' '));
    TESTX(uni::codepoint::is_whitespace(0x3000));
    TESTX(!uni::codepoint::is_whitespace(0));
    TESTX(!uni::codepoint::is_whitespace(0x110000));
    TESTX(!uni::codepoint::is_whitespace(0xFFFFFFFF));

    TESTX(uni::codepoint::is_whitespace(uni::codepoint::prop{U' '}));
    TESTX(uni::codepoint::is_whitespace(uni::codepoint::prop{0x3000}));
    TESTX(!uni::codepoint::is_whitespace(uni::codepoint::prop{0}));
    TESTX(!uni::codepoint::is_whitespace(uni::codepoint::prop{0x110000}));
    TESTX(!uni::codepoint::is_whitespace(uni::codepoint::prop{0xFFFFFFFF}));

    TESTX(uni::codepoint::is_control(0));
    TESTX(uni::codepoint::is_control(0x1F));
    TESTX(!uni::codepoint::is_control(0x20));
    TESTX(uni::codepoint::is_control(0x7F));
    TESTX(uni::codepoint::is_control(0x9F));
    TESTX(!uni::codepoint::is_control(0xFFFF));
    TESTX(!uni::codepoint::is_control(0x10FFFF));
    TESTX(!uni::codepoint::is_control(0x110000));
    TESTX(!uni::codepoint::is_control(0xFFFFFFFF));

    TESTX(uni::codepoint::is_control(uni::codepoint::prop{0}));
    TESTX(uni::codepoint::is_control(uni::codepoint::prop{0x1F}));
    TESTX(!uni::codepoint::is_control(uni::codepoint::prop{0x20}));
    TESTX(uni::codepoint::is_control(uni::codepoint::prop{0x7F}));
    TESTX(uni::codepoint::is_control(uni::codepoint::prop{0x9F}));
    TESTX(!uni::codepoint::is_control(uni::codepoint::prop{0xFFFF}));
    TESTX(!uni::codepoint::is_control(uni::codepoint::prop{0x10FFFF}));
    TESTX(!uni::codepoint::is_control(uni::codepoint::prop{0x110000}));
    TESTX(!uni::codepoint::is_control(uni::codepoint::prop{0xFFFFFFFF}));

    TESTX(!uni::codepoint::is_noncharacter(0));
    TESTX(!uni::codepoint::is_noncharacter(0xFFFD));
    TESTX(uni::codepoint::is_noncharacter(0xFFFE));
    TESTX(uni::codepoint::is_noncharacter(0xFFFF));
    TESTX(uni::codepoint::is_noncharacter(0x10FFFE));
    TESTX(uni::codepoint::is_noncharacter(0x10FFFF));
    TESTX(!uni::codepoint::is_noncharacter(0x110000));
    TESTX(!uni::codepoint::is_noncharacter(0xFFFFFFFF));

    TESTX(!uni::codepoint::is_noncharacter(uni::codepoint::prop{0}));
    TESTX(!uni::codepoint::is_noncharacter(uni::codepoint::prop{0xFFFD}));
    TESTX(uni::codepoint::is_noncharacter(uni::codepoint::prop{0xFFFE}));
    TESTX(uni::codepoint::is_noncharacter(uni::codepoint::prop{0xFFFF}));
    TESTX(uni::codepoint::is_noncharacter(uni::codepoint::prop{0x10FFFE}));
    TESTX(uni::codepoint::is_noncharacter(uni::codepoint::prop{0x10FFFF}));
    TESTX(!uni::codepoint::is_noncharacter(uni::codepoint::prop{0x110000}));
    TESTX(!uni::codepoint::is_noncharacter(uni::codepoint::prop{0xFFFFFFFF}));

    TESTX(!uni::codepoint::is_private_use(0));
    TESTX(uni::codepoint::is_private_use(0xE000));
    TESTX(uni::codepoint::is_private_use(0xF8FF));
    TESTX(!uni::codepoint::is_private_use(0xFFFF));
    TESTX(uni::codepoint::is_private_use(0xF0000));
    TESTX(uni::codepoint::is_private_use(0xFFFFD));
    TESTX(uni::codepoint::is_private_use(0x100000));
    TESTX(uni::codepoint::is_private_use(0x10FFFD));
    TESTX(!uni::codepoint::is_private_use(0x10FFFF));
    TESTX(!uni::codepoint::is_private_use(0x110000));
    TESTX(!uni::codepoint::is_private_use(0xFFFFFFFF));

    TESTX(!uni::codepoint::is_private_use(uni::codepoint::prop{0}));
    TESTX(uni::codepoint::is_private_use(uni::codepoint::prop{0xE000}));
    TESTX(uni::codepoint::is_private_use(uni::codepoint::prop{0xF8FF}));
    TESTX(!uni::codepoint::is_private_use(uni::codepoint::prop{0xFFFF}));
    TESTX(uni::codepoint::is_private_use(uni::codepoint::prop{0xF0000}));
    TESTX(uni::codepoint::is_private_use(uni::codepoint::prop{0xFFFFD}));
    TESTX(uni::codepoint::is_private_use(uni::codepoint::prop{0x100000}));
    TESTX(uni::codepoint::is_private_use(uni::codepoint::prop{0x10FFFD}));
    TESTX(!uni::codepoint::is_private_use(uni::codepoint::prop{0x10FFFF}));
    TESTX(!uni::codepoint::is_private_use(uni::codepoint::prop{0x110000}));
    TESTX(!uni::codepoint::is_private_use(uni::codepoint::prop{0xFFFFFFFF}));

    TESTX(!uni::codepoint::is_surrogate(0));
    TESTX(!uni::codepoint::is_surrogate(0xFFFF));
    TESTX(uni::codepoint::is_surrogate(0xD800));
    TESTX(uni::codepoint::is_surrogate(0xDBFF));
    TESTX(uni::codepoint::is_surrogate(0xDC00));
    TESTX(uni::codepoint::is_surrogate(0xDFFF));
    TESTX(!uni::codepoint::is_surrogate(0x10FFFF));
    TESTX(!uni::codepoint::is_surrogate(0x110000));
    TESTX(!uni::codepoint::is_surrogate(0xFFFFFFFF));

    TESTX(!uni::codepoint::is_surrogate(uni::codepoint::prop{0}));
    TESTX(!uni::codepoint::is_surrogate(uni::codepoint::prop{0xFFFF}));
    TESTX(uni::codepoint::is_surrogate(uni::codepoint::prop{0xD800}));
    TESTX(uni::codepoint::is_surrogate(uni::codepoint::prop{0xDBFF}));
    TESTX(uni::codepoint::is_surrogate(uni::codepoint::prop{0xDC00}));
    TESTX(uni::codepoint::is_surrogate(uni::codepoint::prop{0xDFFF}));
    TESTX(!uni::codepoint::is_surrogate(uni::codepoint::prop{0x10FFFF}));
    TESTX(!uni::codepoint::is_surrogate(uni::codepoint::prop{0x110000}));
    TESTX(!uni::codepoint::is_surrogate(uni::codepoint::prop{0xFFFFFFFF}));

    TESTX(!uni::codepoint::is_supplementary(0));
    TESTX(!uni::codepoint::is_supplementary(0xFFFF));
    TESTX(uni::codepoint::is_supplementary(0x10000));
    TESTX(uni::codepoint::is_supplementary(0x10FFFF));
    TESTX(!uni::codepoint::is_supplementary(0x110000));
    TESTX(!uni::codepoint::is_supplementary(0xFFFFFFFF));

    // First code point from UnicodeData.txt with such General_Category

    char32_t unassigned = 0x0A7777; // Random unassigned code point (change to something else if it will be assigned)

    TESTX(uni::codepoint::prop{0x0041}.General_Category_Lu());
    TESTX(uni::codepoint::prop{0x0061}.General_Category_Ll());
    TESTX(uni::codepoint::prop{0x01C5}.General_Category_Lt());
    TESTX(uni::codepoint::prop{0x02B0}.General_Category_Lm());
    TESTX(uni::codepoint::prop{0x00AA}.General_Category_Lo());
    TESTX(uni::codepoint::prop{0x0300}.General_Category_Mn());
    TESTX(uni::codepoint::prop{0x0903}.General_Category_Mc());
    TESTX(uni::codepoint::prop{0x0488}.General_Category_Me());
    TESTX(uni::codepoint::prop{0x0030}.General_Category_Nd());
    TESTX(uni::codepoint::prop{0x16EE}.General_Category_Nl());
    TESTX(uni::codepoint::prop{0x00B2}.General_Category_No());
    TESTX(uni::codepoint::prop{0x005F}.General_Category_Pc());
    TESTX(uni::codepoint::prop{0x002D}.General_Category_Pd());
    TESTX(uni::codepoint::prop{0x0028}.General_Category_Ps());
    TESTX(uni::codepoint::prop{0x0029}.General_Category_Pe());
    TESTX(uni::codepoint::prop{0x00AB}.General_Category_Pi());
    TESTX(uni::codepoint::prop{0x00BB}.General_Category_Pf());
    TESTX(uni::codepoint::prop{0x0021}.General_Category_Po());
    TESTX(uni::codepoint::prop{0x002B}.General_Category_Sm());
    TESTX(uni::codepoint::prop{0x0024}.General_Category_Sc());
    TESTX(uni::codepoint::prop{0x005E}.General_Category_Sk());
    TESTX(uni::codepoint::prop{0x00A6}.General_Category_So());
    TESTX(uni::codepoint::prop{0x0020}.General_Category_Zs());
    TESTX(uni::codepoint::prop{0x2028}.General_Category_Zl());
    TESTX(uni::codepoint::prop{0x2029}.General_Category_Zp());
    TESTX(uni::codepoint::prop{0x0000}.General_Category_Cc());
    TESTX(uni::codepoint::prop{0x00AD}.General_Category_Cf());
    TESTX(uni::codepoint::prop{0xD800}.General_Category_Cs());
    TESTX(uni::codepoint::prop{0xE000}.General_Category_Co());
    TESTX(uni::codepoint::prop{unassigned}.General_Category_Cn());

    TESTX(uni::codepoint::prop{0x0041}.General_Category_LC());
    TESTX(uni::codepoint::prop{0x0061}.General_Category_LC());
    TESTX(uni::codepoint::prop{0x01C5}.General_Category_LC());
    TESTX(uni::codepoint::prop{0x0041}.General_Category_L());
    TESTX(uni::codepoint::prop{0x0061}.General_Category_L());
    TESTX(uni::codepoint::prop{0x01C5}.General_Category_L());
    TESTX(uni::codepoint::prop{0x02B0}.General_Category_L());
    TESTX(uni::codepoint::prop{0x00AA}.General_Category_L());
    TESTX(uni::codepoint::prop{0x0300}.General_Category_M());
    TESTX(uni::codepoint::prop{0x0903}.General_Category_M());
    TESTX(uni::codepoint::prop{0x0488}.General_Category_M());
    TESTX(uni::codepoint::prop{0x0030}.General_Category_N());
    TESTX(uni::codepoint::prop{0x16EE}.General_Category_N());
    TESTX(uni::codepoint::prop{0x00B2}.General_Category_N());
    TESTX(uni::codepoint::prop{0x005F}.General_Category_P());
    TESTX(uni::codepoint::prop{0x002D}.General_Category_P());
    TESTX(uni::codepoint::prop{0x0028}.General_Category_P());
    TESTX(uni::codepoint::prop{0x0029}.General_Category_P());
    TESTX(uni::codepoint::prop{0x00AB}.General_Category_P());
    TESTX(uni::codepoint::prop{0x00BB}.General_Category_P());
    TESTX(uni::codepoint::prop{0x0021}.General_Category_P());
    TESTX(uni::codepoint::prop{0x002B}.General_Category_S());
    TESTX(uni::codepoint::prop{0x0024}.General_Category_S());
    TESTX(uni::codepoint::prop{0x005E}.General_Category_S());
    TESTX(uni::codepoint::prop{0x00A6}.General_Category_S());
    TESTX(uni::codepoint::prop{0x0020}.General_Category_Z());
    TESTX(uni::codepoint::prop{0x2028}.General_Category_Z());
    TESTX(uni::codepoint::prop{0x2029}.General_Category_Z());
    TESTX(uni::codepoint::prop{0x0000}.General_Category_C());
    TESTX(uni::codepoint::prop{0x00AD}.General_Category_C());
    TESTX(uni::codepoint::prop{0xD800}.General_Category_C());
    TESTX(uni::codepoint::prop{0xE000}.General_Category_C());
    TESTX(uni::codepoint::prop{unassigned}.General_Category_C());

    // Why did you do that? Change them back. They must be stable.
    static_assert(uni::detail::impl_General_Category_Lu == 1);
    static_assert(uni::detail::impl_General_Category_Ll == 2);
    static_assert(uni::detail::impl_General_Category_Lt == 3);
    static_assert(uni::detail::impl_General_Category_Lm == 4);
    static_assert(uni::detail::impl_General_Category_Lo == 5);
    static_assert(uni::detail::impl_General_Category_Mn == 6);
    static_assert(uni::detail::impl_General_Category_Mc == 7);
    static_assert(uni::detail::impl_General_Category_Me == 8);
    static_assert(uni::detail::impl_General_Category_Nd == 9);
    static_assert(uni::detail::impl_General_Category_Nl == 10);
    static_assert(uni::detail::impl_General_Category_No == 11);
    static_assert(uni::detail::impl_General_Category_Pc == 12);
    static_assert(uni::detail::impl_General_Category_Pd == 13);
    static_assert(uni::detail::impl_General_Category_Ps == 14);
    static_assert(uni::detail::impl_General_Category_Pe == 15);
    static_assert(uni::detail::impl_General_Category_Pi == 16);
    static_assert(uni::detail::impl_General_Category_Pf == 17);
    static_assert(uni::detail::impl_General_Category_Po == 18);
    static_assert(uni::detail::impl_General_Category_Sm == 19);
    static_assert(uni::detail::impl_General_Category_Sc == 20);
    static_assert(uni::detail::impl_General_Category_Sk == 21);
    static_assert(uni::detail::impl_General_Category_So == 22);
    static_assert(uni::detail::impl_General_Category_Zs == 23);
    static_assert(uni::detail::impl_General_Category_Zl == 24);
    static_assert(uni::detail::impl_General_Category_Zp == 25);
    static_assert(uni::detail::impl_General_Category_Cc == 26);
    static_assert(uni::detail::impl_General_Category_Cf == 27);
    static_assert(uni::detail::impl_General_Category_Cs == 28);
    static_assert(uni::detail::impl_General_Category_Co == 29);
    static_assert(uni::detail::impl_General_Category_Cn == 0);

    using gc = uni::codepoint::general_category;

    static_assert(uni::detail::impl_General_Category_Lu == static_cast<unsigned char>(gc::Lu));
    static_assert(uni::detail::impl_General_Category_Ll == static_cast<unsigned char>(gc::Ll));
    static_assert(uni::detail::impl_General_Category_Lt == static_cast<unsigned char>(gc::Lt));
    static_assert(uni::detail::impl_General_Category_Lm == static_cast<unsigned char>(gc::Lm));
    static_assert(uni::detail::impl_General_Category_Lo == static_cast<unsigned char>(gc::Lo));
    static_assert(uni::detail::impl_General_Category_Mn == static_cast<unsigned char>(gc::Mn));
    static_assert(uni::detail::impl_General_Category_Mc == static_cast<unsigned char>(gc::Mc));
    static_assert(uni::detail::impl_General_Category_Me == static_cast<unsigned char>(gc::Me));
    static_assert(uni::detail::impl_General_Category_Nd == static_cast<unsigned char>(gc::Nd));
    static_assert(uni::detail::impl_General_Category_Nl == static_cast<unsigned char>(gc::Nl));
    static_assert(uni::detail::impl_General_Category_No == static_cast<unsigned char>(gc::No));
    static_assert(uni::detail::impl_General_Category_Pc == static_cast<unsigned char>(gc::Pc));
    static_assert(uni::detail::impl_General_Category_Pd == static_cast<unsigned char>(gc::Pd));
    static_assert(uni::detail::impl_General_Category_Ps == static_cast<unsigned char>(gc::Ps));
    static_assert(uni::detail::impl_General_Category_Pe == static_cast<unsigned char>(gc::Pe));
    static_assert(uni::detail::impl_General_Category_Pi == static_cast<unsigned char>(gc::Pi));
    static_assert(uni::detail::impl_General_Category_Pf == static_cast<unsigned char>(gc::Pf));
    static_assert(uni::detail::impl_General_Category_Po == static_cast<unsigned char>(gc::Po));
    static_assert(uni::detail::impl_General_Category_Sm == static_cast<unsigned char>(gc::Sm));
    static_assert(uni::detail::impl_General_Category_Sc == static_cast<unsigned char>(gc::Sc));
    static_assert(uni::detail::impl_General_Category_Sk == static_cast<unsigned char>(gc::Sk));
    static_assert(uni::detail::impl_General_Category_So == static_cast<unsigned char>(gc::So));
    static_assert(uni::detail::impl_General_Category_Zs == static_cast<unsigned char>(gc::Zs));
    static_assert(uni::detail::impl_General_Category_Zl == static_cast<unsigned char>(gc::Zl));
    static_assert(uni::detail::impl_General_Category_Zp == static_cast<unsigned char>(gc::Zp));
    static_assert(uni::detail::impl_General_Category_Cc == static_cast<unsigned char>(gc::Cc));
    static_assert(uni::detail::impl_General_Category_Cf == static_cast<unsigned char>(gc::Cf));
    static_assert(uni::detail::impl_General_Category_Cs == static_cast<unsigned char>(gc::Cs));
    static_assert(uni::detail::impl_General_Category_Co == static_cast<unsigned char>(gc::Co));
    static_assert(uni::detail::impl_General_Category_Cn == static_cast<unsigned char>(gc::Cn));
}

void test_prop_case()
{
    TESTX(uni::codepoint::is_lowercase(U'w'));
    TESTX(!uni::codepoint::is_lowercase(U'W'));
    TESTX(uni::codepoint::is_uppercase(U'W'));
    TESTX(!uni::codepoint::is_uppercase(U'w'));

    TESTX(uni::codepoint::is_lowercase(uni::codepoint::prop_case{U'w'}));
    TESTX(!uni::codepoint::is_lowercase(uni::codepoint::prop_case{U'W'}));
    TESTX(uni::codepoint::is_uppercase(uni::codepoint::prop_case{U'W'}));
    TESTX(!uni::codepoint::is_uppercase(uni::codepoint::prop_case{U'w'}));

    TESTX(uni::codepoint::prop_case{U'w'}.Lowercase());
    TESTX(!uni::codepoint::prop_case{U'W'}.Lowercase());
    TESTX(uni::codepoint::prop_case{U'W'}.Uppercase());
    TESTX(!uni::codepoint::prop_case{U'w'}.Uppercase());

    TESTX(uni::codepoint::prop_case{U'w'}.Cased());
    TESTX(uni::codepoint::prop_case{U'W'}.Cased());
    TESTX(!uni::codepoint::prop_case{U':'}.Cased());
    TESTX(uni::codepoint::prop_case{U':'}.Case_Ignorable());
    TESTX(!uni::codepoint::prop_case{U';'}.Case_Ignorable());

    TESTX(uni::codepoint::prop_case{U'j'}.Soft_Dotted());
    TESTX(!uni::codepoint::prop_case{U'J'}.Soft_Dotted());

    TESTX(!uni::codepoint::is_lowercase(0));
    TESTX(!uni::codepoint::is_uppercase(0));
    TESTX(!uni::codepoint::is_lowercase(0xFFFD));
    TESTX(!uni::codepoint::is_uppercase(0xFFFD));
    TESTX(!uni::codepoint::is_lowercase(0x10FFFF));
    TESTX(!uni::codepoint::is_uppercase(0x10FFFF));
    TESTX(!uni::codepoint::is_lowercase(0x110000));
    TESTX(!uni::codepoint::is_uppercase(0x110000));
    TESTX(!uni::codepoint::is_lowercase(0xFFFFFFFF));
    TESTX(!uni::codepoint::is_uppercase(0xFFFFFFFF));

    TESTX(uni::codepoint::to_simple_lowercase(U'W') == U'w');
    TESTX(uni::codepoint::to_simple_uppercase(U'w') == U'W');
    TESTX(uni::codepoint::to_simple_titlecase(U'w') == U'W');
    TESTX(uni::codepoint::to_simple_casefold(U'W') == U'w');

    TESTX(uni::codepoint::to_simple_lowercase(U':') == U':');
    TESTX(uni::codepoint::to_simple_uppercase(U':') == U':');
    TESTX(uni::codepoint::to_simple_titlecase(U':') == U':');
    TESTX(uni::codepoint::to_simple_casefold(U':') == U':');

    TESTX(uni::codepoint::to_simple_lowercase(0) == 0);
    TESTX(uni::codepoint::to_simple_uppercase(0) == 0);
    TESTX(uni::codepoint::to_simple_titlecase(0) == 0);
    TESTX(uni::codepoint::to_simple_casefold(0) == 0);

    TESTX(uni::codepoint::to_simple_lowercase(0x10FFFF) == 0x10FFFF);
    TESTX(uni::codepoint::to_simple_uppercase(0x10FFFF) == 0x10FFFF);
    TESTX(uni::codepoint::to_simple_titlecase(0x10FFFF) == 0x10FFFF);
    TESTX(uni::codepoint::to_simple_casefold(0x10FFFF) == 0x10FFFF);

    TESTX(uni::codepoint::to_simple_lowercase(0x110000) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_uppercase(0x110000) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_titlecase(0x110000) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_casefold(0x110000) == 0xFFFD);

    TESTX(uni::codepoint::to_simple_lowercase(0xFFFFFFFF) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_uppercase(0xFFFFFFFF) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_titlecase(0xFFFFFFFF) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_casefold(0xFFFFFFFF) == 0xFFFD);

    // Surrogates
    TESTX(uni::codepoint::to_simple_lowercase(0xD800) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_lowercase(0xDBFF) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_lowercase(0xDC00) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_lowercase(0xDFFF) == 0xFFFD);

    TESTX(uni::codepoint::to_simple_uppercase(0xD800) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_uppercase(0xDBFF) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_uppercase(0xDC00) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_uppercase(0xDFFF) == 0xFFFD);

    TESTX(uni::codepoint::to_simple_titlecase(0xD800) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_titlecase(0xDBFF) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_titlecase(0xDC00) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_titlecase(0xDFFF) == 0xFFFD);

    TESTX(uni::codepoint::to_simple_casefold(0xD800) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_casefold(0xDBFF) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_casefold(0xDC00) == 0xFFFD);
    TESTX(uni::codepoint::to_simple_casefold(0xDFFF) == 0xFFFD);

    // Eszett
    TESTX(uni::codepoint::is_lowercase(0x00DF));
    TESTX(uni::codepoint::is_uppercase(0x1E9E));
    TESTX(uni::codepoint::is_lowercase(uni::codepoint::prop_case{0x00DF}));
    TESTX(uni::codepoint::is_uppercase(uni::codepoint::prop_case{0x1E9E}));
    TESTX(uni::codepoint::prop_case{0x00DF}.Lowercase());
    TESTX(uni::codepoint::prop_case{0x1E9E}.Uppercase());

    TESTX(uni::codepoint::to_simple_lowercase(0x00DF) == 0x00DF);
    TESTX(uni::codepoint::to_simple_uppercase(0x00DF) == 0x00DF);
    TESTX(uni::codepoint::to_simple_titlecase(0x00DF) == 0x00DF);
    TESTX(uni::codepoint::to_simple_casefold(0x00DF) == 0x00DF);

    TESTX(uni::codepoint::to_simple_lowercase(0x1E9E) == 0x00DF);
    TESTX(uni::codepoint::to_simple_uppercase(0x1E9E) == 0x1E9E);
    TESTX(uni::codepoint::to_simple_titlecase(0x1E9E) == 0x1E9E);
    TESTX(uni::codepoint::to_simple_casefold(0x1E9E) == 0x00DF);

#ifndef UNI_ALGO_DISABLE_FULL_CASE
    TESTX(uni::codepoint::to_lowercase_u32(0x0130) == U"\x0069\x0307");
    TESTX(uni::codepoint::to_uppercase_u32(0x00DF) == U"\x0053\x0053");
    TESTX(uni::codepoint::to_titlecase_u32(0x00DF) == U"\x0053\x0073");
    TESTX(uni::codepoint::to_casefold_u32(0x00DF) == U"\x0073\x0073");

    TESTX(uni::codepoint::to_lowercase_u32(0xFB03) == U"\xFB03");
    TESTX(uni::codepoint::to_uppercase_u32(0xFB03) == U"\x0046\x0046\x0049");
    TESTX(uni::codepoint::to_titlecase_u32(0xFB03) == U"\x0046\x0066\x0069");
    TESTX(uni::codepoint::to_casefold_u32(0xFB03) == U"\x0066\x0066\x0069");

    TESTX(uni::codepoint::to_lowercase_u32(U'W') == U"w");
    TESTX(uni::codepoint::to_uppercase_u32(U'w') == U"W");
    TESTX(uni::codepoint::to_titlecase_u32(U'w') == U"W");
    TESTX(uni::codepoint::to_casefold_u32(U'W') == U"w");

    TESTX(uni::codepoint::to_lowercase_u32(U':') == U":");
    TESTX(uni::codepoint::to_uppercase_u32(U':') == U":");
    TESTX(uni::codepoint::to_titlecase_u32(U':') == U":");
    TESTX(uni::codepoint::to_casefold_u32(U':') == U":");

    TESTX(uni::codepoint::to_lowercase_u32(0) == std::u32string{0});
    TESTX(uni::codepoint::to_uppercase_u32(0) == std::u32string{0});
    TESTX(uni::codepoint::to_titlecase_u32(0) == std::u32string{0});
    TESTX(uni::codepoint::to_casefold_u32(0) == std::u32string{0});

    TESTX(uni::codepoint::to_lowercase_u32(0x10FFFF) == U"\x10FFFF");
    TESTX(uni::codepoint::to_uppercase_u32(0x10FFFF) == U"\x10FFFF");
    TESTX(uni::codepoint::to_titlecase_u32(0x10FFFF) == U"\x10FFFF");
    TESTX(uni::codepoint::to_casefold_u32(0x10FFFF) == U"\x10FFFF");

    TESTX(uni::codepoint::to_lowercase_u32(0x110000) == U"\xFFFD");
    TESTX(uni::codepoint::to_uppercase_u32(0x110000) == U"\xFFFD");
    TESTX(uni::codepoint::to_titlecase_u32(0x110000) == U"\xFFFD");
    TESTX(uni::codepoint::to_casefold_u32(0x110000) == U"\xFFFD");

    TESTX(uni::codepoint::to_lowercase_u32(0xFFFFFFFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_uppercase_u32(0xFFFFFFFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_titlecase_u32(0xFFFFFFFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_casefold_u32(0xFFFFFFFF) == U"\xFFFD");

    // Surrogates
    TESTX(uni::codepoint::to_lowercase_u32(0xD800) == U"\xFFFD");
    TESTX(uni::codepoint::to_lowercase_u32(0xDBFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_lowercase_u32(0xDC00) == U"\xFFFD");
    TESTX(uni::codepoint::to_lowercase_u32(0xDFFF) == U"\xFFFD");

    TESTX(uni::codepoint::to_uppercase_u32(0xD800) == U"\xFFFD");
    TESTX(uni::codepoint::to_uppercase_u32(0xDBFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_uppercase_u32(0xDC00) == U"\xFFFD");
    TESTX(uni::codepoint::to_uppercase_u32(0xDFFF) == U"\xFFFD");

    TESTX(uni::codepoint::to_titlecase_u32(0xD800) == U"\xFFFD");
    TESTX(uni::codepoint::to_titlecase_u32(0xDBFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_titlecase_u32(0xDC00) == U"\xFFFD");
    TESTX(uni::codepoint::to_titlecase_u32(0xDFFF) == U"\xFFFD");

    TESTX(uni::codepoint::to_casefold_u32(0xD800) == U"\xFFFD");
    TESTX(uni::codepoint::to_casefold_u32(0xDBFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_casefold_u32(0xDC00) == U"\xFFFD");
    TESTX(uni::codepoint::to_casefold_u32(0xDFFF) == U"\xFFFD");
#endif // UNI_ALGO_DISABLE_FULL_CASE
}

void test_prop_norm()
{
    TESTX(uni::codepoint::prop_norm{0}.Canonical_Combining_Class() == 0);
    TESTX(uni::codepoint::prop_norm{0x0300}.Canonical_Combining_Class() == 230);
    TESTX(uni::codepoint::prop_norm{0x0315}.Canonical_Combining_Class() == 232);

    TESTX(uni::codepoint::prop_norm{0xFFFD}.Canonical_Combining_Class() == 0);
    TESTX(uni::codepoint::prop_norm{0x110000}.Canonical_Combining_Class() == 0);
    TESTX(uni::codepoint::prop_norm{0xFFFFFFFF}.Canonical_Combining_Class() == 0);

    TESTX(uni::codepoint::prop_norm{0}.NFC_Quick_Check_Yes());
    TESTX(!uni::codepoint::prop_norm{0x0340}.NFC_Quick_Check_Yes()); // NFC_QC=No
    TESTX(!uni::codepoint::prop_norm{0x0300}.NFC_Quick_Check_Yes()); // NFC_QC=Maybe
    TESTX(uni::codepoint::prop_norm{0xFFFD}.NFC_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0x110000}.NFC_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0xFFFFFFFF}.NFC_Quick_Check_Yes());

    TESTX(uni::codepoint::prop_norm{0}.NFD_Quick_Check_Yes());
    TESTX(!uni::codepoint::prop_norm{0x00C0}.NFD_Quick_Check_Yes()); // NFD_QC=No
    TESTX(uni::codepoint::prop_norm{0x0300}.NFD_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0xFFFD}.NFD_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0x110000}.NFD_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0xFFFFFFFF}.NFD_Quick_Check_Yes());

    TESTX(uni::codepoint::prop_norm{0}.NFKC_Quick_Check_Yes());
    TESTX(!uni::codepoint::prop_norm{0x00A0}.NFKC_Quick_Check_Yes()); // NFKC_QC=No
    TESTX(!uni::codepoint::prop_norm{0x0300}.NFKC_Quick_Check_Yes()); // NFKC_QC=Maybe
    TESTX(uni::codepoint::prop_norm{0xFFFD}.NFKC_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0x110000}.NFKC_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0xFFFFFFFF}.NFKC_Quick_Check_Yes());

    TESTX(uni::codepoint::prop_norm{0}.NFKD_Quick_Check_Yes());
    TESTX(!uni::codepoint::prop_norm{0x00A0}.NFKD_Quick_Check_Yes()); // NFKD_QC=No
    TESTX(uni::codepoint::prop_norm{0x0300}.NFKD_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0xFFFD}.NFKD_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0x110000}.NFKD_Quick_Check_Yes());
    TESTX(uni::codepoint::prop_norm{0xFFFFFFFF}.NFKD_Quick_Check_Yes());

    // Decompose
    TESTX(uni::codepoint::to_decompose_u32(0x1F82) == U"\x03B1\x0313\x0300\x0345");
    TESTX(uni::codepoint::to_decompose_compat_u32(0x1F82) == U"\x03B1\x0313\x0300\x0345");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0x1F82) == U"\x1F82");

    TESTX(uni::codepoint::to_decompose_u32(0xFDFA) == U"\xFDFA");
    TESTX(uni::codepoint::to_decompose_compat_u32(0xFDFA)
          == U"\x0635\x0644\x0649\x0020\x0627\x0644\x0644\x0647\x0020\x0639\x0644\x064A\x0647\x0020\x0648\x0633\x0644\x0645");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0xFDFA) == U"\xFDFA");

    TESTX(uni::codepoint::to_decompose_u32(0xAC00) == U"\x1100\x1161"); // Hangul L+V
    TESTX(uni::codepoint::to_decompose_compat_u32(0xAC00) == U"\x1100\x1161");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0xAC00) == U"\x1100\x1161");

    TESTX(uni::codepoint::to_decompose_u32(0xAC01) == U"\x1100\x1161\x11A8"); // Hangul L+V+T
    TESTX(uni::codepoint::to_decompose_compat_u32(0xAC01) == U"\x1100\x1161\x11A8");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0xAC01) == U"\x1100\x1161\x11A8");

    TESTX(uni::codepoint::to_decompose_u32(0x1100) == U"\x1100");
    TESTX(uni::codepoint::to_decompose_compat_u32(0x1100) == U"\x1100");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0x1100) == U"\x1100");

    TESTX(uni::codepoint::to_decompose_u32(U'W') == U"W");
    TESTX(uni::codepoint::to_decompose_compat_u32(U'W') == U"W");
    TESTX(uni::codepoint::to_decompose_hangul_u32(U'W') == U"W");

    TESTX(uni::codepoint::to_decompose_u32(0) == std::u32string{0});
    TESTX(uni::codepoint::to_decompose_compat_u32(0) == std::u32string{0});
    TESTX(uni::codepoint::to_decompose_hangul_u32(0) == std::u32string{0});

    TESTX(uni::codepoint::to_decompose_u32(0x10FFFF) == U"\x10FFFF");
    TESTX(uni::codepoint::to_decompose_compat_u32(0x10FFFF) == U"\x10FFFF");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0x10FFFF) == U"\x10FFFF");

    TESTX(uni::codepoint::to_decompose_u32(0x110000) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_compat_u32(0x110000) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0x110000) == U"\xFFFD");

    TESTX(uni::codepoint::to_decompose_u32(0xFFFFFFFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_compat_u32(0xFFFFFFFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0xFFFFFFFF) == U"\xFFFD");

    // Surrogates
    TESTX(uni::codepoint::to_decompose_u32(0xD800) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_u32(0xDBFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_u32(0xDC00) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_u32(0xDFFF) == U"\xFFFD");

    TESTX(uni::codepoint::to_decompose_compat_u32(0xD800) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_compat_u32(0xDBFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_compat_u32(0xDC00) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_compat_u32(0xDFFF) == U"\xFFFD");

    TESTX(uni::codepoint::to_decompose_hangul_u32(0xD800) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0xDBFF) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0xDC00) == U"\xFFFD");
    TESTX(uni::codepoint::to_decompose_hangul_u32(0xDFFF) == U"\xFFFD");

    // Compose
    TESTX(uni::codepoint::to_compose(0x0041, 0x0300) == 0x00C0);
    TESTX(uni::codepoint::to_compose(0x1100, 0x1161) == 0xAC00); // Hangul L+V
    TESTX(uni::codepoint::to_compose(0xAC00, 0x11A8) == 0xAC01); // Hangul LV+T

    TESTX(uni::codepoint::to_compose(0x0000, 0x0000) == 0xFFFD);
    TESTX(uni::codepoint::to_compose(0x10FFFF, 0x10FFFF) == 0xFFFD);
    TESTX(uni::codepoint::to_compose(0x110000, 0x110000) == 0xFFFD);
    TESTX(uni::codepoint::to_compose(0xFFFFFFFF, 0xFFFFFFFF) == 0xFFFD);

    // Surrogates
    TESTX(uni::codepoint::to_compose(0xD800, 0xD800) == 0xFFFD);
    TESTX(uni::codepoint::to_compose(0xDBFF, 0xDBFF) == 0xFFFD);
    TESTX(uni::codepoint::to_compose(0xDC00, 0xDC00) == 0xFFFD);
    TESTX(uni::codepoint::to_compose(0xDFFF, 0xDFFF) == 0xFFFD);
    TESTX(uni::codepoint::to_compose(0xD800, 0xDC00) == 0xFFFD);
    TESTX(uni::codepoint::to_compose(0xDBFF, 0xDFFF) == 0xFFFD);
}
