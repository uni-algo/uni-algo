/* Generator for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

// How to use: just copy paste this file into your Hello World or whatever and download Unicode data files.
// Check generator() function at the end of the file for a list of the files that are needed for the generator.

// If any assert fails then the generated data must not be used by the library.
//#define ASSERTX(x) if (!(x)) {std::cout << "ASSERT: " << std::string(__FILE__) << ":" << std::to_string(__LINE__) << '\n'; throw std::runtime_error("ERROR");}
// Make it possible to compile with -fno-exceptions the same way we do with our tests
#define ASSERTX(x) if (!(x)) {std::cout << "ASSERT: " << (__FILE__) << ":" << (__LINE__) << '\n'; exit(0);}

// The lowest Unicode version that the generator does work with is 6.0.0
// because DerivedCoreProperties.txt doesn't have properties that we need in lower versions.
// Note that the lowest version for break modules is Unicode 11.0.0 at the moment because break rules change very often.

// TODO: This define is very old and useless. So probably just clean this.
// Uncomment it to use Tailored Casing for German Sharp S (Eszett)
// LATIN SMALL LETTER SHARP S -> LATIN CAPITAL LETTER SHARP S instead of LATIN SMALL LETTER SHARP -> SS
// We don't use it by default for better compatibility with other implementations
// and the implementation won't be conformant with 3.13 Default Case Algorithms if we do this.
// https://www.unicode.org/versions/Unicode12.0.0/ch05.pdf -> 5.18 Case Mappings -> Figure 5-16
// https://www.unicode.org/charts/PDF/U0080.pdf -> LATIN SMALL LETTER SHARP S
//#define GENERATOR_CASEMAP_ESZETT_TAILORED_CASING

// TODO: The generator is a bit of a mess but it does the job perfectly fine.
// TODO: Some of the tables can be optimized for the size much more.
// For example the data can be compressed: https://en.wikipedia.org/wiki/Trie -> Compressing tries
// In this case for stage1/stage2 tables we need another table (?) for every stage1/stage2 with offsets
// (sparse offsets) to get rid of all 0,0,0,0,0 in these tables (probably only in stage2?).
// For stage3_comp that is actually enormous we can store the offsets inside the table.
// For stage3_decomp_nfd/stage3_decomp_nfkd we can use UTF-16 instead of UTF-32.
// etc.
//
// Performance consideration (just some thoughts).
// The problem with these optimizations it will make the lookup more complicated so
// it will affect the performance for example we use uppercase function with 1 random language
// we need 1 cache line for stage1 table and ~128 * 4 / 64 = 8 cache lines for stage2
// (~1 block in stage2 * sizeof(code point) / cache line size)
// so 9 cache lines total. In the worst case scenario we need to load them from DRAM
// but then L1d does the job, so we need to do it once, but if we do complicate the lookup
// there will be extra instructions for every code point (every lookup).
// So even if we decrease the size of the tables and need like 7 cache lines
// it won't gain us much but we can lose much. The only actual problem with bigger tables that
// we are wasting L1d for no reason that may lead to more cache misses and may affect
// the performance but it's hard to predict how much and it is almost impossible to test properly.
// But normalization tables must be optimized anyway
// slow lookup is not a problem there because it is part of slow path anyway.
// -------

// If true then store the data in the compact format.
// The format is unreadable and for tests or when comparing 2 data files are needed the format can be disabled.
// Note that it affects only the size of the generated files but does not affect the size of the data.
const bool COMPACT = true;

static void new_generator_output(const std::string& file1, const std::string& file2,
                                 int stage1_bits, int stage2_bits, bool with_null,
                                 std::map<uint32_t, uint32_t>& map, uint32_t maxmap = 0x10FFFF)
{
    // Two-stage tables are used to reduce Unicode data tables.
    // This is recommended by Unicode Standard see chapter 5.1 for more details.
    // This also gives us O(1) complexety always for all Unicode data tables.

    // All output files must be opened in text mode.
    // We want platform dependent line ending for them.

    std::ofstream output1(file1);
    std::ofstream output2(file2);

    ASSERTX(output1.is_open() && output2.is_open());

    ASSERTX((stage1_bits == 8 || stage1_bits == 16));
    ASSERTX((stage2_bits == 8 || stage2_bits == 16 || stage2_bits == 24 || stage2_bits == 32));

    std::vector<uint32_t> vec1; // Stage 1 table with offsets to blocks
    std::vector<std::vector<uint32_t>> vec2; // Stage 2 table with blocks of data

    // Use block size 128 for all tables. I experimented with block size 256 and tables are a bit bigger for most data.
    const uint32_t block_size = 128;
    static_assert((0x10FFFF + 1) % block_size == 0);

    for (uint32_t i = 0; i <= maxmap;)
    {
        std::vector<uint32_t> block;

        for (uint32_t j = 0; j < block_size; ++j, ++i)
        {
            // allkeys.txt data is a bit different 0 means unassigned
            // Actually it used for everything now
            // only generator_unicodedata() and generator_casefolding()
            // generates data with with_null=false format
            if (with_null)
            {
                if (map.at(i) != 0)
                    block.push_back(map.at(i));
                else
                    block.push_back(0);
            }
            else
            {
                if (map.at(i) != i)
                    block.push_back(map.at(i));
                else
                    block.push_back(0);
            }
        }

        auto find = std::find(vec2.begin(), vec2.end(), block);

        if (find == vec2.end())
        {
            vec1.push_back(vec2.size());
            vec2.push_back(block);
        }
        else
        {
            vec1.push_back(find - vec2.begin());
        }
    }

    for (size_t i = 0; i < vec1.size(); ++i)
    {
        if (i != 0 && i % 8 == 0)
            output1 << '\n';

        char str[64] = {};
        if (stage1_bits == 16) // 16 bit
        {
            ASSERTX(vec1[i] <= 0xFFFF);
            //snprintf(str, 64, COMPACT ? "%u," : "0x%.4X, ", vec1[i]);
            //snprintf(str, 64, COMPACT ? "%u," : "0x%04X, ", vec1[i]);
            snprintf(str, 64, COMPACT ? "%u," : "%5u, ", vec1[i]);
        }
        else if (stage1_bits == 8) // 8 bit
        {
            ASSERTX(vec1[i] <= 0xFF);
            //snprintf(str, 64, COMPACT ? "%u," : "0x%.2X, ", vec1[i]);
            //snprintf(str, 64, COMPACT ? "%u," : "0x%02X, ", vec1[i]);
            snprintf(str, 64, COMPACT ? "%u," : "%3u, ", vec1[i]);
        }
        output1 << str;
    }

    for (size_t i = 0; i < vec2.size(); ++i)
    {
        output2 << "/* Block: " << i << " */";

        for (size_t j = 0; j < vec2[i].size(); ++j)
        {
            if (j % 8 == 0)
                output2 << '\n';

            char str[64] = {};
            if (stage2_bits == 24) // 32 bit (but max code point size)
            {
                ASSERTX(vec2[i][j] <= 0x10FFFF);
                snprintf(str, 64, COMPACT ? "%u," : "0x%.6X, ", vec2[i][j]);
            }
            else if (stage2_bits == 16) // 16 bit
            {
                ASSERTX(vec2[i][j] <= 0xFFFF);
                snprintf(str, 64, COMPACT ? "%u," : "0x%.4X, ", vec2[i][j]);
            }
            else if (stage2_bits == 8) // 8 bit
            {
                ASSERTX(vec2[i][j] <= 0xFF);
                snprintf(str, 64, COMPACT ? "%u," : "0x%.2X, ", vec2[i][j]);
            }
            else if (stage2_bits == 32) // 32 bit
            {
                snprintf(str, 64, COMPACT ? "%u," : "0x%.8X, ", vec2[i][j]);
            }
            output2 << str;
        }

        output2 << '\n';
    }
}

