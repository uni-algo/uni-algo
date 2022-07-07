/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <fstream>

#include "test_visual_data.h"

template<typename T, const T* DATA[], std::size_t DATA_SIZE>
void test_visual_broken(const std::string& filename, const std::basic_string<T>& bom)
{
    std::basic_string<T> result;
    result += bom;

    for (std::size_t i = 0; i < DATA_SIZE; i++)
    {
        result += DATA[i];
        result.push_back('\r');
        result.push_back('\n');
    }

    std::ofstream stream;
    stream.open(filename, std::ios::binary);

    if (stream.is_open())
        stream.write(reinterpret_cast<const char*>(&result[0]), static_cast<std::streamsize>(result.size() * sizeof(T)));
}

template<typename T, const T* DATA[], std::size_t DATA_SIZE, typename T2,
         std::basic_string<T2>(*FNUTF)(std::basic_string_view<T>)>
void test_visual_result(const std::string& filename, const std::basic_string<T2>& bom)
{
    std::basic_string<T2> result;
    result += bom;

    for (std::size_t i = 0; i < DATA_SIZE; i++)
    {
        result += FNUTF(DATA[i]);
        result.push_back('\r');
        result.push_back('\n');
    }

    std::ofstream stream;
    stream.open(filename, std::ios::binary);

    if (stream.is_open())
        stream.write(reinterpret_cast<const char*>(&result[0]), static_cast<std::streamsize>(result.size() * sizeof(T2)));
}

void test_visual()
{
    test_visual_broken<char, broken_utf8_test, std::size(broken_utf8_test)>("test_utf8_broken.txt", "\xEF\xBB\xBF");
    test_visual_broken<char16_t, broken_utf16_test, std::size(broken_utf16_test)>("test_utf16_broken.txt", u"\xFEFF");
    test_visual_broken<char32_t, broken_utf32_test, std::size(broken_utf32_test)>("test_utf32_broken.txt", U"\x0000FEFF");

    test_visual_result<char, broken_utf8_test, std::size(broken_utf8_test), char16_t, uni::utf8to16>("test_utf8to16_result.txt", u"\xFEFF");
    test_visual_result<char16_t, broken_utf16_test, std::size(broken_utf16_test), char, uni::utf16to8>("test_utf16to8_result.txt", "\xEF\xBB\xBF");
    test_visual_result<char32_t, broken_utf32_test, std::size(broken_utf32_test), char, uni::utf32to8>("test_utf32to8_result.txt", "\xEF\xBB\xBF");
}
