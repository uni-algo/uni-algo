/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// If it fails a compiler messed up the UTF-8 encoding of this file.
// If the compiler is MSVC then /utf-8 command line option must be used.
static_assert(U'㋡' == 0x32E1);

test_constexpr bool test_translit_macedonian_to_latin_docs()
{
    using namespace unx::translit;

    // UTF-8

    TESTX(macedonian_to_latin_docs_utf8("Љуботен") == "Ljuboten");
    TESTX(macedonian_to_latin_docs_utf8("ЉУБОТЕН") == "LJUBOTEN");

    // Macedonian alphabet
    TESTX(macedonian_to_latin_docs_utf8("абвгдѓежзѕијклљмнњопрстќуфхцчџш") == "abvgdgjezhzdzijklljmnnjoprstkjufhcchdjsh");
    TESTX(macedonian_to_latin_docs_utf8("АБВГДЃЕЖЗЅИЈКЛЉМНЊОПРСТЌУФХЦЧЏШ") == "ABVGDGJEZHZDZIJKLLJMNNJOPRSTKJUFHCCHDJSH");
    TESTX(macedonian_to_latin_docs_utf8("АаБбВвГгДдЃѓЕеЖжЗзЅѕИиЈјКкЛлЉљМмНнЊњОоПпРрСсТтЌќУуФфХхЦцЧчЏџШш") ==
                                        "AaBbVvGgDdGjgjEeZhzhZzDzdzIiJjKkLlLjljMmNnNjnjOoPpRrSsTtKjkjUuFfHhCcChchDjdjShsh");

    // Decomposed "ѓ Ѓ ќ Ќ" == "gj GJ kj KJ"
    TESTX(macedonian_to_latin_docs_utf8("г\xCC\x81 Г\xCC\x81 к\xCC\x81 К\xCC\x81") == "gj GJ kj KJ");

    // UTF-16

    TESTX(macedonian_to_latin_docs_utf16(u"Љуботен") == u"Ljuboten");
    TESTX(macedonian_to_latin_docs_utf16(u"ЉУБОТЕН") == u"LJUBOTEN");

    // Macedonian alphabet
    TESTX(macedonian_to_latin_docs_utf16(u"абвгдѓежзѕијклљмнњопрстќуфхцчџш") == u"abvgdgjezhzdzijklljmnnjoprstkjufhcchdjsh");
    TESTX(macedonian_to_latin_docs_utf16(u"АБВГДЃЕЖЗЅИЈКЛЉМНЊОПРСТЌУФХЦЧЏШ") == u"ABVGDGJEZHZDZIJKLLJMNNJOPRSTKJUFHCCHDJSH");
    TESTX(macedonian_to_latin_docs_utf16(u"АаБбВвГгДдЃѓЕеЖжЗзЅѕИиЈјКкЛлЉљМмНнЊњОоПпРрСсТтЌќУуФфХхЦцЧчЏџШш") ==
                                         u"AaBbVvGgDdGjgjEeZhzhZzDzdzIiJjKkLlLjljMmNnNjnjOoPpRrSsTtKjkjUuFfHhCcChchDjdjShsh");

    // Decomposed "ѓ Ѓ ќ Ќ" == "gj GJ kj KJ"
    TESTX(macedonian_to_latin_docs_utf16(u"г\x0301 Г\x0301 к\x0301 К\x0301") == u"gj GJ kj KJ");

    return true;
}