static void new_generator_unicodedata(const std::string& file1, const std::string& file2, uint32_t column)
{
    // Note that UnicodeData.txt contains ranges for some code points,
    // search for "First>;" and "Last>;" in UnicodeData.txt
    // we don't handle it here because it doesn't matter for this data
    // but if this algorithm will be used to get something else
    // for example General_Category then it must be handled.

    // All Unicode data files must be opened in binary mode.
    // Opening them in text mode shouldn't break anything
    // but it better to be consistent on all platforms.

    // https://www.unicode.org/reports/tr44/#UnicodeData.txt
    std::ifstream input("UnicodeData.txt", std::ios::binary);
    ASSERTX(input.is_open());

    const uint32_t maxmap = 0x10FFFF; // Do not change!

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= maxmap; ++i)
        map[i] = i;

    std::string line;
    while (std::getline(input, line))
    {
        uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);
        uint32_t n = c;

        for (uint32_t i = 0, col = 0; i < line.size(); ++i)
        {
            if (line[i] == ';')
            {
                col++;
                if (col == column) // 12 - upper, 13 - lower, 14 - title
                {
                    n = (uint32_t)strtoul(line.c_str()+i+1, 0, 16);

                    // Test: UTF8 2,3 sequence crossmap - this is possible
                    //ASSERTX(!((c <= 0x7FF && n > 0x7FF) || (n && n <= 0x7FF && c > 0x7FF)));
                    // Test: UTF16 1,2 sequence crossmap - this should never happen
                    ASSERTX(!((c <= 0xFFFF && n > 0xFFFF) || (n && n <= 0xFFFF && c > 0xFFFF)));

#ifdef GENERATOR_CASEMAP_ESZETT_TAILORED_CASING
                    if (c == 0x00DF && (column == 12 || column == 14))
                        n = 0x1E9E;
#endif
                    if (n == 0)
                        n = c;

                    break;
                }
            }
        }

        if (c <= maxmap && n <= maxmap)
            map.at(c) = n;
    }

    new_generator_output(file1, file2, 8, 24, false, map);
}

static void new_generator_casefolding(const std::string& file1, const std::string& file2)
{
    // https://www.unicode.org/reports/tr44/#CaseFolding.txt
    std::ifstream input("CaseFolding.txt", std::ios::binary);
    ASSERTX(input.is_open());

    const uint32_t maxmap = 0x10FFFF; // Do not change!

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= maxmap; ++i)
        map[i] = i;

    std::string line;
    while (std::getline(input, line))
    {
        // 0x0000 - 0xFFFF
        if (line.size() > 14 && line[4] == ';' && (line[6] == 'C' || line[6] == 'S'))
        {
            uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);
            uint32_t n = (uint32_t)strtoul(line.c_str()+9, 0, 16);
            if (c <= maxmap && n <= maxmap)
                map.at(c) = n;
        }
        // 0xFFFF - 0x10FFFF
        else if (line.size() > 15 && line[5] == ';' && (line[7] == 'C' || line[7] == 'S'))
        {
            uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);
            uint32_t n = (uint32_t)strtoul(line.c_str()+10, 0, 16);
            if (c <= maxmap && n <= maxmap)
                map.at(c) = n;
        }
    }

    new_generator_output(file1, file2, 8, 24, false, map);
}

static void new_generator_allkeys(const std::string& file1, const std::string& file2)
{
    // DUCET (Default Unicode Collation Element Table)
    std::ifstream input("allkeys.txt", std::ios::binary);
    ASSERTX(input.is_open());

    const uint32_t maxmap = 0x10FFFF; // Do not change!

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= maxmap; ++i)
        map[i] = 0; // 0 means unassigned code point

    // Get only the order of code points from DUCET
    std::string line;
    for (uint32_t index = 0; std::getline(input, line);)
    {
        if (line.size() > 8 && line[6] == ';' && line[8] == '[')
        {
            uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);
            if (c <= maxmap)
                map.at(c) = ++index;
        }
    }

    new_generator_output(file1, file2, 16, 16, true, map);
}

static void new_generator_output1(const std::string& file, const std::vector<uint32_t>& vec)
{
    std::ofstream output(file);
    ASSERTX(output.is_open());

    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        if (i == 0)
            output << "{";
        else if (i != 0 && i % 4 == 0)
            output << "},\n{";
        else
            output << (COMPACT ? "," : ", ");

        ASSERTX(vec[i] <= 0x10FFFF); // 32 bit (but max code point size)

        char str[64] = {};
        snprintf(str, 64, COMPACT ? "%u" : "0x%.4X", vec[i]);

        output << str;
    }
    output << "}";
}

static void new_generator_casefolding_full(const std::string& file1, const std::string& file2, const std::string& file3)
{
    // https://www.unicode.org/reports/tr44/#CaseFolding.txt
    std::ifstream input("CaseFolding.txt", std::ios::binary);
    ASSERTX(input.is_open());

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= 0x10FFFF; ++i)
        map[i] = 0;

    std::vector<uint32_t> vec(4, 0);

    std::string line;
    while (std::getline(input, line))
    {
        // There should not be cases where code point > 0xFFFF and full (F) case folding
        ASSERTX(!(line.size() > 15 && line[5] == ';' && line[7] == 'F'));

        if (line.size() > 15 && line[4] == ';' && line[6] == 'F')
        {
            uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);

            uint32_t special[3];
            uint32_t number = 0;
            for (size_t i = 8; i < line.size(); ++i)
            {
                if (line[i] == ';')
                    break;
                else if (line[i] == ' ')
                {
                    uint32_t n = (uint32_t)strtoul(line.c_str()+i+1, 0, 16);
                    ASSERTX(number < 3); // Max is 3
                    special[number++] = n;
                }
            }

            map.at(c) = vec.size() / 4;
            vec.push_back(number);
            for (uint32_t i = 0; i < 3; ++i)
                vec.push_back(i < number ? special[i] : 0);
        }
    }

    new_generator_output(file1, file2, 8, 8, true, map, 0xFFFF);
    new_generator_output1(file3, vec);
}

static void new_generator_special_casing(const std::string& file1, const std::string& file2, const std::string& file3, size_t column)
{
    // https://www.unicode.org/reports/tr44/#SpecialCasing.txt
    std::ifstream input("SpecialCasing.txt", std::ios::binary);
    ASSERTX(input.is_open());

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= 0x10FFFF; ++i)
        map[i] = 0;

    std::vector<uint32_t> vec(4, 0);

    std::string line;
    while (std::getline(input, line))
    {
        if (line.size() > 15 && line[4] == ';')
        {
            std::string codepoint = line.substr(0, line.find(';'));

            uint32_t c = (uint32_t)strtoul(codepoint.c_str(), 0, 16);

            // There should not be cases where code point > 0xFFFF
            ASSERTX(codepoint.size() == 4 && c <= 0xFFFF);

#ifdef GENERATOR_CASEMAP_ESZETT_TAILORED_CASING
            if (c == 0x00DF)
                continue;
#endif

            std::string result;
            bool condidional = false;

            for (size_t i = 0, col = 0; i < line.size(); ++i)
            {
                if (line[i] == ';')
                {
                    col++;
                    if (col == column) // 1 - lower, 2 - title, 3 - upper
                        result = line.substr(i+2, line.find(';', i+2)-(i+2));
                    if (col == 4 && line[i+2] != '#')
                        condidional = true;
                }
            }

            ASSERTX(!result.empty() || condidional);

            // Skip conditional and 1 to 1 mapping (this is already in UnicodeData.txt)
            if (!condidional && result.size() != 4)
            {
                uint32_t special[3];
                uint32_t number = 0;
                for (size_t i = 0; i < result.size(); ++i)
                {
                    if (i == 0 || result[i] == ' ')
                    {
                        uint32_t n = (uint32_t)strtoul(result.c_str()+i, 0, 16);
                        ASSERTX(n <= 0xFFFF); // There should not be cases where code point > 0xFFFF
                        ASSERTX(number < 3); // Max is 3
                        special[number++] = n;
                    }
                }

                map.at(c) = vec.size() / 4;
                vec.push_back(number);
                for (uint32_t i = 0; i < 3; ++i)
                    vec.push_back(i < number ? special[i] : 0);
            }
        }
    }

    // In the current version of Unicode (3.2.0 - 13.0.0) there is only 1 special case for lower and we handle it in place
    // if it changes in the future we have this assert and then we probably should handle lower the same way as upper
    // but it probably never changes.
    ASSERTX(!(column == 1 && (vec.size() / 4 - 1) != 1));

    new_generator_output(file1, file2, 8, 8, true, map, 0xFFFF);
    new_generator_output1(file3, vec);
}

