/* Example for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include "uni_algo/case.h"
#include "uni_algo/ranges_word.h"

void find_whole_word_in_file(const std::string& file, std::string_view word)
{
    // Note that the algorithm works only with files in UTF-8 encoding

    // Open the file in the binary mode so std::getline
    // behaviour will be exactly the same on all platforms
    std::ifstream input(file, std::ios::binary);

    if (!input.is_open() || word.empty())
        return;

    std::size_t line_number = 0;

    for (std::string line; std::getline(input, line, '\n');)
    {
        line_number++;

        std::string_view line_view = line;

        // If the file uses CRLF remove CR
        while (!line_view.empty() && line_view.back() == '\r')
            line_view.remove_suffix(1);

        // In this example we use una::views::word_only::utf8 and una::caseless::compare_utf8
        // to find all occurrences of the whole word in the line

        auto word_view = line_view | una::views::word_only::utf8;

        for (auto it = word_view.begin(); it != una::sentinel; ++it)
        {
            if (una::caseless::compare_utf8(*it, word) == 0)
                std::cout << "Found word at " << line_number << ':' << it.begin() - line_view.begin() << '\n';
        }
    }
}

void find_string_in_file(const std::string& file, std::string_view string)
{
    // Note that the algorithm works only with files in UTF-8 encoding

    std::ifstream input(file, std::ios::binary);

    if (!input.is_open() || string.empty())
        return;

    std::size_t line_number = 0;

    for (std::string line; std::getline(input, line, '\n');)
    {
        line_number++;

        std::string_view line_view = line;

        while (!line_view.empty() && line_view.back() == '\r')
            line_view.remove_suffix(1);

        // In this example we use una::caseless::find_utf8
        // to find all occurrences of the string in the line

        std::size_t start = 0;
        while (true)
        {
            una::found found = una::caseless::find_utf8(line_view.substr(start), string);
            if (!found || string.empty())
                break;

            std::cout << "Found string at " << line_number << ':' << start + found.pos() << '\n';

            // Now we need to move start position for next search but we cannot do it like this:
            // start += found.pos() + string.size();
            // It won't work properly in Unicode because the size of the string we found may not
            // be the same as the size of the string we search for. So we must do it like this:
            start += found.end_pos();
        }
    }
}