test_constexpr bool test_translit_japanese_kana_to_romaji_hepburn()
{
    using namespace unx::translit;

    // From: https://en.wikipedia.org/wiki/Hepburn_romanization#Features
    // https://en.wikipedia.org/w/index.php?title=Hepburn_romanization&oldid=986179396
    // https://en.wikipedia.org/w/index.php?title=Hepburn_romanization&oldid=962233075

    // UTF-8

    TESTX(japanese_kana_to_romaji_hepburn_utf8("ローマじ") == "rōmaji");

    // A + A
    TESTX(japanese_kana_to_romaji_hepburn_utf8("おばあさん") == "obāsan"); // grandmother
    TESTX(japanese_kana_to_romaji_hepburn_utf8("じゃあく") == "jāku"); // [jaaku] evil
    // I + I
    TESTX(japanese_kana_to_romaji_hepburn_utf8("おにいさん") == "oniisan"); // older brother
    TESTX(japanese_kana_to_romaji_hepburn_utf8("おいしい") == "oishii"); // delicious
    TESTX(japanese_kana_to_romaji_hepburn_utf8("はいいろ") == "haiiro"); // grey
    // U + U
    TESTX(japanese_kana_to_romaji_hepburn_utf8("すうがく") == "sūgaku"); // mathematics
    TESTX(japanese_kana_to_romaji_hepburn_utf8("ちゅうい") == "chūi"); // attention
    TESTX(japanese_kana_to_romaji_hepburn_utf8("みずうみ") == "mizūmi"); // [mizuumi] lake
    // E + E
    TESTX(japanese_kana_to_romaji_hepburn_utf8("おねえさん") == "onēsan"); // older sister
    TESTX(japanese_kana_to_romaji_hepburn_utf8("ぬれえん") == "nurēn"); // [nureen] open veranda
    // O + O
    TESTX(japanese_kana_to_romaji_hepburn_utf8("こおり") == "kōri"); // ice
    TESTX(japanese_kana_to_romaji_hepburn_utf8("こおどり") == "kōdori"); // [koodori] dance
    // O + U
    TESTX(japanese_kana_to_romaji_hepburn_utf8("がっこう") == "gakkō"); // school
    TESTX(japanese_kana_to_romaji_hepburn_utf8("こうし") == "kōshi"); // [koushi] calf
    // E + I
    TESTX(japanese_kana_to_romaji_hepburn_utf8("がくせい") == "gakusei"); // student
    TESTX(japanese_kana_to_romaji_hepburn_utf8("かるい") == "karui"); // light
    TESTX(japanese_kana_to_romaji_hepburn_utf8("おい") == "oi"); // nephew
    // Loanwords
    TESTX(japanese_kana_to_romaji_hepburn_utf8("セーラー") == "sērā"); // sailor
    TESTX(japanese_kana_to_romaji_hepburn_utf8("タクシー") == "takushī"); // taxi
    TESTX(japanese_kana_to_romaji_hepburn_utf8("コンクール") == "konkūru"); // competition
    TESTX(japanese_kana_to_romaji_hepburn_utf8("バレーボール") == "barēbōru"); // volleyball
    TESTX(japanese_kana_to_romaji_hepburn_utf8("ミイラ") == "miira"); // mummy
    // Syllabic n
    TESTX(japanese_kana_to_romaji_hepburn_utf8("あんない") == "annai"); // guide
    TESTX(japanese_kana_to_romaji_hepburn_utf8("かんい") == "kan'i"); // simple
    TESTX(japanese_kana_to_romaji_hepburn_utf8("しんよう") == "shin'yō"); // trust
    // Long consonants
    TESTX(japanese_kana_to_romaji_hepburn_utf8("けっか") == "kekka"); // result
    TESTX(japanese_kana_to_romaji_hepburn_utf8("さっさと") == "sassato"); // quickly
    TESTX(japanese_kana_to_romaji_hepburn_utf8("ずっと") == "zutto"); // all the time
    TESTX(japanese_kana_to_romaji_hepburn_utf8("きっぷ") == "kippu"); // ticket
    TESTX(japanese_kana_to_romaji_hepburn_utf8("ざっし") == "zasshi"); // magazine
    TESTX(japanese_kana_to_romaji_hepburn_utf8("いっしょ") == "issho"); // together
    TESTX(japanese_kana_to_romaji_hepburn_utf8("こっち") == "kotchi"); // this way
    TESTX(japanese_kana_to_romaji_hepburn_utf8("まっちゃ") == "matcha"); // matcha
    TESTX(japanese_kana_to_romaji_hepburn_utf8("みっつ") == "mittsu"); // three

    // Corner case (also words in square brackets above)
    // It just impossible to handle it algorithmically
    // some rules always interfere with each other
    // https://en.wiktionary.org/wiki/s%C5%8Dru
    TESTX(japanese_kana_to_romaji_hepburn_utf8("ソール") == "sōru"); // sole
    TESTX(japanese_kana_to_romaji_hepburn_utf8("ソウル") == "sōru"); // soul, must be souru

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

    TESTX(japanese_kana_to_romaji_hepburn_utf8(hira + '\n' + kata) == (roma + '\n' + roma));

    // UTF-16

    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"ローマじ") == u"rōmaji");

    // A + A
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"おばあさん") == u"obāsan"); // grandmother
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"じゃあく") == u"jāku"); // [jaaku] evil
    // I + I
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"おにいさん") == u"oniisan"); // older brother
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"おいしい") == u"oishii"); // delicious
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"はいいろ") == u"haiiro"); // grey
    // U + U
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"すうがく") == u"sūgaku"); // mathematics
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"ちゅうい") == u"chūi"); // attention
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"みずうみ") == u"mizūmi"); // [mizuumi] lake
    // E + E
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"おねえさん") == u"onēsan"); // older sister
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"ぬれえん") == u"nurēn"); // [nureen] open veranda
    // O + O
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"こおり") == u"kōri"); // ice
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"こおどり") == u"kōdori"); // [koodori] dance
    // O + U
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"がっこう") == u"gakkō"); // school
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"こうし") == u"kōshi"); // [koushi] calf
    // E + I
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"がくせい") == u"gakusei"); // student
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"かるい") == u"karui"); // light
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"おい") == u"oi"); // nephew
    // Loanwords
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"セーラー") == u"sērā"); // sailor
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"タクシー") == u"takushī"); // taxi
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"コンクール") == u"konkūru"); // competition
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"バレーボール") == u"barēbōru"); // volleyball
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"ミイラ") == u"miira"); // mummy
    // Syllabic n
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"あんない") == u"annai"); // guide
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"かんい") == u"kan'i"); // simple
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"しんよう") == u"shin'yō"); // trust
    // Long consonants
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"けっか") == u"kekka"); // result
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"さっさと") == u"sassato"); // quickly
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"ずっと") == u"zutto"); // all the time
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"きっぷ") == u"kippu"); // ticket
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"ざっし") == u"zasshi"); // magazine
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"いっしょ") == u"issho"); // together
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"こっち") == u"kotchi"); // this way
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"まっちゃ") == u"matcha"); // matcha
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"みっつ") == u"mittsu"); // three

    // Corner case (also words in square brackets above)
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"ソール") == u"sōru"); // sole
    TESTX(japanese_kana_to_romaji_hepburn_utf16(u"ソウル") == u"sōru"); // soul, must be souru

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

    TESTX(japanese_kana_to_romaji_hepburn_utf16(hira16 + u'\n' + kata16) == (roma16 + u'\n' + roma16));

    return true;
}