static void new_generator_output2(const std::string& file, const std::vector<uint32_t>& vec)
{
    std::ofstream output(file);
    ASSERTX(output.is_open());

    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        if (i != 0 && i % 8 == 0)
            output << '\n';

        ASSERTX(vec[i] <= 0x10FFFF); // 32 bit (but max code point size)

        char str[64] = {};
        snprintf(str, 64, COMPACT ? "%u," : "0x%.6X, ", vec[i]);

        output << str;
    }
}

static void new_generator_unicodedata_decompose_ccc_qc(const std::string& file1, const std::string& file2, const std::string& file3, bool compat,
                                                       const std::string& file4 = {}, const std::string& file5 = {})
{
    // https://www.unicode.org/reports/tr44/#UnicodeData.txt
    std::ifstream input("UnicodeData.txt", std::ios::binary);
    ASSERTX(input.is_open());

    std::map<uint32_t, std::vector<uint32_t>> map_vec;

    // Add all code points in NFD
    std::string line;
    while (std::getline(input, line))
    {
        uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);

        for (size_t i = 0, col = 0; i < line.size(); ++i)
        {
            if (line[i] == ';')
            {
                col++;
                if (col == 5)
                {
                    if (line[i+1] == ';')
                        break;

                    if (line[i+1] == '<') // Skip NFKD
                        break;

                    uint32_t decomp[2];
                    uint32_t number = 0;
                    for (size_t j = i+1; j < line.size(); ++j)
                    {
                        if (line[j] == ';')
                            break;
                        else if (j == i+1 || line[j] == ' ')
                        {
                            uint32_t n = (uint32_t)strtoul(line.c_str()+j, 0, 16);
                            ASSERTX(number < 2); // Max is 2
                            decomp[number++] = n;
                        }
                    }

                    std::vector<uint32_t> vec;
                    for (uint32_t j = 0; j < number; ++j)
                        vec.push_back(decomp[j]);

                    map_vec[c] = vec;

                    break;
                }
            }
        }
    }

    // Fully decompose all code points to NFD (max code points in NFD will be 4)
    // It increases the data just a little bit and we don't need to bother with this in the code
    for (uint32_t c = 0; c <= 0x10FFFF; ++c)
    {
        if (map_vec.count(c))
        {
            std::vector<uint32_t> vec_full = map_vec.at(c);
            std::vector<uint32_t> vec_temp;
            while (true)
            {
                vec_temp.clear();
                for (size_t i = 0; i < vec_full.size(); ++i)
                {
                    if (map_vec.count(vec_full[i]))
                        vec_temp.insert(vec_temp.end(),
                            map_vec.at(vec_full[i]).begin(), map_vec.at(vec_full[i]).end());
                    else
                        vec_temp.push_back(vec_full[i]);
                }

                if (vec_full == vec_temp)
                    break;
                vec_full = vec_temp;
            }

            map_vec.at(c) = vec_full;

            ASSERTX(vec_full.size() <= 4); // max is 4
        }
    }

    // And add code points in NFKD
    if (compat)
    {
        input.clear();
        input.seekg(0, std::ios::beg);

        std::string line;
        while (std::getline(input, line))
        {
            uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);

            for (size_t i = 0, col = 0; i < line.size(); ++i)
            {
                if (line[i] == ';')
                {
                    col++;
                    if (col == 5)
                    {
                        if (line[i+1] == ';')
                            break;

                        if (line[i+1] != '<') // Skip NFD
                            break;

                        i = line.find('>', i) + 1;

                        uint32_t decomp[18];
                        uint32_t number = 0;
                        for (size_t j = i+1; j < line.size(); ++j)
                        {
                            if (line[j] == ';')
                                break;
                            else if (j == i+1 || line[j] == ' ')
                            {
                                uint32_t n = (uint32_t)strtoul(line.c_str()+j, 0, 16);
                                ASSERTX(number < 18); // Max is 18
                                decomp[number++] = n;
                            }
                        }

                        std::vector<uint32_t> vec;
                        for (uint32_t j = 0; j < number; ++j)
                            vec.push_back(decomp[j]);

                        ASSERTX(map_vec.count(c) == 0);

                        map_vec[c] = vec;

                        break;
                    }
                }
            }
        }

        // Copy paste of the code above
        // Fully decompose all code points to NFKD (max code points in NFKD will be 18)
        // Note that we decompose NFD codepoints again but this time using NFKD data
        // Also this means that fully decomposed NFD and NFKD data will be different and tables cannot be merged
        for (uint32_t c = 0; c <= 0x10FFFF; ++c)
        {
            if (map_vec.count(c))
            {
                std::vector<uint32_t> vec_full = map_vec.at(c);
                std::vector<uint32_t> vec_temp;
                while (true)
                {
                    vec_temp.clear();
                    for (size_t i = 0; i < vec_full.size(); ++i)
                    {
                        if (map_vec.count(vec_full[i]))
                            vec_temp.insert(vec_temp.end(),
                                map_vec.at(vec_full[i]).begin(), map_vec.at(vec_full[i]).end());
                        else
                            vec_temp.push_back(vec_full[i]);
                    }

                    if (vec_full == vec_temp)
                        break;
                    vec_full = vec_temp;
                }

                map_vec.at(c) = vec_full;

                ASSERTX(vec_full.size() <= 18); // max is 18
            }
        }
    }

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= 0x10FFFF; ++i)
        map[i] = 0;

    std::vector<uint32_t> vec(1, 0);

    for (uint32_t c = 0; c <= 0x10FFFF; ++c)
    {
        if (map_vec.count(c))
        {
            map.at(c) = vec.size();
            vec.push_back(map_vec.at(c).size());
            vec.insert(vec.end(), map_vec.at(c).begin(), map_vec.at(c).end());
            // Test: decrease data size
            // first remove vec.push_back(map_vec.at(c).size());
            //vec[vec.size() - map_vec.at(c).size()] = (map_vec.at(c).size() << 24) | vec[vec.size() - map_vec.at(c).size()];
            // looks like bad approach actually
        }
    }

    new_generator_output(file1, file2, 8, 16, true, map, 0x10FFFF);
    new_generator_output2(file3, vec);

    // Generate ccc_qc data here too because we need map_vec with NFKD data generated before
    if (compat)
    {
        std::map<uint32_t, uint32_t> map;

        for (uint32_t i = 0; i <= 0x10FFFF; ++i)
            map[i] = 0;

        input.clear();
        input.seekg(0, std::ios::beg);

        std::string line;
        while (std::getline(input, line))
        {
            uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);

            for (size_t i = 0, col = 0; i < line.size(); ++i)
            {
                if (line[i] == ';')
                {
                    col++;
                    if (col == 3)
                    {
                        map.at(c) = (uint32_t)atoi(line.c_str()+i+1);

                        break;
                    }
                }
            }
        }

        // https://www.unicode.org/reports/tr44/#DerivedNormalizationProps.txt
        std::ifstream input_props("DerivedNormalizationProps.txt", std::ios::binary);
        ASSERTX(input_props.is_open());

        line.clear();
        while (std::getline(input_props, line))
        {
            std::size_t semicolon = line.find(';');

            if (line.size() > 15 && semicolon != std::string::npos)
            {
                bool NFC_QC_N = false;
                bool NFC_QC_M = false;
                bool NFD_QC_N = false;
                bool NFKC_QC_N = false;
                bool NFKC_QC_M = false;
                bool NFKD_QC_N = false;

                if (line.find("; NFC_QC; N", semicolon) == semicolon)
                    NFC_QC_N = true;
                if (line.find("; NFC_QC; M", semicolon) == semicolon)
                    NFC_QC_M = true;
                if (line.find("; NFD_QC; N", semicolon) == semicolon)
                    NFD_QC_N = true;
                if (line.find("; NFKC_QC; N", semicolon) == semicolon)
                    NFKC_QC_N = true;
                if (line.find("; NFKC_QC; M", semicolon) == semicolon)
                    NFKC_QC_M = true;
                if (line.find("; NFKD_QC; N", semicolon) == semicolon)
                    NFKD_QC_N = true;

                if (NFC_QC_N || NFC_QC_M || NFD_QC_N || NFKC_QC_N || NFKC_QC_M || NFKD_QC_N)
                {
                    uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                    uint32_t c2 = c1;
                    std::size_t dots = line.find("..");
                    if (dots != std::string::npos)
                        c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                    for (uint32_t i = c1; i <= c2; ++i)
                    /*{
                        map.at(i) |=
                            NFC_QC_N  << 8  |
                            NFC_QC_M  << 9  |
                            NFD_QC_N  << 10 |
                            NFKC_QC_N << 11 |
                            NFKC_QC_M << 12 |
                            NFKD_QC_N << 13;
                    }*/
                    {
                        map.at(i) |=
                            (NFC_QC_N || NFC_QC_M)   << 8  |
                            NFD_QC_N                 << 9  |
                            (NFKC_QC_N || NFKC_QC_M) << 10 |
                            NFKD_QC_N                << 11;
                    }
                }
            }
        }

        for (uint32_t c = 0; c <= 0x10FFFF; ++c)
        {
            if (map_vec.count(c))
            {
                // Count initial/trailing non-starters in NFKD
                bool initial = true;
                bool trailing = false;
                uint32_t initial_count = 0;
                uint32_t trailing_count = 0;
                for (size_t i = 0; i < map_vec.at(c).size(); ++i)
                {
                    if ((map.at(map_vec.at(c)[i]) & 0xFF) != 0) // non-starter
                    {
                        if (initial) initial_count++;
                        if (trailing) trailing_count++;
                    }
                    else // starter
                    {
                        initial = false;
                        //initial_count = 0; // Don't do this for assert "Cannot be both..."
                        trailing = true;
                        trailing_count = 0;
                    }
                }

                ASSERTX(initial_count <= 3 && trailing_count <= 3); // We have only 2 bits to store the data
                ASSERTX(!(initial_count > 0 && trailing_count > 0)); // Cannot be both initial and trailing
                ASSERTX(!(initial_count > 0 && trailing)); // Cannot be initial non-starter(s) and a starter

                if (initial_count || trailing_count)
                {
                    if (initial_count)
                        map.at(c) |= initial_count << 14;
                    else
                        map.at(c) |= trailing_count << 12;
                }

            }
            // Original version. Generates the same data but without "Cannot be both..." assert
            /*if (map_vec.count(c))
            {
                // Count initial/trailing non-starters in NFKD
                bool initial = true;
                uint32_t count = 0;
                for (size_t i = 0; i < map_vec.at(c).size(); ++i)
                {
                    if ((map.at(map_vec.at(c)[i]) & 0xFF) != 0) // non-starter
                        count++;
                    else // starter
                    {
                        count = 0;
                        initial = false;
                    }
                }

                ASSERTX(count <= 3); // We have only 2 bits to store the data

                if (count > 0)
                {
                    if (initial)
                        map.at(c) |= count << 14;
                    else
                        map.at(c) |= count << 12;
                }

            }*/
            else if ((map.at(c) & 0xFF) != 0)
            {
                map.at(c) |= 1 << 14;
            }
        }

        new_generator_output(file4, file5, 8, 16, true, map, 0x10FFFF);
    }
}

