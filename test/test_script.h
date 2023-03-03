/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

test_constexpr bool test_script()
{
    // PART 1

    // The value of "unknown" script (ISO 15924 code Zzzz) is given to unassigned, private-use, noncharacter, and surrogate code points.

    char32_t unassigned = 0x0A7777; // Random unassigned code point (change to something else if it will be assigned)

    // Unassigned must be Unknown (Zzzz) script
    TESTX(una::codepoint::get_script(unassigned) == una::locale::script{"Zzzz"});

    // Surrogates must be Unknown (Zzzz) script
    TESTX(una::codepoint::get_script(0xD800) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0xDBFF) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0xDC00) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0xDFFF) == una::locale::script{"Zzzz"});

    // Private-use must be Unknown (Zzzz) script
    TESTX(una::codepoint::get_script(0xE000) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0xF0000) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0x100000) == una::locale::script{"Zzzz"});

    // Noncharacters must be Unknown (Zzzz) script
    TESTX(una::codepoint::get_script(0xFDD0) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0xFDEF) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0xFFFE) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0xFFFF) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0x10FFFE) == una::locale::script{"Zzzz"});
    TESTX(una::codepoint::get_script(0x10FFFF) == una::locale::script{"Zzzz"});

    // REPLACEMENT CHARACTER U+FFFD must be Common (Zyyy) script
    TESTX(una::codepoint::get_script(0xFFFD) == una::locale::script{"Zyyy"});

    // Ill-formed must be the same as U+FFFD so Common (Zyyy) script
    TESTX(una::codepoint::get_script(0x110000) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::get_script(0xFFFFFFFF) == una::locale::script{"Zyyy"});

    // Common (Zyyy) script
    TESTX(una::codepoint::get_script(0) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::get_script(0x0020) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::get_script(0x0E0001) == una::locale::script{"Zyyy"});

    // PART 2

    // First code point with such script for other tests.
    // All code points are from the Unicode Standard -> Scripts.txt
    // and sorted the same, after Unicode 5.0.0 they sorted by the version they added in that file.

    // NOTE from mg152: This is not generated. I wrote it by hand to recheck everything, spend like two hours on this shit.

    // Unicode 1.0.0 - 5.0.0

    TESTX(una::codepoint::get_script(0x0041)  == una::locale::script{"Latn"}); // Latin
    TESTX(una::codepoint::get_script(0x0370)  == una::locale::script{"Grek"}); // Greek
    TESTX(una::codepoint::get_script(0x0400)  == una::locale::script{"Cyrl"}); // Cyrillic
    TESTX(una::codepoint::get_script(0x0531)  == una::locale::script{"Armn"}); // Armenian
    TESTX(una::codepoint::get_script(0x0591)  == una::locale::script{"Hebr"}); // Hebrew
    TESTX(una::codepoint::get_script(0x0600)  == una::locale::script{"Arab"}); // Arabic
    TESTX(una::codepoint::get_script(0x0700)  == una::locale::script{"Syrc"}); // Syriac
    TESTX(una::codepoint::get_script(0x0780)  == una::locale::script{"Thaa"}); // Thaana
    TESTX(una::codepoint::get_script(0x0900)  == una::locale::script{"Deva"}); // Devanagari
    TESTX(una::codepoint::get_script(0x0980)  == una::locale::script{"Beng"}); // Bengali

    TESTX(una::codepoint::get_script(0x0A01)  == una::locale::script{"Guru"}); // Gurmukhi
    TESTX(una::codepoint::get_script(0x0A81)  == una::locale::script{"Gujr"}); // Gujarati
    TESTX(una::codepoint::get_script(0x0B01)  == una::locale::script{"Orya"}); // Oriya
    TESTX(una::codepoint::get_script(0x0B82)  == una::locale::script{"Taml"}); // Tamil
    TESTX(una::codepoint::get_script(0x0C00)  == una::locale::script{"Telu"}); // Telugu
    TESTX(una::codepoint::get_script(0x0C80)  == una::locale::script{"Knda"}); // Kannada
    TESTX(una::codepoint::get_script(0x0D00)  == una::locale::script{"Mlym"}); // Malayalam
    TESTX(una::codepoint::get_script(0x0D81)  == una::locale::script{"Sinh"}); // Sinhala
    TESTX(una::codepoint::get_script(0x0E01)  == una::locale::script{"Thai"}); // Thai
    TESTX(una::codepoint::get_script(0x0E81)  == una::locale::script{"Laoo"}); // Lao

    TESTX(una::codepoint::get_script(0x0F00)  == una::locale::script{"Tibt"}); // Tibetan
    TESTX(una::codepoint::get_script(0x1000)  == una::locale::script{"Mymr"}); // Myanmar
    TESTX(una::codepoint::get_script(0x10A0)  == una::locale::script{"Geor"}); // Georgian
    TESTX(una::codepoint::get_script(0x1100)  == una::locale::script{"Hang"}); // Hangul
    TESTX(una::codepoint::get_script(0x1200)  == una::locale::script{"Ethi"}); // Ethiopic
    TESTX(una::codepoint::get_script(0x13A0)  == una::locale::script{"Cher"}); // Cherokee
    TESTX(una::codepoint::get_script(0x1400)  == una::locale::script{"Cans"}); // Canadian_Aboriginal
    TESTX(una::codepoint::get_script(0x1680)  == una::locale::script{"Ogam"}); // Ogham
    TESTX(una::codepoint::get_script(0x16A0)  == una::locale::script{"Runr"}); // Runic
    TESTX(una::codepoint::get_script(0x1780)  == una::locale::script{"Khmr"}); // Khmer

    TESTX(una::codepoint::get_script(0x1800)  == una::locale::script{"Mong"}); // Mongolian
    TESTX(una::codepoint::get_script(0x3041)  == una::locale::script{"Hira"}); // Hiragana
    TESTX(una::codepoint::get_script(0x30A1)  == una::locale::script{"Kana"}); // Katakana
    TESTX(una::codepoint::get_script(0x02EA)  == una::locale::script{"Bopo"}); // Bopomofo
    TESTX(una::codepoint::get_script(0x2E80)  == una::locale::script{"Hani"}); // Han
    TESTX(una::codepoint::get_script(0xA000)  == una::locale::script{"Yiii"}); // Yi
    TESTX(una::codepoint::get_script(0x10300) == una::locale::script{"Ital"}); // Old_Italic
    TESTX(una::codepoint::get_script(0x10330) == una::locale::script{"Goth"}); // Gothic
    TESTX(una::codepoint::get_script(0x10400) == una::locale::script{"Dsrt"}); // Deseret
    TESTX(una::codepoint::get_script(0x0300)  == una::locale::script{"Zinh"}); // Inherited

    TESTX(una::codepoint::get_script(0x1700)  == una::locale::script{"Tglg"}); // Tagalog
    TESTX(una::codepoint::get_script(0x1720)  == una::locale::script{"Hano"}); // Hanunoo
    TESTX(una::codepoint::get_script(0x1740)  == una::locale::script{"Buhd"}); // Buhid
    TESTX(una::codepoint::get_script(0x1760)  == una::locale::script{"Tagb"}); // Tagbanwa
    TESTX(una::codepoint::get_script(0x1900)  == una::locale::script{"Limb"}); // Limbu
    TESTX(una::codepoint::get_script(0x1950)  == una::locale::script{"Tale"}); // Tai_Le
    TESTX(una::codepoint::get_script(0x10000) == una::locale::script{"Linb"}); // Linear_B
    TESTX(una::codepoint::get_script(0x10380) == una::locale::script{"Ugar"}); // Ugaritic
    TESTX(una::codepoint::get_script(0x10450) == una::locale::script{"Shaw"}); // Shavian
    TESTX(una::codepoint::get_script(0x10480) == una::locale::script{"Osma"}); // Osmanya

    TESTX(una::codepoint::get_script(0x10800) == una::locale::script{"Cprt"}); // Cypriot
    TESTX(una::codepoint::get_script(0x2800)  == una::locale::script{"Brai"}); // Braille
    TESTX(una::codepoint::get_script(0x1A00)  == una::locale::script{"Bugi"}); // Buginese
    TESTX(una::codepoint::get_script(0x03E2)  == una::locale::script{"Copt"}); // Coptic
    TESTX(una::codepoint::get_script(0x1980)  == una::locale::script{"Talu"}); // New_Tai_Lue
    TESTX(una::codepoint::get_script(0x2C00)  == una::locale::script{"Glag"}); // Glagolitic
    TESTX(una::codepoint::get_script(0x1E000) == una::locale::script{"Glag"}); // Glagolitic
    TESTX(una::codepoint::get_script(0x2D30)  == una::locale::script{"Tfng"}); // Tifinagh
    TESTX(una::codepoint::get_script(0xA800)  == una::locale::script{"Sylo"}); // Syloti_Nagri
    TESTX(una::codepoint::get_script(0x103A0) == una::locale::script{"Xpeo"}); // Old_Persian
    TESTX(una::codepoint::get_script(0x10A00) == una::locale::script{"Khar"}); // Kharoshthi

    // Unicode 5.0.0

    TESTX(una::codepoint::get_script(0x1B00)  == una::locale::script{"Bali"}); // Balinese
    TESTX(una::codepoint::get_script(0x12000) == una::locale::script{"Xsux"}); // Cuneiform
    TESTX(una::codepoint::get_script(0x10900) == una::locale::script{"Phnx"}); // Phoenician
    TESTX(una::codepoint::get_script(0xA840)  == una::locale::script{"Phag"}); // Phags_Pa
    TESTX(una::codepoint::get_script(0x07C0)  == una::locale::script{"Nkoo"}); // Nko

    // Unicode 5.1.0

    TESTX(una::codepoint::get_script(0x1B80)  == una::locale::script{"Sund"}); // Sundanese
    TESTX(una::codepoint::get_script(0x1C00)  == una::locale::script{"Lepc"}); // Lepcha
    TESTX(una::codepoint::get_script(0x1C50)  == una::locale::script{"Olck"}); // Ol_Chiki
    TESTX(una::codepoint::get_script(0xA500)  == una::locale::script{"Vaii"}); // Vai
    TESTX(una::codepoint::get_script(0xA880)  == una::locale::script{"Saur"}); // Saurashtra
    TESTX(una::codepoint::get_script(0xA900)  == una::locale::script{"Kali"}); // Kayah_Li
    TESTX(una::codepoint::get_script(0xA930)  == una::locale::script{"Rjng"}); // Rejang
    TESTX(una::codepoint::get_script(0x10280) == una::locale::script{"Lyci"}); // Lycian
    TESTX(una::codepoint::get_script(0x102A0) == una::locale::script{"Cari"}); // Carian
    TESTX(una::codepoint::get_script(0x10920) == una::locale::script{"Lydi"}); // Lydian
    TESTX(una::codepoint::get_script(0xAA00)  == una::locale::script{"Cham"}); // Cham

    // Unicode 5.2.0

    TESTX(una::codepoint::get_script(0x1A20)  == una::locale::script{"Lana"}); // Tai_Tham
    TESTX(una::codepoint::get_script(0xAA80)  == una::locale::script{"Tavt"}); // Tai_Viet
    TESTX(una::codepoint::get_script(0x10B00) == una::locale::script{"Avst"}); // Avestan
    TESTX(una::codepoint::get_script(0x13000) == una::locale::script{"Egyp"}); // Egyptian_Hieroglyphs
    TESTX(una::codepoint::get_script(0x0800)  == una::locale::script{"Samr"}); // Samaritan
    TESTX(una::codepoint::get_script(0xA4D0)  == una::locale::script{"Lisu"}); // Lisu
    TESTX(una::codepoint::get_script(0xA6A0)  == una::locale::script{"Bamu"}); // Bamums
    TESTX(una::codepoint::get_script(0xA980)  == una::locale::script{"Java"}); // Javanese
    TESTX(una::codepoint::get_script(0xAAE0)  == una::locale::script{"Mtei"}); // Meetei_Mayek
    TESTX(una::codepoint::get_script(0x10840) == una::locale::script{"Armi"}); // Imperial_Aramaic

    TESTX(una::codepoint::get_script(0x10A60) == una::locale::script{"Sarb"}); // Old_South_Arabian
    TESTX(una::codepoint::get_script(0x10B40) == una::locale::script{"Prti"}); // Inscriptional_Parthian
    TESTX(una::codepoint::get_script(0x10B60) == una::locale::script{"Phli"}); // Inscriptional_Pahlavi
    TESTX(una::codepoint::get_script(0x10C00) == una::locale::script{"Orkh"}); // Old_Turkic
    TESTX(una::codepoint::get_script(0x11080) == una::locale::script{"Kthi"}); // Kaithi

    // Unicode 6.0.0

    TESTX(una::codepoint::get_script(0x1BC0)  == una::locale::script{"Batk"}); // Batak
    TESTX(una::codepoint::get_script(0x11000) == una::locale::script{"Brah"}); // Brahmi
    TESTX(una::codepoint::get_script(0x0840)  == una::locale::script{"Mand"}); // Mandaic

    // Unicode 6.1.0

    TESTX(una::codepoint::get_script(0x11100) == una::locale::script{"Cakm"}); // Chakma
    TESTX(una::codepoint::get_script(0x109A0) == una::locale::script{"Merc"}); // Meroitic_Cursive
    TESTX(una::codepoint::get_script(0x10980) == una::locale::script{"Mero"}); // Meroitic_Hieroglyphs
    TESTX(una::codepoint::get_script(0x16F00) == una::locale::script{"Plrd"}); // Miao
    TESTX(una::codepoint::get_script(0x11180) == una::locale::script{"Shrd"}); // Sharada
    TESTX(una::codepoint::get_script(0x110D0) == una::locale::script{"Sora"}); // Sora_Sompeng
    TESTX(una::codepoint::get_script(0x11680) == una::locale::script{"Takr"}); // Takri

    // Unicode 7.0.0

    TESTX(una::codepoint::get_script(0x10530) == una::locale::script{"Aghb"}); // Caucasian_Albanian
    TESTX(una::codepoint::get_script(0x16AD0) == una::locale::script{"Bass"}); // Bassa_Vah
    TESTX(una::codepoint::get_script(0x1BC00) == una::locale::script{"Dupl"}); // Duployan
    TESTX(una::codepoint::get_script(0x10500) == una::locale::script{"Elba"}); // Elbasan
    TESTX(una::codepoint::get_script(0x11300) == una::locale::script{"Gran"}); // Grantha
    TESTX(una::codepoint::get_script(0x16B00) == una::locale::script{"Hmng"}); // Pahawh_Hmong
    TESTX(una::codepoint::get_script(0x11200) == una::locale::script{"Khoj"}); // Khojki
    TESTX(una::codepoint::get_script(0x10600) == una::locale::script{"Lina"}); // Linear_A
    TESTX(una::codepoint::get_script(0x11150) == una::locale::script{"Mahj"}); // Mahajani
    TESTX(una::codepoint::get_script(0x10AC0) == una::locale::script{"Mani"}); // Manichaean

    TESTX(una::codepoint::get_script(0x1E800) == una::locale::script{"Mend"}); // Mende_Kikakui
    TESTX(una::codepoint::get_script(0x11600) == una::locale::script{"Modi"}); // Modi
    TESTX(una::codepoint::get_script(0x16A40) == una::locale::script{"Mroo"}); // Mro
    TESTX(una::codepoint::get_script(0x10A80) == una::locale::script{"Narb"}); // Old_North_Arabian
    TESTX(una::codepoint::get_script(0x10880) == una::locale::script{"Nbat"}); // Nabataean
    TESTX(una::codepoint::get_script(0x10860) == una::locale::script{"Palm"}); // Palmyrene
    TESTX(una::codepoint::get_script(0x11AC0) == una::locale::script{"Pauc"}); // Pau_Cin_Hau
    TESTX(una::codepoint::get_script(0x10350) == una::locale::script{"Perm"}); // Old_Permic
    TESTX(una::codepoint::get_script(0x10B80) == una::locale::script{"Phlp"}); // Psalter_Pahlavi
    TESTX(una::codepoint::get_script(0x11580) == una::locale::script{"Sidd"}); // Siddham

    TESTX(una::codepoint::get_script(0x112B0) == una::locale::script{"Sind"}); // Khudawadi
    TESTX(una::codepoint::get_script(0x11480) == una::locale::script{"Tirh"}); // Tirhuta
    TESTX(una::codepoint::get_script(0x118A0) == una::locale::script{"Wara"}); // Warang_Citi

    // Unicode 8.0.0

    TESTX(una::codepoint::get_script(0x11700) == una::locale::script{"Ahom"}); // Ahom
    TESTX(una::codepoint::get_script(0x14400) == una::locale::script{"Hluw"}); // Anatolian_Hieroglyphs
    TESTX(una::codepoint::get_script(0x108E0) == una::locale::script{"Hatr"}); // Hatran
    TESTX(una::codepoint::get_script(0x11280) == una::locale::script{"Mult"}); // Multani
    TESTX(una::codepoint::get_script(0x10C80) == una::locale::script{"Hung"}); // Old_Hungarian
    TESTX(una::codepoint::get_script(0x1D800) == una::locale::script{"Sgnw"}); // SignWriting

    // Unicode 9.0.0

    TESTX(una::codepoint::get_script(0x1E900) == una::locale::script{"Adlm"}); // Adlam
    TESTX(una::codepoint::get_script(0x11C00) == una::locale::script{"Bhks"}); // Bhaiksuki
    TESTX(una::codepoint::get_script(0x11C70) == una::locale::script{"Marc"}); // Marchen
    TESTX(una::codepoint::get_script(0x11400) == una::locale::script{"Newa"}); // Newa
    TESTX(una::codepoint::get_script(0x104B0) == una::locale::script{"Osge"}); // Osage
    TESTX(una::codepoint::get_script(0x16FE0) == una::locale::script{"Tang"}); // Tangut

    // Unicode 10.0.0

    TESTX(una::codepoint::get_script(0x11D00) == una::locale::script{"Gonm"}); // Masaram_Gondi
    TESTX(una::codepoint::get_script(0x16FE1) == una::locale::script{"Nshu"}); // Nushu
    TESTX(una::codepoint::get_script(0x11A50) == una::locale::script{"Soyo"}); // Soyombo
    TESTX(una::codepoint::get_script(0x11A00) == una::locale::script{"Zanb"}); // Zanabazar_Square

    // Unicode 11.0.0

    TESTX(una::codepoint::get_script(0x11800) == una::locale::script{"Dogr"}); // Dogra
    TESTX(una::codepoint::get_script(0x11D60) == una::locale::script{"Gong"}); // Gunjala_Gondi
    TESTX(una::codepoint::get_script(0x11EE0) == una::locale::script{"Maka"}); // Makasar
    TESTX(una::codepoint::get_script(0x16E40) == una::locale::script{"Medf"}); // Medefaidrin
    TESTX(una::codepoint::get_script(0x10D00) == una::locale::script{"Rohg"}); // Hanifi_Rohingya
    TESTX(una::codepoint::get_script(0x10F30) == una::locale::script{"Sogd"}); // Sogdian
    TESTX(una::codepoint::get_script(0x10F00) == una::locale::script{"Sogo"}); // Old_Sogdian

    // Unicode 12.0.0

    TESTX(una::codepoint::get_script(0x10FE0) == una::locale::script{"Elym"}); // Elymaic
    TESTX(una::codepoint::get_script(0x119A0) == una::locale::script{"Nand"}); // Nandinagari
    TESTX(una::codepoint::get_script(0x1E100) == una::locale::script{"Hmnp"}); // Nyiakeng_Puachue_Hmong
    TESTX(una::codepoint::get_script(0x1E2C0) == una::locale::script{"Wcho"}); // Wancho

    // Unicode 13.0.0

    TESTX(una::codepoint::get_script(0x10FB0) == una::locale::script{"Chrs"}); // Chorasmian
    TESTX(una::codepoint::get_script(0x11900) == una::locale::script{"Diak"}); // Dives_Akuru
    TESTX(una::codepoint::get_script(0x16FE4) == una::locale::script{"Kits"}); // Khitan_Small_Script
    TESTX(una::codepoint::get_script(0x10E80) == una::locale::script{"Yezi"}); // Yezidi

    // Unicode 14.0.0

    TESTX(una::codepoint::get_script(0x12F90) == una::locale::script{"Cpmn"}); // Cypro_Minoan
    TESTX(una::codepoint::get_script(0x10F70) == una::locale::script{"Ougr"}); // Old_Uyghur
    TESTX(una::codepoint::get_script(0x16A70) == una::locale::script{"Tnsa"}); // Tangsa
    TESTX(una::codepoint::get_script(0x1E290) == una::locale::script{"Toto"}); // Toto
    TESTX(una::codepoint::get_script(0x10570) == una::locale::script{"Vith"}); // Vithkuqi

    // Unicode 15.0.0

    TESTX(una::codepoint::get_script(0x11F00) == una::locale::script{"Kawi"}); // Kawi
    TESTX(una::codepoint::get_script(0x1E4D0) == una::locale::script{"Nagm"}); // Nag_Mundari

    // Number of scripts in Unicode 15.0.0: 161 + 3 special scripts: Unknown (Zzzz), Common (Zyyy), Inherited (Zinh)
    //static_assert(una::detail::stage3_script.size() == 161 + 3);

    return true;
}

