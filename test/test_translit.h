/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// If it fails a compiler messed up the UTF-8 encoding of this file.
// If the compiler is MSVC then /utf-8 command line option must be used.
static_assert(U'㋡' == 0x32E1);

void test_translit_macedonian_to_latin_docs()
{
    // UTF-8

    TESTX(uni::translit::utf8_macedonian_to_latin_docs("Љуботен") == "Ljuboten");
    TESTX(uni::translit::utf8_macedonian_to_latin_docs("ЉУБОТЕН") == "LJUBOTEN");

    // Macedonian alphabet
    TESTX(uni::translit::utf8_macedonian_to_latin_docs("абвгдѓежзѕијклљмнњопрстќуфхцчџш") == "abvgdgjezhzdzijklljmnnjoprstkjufhcchdjsh");
    TESTX(uni::translit::utf8_macedonian_to_latin_docs("АБВГДЃЕЖЗЅИЈКЛЉМНЊОПРСТЌУФХЦЧЏШ") == "ABVGDGJEZHZDZIJKLLJMNNJOPRSTKJUFHCCHDJSH");
    TESTX(uni::translit::utf8_macedonian_to_latin_docs("АаБбВвГгДдЃѓЕеЖжЗзЅѕИиЈјКкЛлЉљМмНнЊњОоПпРрСсТтЌќУуФфХхЦцЧчЏџШш") ==
                                                       "AaBbVvGgDdGjgjEeZhzhZzDzdzIiJjKkLlLjljMmNnNjnjOoPpRrSsTtKjkjUuFfHhCcChchDjdjShsh");

    // Decomposed "ѓ Ѓ ќ Ќ" == "gj GJ kj KJ"
    TESTX(uni::translit::utf8_macedonian_to_latin_docs("г\xCC\x81 Г\xCC\x81 к\xCC\x81 К\xCC\x81") == "gj GJ kj KJ");

    // UTF-16

    TESTX(uni::translit::utf16_macedonian_to_latin_docs(u"Љуботен") == u"Ljuboten");
    TESTX(uni::translit::utf16_macedonian_to_latin_docs(u"ЉУБОТЕН") == u"LJUBOTEN");

    // Macedonian alphabet
    TESTX(uni::translit::utf16_macedonian_to_latin_docs(u"абвгдѓежзѕијклљмнњопрстќуфхцчџш") == u"abvgdgjezhzdzijklljmnnjoprstkjufhcchdjsh");
    TESTX(uni::translit::utf16_macedonian_to_latin_docs(u"АБВГДЃЕЖЗЅИЈКЛЉМНЊОПРСТЌУФХЦЧЏШ") == u"ABVGDGJEZHZDZIJKLLJMNNJOPRSTKJUFHCCHDJSH");
    TESTX(uni::translit::utf16_macedonian_to_latin_docs(u"АаБбВвГгДдЃѓЕеЖжЗзЅѕИиЈјКкЛлЉљМмНнЊњОоПпРрСсТтЌќУуФфХхЦцЧчЏџШш") ==
                                                        u"AaBbVvGgDdGjgjEeZhzhZzDzdzIiJjKkLlLjljMmNnNjnjOoPpRrSsTtKjkjUuFfHhCcChchDjdjShsh");

    // Decomposed "ѓ Ѓ ќ Ќ" == "gj GJ kj KJ"
    TESTX(uni::translit::utf16_macedonian_to_latin_docs(u"г\x0301 Г\x0301 к\x0301 К\x0301") == u"gj GJ kj KJ");
}

void test_translit_japanese_kana_to_romaji_hepburn()
{
    // From: https://en.wikipedia.org/wiki/Hepburn_romanization#Features

    // UTF-8

    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ローマじ") == "rōmaji");

    // A + A
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("おばあさん") == "obāsan"); // grandmother
    // I + I
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("おにいさん") == "oniisan"); // older brother
    // U + U
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("すうがく") == "sūgaku"); // mathematics
    // E + E
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("おねえさん") == "onēsan"); // older sister
    // O + O
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("こおり") == "kōri"); // ice
    // O + U
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("がっこう") == "gakkō"); // school
    // E + I
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("がくせい") == "gakusei"); // student
    // Loanwords
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("セーラー") == "sērā"); // sailor
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ミイラ") == "miira"); // mummy
    // Syllabic n
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("あんない") == "annai"); // guide
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("しんよう") == "shin'yō"); // trust
    // Long consonants
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("けっか") == "kekka"); // result
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("こっち") == "kotchi"); // this way

    // Corner case
    // It just impossible to handle it algorithmically
    // some rules always interfere with each other
    // https://en.wiktionary.org/wiki/s%C5%8Dru
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ソール") == "sōru"); // sole
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ソウル") == "sōru"); // soul, must be souru

    // UTF-16

    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ローマじ") == u"rōmaji");

    // A + A
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"おばあさん") == u"obāsan"); // grandmother
    // I + I
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"おにいさん") == u"oniisan"); // older brother
    // U + U
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"すうがく") == u"sūgaku"); // mathematics
    // E + E
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"おねえさん") == u"onēsan"); // older sister
    // O + O
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"こおり") == u"kōri"); // ice
    // O + U
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"がっこう") == u"gakkō"); // school
    // E + I
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"がくせい") == u"gakusei"); // student
    // Loanwords
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"セーラー") == u"sērā"); // sailor
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ミイラ") == u"miira"); // mummy
    // Syllabic n
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"あんない") == u"annai"); // guide
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"しんよう") == u"shin'yō"); // trust
    // Long consonants
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"けっか") == u"kekka"); // result
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"こっち") == u"kotchi"); // this way

    // Corner case
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ソール") == u"sōru"); // sole
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ソウル") == u"sōru"); // soul, must be souru
}