static void new_generator_output3(const std::string& file, const std::vector<std::vector<uint32_t>>& vec)
{
    std::ofstream output(file);
    ASSERTX(output.is_open());

    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        output << "{";

        for (std::size_t j = 0; j < vec[i].size(); ++j)
        {
            if (vec[i][j] == 0)
                output << (COMPACT ? "0" : "       0");
            else
            {
                ASSERTX(vec[i][j] <= 0x10FFFF); // 32 bit (but max code point size)

                char str[64] = {};
                snprintf(str, 64, COMPACT ? "%u" : "0x%.6X", vec[i][j]);

                output << str;
            }

            if (j < vec[i].size() - 1)
                output << (COMPACT ? "," : ", ");
        }

        output << "},\n";
    }
}

static void new_generator_unicodedata_compose(const std::string& file1, const std::string& file2,
                                              const std::string& file3, const std::string& file4, const std::string& file5)
{
    // https://www.unicode.org/reports/tr44/#UnicodeData.txt
    std::ifstream input("UnicodeData.txt", std::ios::binary);
    ASSERTX(input.is_open());

    // Some of code points must never be composed after decomposition they are called composition exclusions
    // Check CompositionExclusions.txt for human readable format

    // https://www.unicode.org/reports/tr44/#DerivedNormalizationProps.txt
    std::ifstream input_exclusions("DerivedNormalizationProps.txt", std::ios::binary);
    ASSERTX(input_exclusions.is_open());

    std::set<uint32_t> set_exclusions;
    std::string line;
    while (std::getline(input_exclusions, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            if (line.find("; Full_Composition_Exclusion", semicolon) == semicolon)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                    set_exclusions.insert(i);
            }
        }
    }

    std::map<uint32_t, uint32_t> map1;
    std::map<uint32_t, uint32_t> map2;

    for (uint32_t i = 0; i <= 0x10FFFF; ++i)
        map1[i] = 0;

    for (uint32_t i = 0; i <= 0x10FFFF; ++i)
        map2[i] = 0;

    std::map<uint32_t, uint32_t> map1_vec;
    std::map<uint32_t, uint32_t> map2_vec;

    // Prepare data for 2-dimensional vector

    line.clear();
    while (std::getline(input, line))
    {
        uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);

        for (size_t i = 0, col = 0; i < line.size(); ++i)
        {
            if (line[i] == ';')
            {
                col++;
                if (col == 5)
                {
                    if (line[i+1] == ';' || line[i+1] == '<')
                        break;

                    uint32_t decomp[2];
                    uint32_t number = 0;
                    for (size_t j = i+1; j < line.size(); ++j)
                    {

                        if (line[j] == ';')
                            break;
                        else if (j == i+1 || line[j] == ' ')
                        {
                            uint32_t n = (uint32_t)strtoul(line.c_str()+j, 0, 16);
                            ASSERTX(number < 2); // Max is 2
                            decomp[number++] = n;
                        }
                    }

                    //if (number != 2) // Skip Singleton Decompositions (handled by set_exclusions)
                    //	break;

                    if (set_exclusions.count(c))
                        break;

                    ASSERTX(number == 2);

                    if (!map1_vec.count(decomp[0]))
                        map1_vec[decomp[0]] = map1_vec.size() + 1;
                    if (!map2_vec.count(decomp[1]))
                        map2_vec[decomp[1]] = map2_vec.size() + 1;

                    break;
                }
            }
        }
    }

    // Use 2 maps (map1_vec, map2_vec) to create 2-dimensional vector (vec)
    // Each dimension is a number that will lead to composed code point in O(1)
    // To get the number for both code points use two-stage tables as usual (map1, map2) that is also O(1)
    // This is exactly as recommended here:
    // 14.1.2 Optimizing Tables for NFC Composition: https://unicode.org/reports/tr15/#Optimization_Strategies
    // TODO: The size of the table can be optimized but it adds complexity so I don't care for now

    std::vector<std::vector<uint32_t>> vec(map1_vec.size() + 1, std::vector<uint32_t>(map2_vec.size() + 1));

    input.clear();
    input.seekg(0, std::ios::beg);

    while (std::getline(input, line))
    {
        uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);

        for (size_t i = 0, col = 0; i < line.size(); ++i)
        {
            if (line[i] == ';')
            {
                col++;
                if (col == 5)
                {
                    if (line[i+1] == ';' || line[i+1] == '<')
                        break;

                    uint32_t decomp[2];
                    uint32_t number = 0;
                    for (size_t j = i+1; j < line.size(); ++j)
                    {


                        if (line[j] == ';')
                            break;
                        else if (j == i+1 || line[j] == ' ')
                        {
                            uint32_t n = (uint32_t)strtoul(line.c_str()+j, 0, 16);
                            ASSERTX(number < 2); // Max is 2
                            decomp[number++] = n;
                        }
                    }

                    //if (number != 2) // Skip Singleton Decompositions (handled by set_exclusions)
                    //	break;

                    if (set_exclusions.count(c))
                        break;

                    ASSERTX(number == 2);

                    map1.at(decomp[0]) = map1_vec.at(decomp[0]);
                    map2.at(decomp[1]) = map2_vec.at(decomp[1]);

                    vec[map1_vec.at(decomp[0])][map2_vec.at(decomp[1])] = c;

                    // Test: decrease data size
                    //map1.at(decomp[0]) = (map1_vec.at(decomp[0]) << 5) | ((c >> 16) & 0x1F);
                    //map2.at(decomp[1]) = map2_vec.at(decomp[1]);

                    //vec[map1_vec.at(decomp[0])][map2_vec.at(decomp[1])] = (c & 0xFFFF);

                    break;
                }
            }
        }
    }

    // If asserts the number must be changed here and in stage3_comp[][SECOND_DIMENSION_SIZE]
    ASSERTX(vec.size() > 0 && vec[0].size() == 64);

    new_generator_output(file1, file2, 8, 16, true, map1, 0x10FFFF);
    new_generator_output(file3, file4, 8, 16, true, map2, 0x10FFFF);
    new_generator_output3(file5, vec);
}