test_constexpr bool test_script_ext()
{
    // PART 1

    // First redo PART 1 tests from previous test_script function
    // but with codepoint::has_script function instead of codepoint::get_script

    // The value of "unknown" script (ISO 15924 code Zzzz) is given to unassigned, private-use, noncharacter, and surrogate code points.

    char32_t unassigned = 0x0A7777; // Random unassigned code point (change to something else if it will be assigned)

    // Unassigned must be Unknown (Zzzz) script
    TESTX(una::codepoint::has_script(unassigned, una::locale::script{"Zzzz"}));

    // Surrogates must be Unknown (Zzzz) script
    TESTX(una::codepoint::has_script(0xD800, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0xDBFF, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0xDC00, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0xDFFF, una::locale::script{"Zzzz"}));

    // Private-use must be Unknown (Zzzz) script
    TESTX(una::codepoint::has_script(0xE000, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0xF0000, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0x100000, una::locale::script{"Zzzz"}));

    // Noncharacters must be Unknown (Zzzz) script
    TESTX(una::codepoint::has_script(0xFDD0, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0xFDEF, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0xFFFE, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0xFFFF, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0x10FFFE, una::locale::script{"Zzzz"}));
    TESTX(una::codepoint::has_script(0x10FFFF, una::locale::script{"Zzzz"}));

    // REPLACEMENT CHARACTER U+FFFD must be Common (Zyyy) script
    TESTX(una::codepoint::has_script(0xFFFD, una::locale::script{"Zyyy"}));
    TESTX(!una::codepoint::has_script(0xFFFD, una::locale::script{"Zzzz"}));

    // Ill-formed must be the same as U+FFFD so Common (Zyyy) script
    TESTX(una::codepoint::has_script(0x110000, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0xFFFFFFFF, una::locale::script{"Zyyy"}));
    TESTX(!una::codepoint::has_script(0x110000, una::locale::script{"Zzzz"}));
    TESTX(!una::codepoint::has_script(0xFFFFFFFF, una::locale::script{"Zzzz"}));

    // Common (Zyyy) script
    TESTX(una::codepoint::has_script(0, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x0020, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x0E0001, una::locale::script{"Zyyy"}));
    TESTX(!una::codepoint::has_script(0, una::locale::script{"Zzzz"}));
    TESTX(!una::codepoint::has_script(0x0020, una::locale::script{"Zzzz"}));
    TESTX(!una::codepoint::has_script(0x0E0001, una::locale::script{"Zzzz"}));

    // PART 2

    // Some code points from the Unicode Standard -> ScriptExtensions.txt
    // for other tests. All sorted the same as in that file.

    // U+1CF7
    TESTX(una::codepoint::get_script(0x1CF7) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::has_script(0x1CF7, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x1CF7, una::locale::script{"Beng"}));
    TESTX(!una::codepoint::has_script(0x1CF7, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x1CF7, una::locale::script{"Zzzz"}));

    // U+1CD1
    TESTX(una::codepoint::get_script(0x1CD1) == una::locale::script{"Zinh"});
    TESTX(una::codepoint::has_script(0x1CD1, una::locale::script{"Zinh"}));
    TESTX(una::codepoint::has_script(0x1CD1, una::locale::script{"Deva"}));
    TESTX(!una::codepoint::has_script(0x1CD1, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x1CD1, una::locale::script{"Zyyy"}));

    // U+1BCA0
    TESTX(una::codepoint::get_script(0x1BCA0) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::has_script(0x1BCA0, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x1BCA0, una::locale::script{"Dupl"}));
    TESTX(!una::codepoint::has_script(0x1BCA0, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x1BCA0, una::locale::script{"Zzzz"}));

    // U+102E0
    TESTX(una::codepoint::get_script(0x102E0) == una::locale::script{"Zinh"});
    TESTX(una::codepoint::has_script(0x102E0, una::locale::script{"Zinh"}));
    TESTX(una::codepoint::has_script(0x102E0, una::locale::script{"Arab"}));
    TESTX(una::codepoint::has_script(0x102E0, una::locale::script{"Copt"}));
    TESTX(!una::codepoint::has_script(0x102E0, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x102E0, una::locale::script{"Zyyy"}));

    // U+A700
    TESTX(una::codepoint::get_script(0xA700) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::has_script(0xA700, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0xA700, una::locale::script{"Hani"}));
    TESTX(una::codepoint::has_script(0xA700, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0xA700, una::locale::script{"Zzzz"}));

    // U+3031
    TESTX(una::codepoint::get_script(0x3031) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::has_script(0x3031, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x3031, una::locale::script{"Hira"}));
    TESTX(una::codepoint::has_script(0x3031, una::locale::script{"Kana"}));
    TESTX(!una::codepoint::has_script(0x3031, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x3031, una::locale::script{"Zzzz"}));

    // U+061C
    TESTX(una::codepoint::get_script(0x061C) == una::locale::script{"Arab"});
    TESTX(una::codepoint::has_script(0x061C, una::locale::script{"Arab"}));
    // NOTE: Arab for U+061C is duplicated in Scripts.txt and ScriptExtensions.txt for some reason
    TESTX(una::codepoint::has_script(0x061C, una::locale::script{"Arab"}));
    TESTX(una::codepoint::has_script(0x061C, una::locale::script{"Syrc"}));
    TESTX(una::codepoint::has_script(0x061C, una::locale::script{"Thaa"}));
    TESTX(!una::codepoint::has_script(0x061C, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x061C, una::locale::script{"Zyyy"}));

    // U+10100
    TESTX(una::codepoint::get_script(0x10100) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::has_script(0x10100, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x10100, una::locale::script{"Cpmn"}));
    TESTX(una::codepoint::has_script(0x10100, una::locale::script{"Cprt"}));
    TESTX(una::codepoint::has_script(0x10100, una::locale::script{"Linb"}));
    TESTX(!una::codepoint::has_script(0x10100, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x10100, una::locale::script{"Zzzz"}));

    // U+303C
    TESTX(una::codepoint::get_script(0x303C) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::has_script(0x303C, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x303C, una::locale::script{"Hani"}));
    TESTX(una::codepoint::has_script(0x303C, una::locale::script{"Hira"}));
    TESTX(una::codepoint::has_script(0x303C, una::locale::script{"Kana"}));
    TESTX(!una::codepoint::has_script(0x303C, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x303C, una::locale::script{"Zzzz"}));

    // U+3001
    TESTX(una::codepoint::get_script(0x3001) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::has_script(0x3001, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x3001, una::locale::script{"Bopo"}));
    TESTX(una::codepoint::has_script(0x3001, una::locale::script{"Hang"}));
    TESTX(una::codepoint::has_script(0x3001, una::locale::script{"Hani"}));
    TESTX(una::codepoint::has_script(0x3001, una::locale::script{"Hira"}));
    TESTX(una::codepoint::has_script(0x3001, una::locale::script{"Kana"}));
    TESTX(una::codepoint::has_script(0x3001, una::locale::script{"Yiii"}));
    TESTX(!una::codepoint::has_script(0x3001, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x3001, una::locale::script{"Zzzz"}));

    // U+0965
    TESTX(una::codepoint::get_script(0x0965) == una::locale::script{"Zyyy"});
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Zyyy"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Beng"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Deva"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Dogr"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Gong"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Gonm"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Gran"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Gujr"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Guru"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Knda"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Limb"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Mahj"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Mlym"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Nand"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Orya"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Sind"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Sinh"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Sylo"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Takr"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Taml"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Telu"}));
    TESTX(una::codepoint::has_script(0x0965, una::locale::script{"Tirh"}));
    TESTX(!una::codepoint::has_script(0x0965, una::locale::script{"Latn"}));
    TESTX(!una::codepoint::has_script(0x0965, una::locale::script{"Zzzz"}));

    return true;
}
