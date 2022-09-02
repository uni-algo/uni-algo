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
    // https://en.wikipedia.org/w/index.php?title=Hepburn_romanization&oldid=986179396
    // https://en.wikipedia.org/w/index.php?title=Hepburn_romanization&oldid=962233075

    // UTF-8

    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ローマじ") == "rōmaji");

    // A + A
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("おばあさん") == "obāsan"); // grandmother
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("じゃあく") == "jāku"); // [jaaku] evil
    // I + I
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("おにいさん") == "oniisan"); // older brother
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("おいしい") == "oishii"); // delicious
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("はいいろ") == "haiiro"); // grey
    // U + U
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("すうがく") == "sūgaku"); // mathematics
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ちゅうい") == "chūi"); // attention
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("みずうみ") == "mizūmi"); // [mizuumi] lake
    // E + E
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("おねえさん") == "onēsan"); // older sister
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ぬれえん") == "nurēn"); // [nureen] open veranda
    // O + O
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("こおり") == "kōri"); // ice
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("こおどり") == "kōdori"); // [koodori] dance
    // O + U
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("がっこう") == "gakkō"); // school
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("こうし") == "kōshi"); // [koushi] calf
    // E + I
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("がくせい") == "gakusei"); // student
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("かるい") == "karui"); // light
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("おい") == "oi"); // nephew
    // Loanwords
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("セーラー") == "sērā"); // sailor
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("タクシー") == "takushī"); // taxi
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("コンクール") == "konkūru"); // competition
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("バレーボール") == "barēbōru"); // volleyball
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ミイラ") == "miira"); // mummy
    // Syllabic n
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("あんない") == "annai"); // guide
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("かんい") == "kan'i"); // simple
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("しんよう") == "shin'yō"); // trust
    // Long consonants
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("けっか") == "kekka"); // result
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("さっさと") == "sassato"); // quickly
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ずっと") == "zutto"); // all the time
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("きっぷ") == "kippu"); // ticket
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ざっし") == "zasshi"); // magazine
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("いっしょ") == "issho"); // together
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("こっち") == "kotchi"); // this way
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("まっちゃ") == "matcha"); // matcha
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("みっつ") == "mittsu"); // three

    // Corner case (also words in square brackets above)
    // It just impossible to handle it algorithmically
    // some rules always interfere with each other
    // https://en.wiktionary.org/wiki/s%C5%8Dru
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ソール") == "sōru"); // sole
    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn("ソウル") == "sōru"); // soul, must be souru

    std::string hira = "ぁ あ ぃ い ぅ う ぇ え ぉ お か が き ぎ く\n"
                       "ぐ け げ こ ご さ ざ し じ す ず せ ぜ そ ぞ た\n"
                       "だ ち ぢ っ つ づ て で と ど な に ぬ ね の は\n"
                       "ば ぱ ひ び ぴ ふ ぶ ぷ へ べ ぺ ほ ぼ ぽ ま み\n"
                       "む め も ゃ や ゅ ゆ ょ よ ら り る れ ろ ゎ わ\n"
                       "ゐ ゑ を ん ゔ ゕ ゖ\n"
                       "きゃ きゅ きょ しゃ しゅ しょ ちゃ ちゅ ちょ にゃ にゅ にょ\n"
                       "ひゃ ひゅ ひょ みゃ みゅ みょ りゃ りゅ りょ ぎゃ ぎゅ ぎょ\n"
                       "じゃ じゅ じょ ぢゃ ぢゅ ぢょ びゃ びゅ びょ ぴゃ ぴゅ ぴょ\n";

    std::string kata = "ァ ア ィ イ ゥ ウ ェ エ ォ オ カ ガ キ ギ ク\n"
                       "グ ケ ゲ コ ゴ サ ザ シ ジ ス ズ セ ゼ ソ ゾ タ\n"
                       "ダ チ ヂ ッ ツ ヅ テ デ ト ド ナ ニ ヌ ネ ノ ハ\n"
                       "バ パ ヒ ビ ピ フ ブ プ ヘ ベ ペ ホ ボ ポ マ ミ\n"
                       "ム メ モ ャ ヤ ュ ユ ョ ヨ ラ リ ル レ ロ ヮ ワ\n"
                       "ヰ ヱ ヲ ン ヴ ヵ ヶ\n"
                       "キャ キュ キョ シャ シュ ショ チャ チュ チョ ニャ ニュ ニョ\n"
                       "ヒャ ヒュ ヒョ ミャ ミュ ミョ リャ リュ リョ ギャ ギュ ギョ\n"
                       "ジャ ジュ ジョ ヂャ ヂュ ヂョ ビャ ビュ ビョ ピャ ピュ ピョ\n";

    std::string roma = "a a i i u u e e o o ka ga ki gi ku\n"
                       "gu ke ge ko go sa za shi ji su zu se ze so zo ta\n"
                       "da chi di tsu tsu du te de to do na ni nu ne no ha\n"
                       "ba pa hi bi pi fu bu pu he be pe ho bo po ma mi\n"
                       "mu me mo ya ya yu yu yo yo ra ri ru re ro wa wa\n"
                       "wi we wo n vu ka ke\n"
                       "kya kyu kyo sha shu sho cha chu cho nya nyu nyo\n"
                       "hya hyu hyo mya myu myo rya ryu ryo gya gyu gyo\n"
                       "ja ju jo ja ju jo bya byu byo pya pyu pyo\n";

    TESTX(uni::translit::utf8_japanese_kana_to_romaji_hepburn(hira + '\n' + kata) == (roma + '\n' + roma));

    // UTF-16

    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ローマじ") == u"rōmaji");

    // A + A
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"おばあさん") == u"obāsan"); // grandmother
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"じゃあく") == u"jāku"); // [jaaku] evil
    // I + I
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"おにいさん") == u"oniisan"); // older brother
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"おいしい") == u"oishii"); // delicious
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"はいいろ") == u"haiiro"); // grey
    // U + U
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"すうがく") == u"sūgaku"); // mathematics
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ちゅうい") == u"chūi"); // attention
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"みずうみ") == u"mizūmi"); // [mizuumi] lake
    // E + E
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"おねえさん") == u"onēsan"); // older sister
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ぬれえん") == u"nurēn"); // [nureen] open veranda
    // O + O
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"こおり") == u"kōri"); // ice
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"こおどり") == u"kōdori"); // [koodori] dance
    // O + U
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"がっこう") == u"gakkō"); // school
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"こうし") == u"kōshi"); // [koushi] calf
    // E + I
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"がくせい") == u"gakusei"); // student
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"かるい") == u"karui"); // light
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"おい") == u"oi"); // nephew
    // Loanwords
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"セーラー") == u"sērā"); // sailor
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"タクシー") == u"takushī"); // taxi
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"コンクール") == u"konkūru"); // competition
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"バレーボール") == u"barēbōru"); // volleyball
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ミイラ") == u"miira"); // mummy
    // Syllabic n
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"あんない") == u"annai"); // guide
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"かんい") == u"kan'i"); // simple
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"しんよう") == u"shin'yō"); // trust
    // Long consonants
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"けっか") == u"kekka"); // result
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"さっさと") == u"sassato"); // quickly
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ずっと") == u"zutto"); // all the time
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"きっぷ") == u"kippu"); // ticket
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ざっし") == u"zasshi"); // magazine
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"いっしょ") == u"issho"); // together
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"こっち") == u"kotchi"); // this way
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"まっちゃ") == u"matcha"); // matcha
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"みっつ") == u"mittsu"); // three

    // Corner case (also words in square brackets above)
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ソール") == u"sōru"); // sole
    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(u"ソウル") == u"sōru"); // soul, must be souru

    std::u16string hira16 = u"ぁ あ ぃ い ぅ う ぇ え ぉ お か が き ぎ く\n"
                       u"ぐ け げ こ ご さ ざ し じ す ず せ ぜ そ ぞ た\n"
                       u"だ ち ぢ っ つ づ て で と ど な に ぬ ね の は\n"
                       u"ば ぱ ひ び ぴ ふ ぶ ぷ へ べ ぺ ほ ぼ ぽ ま み\n"
                       u"む め も ゃ や ゅ ゆ ょ よ ら り る れ ろ ゎ わ\n"
                       u"ゐ ゑ を ん ゔ ゕ ゖ\n"
                       u"きゃ きゅ きょ しゃ しゅ しょ ちゃ ちゅ ちょ にゃ にゅ にょ\n"
                       u"ひゃ ひゅ ひょ みゃ みゅ みょ りゃ りゅ りょ ぎゃ ぎゅ ぎょ\n"
                       u"じゃ じゅ じょ ぢゃ ぢゅ ぢょ びゃ びゅ びょ ぴゃ ぴゅ ぴょ\n";

    std::u16string kata16 = u"ァ ア ィ イ ゥ ウ ェ エ ォ オ カ ガ キ ギ ク\n"
                       u"グ ケ ゲ コ ゴ サ ザ シ ジ ス ズ セ ゼ ソ ゾ タ\n"
                       u"ダ チ ヂ ッ ツ ヅ テ デ ト ド ナ ニ ヌ ネ ノ ハ\n"
                       u"バ パ ヒ ビ ピ フ ブ プ ヘ ベ ペ ホ ボ ポ マ ミ\n"
                       u"ム メ モ ャ ヤ ュ ユ ョ ヨ ラ リ ル レ ロ ヮ ワ\n"
                       u"ヰ ヱ ヲ ン ヴ ヵ ヶ\n"
                       u"キャ キュ キョ シャ シュ ショ チャ チュ チョ ニャ ニュ ニョ\n"
                       u"ヒャ ヒュ ヒョ ミャ ミュ ミョ リャ リュ リョ ギャ ギュ ギョ\n"
                       u"ジャ ジュ ジョ ヂャ ヂュ ヂョ ビャ ビュ ビョ ピャ ピュ ピョ\n";

    std::u16string roma16 = u"a a i i u u e e o o ka ga ki gi ku\n"
                       u"gu ke ge ko go sa za shi ji su zu se ze so zo ta\n"
                       u"da chi di tsu tsu du te de to do na ni nu ne no ha\n"
                       u"ba pa hi bi pi fu bu pu he be pe ho bo po ma mi\n"
                       u"mu me mo ya ya yu yu yo yo ra ri ru re ro wa wa\n"
                       u"wi we wo n vu ka ke\n"
                       u"kya kyu kyo sha shu sho cha chu cho nya nyu nyo\n"
                       u"hya hyu hyo mya myu myo rya ryu ryo gya gyu gyo\n"
                       u"ja ju jo ja ju jo bya byu byo pya pyu pyo\n";

    TESTX(uni::translit::utf16_japanese_kana_to_romaji_hepburn(hira16 + u'\n' + kata16) == (roma16 + u'\n' + roma16));
}
