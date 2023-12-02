/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr bool test_locale()
{
    // Ill-formed locale tags are allowed because they are constexpr
    // and we don't want to slow down compilation for no reason with checks
    // So any garbage is allowed in locale tags but no more than 4 chars

    TESTX(una::locale::language{} == una::locale::language{""});
    TESTX(una::locale::language{} != una::locale::language{"1"});
    TESTX(una::locale::language{} != una::locale::language{"12"});
    TESTX(una::locale::language{} != una::locale::language{"123"});
    TESTX(una::locale::language{} != una::locale::language{"1234"});
    TESTX(una::locale::language{} == una::locale::language{"12345"});

    TESTX(una::locale::script{} == una::locale::script{""});
    TESTX(una::locale::script{} != una::locale::script{"1"});
    TESTX(una::locale::script{} != una::locale::script{"12"});
    TESTX(una::locale::script{} != una::locale::script{"123"});
    TESTX(una::locale::script{} != una::locale::script{"1234"});
    TESTX(una::locale::script{} == una::locale::script{"12345"});

    TESTX(una::locale::region{} == una::locale::region{""});
    TESTX(una::locale::region{} != una::locale::region{"1"});
    TESTX(una::locale::region{} != una::locale::region{"12"});
    TESTX(una::locale::region{} != una::locale::region{"123"});
    TESTX(una::locale::region{} != una::locale::region{"1234"});
    TESTX(una::locale::region{} == una::locale::region{"12345"});

    // Helper for the test below
    auto eq = [](const una::locale& loc1, const una::locale& loc2) -> bool
    {
        return loc1.get_language() == loc2.get_language() &&
               loc1.get_script() == loc2.get_script() &&
               loc1.get_region() == loc2.get_region();
    };

    // Locale string parser must normalize locale tags always

    TESTX(eq(una::locale{}, una::locale{{}, {}, {}}));
    TESTX(eq(una::locale{}, una::locale{{}, una::locale::script{}}));
    TESTX(eq(una::locale{}, una::locale{{}, una::locale::region{}}));
    TESTX(eq(una::locale{}, una::locale{{}, una::locale::script{}, una::locale::region{}}));
    TESTX(eq(una::locale{}, una::locale{una::locale::language{}, una::locale::script{}, una::locale::region{}}));
    TESTX(eq(una::locale{}, una::locale{una::locale::language{}}));
    TESTX(eq(una::locale{""}, una::locale{}));
    TESTX(eq(una::locale{"e"}, una::locale{}));
    TESTX(eq(una::locale{"1"}, una::locale{}));
    TESTX(eq(una::locale{"12"}, una::locale{}));
    TESTX(eq(una::locale{"123"}, una::locale{}));
    TESTX(eq(una::locale{"1234"}, una::locale{}));
    TESTX(eq(una::locale{"12345"}, una::locale{}));

    TESTX(eq(una::locale{"en"}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"en-US"}, una::locale{una::locale::language{"en"}, {}, una::locale::region{"US"}}));
    TESTX(eq(una::locale{"en-Latn"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, {}}));
    TESTX(eq(una::locale{"en-Latn-US"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, una::locale::region{"US"}}));
    TESTX(eq(una::locale{"en-Latn-029"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, una::locale::region{"029"}}));
    TESTX(eq(una::locale{"en-029"}, una::locale{una::locale::language{"en"}, {}, una::locale::region{"029"}}));
    TESTX(eq(una::locale{"en-029"}, una::locale{una::locale::language{"en"}, una::locale::region{"029"}}));

    TESTX(eq(una::locale{"EN"}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"EN-us"}, una::locale{una::locale::language{"en"}, {}, una::locale::region{"US"}}));
    TESTX(eq(una::locale{"EN-lATN"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, {}}));
    TESTX(eq(una::locale{"EN-lATN-us"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, una::locale::region{"US"}}));
    TESTX(eq(una::locale{"EN-lATN-029"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, una::locale::region{"029"}}));
    TESTX(eq(una::locale{"EN-029"}, una::locale{una::locale::language{"en"}, {}, una::locale::region{"029"}}));
    TESTX(eq(una::locale{"EN-029"}, una::locale{una::locale::language{"en"}, una::locale::region{"029"}}));

    TESTX(eq(una::locale{"tzm"}, una::locale{una::locale::language{"tzm"}, {}, {}}));
    TESTX(eq(una::locale{"tzm-DZ"}, una::locale{una::locale::language{"tzm"}, {}, una::locale::region{"DZ"}}));
    TESTX(eq(una::locale{"tzm-Latn"}, una::locale{una::locale::language{"tzm"}, una::locale::script{"Latn"}, {}}));
    TESTX(eq(una::locale{"tzm-Latn-DZ"}, una::locale{una::locale::language{"tzm"}, una::locale::script{"Latn"}, una::locale::region{"DZ"}}));
    TESTX(eq(una::locale{"tzm-Latn-999"}, una::locale{una::locale::language{"tzm"}, una::locale::script{"Latn"}, una::locale::region{"999"}}));
    TESTX(eq(una::locale{"tzm-999"}, una::locale{una::locale::language{"tzm"}, {}, una::locale::region{"999"}}));
    TESTX(eq(una::locale{"tzm-999"}, una::locale{una::locale::language{"tzm"}, una::locale::region{"999"}}));

    TESTX(eq(una::locale{"en_US.UTF8"}, una::locale{una::locale::language{"en"}, {}, una::locale::region{"US"}}));
    TESTX(eq(una::locale{"en_Latn_US.UTF8"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, una::locale::region{"US"}}));
    TESTX(eq(una::locale{"en_US_POSIX"}, una::locale{una::locale::language{"en"}, {}, una::locale::region{"US"}}));
    TESTX(eq(una::locale{"en_POSIX_US"}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"POSIX"}, una::locale{{}, {}, {}}));

    TESTX(eq(una::locale{"en-0029"}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"en-29"}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"en-9"}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"en-Latn-0029"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, {}}));
    TESTX(eq(una::locale{"en-Latn-29"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, {}}));
    TESTX(eq(una::locale{"en-Latn-9"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, {}}));

    TESTX(eq(una::locale{"en-"}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"en-Latn-"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, {}}));
    TESTX(eq(una::locale{"en-Latn-US-"}, una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, una::locale::region{"US"}}));

    TESTX(eq(una::locale{"en."}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"en.Latn"}, una::locale{una::locale::language{"en"}, {}, {}}));
    TESTX(eq(una::locale{"en.Latn-US"}, una::locale{una::locale::language{"en"}, {}, {}}));

    TESTX(eq(una::locale{"es-ES_tradnl"}, una::locale{una::locale::language{"es"}, {}, una::locale::region{"ES"}}));

    // TEST Locale to_string

    TESTX(una::locale{""}.to_string() == "");
    TESTX(una::locale{"1"}.to_string() == "");
    TESTX(una::locale{"e"}.to_string() == "");
    TESTX(una::locale{"en"}.to_string() == "en");
    TESTX(una::locale{"en-"}.to_string() == "en");
    TESTX(una::locale{"en-US"}.to_string() == "en-US");
    TESTX(una::locale{"en-Latn"}.to_string() == "en-Latn");
    TESTX(una::locale{"en_Latn_US"}.to_string() == "en-Latn-US");
    TESTX(una::locale{"en_Latn_023"}.to_string() == "en-Latn-023");
    TESTX(una::locale{"en_023"}.to_string() == "en-023");
    TESTX(una::locale{"TzM_lATN_dz"}.to_string() == "tzm-Latn-DZ");
    TESTX(una::locale{"TzM_lATN_999"}.to_string() == "tzm-Latn-999");
    TESTX(una::locale{"TzM_999"}.to_string() == "tzm-999");

    // Creating locale with ill-formed locale tags directly allows ill-formed locale and ill-formed output (to_string)

    TESTX((una::locale{{}, {}, {}}.to_string() == "")); // Well-formed empty
    TESTX((una::locale{una::locale::language{""}, una::locale::script{""}, una::locale::region{""}}.to_string() == "")); // Well-formed empty
    TESTX((una::locale{una::locale::language{"en"}, {}, {}}.to_string() == "en")); // Well-formed
    TESTX((una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, {}}.to_string() == "en-Latn")); // Well-formed
    TESTX((una::locale{una::locale::language{"en"}, una::locale::script{"Latn"}, una::locale::region{"US"}}.to_string() == "en-Latn-US")); // Well-formed
    TESTX((una::locale{{}, una::locale::script{"Latn"}, una::locale::region{"US"}}.to_string() == "-Latn-US")); // Ill-formed no language
    TESTX((una::locale{{}, una::locale::script{"Latn"}, {}}.to_string() == "-Latn")); // Ill-formed no language
    TESTX((una::locale{{}, {}, una::locale::region{"US"}}.to_string() == "-US")); // Ill-formed no language
    // Ill-formed garbage
    TESTX((una::locale{una::locale::language{"1234"}, una::locale::script{"1234"}, una::locale::region{"1234"}}.to_string() == "1234-1234-1234"));
    // Well-formed empty just because
    TESTX((una::locale{una::locale::language{"12345"}, una::locale::script{"12345"}, una::locale::region{"12345"}}.to_string() == ""));

    // Ill-formed no language because according to BCP47 2.2.1:
    // The primary language subtag is the first subtag in a language tag and cannot be omitted, with two exceptions: blah blah blah not important
    // -------

    TESTX(una::locale{}.is_empty());
    TESTX(una::locale{""}.is_empty());
    TESTX(!una::locale{"en-US"}.is_empty());

    return true;
}
