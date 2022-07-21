/* Performance test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// All performance tests are a mess. If you want to use them you're on your own.

//#define MSVC_COMPILER
//#define ENABLE_UTF8PROC_TEST
//#define ENABLE_ICU_TEST

#ifdef MSVC_COMPILER
#include "stdafx.h"
#ifdef ICU_UTF8TO16
#pragma comment(lib, "icuio.lib")
#pragma comment(lib, "icuuc.lib")
#pragma comment(lib, "icuin.lib")
#endif
#endif
#ifdef ENABLE_ICU_TEST
#include "unicode\ustream.h"
#include "unicode\errorcode.h"
#endif
#ifdef ENABLE_UTF8PROC_TEST
#define UTF8PROC_STATIC
#include "utf8proc.h"
//#pragma comment(lib, "StaticLib1.lib")
//#include "utf8rewind.h"
#endif
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <stdexcept>
#include <uni/convert.h>
#include <uni/norm.h>

#ifdef ENABLE_UTF8PROC_TEST
std::string norm_utf8proc(std::string_view str)
{
    const char* utf8proc_string = (const char*)utf8proc_NFC((utf8proc_uint8_t*)str.data());
    std::string result = utf8proc_string;
    free((void*)utf8proc_string);
    return result;
    /*std::string result;
    result.resize(str.size() * 4);
    int32_t errors = 0;
    result.resize(utf8normalize(str.data(), str.size(), result.data(), result.size(), UTF8_NORMALIZE_DECOMPOSE, nullptr));
    //if (errors == 0)
    //	std::cout <<utf8normalize(str.data(), str.size()-10, 0, 0, UTF8_NORMALIZE_DECOMPOSE, nullptr) << " \n";
    result.shrink_to_fit();
    return result;*/
}
#endif

#ifdef ENABLE_ICU_TEST
UErrorCode icu_status = U_ZERO_ERROR;
const icu::Normalizer2 *icu_norm2 = icu::Normalizer2::getNFCInstance(icu_status);
std::string norm_ICU(std::string_view str)
{
    icu::StringPiece cs(str.data(), str.size());
    std::string data;
    icu::StringByteSink<std::string> sk(&data);
    icu_norm2->normalizeUTF8(0, cs, sk, NULL, icu_status);
    return data;
}
#endif

const size_t number_of_passes = 50000; // Text/File
//const size_t number_of_passes = 1000000; // Generator (100000 for long string)
const size_t string_length = 50; // Generator (500 for long strings)
std::vector<std::string> strs;

void fill_1()
{
    if (number_of_passes > 50000)
        throw std::runtime_error("RAM! RAM! RAM!");

    // Random text from: https://creativecommons.org/licenses/by-sa/1.0/deed.en
    std::u32string s1 = U"Attribution You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.";
    std::u32string s2 = U"Attiecinājums — Jums ir atbilstoši jāatsaucas uz darbu, norādot saiti uz licenci un jānorāda veiktās izmaiņas. Jūs varat to darīt dažādos saprātīgos veidos, bet noteikti ne tā, kas liktu domāt, ka licencētājs ir apstiprinājis jūsu darbu vai tajā veiktās izmaiņas.";
    std::u32string s3 = U"«Attribution» («Атрибуция») — Вы должны обеспечить соответствующее указание авторства, предоставить ссылку на лицензию, и обозначить изменения, если таковые были сделаны. Вы можете это делать любым разумным способом, но не таким, который подразумевал бы, что лицензиар одобряет вас или ваш способ использования произведения.";
    std::u32string s4 = U"অ্যাট্রিবিউশন — আপনাকে অবশ্যই যথাযথ কৃতিত্ব দিতে হবে, লাইসেন্সের একটি লিঙ্ক প্রদান করতে হবে, এবং কোন পরিবর্তন করা হলে তা নির্দেশ করতে হবে। আপনি যে কোন যুক্তিসঙ্গত পদ্ধতিতে তা করতে পারেন, কিন্তু এমন কোন পদ্ধতিতে নয় যাতে মনে হয় লাইসেন্সকারী আপনাকে বা আপনার এই ব্যবহারের জন্য অনুমোদন দিয়েছেন।";
    std::u32string s5 = U"署名 — 您必须给出适当的署名，提供指向本许可协议的链接，同时标明是否（对原始作品）作了修改。您可以用任何合理的方式来署名，但是不得以任何方式暗示许可人为您或您的使用背书。";
    std::u32string s6 = U"表示 — あなたは 適切なクレジットを表示し、ライセンスへのリンクを提供し、変更があったらその旨を示さなければなりません。これらは合理的であればどのような方法で行っても構いませんが、許諾者があなたやあなたの利用行為を支持していると示唆するような方法は除きます。";
    std::u32string s7 = U"저작자표시 — 적절한 출처와, 해당 라이센스 링크를 표시하고, 변경이 있는 경우 공지해야 합니다. 합리적인 방식으로 이렇게 하면 되지만, 이용 허락권자가 귀하에게 권리를 부여한다거나 귀하의 사용을 허가한다는 내용을 나타내서는 안 됩니다.";

    // MSVS messed up encoding. Again!
    static_assert(U'㋡' == 0x32E1);

    // Stress test, full string of code points that need to be decomposed -> composed
    std::u32string sx = U"\x2ADC\x0044\x031B\x0307\x0323\x0112\x0300\x2ADC\x0044\x031B\x0307\x0323\x0112\x0300\x2ADC\x0044\x031B\x0307\x0323\x0112\x0300\x2ADC\x0044\x031B\x0307\x0323\x0112\x0300\x2ADC\x0044\x031B\x0307\x0323\x0112\x0300";

    for (size_t i = 0; i < number_of_passes; i++)
    {
        std::u32string str;
        for (size_t j = 0; j < 3; j++)
        {
            str += s1 + s2 + s3 + s4 + s5 + s6 + s7;
            //str += sx + sx + sx + sx + sx + sx;
        }
        strs.emplace_back(uni::utf32to8<char32_t, char>(str));
    }
}

