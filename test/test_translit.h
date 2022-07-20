/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <uni/ext/translit/macedonian_to_latin_docs.h>
#include <uni/ext/translit/japanese_kana_to_romaji_hepburn.h>

// If it fails a compiler messed up the encoding of this file
// It the compiler is MSVC then /utf-8 command line option must be used
static_assert(U'㋡' == 0x32E1);

void test_translit_macedonian_to_latin_docs()
{
    uni::translit::macedonian_to_latin_docs translit;

    // UTF-8

    TESTX(translit.utf8("Љуботен") == "Ljuboten");
    TESTX(translit.utf8("ЉУБОТЕН") == "LJUBOTEN");

    // Macedonian alphabet
    TESTX(translit.utf8("абвгдѓежзѕијклљмнњопрстќуфхцчџш") == "abvgdgjezhzdzijklljmnnjoprstkjufhcchdjsh");
    TESTX(translit.utf8("АБВГДЃЕЖЗЅИЈКЛЉМНЊОПРСТЌУФХЦЧЏШ") == "ABVGDGJEZHZDZIJKLLJMNNJOPRSTKJUFHCCHDJSH");
    TESTX(translit.utf8("АаБбВвГгДдЃѓЕеЖжЗзЅѕИиЈјКкЛлЉљМмНнЊњОоПпРрСсТтЌќУуФфХхЦцЧчЏџШш") ==
        "AaBbVvGgDdGjgjEeZhzhZzDzdzIiJjKkLlLjljMmNnNjnjOoPpRrSsTtKjkjUuFfHhCcChchDjdjShsh");

    // Decomposed "ѓ Ѓ ќ Ќ" == "gj GJ kj KJ"
    TESTX(translit.utf8("г\xCC\x81 Г\xCC\x81 к\xCC\x81 К\xCC\x81") == "gj GJ kj KJ");

    // UTF-16

    TESTX(translit.utf16(u"Љуботен") == u"Ljuboten");
    TESTX(translit.utf16(u"ЉУБОТЕН") == u"LJUBOTEN");

    // Macedonian alphabet
    TESTX(translit.utf16(u"абвгдѓежзѕијклљмнњопрстќуфхцчџш") == u"abvgdgjezhzdzijklljmnnjoprstkjufhcchdjsh");
    TESTX(translit.utf16(u"АБВГДЃЕЖЗЅИЈКЛЉМНЊОПРСТЌУФХЦЧЏШ") == u"ABVGDGJEZHZDZIJKLLJMNNJOPRSTKJUFHCCHDJSH");
    TESTX(translit.utf16(u"АаБбВвГгДдЃѓЕеЖжЗзЅѕИиЈјКкЛлЉљМмНнЊњОоПпРрСсТтЌќУуФфХхЦцЧчЏџШш") ==
        u"AaBbVvGgDdGjgjEeZhzhZzDzdzIiJjKkLlLjljMmNnNjnjOoPpRrSsTtKjkjUuFfHhCcChchDjdjShsh");

    // Decomposed "ѓ Ѓ ќ Ќ" == "gj GJ kj KJ"
    TESTX(translit.utf16(u"г\x0301 Г\x0301 к\x0301 К\x0301") == u"gj GJ kj KJ");
}

void test_translit_japanese_kana_to_romaji_hepburn()
{
    // From: https://en.wikipedia.org/wiki/Hepburn_romanization#Features

    uni::translit::japanese_kana_to_romaji_hepburn translit;

    // UTF-8

    TESTX(translit.utf8("ローマじ") == "rōmaji");

    // A + A
    TESTX(translit.utf8("おばあさん") == "obāsan"); // grandmother
    // I + I
    TESTX(translit.utf8("おにいさん") == "oniisan"); // older brother
    // U + U
    TESTX(translit.utf8("すうがく") == "sūgaku"); // mathematics
    // E + E
    TESTX(translit.utf8("おねえさん") == "onēsan"); // older sister
    // O + O
    TESTX(translit.utf8("こおり") == "kōri"); // ice
    // O + U
    TESTX(translit.utf8("がっこう") == "gakkō"); // school
    // E + I
    TESTX(translit.utf8("がくせい") == "gakusei"); // student
    // Loanwords
    TESTX(translit.utf8("セーラー") == "sērā"); // sailor
    TESTX(translit.utf8("ミイラ") == "miira"); // mummy
    // Syllabic n
    TESTX(translit.utf8("あんない") == "annai"); // guide
    TESTX(translit.utf8("しんよう") == "shin'yō"); // trust
    // Long consonants
    TESTX(translit.utf8("けっか") == "kekka"); // result
    TESTX(translit.utf8("こっち") == "kotchi"); // this way

    // Corner case
    // It just impossible to handle it algorithmically
    // some rules always interfere with each other
    // https://en.wiktionary.org/wiki/s%C5%8Dru
    TESTX(translit.utf8("ソール") == "sōru"); // sole
    TESTX(translit.utf8("ソウル") == "sōru"); // soul, must be souru

    // UTF-16

    TESTX(translit.utf16(u"ローマじ") == u"rōmaji");

    // A + A
    TESTX(translit.utf16(u"おばあさん") == u"obāsan"); // grandmother
    // I + I
    TESTX(translit.utf16(u"おにいさん") == u"oniisan"); // older brother
    // U + U
    TESTX(translit.utf16(u"すうがく") == u"sūgaku"); // mathematics
    // E + E
    TESTX(translit.utf16(u"おねえさん") == u"onēsan"); // older sister
    // O + O
    TESTX(translit.utf16(u"こおり") == u"kōri"); // ice
    // O + U
    TESTX(translit.utf16(u"がっこう") == u"gakkō"); // school
    // E + I
    TESTX(translit.utf16(u"がくせい") == u"gakusei"); // student
    // Loanwords
    TESTX(translit.utf16(u"セーラー") == u"sērā"); // sailor
    TESTX(translit.utf16(u"ミイラ") == u"miira"); // mummy
    // Syllabic n
    TESTX(translit.utf16(u"あんない") == u"annai"); // guide
    TESTX(translit.utf16(u"しんよう") == u"shin'yō"); // trust
    // Long consonants
    TESTX(translit.utf16(u"けっか") == u"kekka"); // result
    TESTX(translit.utf16(u"こっち") == u"kotchi"); // this way

    // Corner case
    TESTX(translit.utf16(u"ソール") == u"sōru"); // sole
    TESTX(translit.utf16(u"ソウル") == u"sōru"); // soul, must be souru
}