static void new_generator_break_grapheme(const std::string& file1, const std::string& file2)
{
    std::ifstream input("GraphemeBreakProperty.txt", std::ios::binary);
    ASSERTX(input.is_open());

    const uint32_t maxmap = 0x10FFFF; // Do not change!

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= maxmap; ++i)
        map[i] = 0;

    std::string line;
    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool Prepend = false;
            bool CR = false;
            bool LF = false;
            bool Control = false;
            bool Extend = false;
            bool Regional_Indicator = false;
            bool SpacingMark = false;
            bool L = false;
            bool V = false;
            bool T = false;
            bool LV = false;
            bool LVT = false;
            bool ZWJ = false;

            if (line.find("; Prepend #", semicolon) == semicolon)
                Prepend = true;
            if (line.find("; CR #", semicolon) == semicolon)
                CR = true;
            if (line.find("; LF #", semicolon) == semicolon)
                LF = true;
            if (line.find("; Control #", semicolon) == semicolon)
                Control = true;
            if (line.find("; Extend #", semicolon) == semicolon)
                Extend = true;
            if (line.find("; Regional_Indicator #", semicolon) == semicolon)
                Regional_Indicator = true;
            if (line.find("; SpacingMark #", semicolon) == semicolon)
                SpacingMark = true;
            if (line.find("; L #", semicolon) == semicolon)
                L = true;
            if (line.find("; V #", semicolon) == semicolon)
                V = true;
            if (line.find("; T #", semicolon) == semicolon)
                T = true;
            if (line.find("; LV #", semicolon) == semicolon)
                LV = true;
            if (line.find("; LVT #", semicolon) == semicolon)
                LVT = true;
            if (line.find("; ZWJ #", semicolon) == semicolon)
                ZWJ = true;

            if (Prepend || CR || LF || Control || Extend || Regional_Indicator || SpacingMark || L || V || T || LV || LVT || ZWJ)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    // These values do not intersect with each other
                    ASSERTX(map.at(i) == 0);

                    if (Prepend)            map.at(i) = 1;
                    if (CR)                 map.at(i) = 2;
                    if (LF)                 map.at(i) = 3;
                    if (Control)            map.at(i) = 4;
                    if (Extend)             map.at(i) = 5;
                    if (Regional_Indicator) map.at(i) = 6;
                    if (SpacingMark)        map.at(i) = 7;
                    if (L)                  map.at(i) = 8;
                    if (V)                  map.at(i) = 9;
                    if (T)                  map.at(i) = 10;
                    if (LV)                 map.at(i) = 11;
                    if (LVT)                map.at(i) = 12;
                    if (ZWJ)                map.at(i) = 13;
                    // if (Extended_Pictographic) map.at(i) = 14; // See below
                }
            }
        }
    }

    input.close();
    line.clear();

    input.open("emoji-data.txt", std::ios::binary);
    ASSERTX(input.is_open());

    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool Extended_Pictographic = false;

            if (line.find("; Extended_Pictographic#", semicolon) == semicolon)
                Extended_Pictographic = true;

            if (Extended_Pictographic)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    ASSERTX(map.at(i) == 0);

                    if (Extended_Pictographic) map.at(i) = 14;
                }
            }
        }
    }

    new_generator_output(file1, file2, 8, 8, true, map);
}