void fill_2()
{
    // The file contains the same data that is generated with fill_1() function above
    // The performance can be a bit different just because
    std::string file = "random_text.txt";

    std::ifstream stream;
    stream.open(file, std::ios::binary);
    if (!stream.is_open())
        throw std::runtime_error(file + " file not found");

    size_t size = (size_t)stream.tellg();
    stream.seekg(0, std::ios::end);
    size = (size_t)stream.tellg() - size;
    stream.seekg(0, std::ios::beg);

    if (number_of_passes * size > 1000000 * 500)
        throw std::runtime_error("RAM! REM! RAM!");

    std::istreambuf_iterator<char> eos;
    std::string str(std::istreambuf_iterator<char>(stream), eos);

    for (size_t i = 0; i < number_of_passes; i++)
        strs.emplace_back(str);
}

void fill_3()
{
    if (number_of_passes * string_length > 1000000 * 50)
        throw std::runtime_error("RAM! RAM! RAM!");

    // Generate random UTF8 strings, rand() is good enough for this
    srand((unsigned int)time(nullptr));
    for (size_t i = 0; i < number_of_passes; i++)
    {
        std::u32string str;
        str.resize(string_length + rand() % 10);
        for (size_t j = 0; j < str.size(); j++)
        {
            // +1 because I don't know how other implementations handle null char so it's better to avoid it

            //str[j] = rand() % 100 + 1; // 1-byte sequence (ASCII)
            str[j] = rand() % 200 + 1; // 2-byte sequence (with mix of ASCII but it's even better makes it closer to a real text)
            //str[j] = rand() % 10000 + 1; // 3-byte sequence
            //str[j] = rand() % 0x10FFF0 + 1; // 4-byte sequence
        }
        strs.emplace_back(uni::utf32to8<char32_t, char>(str));
    }
}

void test_performance();
void generate_table();

int main5()
{
    fill_1();

    test_performance();
    //generate_table();

    return 0;
}

void test_performance()
{
    unsigned int nothing = 0;

    for (int j = 0; j < 10; j++)
    {
        double duration1 = 0.0;

        {
            auto time1 = std::chrono::steady_clock::now();
            for (size_t i = 0; i < number_of_passes; i++)
            {
                std::string result = uni::norm::utf8_nfc<char>(strs[i]);
                //std::string result = norm_utf8proc(strs2[i]);
                //std::string result = norm_ICU(strs2[i]);

                // Use it somehow just because
                nothing += result.back();
            }
            auto time2 = std::chrono::steady_clock::now();
            duration1 = std::chrono::duration<double, std::milli>(time2 - time1).count();
        }

        std::cout << duration1 << '\n';
    }

    std::cout << nothing << '\n';
}

void generate_table()
{
    unsigned int nothing = 0;

    std::cout << "UNI" << '\t' << "U8PROC" << '\t' << "ICU" << '\n';

    for (size_t j = 0; j < 10; j++)
    {
        double duration1 = 0.0;
        double duration2 = 0.0;
        double duration3 = 0.0;

        {
            auto time1 = std::chrono::steady_clock::now();
            for (size_t i = 0; i < number_of_passes; i++)
            {
                std::string result = uni::norm::utf8_nfc<char>(strs[i]);
                nothing += result.back();
            }
            auto time2 = std::chrono::steady_clock::now();
            duration1 = std::chrono::duration<double, std::milli>(time2 - time1).count();
        }
#ifdef ENABLE_UTF8PROC_TEST
        {
            auto time1 = std::chrono::steady_clock::now();
            for (size_t i = 0; i < number_of_passes; i++)
            {
                std::string result = norm_utf8proc(strs[i]);
                nothing += result.back();
            }
            auto time2 = std::chrono::steady_clock::now();
            duration2 = std::chrono::duration<double, std::milli>(time2 - time1).count();
        }
#endif
#ifdef ENABLE_ICU_TEST
        {
            auto time1 = std::chrono::steady_clock::now();
            for (size_t i = 0; i < number_of_passes; i++)
            {
                std::string result = norm_ICU(strs[i]);
                nothing += result.back();
            }
            auto time2 = std::chrono::steady_clock::now();
            duration3 = std::chrono::duration<double, std::milli>(time2 - time1).count();
        }
#endif

        std::cout << duration1 << '\t' << duration2 << '\t' << duration3 << '\n';
    }

    std::cout << nothing << '\n';
}
