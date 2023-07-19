/* Amalgamation Generator for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

#define NOTFOUND(x, y) do {if (!(x)) {std::cout << "NOT FOUND: " << (y) << '\n'; std::exit(0);}} while(0)

// Use for files with header guards
std::string amalgam_part(const std::string& file)
{
    std::string result;
    result += "// AMALGAMATION: " + file + '\n' + '\n';

    std::ifstream input{file};
    NOTFOUND(input.is_open(), file);

    bool skip = true;
    std::string line;
    while (std::getline(input, line))
    {
        // Skip header guard and everything before
        if (skip)
        {
            if (line.substr(0, 7) == "#define")
                skip = false;
            continue;
        }

        // Comment all our includes (but keep C++ Standard Library includes)
        if (line.substr(0, 10) == "#include \"")
        {
            result += "//!" + line;
            result += '\n';
            continue;
        }
        // Remove all our includes (but keep C++ Standard Library includes)
        //if (line.substr(0, 10) == "#include \"")
        //    continue;

        //if (!line.empty()) // Remove all empty lines
        {
            result += line;
            result += '\n';
        }
    }

    // Skip header guard and everything after
    std::size_t rfind = result.rfind("#endif");
    if (rfind != std::string::npos)
    {
        result = result.substr(0, rfind);
        result += '\n';
    }

    //result += '\n';
    return result;
}

// Use for files without header guards
std::string amalgam_full(const std::string& file)
{
    std::string result;
    result += "// AMALGAMATION: " + file + '\n' + '\n';

    std::ifstream input{file};
    NOTFOUND(input.is_open(), file);

    std::string line;
    while (std::getline(input, line))
    {
        // Comment all our includes (but keep C++ Standard Library includes)
        if (line.substr(0, 10) == "#include \"")
        {
            result += "//!" + line;
            result += '\n';
            continue;
        }
        // Remove all our includes (but keep C++ Standard Library includes)
        //if (line.substr(0, 10) == "#include \"")
        //    continue;

        //if (!line.empty()) // Remove all empty lines
        {
            result += line;
            result += '\n';
        }
    }

    result += '\n';
    return result;
}

// Use to add all the content of the file
std::string amalgam_data(const std::string& file)
{
    std::string result;
    result += "// AMALGAMATION: " + file + '\n' + '\n';

    std::ifstream input{file};
    NOTFOUND(input.is_open(), file);

    result += std::string{std::istreambuf_iterator<char>{input}, std::istreambuf_iterator<char>{}};

    result += '\n';
    return result;
}

std::string amalgam_license_header()
{
    std::string license =
        "/* C++ Standard Library wrapper for Unicode Algorithms Implementation.\n"
        " * License: Public Domain or MIT - choose whatever you want.\n"
        " * See notice at the end of this file. */\n";

    return license;
}

std::string amalgam_license_footer()
{
    std::string license =
        "/* Public Domain License\n"
        " *\n"
        " * This is free and unencumbered software released into the public domain.\n"
        " *\n"
        " * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this\n"
        " * software, either in source code form or as a compiled binary, for any purpose,\n"
        " * commercial or non-commercial, and by any means.\n"
        " *\n"
        " * In jurisdictions that recognize copyright laws, the author or authors of this\n"
        " * software dedicate any and all copyright interest in the software to the public\n"
        " * domain. We make this dedication for the benefit of the public at large and to\n"
        " * the detriment of our heirs and successors. We intend this dedication to be an\n"
        " * overt act of relinquishment in perpetuity of all present and future rights to\n"
        " * this software under copyright law.\n"
        " *\n"
        " * THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        " * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        " * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
        " * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN\n"
        " * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION\n"
        " * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n"
        " *\n"
        " * MIT License\n"
        " *\n"
        " * Permission is hereby granted, free of charge, to any person obtaining a copy\n"
        " * of this software and associated documentation files (the \"Software\"), to deal\n"
        " * in the Software without restriction, including without limitation the rights\n"
        " * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
        " * copies of the Software, and to permit persons to whom the Software is\n"
        " * furnished to do so, subject to the following conditions:\n"
        " *\n"
        " * The above copyright notice and this permission notice shall be included in all\n"
        " * copies or substantial portions of the Software.\n"
        " *\n"
        " * THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
        " * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
        " * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
        " * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
        " * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
        " * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
        " * SOFTWARE.\n"
        " */\n";

    return license;
}