static void new_generator_break_word(const std::string& file1, const std::string& file2)
{
    std::ifstream input("WordBreakProperty.txt", std::ios::binary);
    ASSERTX(input.is_open());

    const uint32_t maxmap = 0x10FFFF; // Do not change!

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= maxmap; ++i)
        map[i] = 0;

    std::string line;
    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool Double_Quote = false;
            bool Single_Quote = false;
            bool Hebrew_Letter = false;
            bool CR = false;
            bool LF = false;
            bool Newline = false;
            bool Extend = false;
            bool Regional_Indicator = false;
            bool Format = false;
            bool Katakana = false;
            bool ALetter = false;
            bool MidLetter = false;
            bool MidNum = false;
            bool MidNumLet = false;
            bool Numeric = false;
            bool ExtendNumLet = false;
            bool ZWJ = false;
            bool WSegSpace = false;

            if (line.find("; Double_Quote #", semicolon) == semicolon)
                Double_Quote = true;
            if (line.find("; Single_Quote #", semicolon) == semicolon)
                Single_Quote = true;
            if (line.find("; Hebrew_Letter #", semicolon) == semicolon)
                Hebrew_Letter = true;
            if (line.find("; CR #", semicolon) == semicolon)
                CR = true;
            if (line.find("; LF #", semicolon) == semicolon)
                LF = true;
            if (line.find("; Newline #", semicolon) == semicolon)
                Newline = true;
            if (line.find("; Extend #", semicolon) == semicolon)
                Extend = true;
            if (line.find("; Regional_Indicator #", semicolon) == semicolon)
                Regional_Indicator = true;
            if (line.find("; Format #", semicolon) == semicolon)
                Format = true;
            if (line.find("; Katakana #", semicolon) == semicolon)
                Katakana = true;
            if (line.find("; ALetter #", semicolon) == semicolon)
                ALetter = true;
            if (line.find("; MidLetter #", semicolon) == semicolon)
                MidLetter = true;
            if (line.find("; MidNum #", semicolon) == semicolon)
                MidNum = true;
            if (line.find("; MidNumLet #", semicolon) == semicolon)
                MidNumLet = true;
            if (line.find("; Numeric #", semicolon) == semicolon)
                Numeric = true;
            if (line.find("; ExtendNumLet #", semicolon) == semicolon)
                ExtendNumLet = true;
            if (line.find("; ZWJ #", semicolon) == semicolon)
                ZWJ = true;
            if (line.find("; WSegSpace #", semicolon) == semicolon)
                WSegSpace = true;

            if (Double_Quote || Single_Quote || Hebrew_Letter || CR || LF || Newline ||
                Extend || Regional_Indicator || Format || Katakana || ALetter || MidLetter ||
                MidNum || MidNumLet || Numeric || ExtendNumLet || ZWJ || WSegSpace)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    // These values do not intersect with each other
                    ASSERTX(map.at(i) == 0);

                    if (Double_Quote)       map.at(i) = 1;
                    if (Single_Quote)       map.at(i) = 2;
                    if (Hebrew_Letter)      map.at(i) = 3;
                    if (CR)                 map.at(i) = 4;
                    if (LF)                 map.at(i) = 5;
                    if (Newline)            map.at(i) = 6;
                    if (Extend)             map.at(i) = 7;
                    if (Regional_Indicator) map.at(i) = 8;
                    if (Format)             map.at(i) = 9;
                    if (Katakana)           map.at(i) = 10;
                    if (ALetter)            map.at(i) = 11;
                    if (MidLetter)          map.at(i) = 12;
                    if (MidNum)             map.at(i) = 13;
                    if (MidNumLet)          map.at(i) = 14;
                    if (Numeric)            map.at(i) = 15;
                    if (ExtendNumLet)       map.at(i) = 16;
                    if (ZWJ)                map.at(i) = 17;
                    if (WSegSpace)          map.at(i) = 18;
                    // if (Extended_Pictographic) map.at(i) = 19; // See below
                    // if (Alphabetic)            map.at(i) = 20; // See below
                }
            }
        }
    }

    input.close();
    line.clear();

    input.open("emoji-data.txt", std::ios::binary);
    ASSERTX(input.is_open());

    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool Extended_Pictographic = false;

            if (line.find("; Extended_Pictographic#", semicolon) == semicolon)
                Extended_Pictographic = true;

            if (Extended_Pictographic)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    //ASSERTX(map.at(i) == 0);
                    //if (map.at(i) != 0) { std::cout << std::hex << i << std::dec << ':' << map.at(i) << '\n'; }
                    // There are 5 code points that have ALetter property and Extended_Pictographic:
                    // U+2139, U+24C2, U+1F170, U+1F171, U+1F17E, U+1F17F,
                    // these "emoji" never used together with ZWJ see: emoji-zwj-sequences.txt
                    // so it won't be a problem for now (Unicode 13.0).
                    // TODO: this can be fixed if we store Extended_Pictographic in the first bit but this is pure bullsh...
                    // Actually it must be fixed anyway, it won't complicate the code in impl_break_word.h too much.
                    // I tested in ICU and it handles these cases, another reason to fix this.
                    // See test_break_word_corner_cases in test/test_break.h
                    if (map.at(i) != 0) // Do not override ALetter with Extended_Pictographic for these code points
                        continue;

                    if (Extended_Pictographic) map.at(i) = 19;
                }
            }
        }
    }

    input.close();
    line.clear();

    // Get remaining alphabetic code points from DerivedCoreProperties.txt too for word indicator

    input.open("DerivedCoreProperties.txt", std::ios::binary);
    ASSERTX(input.is_open());

    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool Alphabetic = false;

            if (line.find("; Alphabetic #", semicolon) == semicolon)
                Alphabetic = true;

            if (Alphabetic)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    if (map.at(i) == 0) // Do not override existing word break properties
                        map.at(i) = 20;
                }
            }
        }
    }

    new_generator_output(file1, file2, 8, 8, true, map);
}

static void new_generator_unicodedata_norm_mn(const std::string& file1, const std::string& file2)
{
    // https://www.unicode.org/reports/tr44/#UnicodeData.txt
    std::ifstream input("UnicodeData.txt", std::ios::binary);
    ASSERTX(input.is_open());

    const uint32_t maxmap = 0x10FFFF; // Do not change!

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= maxmap; ++i)
        map[i] = 0;

    std::string line;
    while (std::getline(input, line))
    {
        uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);

        for (uint32_t i = 0, col = 0; i < line.size(); ++i)
        {
            if (line[i] == ';')
            {
                col++;
                if (col == 2) // General Category
                {
                    // Mn (Nonspacing Mark)
                    if (line.size() > i+2 && line[i+1] == 'M' && line[i+2] == 'n')
                    {
                        map.at(c) = 1;
                    }
                    break;
                }
            }
        }
    }

    new_generator_output(file1, file2, 8, 8, true, map);
}

// TODO: for Unicode property module that is planned
static void new_generator_meow(const std::string& file1, const std::string& file2)
{
    // https://www.unicode.org/reports/tr44/#UnicodeData.txt
    std::ifstream input("UnicodeData.txt", std::ios::binary);
    ASSERTX(input.is_open());

    const uint32_t maxmap = 0x10FFFF; // Do not change!

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= maxmap; ++i)
        map[i] = 0;

    uint32_t range_c = 0, range_last = 0;
    std::string line;
    while (range_last || std::getline(input, line))
    {
        uint32_t c = 0;

        if (range_last == 0)
        {
            c = (uint32_t)strtoul(line.c_str(), 0, 16);

            if (line.find(", First>;") != std::string::npos)
            {
                ASSERTX(range_c == 0 && range_last == 0);
                range_c = c;
                continue;
            }
            if (line.find(", Last>;") != std::string::npos)
            {
                ASSERTX(range_c != 0 && range_last == 0);
                range_last = c;
                c = range_c;
            }
        }
        else
        {
            c = ++range_c;
            if (range_c == range_last)
            {
                range_c = 0;
                range_last = 0;
            }
        }

        for (uint32_t i = 0, col = 0; i < line.size(); ++i)
        {
            if (line[i] == ';')
            {
                col++;
                if (col == 2 && line.size() > i+2) // General Category
                {
                    // https://www.unicode.org/reports/tr44/#General_Category_Values

                    if      (line[i+1] == 'L' && line[i+2] == 'u') map.at(c) = 1;
                    else if (line[i+1] == 'L' && line[i+2] == 'l') map.at(c) = 2;
                    else if (line[i+1] == 'L' && line[i+2] == 't') map.at(c) = 3;
                    else if (line[i+1] == 'L' && line[i+2] == 'm') map.at(c) = 4;
                    else if (line[i+1] == 'L' && line[i+2] == 'o') map.at(c) = 5;
                    else if (line[i+1] == 'M' && line[i+2] == 'n') map.at(c) = 6;
                    else if (line[i+1] == 'M' && line[i+2] == 'c') map.at(c) = 7;
                    else if (line[i+1] == 'M' && line[i+2] == 'e') map.at(c) = 8;
                    else if (line[i+1] == 'N' && line[i+2] == 'd') map.at(c) = 9;
                    else if (line[i+1] == 'N' && line[i+2] == 'l') map.at(c) = 10;
                    else if (line[i+1] == 'N' && line[i+2] == 'o') map.at(c) = 11;
                    else if (line[i+1] == 'P' && line[i+2] == 'c') map.at(c) = 12;
                    else if (line[i+1] == 'P' && line[i+2] == 'd') map.at(c) = 13;
                    else if (line[i+1] == 'P' && line[i+2] == 's') map.at(c) = 14;
                    else if (line[i+1] == 'P' && line[i+2] == 'e') map.at(c) = 15;
                    else if (line[i+1] == 'P' && line[i+2] == 'i') map.at(c) = 16;
                    else if (line[i+1] == 'P' && line[i+2] == 'f') map.at(c) = 17;
                    else if (line[i+1] == 'P' && line[i+2] == 'o') map.at(c) = 18;
                    else if (line[i+1] == 'S' && line[i+2] == 'm') map.at(c) = 19;
                    else if (line[i+1] == 'S' && line[i+2] == 'c') map.at(c) = 20;
                    else if (line[i+1] == 'S' && line[i+2] == 'k') map.at(c) = 21;
                    else if (line[i+1] == 'S' && line[i+2] == 'o') map.at(c) = 22;
                    else if (line[i+1] == 'Z' && line[i+2] == 's') map.at(c) = 23;
                    else if (line[i+1] == 'Z' && line[i+2] == 'l') map.at(c) = 24;
                    else if (line[i+1] == 'Z' && line[i+2] == 'p') map.at(c) = 25;
                    else if (line[i+1] == 'C' && line[i+2] == 'c') map.at(c) = 26;
                    else if (line[i+1] == 'C' && line[i+2] == 'f') map.at(c) = 27;
                    else if (line[i+1] == 'C' && line[i+2] == 's') map.at(c) = 28;
                    else if (line[i+1] == 'C' && line[i+2] == 'o') map.at(c) = 29;
                    // There are no such code points in UnicodeData.txt
                    // this means all other code points are General_Category=Cn (General_Category=Unassigned)
                    //else if (line[i+1] == 'C' && line[i+2] == 'n') map.at(c) = 0;

                    break;
                }
            }
        }
    }

    input.close();
    line.clear();

    // https://www.unicode.org/reports/tr44/#DerivedCoreProperties.txt
    input.open("DerivedCoreProperties.txt", std::ios::binary);
    ASSERTX(input.is_open());

    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool Alphabetic = false;

            if (line.find("; Alphabetic #", semicolon) == semicolon)
                Alphabetic = true;

            if (Alphabetic)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    map.at(i) |= Alphabetic << 5;
                }
            }
        }
    }

    input.close();
    line.clear();

    // https://www.unicode.org/reports/tr44/#PropList.txt
    input.open("PropList.txt", std::ios::binary);
    ASSERTX(input.is_open());

    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool White_Space = false;
            bool Noncharacter_Code_Point = false;

            if (line.find("; White_Space #", semicolon) == semicolon)
                White_Space = true;
            if (line.find("; Noncharacter_Code_Point #", semicolon) == semicolon)
                Noncharacter_Code_Point = true;


            if (White_Space || Noncharacter_Code_Point)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    map.at(i) |=
                        White_Space             << 6 |
                        Noncharacter_Code_Point << 7;
                }
            }
        }
    }

    new_generator_output(file1, file2, 8, 8, true, map);
}

