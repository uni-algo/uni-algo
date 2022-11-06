/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

void test_locale()
{
    // Ill-formed locale tags are allowed because they are constexpr
    // and we don't want to slow down compilation for no reason with checks
    // So any garbage is allowed in locale tags but no more than 4 chars

    TESTX(uni::locale::language{} == uni::locale::language{""});
    TESTX(uni::locale::language{} != uni::locale::language{"1"});
    TESTX(uni::locale::language{} != uni::locale::language{"12"});
    TESTX(uni::locale::language{} != uni::locale::language{"123"});
    TESTX(uni::locale::language{} != uni::locale::language{"1234"});
    TESTX(uni::locale::language{} == uni::locale::language{"12345"});

    TESTX(uni::locale::script{} == uni::locale::script{""});
    TESTX(uni::locale::script{} != uni::locale::script{"1"});
    TESTX(uni::locale::script{} != uni::locale::script{"12"});
    TESTX(uni::locale::script{} != uni::locale::script{"123"});
    TESTX(uni::locale::script{} != uni::locale::script{"1234"});
    TESTX(uni::locale::script{} == uni::locale::script{"12345"});

    TESTX(uni::locale::region{} == uni::locale::region{""});
    TESTX(uni::locale::region{} != uni::locale::region{"1"});
    TESTX(uni::locale::region{} != uni::locale::region{"12"});
    TESTX(uni::locale::region{} != uni::locale::region{"123"});
    TESTX(uni::locale::region{} != uni::locale::region{"1234"});
    TESTX(uni::locale::region{} == uni::locale::region{"12345"});

    // Locale string parser must normalize locale tags always

    TESTX((uni::locale{} == uni::locale{{}, {}, {}}));
    TESTX((uni::locale{} == uni::locale{{}, uni::locale::script{}}));
    TESTX((uni::locale{} == uni::locale{{}, uni::locale::region{}}));
    TESTX((uni::locale{} == uni::locale{{}, uni::locale::script{}, uni::locale::region{}}));
    TESTX((uni::locale{} == uni::locale{uni::locale::language{}, uni::locale::script{}, uni::locale::region{}}));
    TESTX((uni::locale{} == uni::locale{uni::locale::language{}}));
    TESTX((uni::locale{""} == uni::locale{}));
    TESTX((uni::locale{"e"} == uni::locale{}));
    TESTX((uni::locale{"1"} == uni::locale{}));
    TESTX((uni::locale{"12"} == uni::locale{}));
    TESTX((uni::locale{"123"} == uni::locale{}));
    TESTX((uni::locale{"1234"} == uni::locale{}));
    TESTX((uni::locale{"12345"} == uni::locale{}));

    TESTX((uni::locale{"en"} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"en-US"} == uni::locale{uni::locale::language{"en"}, {}, uni::locale::region{"US"}}));
    TESTX((uni::locale{"en-Latn"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, {}}));
    TESTX((uni::locale{"en-Latn-US"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, uni::locale::region{"US"}}));
    TESTX((uni::locale{"en-Latn-029"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, uni::locale::region{"029"}}));
    TESTX((uni::locale{"en-029"} == uni::locale{uni::locale::language{"en"}, {}, uni::locale::region{"029"}}));
    TESTX((uni::locale{"en-029"} == uni::locale{uni::locale::language{"en"}, uni::locale::region{"029"}}));

    TESTX((uni::locale{"EN"} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"EN-us"} == uni::locale{uni::locale::language{"en"}, {}, uni::locale::region{"US"}}));
    TESTX((uni::locale{"EN-lATN"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, {}}));
    TESTX((uni::locale{"EN-lATN-us"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, uni::locale::region{"US"}}));
    TESTX((uni::locale{"EN-lATN-029"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, uni::locale::region{"029"}}));
    TESTX((uni::locale{"EN-029"} == uni::locale{uni::locale::language{"en"}, {}, uni::locale::region{"029"}}));
    TESTX((uni::locale{"EN-029"} == uni::locale{uni::locale::language{"en"}, uni::locale::region{"029"}}));

    TESTX((uni::locale{"tzm"} == uni::locale{uni::locale::language{"tzm"}, {}, {}}));
    TESTX((uni::locale{"tzm-DZ"} == uni::locale{uni::locale::language{"tzm"}, {}, uni::locale::region{"DZ"}}));
    TESTX((uni::locale{"tzm-Latn"} == uni::locale{uni::locale::language{"tzm"}, uni::locale::script{"Latn"}, {}}));
    TESTX((uni::locale{"tzm-Latn-DZ"} == uni::locale{uni::locale::language{"tzm"}, uni::locale::script{"Latn"}, uni::locale::region{"DZ"}}));
    TESTX((uni::locale{"tzm-Latn-999"} == uni::locale{uni::locale::language{"tzm"}, uni::locale::script{"Latn"}, uni::locale::region{"999"}}));
    TESTX((uni::locale{"tzm-999"} == uni::locale{uni::locale::language{"tzm"}, {}, uni::locale::region{"999"}}));
    TESTX((uni::locale{"tzm-999"} == uni::locale{uni::locale::language{"tzm"}, uni::locale::region{"999"}}));

    TESTX((uni::locale{"en_US.UTF8"} == uni::locale{uni::locale::language{"en"}, {}, uni::locale::region{"US"}}));
    TESTX((uni::locale{"en_Latn_US.UTF8"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, uni::locale::region{"US"}}));
    TESTX((uni::locale{"en_US_POSIX"} == uni::locale{uni::locale::language{"en"}, {}, uni::locale::region{"US"}}));
    TESTX((uni::locale{"en_POSIX_US"} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"POSIX"} == uni::locale{{}, {}, {}}));

    TESTX((uni::locale{"en-0029"} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"en-29"} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"en-9"} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"en-Latn-0029"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, {}}));
    TESTX((uni::locale{"en-Latn-29"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, {}}));
    TESTX((uni::locale{"en-Latn-9"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, {}}));

    TESTX((uni::locale{"en-"} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"en-Latn-"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, {}}));
    TESTX((uni::locale{"en-Latn-US-"} == uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, uni::locale::region{"US"}}));

    TESTX((uni::locale{"en."} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"en.Latn"} == uni::locale{uni::locale::language{"en"}, {}, {}}));
    TESTX((uni::locale{"en.Latn-US"} == uni::locale{uni::locale::language{"en"}, {}, {}}));

    TESTX((uni::locale{"es-ES_tradnl"} == uni::locale{uni::locale::language{"es"}, {}, uni::locale::region{"ES"}}));

    // TEST Locale to_string

    TESTX(uni::locale{""}.to_string() == "");
    TESTX(uni::locale{"1"}.to_string() == "");
    TESTX(uni::locale{"e"}.to_string() == "");
    TESTX(uni::locale{"en"}.to_string() == "en");
    TESTX(uni::locale{"en-"}.to_string() == "en");
    TESTX(uni::locale{"en-US"}.to_string() == "en-US");
    TESTX(uni::locale{"en-Latn"}.to_string() == "en-Latn");
    TESTX(uni::locale{"en_Latn_US"}.to_string() == "en-Latn-US");
    TESTX(uni::locale{"en_Latn_023"}.to_string() == "en-Latn-023");
    TESTX(uni::locale{"en_023"}.to_string() == "en-023");
    TESTX(uni::locale{"TzM_lATN_dz"}.to_string() == "tzm-Latn-DZ");
    TESTX(uni::locale{"TzM_lATN_999"}.to_string() == "tzm-Latn-999");
    TESTX(uni::locale{"TzM_999"}.to_string() == "tzm-999");

    // Creating locale with ill-formed locale tags directly allows ill-formed locale and ill-formed output (to_string)

    TESTX((uni::locale{{}, {}, {}}.to_string() == "")); // Well-formed empty
    TESTX((uni::locale{uni::locale::language{""}, uni::locale::script{""}, uni::locale::region{""}}.to_string() == "")); // Well-formed empty
    TESTX((uni::locale{uni::locale::language{"en"}, {}, {}}.to_string() == "en")); // Well-formed
    TESTX((uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, {}}.to_string() == "en-Latn")); // Well-formed
    TESTX((uni::locale{uni::locale::language{"en"}, uni::locale::script{"Latn"}, uni::locale::region{"US"}}.to_string() == "en-Latn-US")); // Well-formed
    TESTX((uni::locale{{}, uni::locale::script{"Latn"}, uni::locale::region{"US"}}.to_string() == "-Latn-US")); // Ill-formed no language
    TESTX((uni::locale{{}, uni::locale::script{"Latn"}, {}}.to_string() == "-Latn")); // Ill-formed no language
    TESTX((uni::locale{{}, {}, uni::locale::region{"US"}}.to_string() == "-US")); // Ill-formed no language
    // Ill-formed garbage
    TESTX((uni::locale{uni::locale::language{"1234"}, uni::locale::script{"1234"}, uni::locale::region{"1234"}}.to_string() == "1234-1234-1234"));
    // Well-formed empty just because
    TESTX((uni::locale{uni::locale::language{"12345"}, uni::locale::script{"12345"}, uni::locale::region{"12345"}}.to_string() == ""));

    // Ill-formed no language because according to BCP47 2.2.1:
    // The primary language subtag is the first subtag in a language tag and cannot be omitted, with two exceptions: blah blah blah not important
    // -------

    TESTX(uni::locale{}.is_empty());
    TESTX(uni::locale{""}.is_empty());
    TESTX(!uni::locale{"en-US"}.is_empty());
}