void amalgam(bool with_ext)
{
    std::string output_file = with_ext ? "uni_algo_ext.h" : "uni_algo.h";

    std::ofstream output{output_file};
    NOTFOUND(output.is_open(), output_file);

    output << amalgam_license_header() << '\n';

    output << "// AMALGAMATION GENERATED BY: test/random_stuff/amalgamation.h" << '\n' << '\n';

    // Include guard
    output << "#ifndef UNI_ALGO_H_AMALGAMATION" << '\n';
    output << "#define UNI_ALGO_H_AMALGAMATION" << '\n' << '\n';

    // Enable header-only (make all data tables inline constexpr)
    output << "#define UNI_ALGO_STATIC_DATA" << '\n';

    // Disable system locale or it will leak windows.h
    // And won't compile without this define at the monent anyway
    output << "#define UNI_ALGO_DISABLE_SYSTEM_LOCALE" << '\n' << '\n';

    // Config needs versions
    output << amalgam_full("uni_algo/impl/impl_cpp_lib_version.h");
    output << amalgam_full("uni_algo/impl/impl_unicode_version.h");

    // LOW-LEVEL needs config
    output << amalgam_part("uni_algo/config.h"); // Old full

    // LOW-LEVEL needs safe layer
    output << amalgam_part("uni_algo/internal/safe_layer.h");

    // LOW-LEVEL BEGIN

    output << amalgam_full("uni_algo/impl/internal_defines.h");

    // The order for data files are not important
    output << amalgam_data("uni_algo/impl/data/data_case.h");
    output << amalgam_data("uni_algo/impl/data/data_norm.h");
    output << amalgam_data("uni_algo/impl/data/data_prop.h");
    output << amalgam_data("uni_algo/impl/data/data_script.h");
    output << amalgam_data("uni_algo/impl/data/data_segment_grapheme.h");
    output << amalgam_data("uni_algo/impl/data/data_segment_word.h");

    output << amalgam_part("uni_algo/impl/internal_stages.h"); // Old full

    // The order for implementation files are important
    output << amalgam_part("uni_algo/impl/impl_iter.h"); // Must be first
    output << amalgam_part("uni_algo/impl/impl_conv.h");
    output << amalgam_part("uni_algo/impl/impl_segment_grapheme.h");
    output << amalgam_part("uni_algo/impl/impl_segment_word.h"); // Must be before case
    output << amalgam_part("uni_algo/impl/impl_locale.h"); // Must be before case
    output << amalgam_part("uni_algo/impl/impl_case.h");
    output << amalgam_part("uni_algo/impl/impl_case_locale.h"); // Must be after case
    output << amalgam_part("uni_algo/impl/impl_script.h");
    output << amalgam_part("uni_algo/impl/impl_prop.h"); // Must be before norm
    output << amalgam_part("uni_algo/impl/impl_norm.h");

    output << amalgam_full("uni_algo/impl/internal_undefs.h");

    // LOW-LEVEL END

    // WRAPPER BEGIN

    output << amalgam_part("uni_algo/version.h"); // Old full

    // Internals must be first
    output << amalgam_part("uni_algo/internal/error.h");
    output << amalgam_part("uni_algo/internal/found.h");
    output << amalgam_part("uni_algo/internal/ranges_core.h");
    output << amalgam_part("uni_algo/internal/ranges_translit.h");

    // Use the same order as low-level
    output << amalgam_part("uni_algo/conv.h");
    output << amalgam_part("uni_algo/locale.h");
    output << amalgam_part("uni_algo/case.h");
    output << amalgam_part("uni_algo/script.h");
    output << amalgam_part("uni_algo/prop.h");
    output << amalgam_part("uni_algo/norm.h");
    output << amalgam_part("uni_algo/ranges.h");
    output << amalgam_part("uni_algo/ranges_conv.h");
    output << amalgam_part("uni_algo/ranges_norm.h");
    output << amalgam_part("uni_algo/ranges_grapheme.h");
    output << amalgam_part("uni_algo/ranges_word.h");

    // WRAPPER END

    // EXTENSIONS BEGIN
    if (with_ext)
    {
        // ASCII
        output << amalgam_part("uni_algo/ext/ascii.h");

        // Transliterators
        output << amalgam_part("uni_algo/ext/translit/macedonian_to_latin_docs.h");
        output << amalgam_part("uni_algo/ext/translit/japanese_kana_to_romaji_hepburn.h");
    }
    // EXTENSIONS END

    // Include guard
    output << "#endif // UNI_ALGO_H_AMALGAMATION" << '\n' << '\n';

    output << amalgam_license_footer(); // This function produces '\n' at the end of the file
}