static void new_generator_case_locale(const std::string& file1, const std::string& file2)
{
    // https://www.unicode.org/reports/tr44/#DerivedCoreProperties.txt
    std::ifstream input("DerivedCoreProperties.txt", std::ios::binary);
    ASSERTX(input.is_open());

    const uint32_t maxmap = 0x10FFFF; // Do not change!

    std::map<uint32_t, uint32_t> map;

    for (uint32_t i = 0; i <= maxmap; ++i)
        map[i] = 0;

    std::string line;
    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool Cased = false;
            bool Case_Ignorable = false;
            bool Lowercase = false;
            bool Uppercase = false;
            bool Changes_When_Lowercased = false;
            bool Changes_When_Uppercased = false;
            bool Changes_When_Titlecased = false;
            bool Changes_When_Casefolded = false;
            bool Changes_When_Casemapped = false;

            if (line.find("; Cased", semicolon) == semicolon)
                Cased = true;
            if (line.find("; Case_Ignorable", semicolon) == semicolon)
                Case_Ignorable = true;
            if (line.find("; Lowercase", semicolon) == semicolon)
                Lowercase = true;
            if (line.find("; Uppercase", semicolon) == semicolon)
                Uppercase = true;
            //if (line.find("; Changes_When_Lowercased", semicolon) == semicolon)
            //	Changes_When_Lowercased = true;
            //if (line.find("; Changes_When_Uppercased", semicolon) == semicolon)
            //	Changes_When_Uppercased = true;
            //if (line.find("; Changes_When_Titlecased", semicolon) == semicolon)
            //	Changes_When_Titlecased = true;
            //if (line.find("; Changes_When_Casefolded", semicolon) == semicolon)
            //	Changes_When_Casefolded = true;
            //if (line.find("; Changes_When_Casemapped", semicolon) == semicolon)
            //	Changes_When_Casemapped = true;

            if (Cased || Case_Ignorable || Lowercase || Uppercase ||
                Changes_When_Lowercased || Changes_When_Uppercased ||
                Changes_When_Titlecased || Changes_When_Casefolded || Changes_When_Casemapped)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    /*map.at(i) |=
                        Cased                   << 0 | // prop_Cased
                        Case_Ignorable          << 1 | // prop_Cased_Ignorable
                        Changes_When_Lowercased << 5 | // prop_Changes_When_Lowercased
                        Changes_When_Uppercased << 6 | // prop_Changes_When_Uppercased
                        Changes_When_Titlecased << 7;  // prop_Changes_When_Titlecased*/

                    map.at(i) |=
                        Cased          << 0 | // prop_Cased
                        Case_Ignorable << 1 | // prop_Cased_Ignorable
                        Lowercase      << 2 | // prop_Lowercase
                        Uppercase      << 3;  // prop_Uppercase
                }
            }
        }
    }

    input.close();
    line.clear();

    // https://www.unicode.org/reports/tr44/#PropList.txt
    input.open("PropList.txt", std::ios::binary);
    ASSERTX(input.is_open());

    while (std::getline(input, line))
    {
        std::size_t semicolon = line.find(';');

        if (line.size() > 15 && semicolon != std::string::npos)
        {
            bool Soft_Dotted = false;

            if (line.find("; Soft_Dotted", semicolon) == semicolon)
                Soft_Dotted = true;

            if (Soft_Dotted)
            {
                uint32_t c1 = (uint32_t)strtoul(line.c_str(), 0, 16);
                uint32_t c2 = c1;
                std::size_t dots = line.find("..");
                if (dots != std::string::npos)
                    c2 = (uint32_t)strtoul(line.c_str()+dots+2, 0, 16);

                for (uint32_t i = c1; i <= c2; ++i)
                {
                    map.at(i) |= Soft_Dotted << 4; // prop_Soft_Dotted
                }
            }
        }
    }

    input.close();
    line.clear();

    // https://www.unicode.org/reports/tr44/#UnicodeData.txt
    input.open("UnicodeData.txt", std::ios::binary);
    ASSERTX(input.is_open());

    while (std::getline(input, line))
    {
        uint32_t c = (uint32_t)strtoul(line.c_str(), 0, 16);

        for (uint32_t i = 0, col = 0; i < line.size(); ++i)
        {
            if (line[i] == ';')
            {
                col++;
                if (col == 3) // Canonical_Combining_Class
                {
                    uint32_t n = (uint32_t)atoi(line.c_str()+i+1);

                    // Use (n != 0) instead of (n == 0) to decrease the size of the data
                    if (n != 0)
                        map.at(c) |= 1 << 5; // prop_CCC_NOT_0
                    if (n == 230)
                        map.at(c) |= 1 << 6; // prop_CCC_230

                    break;
                }
            }
        }
    }

    new_generator_output(file1, file2, 8, 8, true, map);
}

static void new_generator()
{
    // List of files that are needed for the generator:
    // UnicodeData.txt
    // CaseFolding.txt
    // SpecialCasing.txt
    // DerivedCoreProperties.txt
    // allkeys.txt
    // DerivedNormalizationProps.txt
    // CompositionExclusions.txt (not used anymore but contains human readable format of composition exclusions)
    // GraphemeBreakProperty.txt
    // WordBreakProperty.txt
    // emoji-data.txt
    // PropList.txt

    // Download from:
    // Latest Unicode version:
    // ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt
    // ftp://ftp.unicode.org/Public/UCA/latest/allkeys.txt
    // etc.
    // Other Unicode versions:
    // ftp://ftp.unicode.org/Public/12.1.0/ucd/UnicodeData.txt
    // ftp://ftp.unicode.org/Public/UCA/12.1.0/allkeys.txt
    // etc.
    // The lowest Unicode version the generator can use is 6.0.0
    // For break module the lowest version is Unicode 11.0.0
    //
    // Do not mess up different versions of these files. There is no check for this crap.

    new_generator_unicodedata("new_stage1_upper.txt", "new_stage2_upper.txt", 12);
    new_generator_unicodedata("new_stage1_lower.txt", "new_stage2_lower.txt", 13);
    new_generator_unicodedata("new_stage1_title.txt", "new_stage2_title.txt", 14);
    new_generator_casefolding("new_stage1_fold.txt", "new_stage2_fold.txt");
    new_generator_allkeys("new_stage1_order.txt", "new_stage2_order.txt");

    new_generator_casefolding_full("new_stage1_special_fold.txt", "new_stage2_special_fold.txt", "new_stage3_special_fold.txt");
    new_generator_special_casing("new_stage1_special_upper.txt", "new_stage2_special_upper.txt", "new_stage3_special_upper.txt", 3);
    new_generator_special_casing("new_stage1_special_lower.txt", "new_stage2_special_lower.txt", "new_stage3_special_lower.txt", 1);
    new_generator_special_casing("new_stage1_special_title.txt", "new_stage2_special_title.txt", "new_stage3_special_title.txt", 2);
    //new_generator_case_properties("new_stage1_prop.txt", "new_stage2_prop.txt");
    new_generator_case_locale("new_stage1_case_prop.txt", "new_stage2_case_prop.txt");
    new_generator_meow("new_stage1_prop.txt", "new_stage2_prop.txt");

    new_generator_unicodedata_compose("new_stage1_comp_cp1.txt", "new_stage2_comp_cp1.txt", "new_stage1_comp_cp2.txt", "new_stage2_comp_cp2.txt", "new_stage3_comp.txt");
    new_generator_unicodedata_decompose_ccc_qc("new_stage1_decomp_nfd.txt", "new_stage2_decomp_nfd.txt", "new_stage3_decomp_nfd.txt", false);
    new_generator_unicodedata_decompose_ccc_qc("new_stage1_decomp_nfkd.txt", "new_stage2_decomp_nfkd.txt", "new_stage3_decomp_nfkd.txt", true, "new_stage1_ccc_qc.txt", "new_stage2_ccc_qc.txt");
    new_generator_unicodedata_norm_mn("new_stage1_norm_mn.txt", "new_stage2_norm_mn.txt");

    new_generator_break_grapheme("new_stage1_break_grapheme.txt", "new_stage2_break_grapheme.txt");
    new_generator_break_word("new_stage1_break_word.txt", "new_stage2_break_word.txt");
}

static void new_merger_replace_string(std::string& data, const std::string& file)
{
    std::ifstream input(file);
    ASSERTX(input.is_open());

    std::string old_str = file;
    std::string new_str;
    new_str.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());

    std::size_t found = new_str.find_last_not_of(" ,\n");
    if (found != std::string::npos)
        new_str.erase(found + 1);

    std::size_t index = data.find(old_str);
    if (index != std::string::npos)
        data.replace(index, old_str.size(), new_str);
}

static void new_merger()
{
    std::ifstream input("impl_case_data.h_blank");
    ASSERTX(input.is_open());

    std::string data;
    data.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());

    new_merger_replace_string(data, "new_stage1_lower.txt");
    new_merger_replace_string(data, "new_stage2_lower.txt");
    new_merger_replace_string(data, "new_stage1_upper.txt");
    new_merger_replace_string(data, "new_stage2_upper.txt");
    new_merger_replace_string(data, "new_stage1_fold.txt");
    new_merger_replace_string(data, "new_stage2_fold.txt");
    new_merger_replace_string(data, "new_stage1_title.txt");
    new_merger_replace_string(data, "new_stage2_title.txt");
    new_merger_replace_string(data, "new_stage1_order.txt");
    new_merger_replace_string(data, "new_stage2_order.txt");
    new_merger_replace_string(data, "new_stage1_case_prop.txt");
    new_merger_replace_string(data, "new_stage2_case_prop.txt");
    new_merger_replace_string(data, "new_stage1_special_upper.txt");
    new_merger_replace_string(data, "new_stage2_special_upper.txt");
    new_merger_replace_string(data, "new_stage3_special_upper.txt");
    new_merger_replace_string(data, "new_stage1_special_fold.txt");
    new_merger_replace_string(data, "new_stage2_special_fold.txt");
    new_merger_replace_string(data, "new_stage3_special_fold.txt");
    new_merger_replace_string(data, "new_stage1_special_lower.txt");
    new_merger_replace_string(data, "new_stage2_special_lower.txt");
    new_merger_replace_string(data, "new_stage3_special_lower.txt");
    new_merger_replace_string(data, "new_stage1_special_title.txt");
    new_merger_replace_string(data, "new_stage2_special_title.txt");
    new_merger_replace_string(data, "new_stage3_special_title.txt");

    std::ofstream output("impl_case_data.h");
    ASSERTX(output.is_open());
    output << data;

    input.close();
    output.close();

    input.open("impl_norm_data.h_blank");
    ASSERTX(input.is_open());

    data.clear();
    data.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());

    new_merger_replace_string(data, "new_stage1_ccc_qc.txt");
    new_merger_replace_string(data, "new_stage2_ccc_qc.txt");
    new_merger_replace_string(data, "new_stage1_decomp_nfd.txt");
    new_merger_replace_string(data, "new_stage2_decomp_nfd.txt");
    new_merger_replace_string(data, "new_stage3_decomp_nfd.txt");
    new_merger_replace_string(data, "new_stage1_decomp_nfkd.txt");
    new_merger_replace_string(data, "new_stage2_decomp_nfkd.txt");
    new_merger_replace_string(data, "new_stage3_decomp_nfkd.txt");
    new_merger_replace_string(data, "new_stage1_comp_cp1.txt");
    new_merger_replace_string(data, "new_stage2_comp_cp1.txt");
    new_merger_replace_string(data, "new_stage1_comp_cp2.txt");
    new_merger_replace_string(data, "new_stage2_comp_cp2.txt");
    new_merger_replace_string(data, "new_stage3_comp.txt");
    new_merger_replace_string(data, "new_stage1_norm_mn.txt");
    new_merger_replace_string(data, "new_stage2_norm_mn.txt");

    output.open("impl_norm_data.h");
    ASSERTX(output.is_open());
    output << data;

    input.close();
    output.close();

    input.open("impl_break_grapheme_data.h_blank");
    ASSERTX(input.is_open());

    data.clear();
    data.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());

    new_merger_replace_string(data, "new_stage1_break_grapheme.txt");
    new_merger_replace_string(data, "new_stage2_break_grapheme.txt");

    output.open("impl_break_grapheme_data.h");
    ASSERTX(output.is_open());
    output << data;

    input.close();
    output.close();

    input.open("impl_break_word_data.h_blank");
    ASSERTX(input.is_open());

    data.clear();
    data.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());

    new_merger_replace_string(data, "new_stage1_break_word.txt");
    new_merger_replace_string(data, "new_stage2_break_word.txt");

    output.open("impl_break_word_data.h");
    ASSERTX(output.is_open());
    output << data;

    input.close();
    output.close();
}

int main3()
{
    new_generator();
    new_merger();

    return 0;
}

/* Public Domain License
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
