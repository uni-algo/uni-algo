/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include <fstream>
#include <set>
#include <iostream>
#include <string_view>

// Forward declaration for normalization functions that uses high-level iterators
// We must test the iterators together with fast functions
// See the end of the file for bodies
std::string utf8_nfc(std::string_view str);
std::string utf8_nfd(std::string_view str);
std::string utf8_nfkc(std::string_view str);
std::string utf8_nfkd(std::string_view str);
std::u16string utf16_nfc(std::u16string_view str);
std::u16string utf16_nfd(std::u16string_view str);
std::u16string utf16_nfkc(std::u16string_view str);
std::u16string utf16_nfkd(std::u16string_view str);

size_t test_norm_impl(bool with_part2 = true, size_t with_prefix = 0, size_t with_suffix = 0)
{
    std::ifstream input("NormalizationTest.txt", std::ios::binary);
    TESTX(input.is_open());
    // Latest version:
    // ftp://ftp.unicode.org/Public/UNIDATA/NormalizationTest.txt
    // Other versions:
    // ftp://ftp.unicode.org/Public/12.0.0/ucd/NormalizationTest.txt
    // ftp://ftp.unicode.org/Public/12.1.0/ucd/NormalizationTest.txt
    // etc.

    std::u32string source, NFC, NFD, NFKC, NFKD;
    std::set<char32_t> set;
    size_t count_lines = 0;

    std::string line;
    while (std::getline(input, line))
    {
        count_lines++;

        if (line.empty() || line[0] == '#' || line[0] == '@')
            continue;

        for (size_t i = 0, column = 0; i < line.size(); i++)
        {
            if (i == 0 || line[i] == ';')
            {
                column++;

                if (column > 5)
                    break;

                uint32_t buffer[64] = {};
                uint32_t number = 0;

                if (line[i] == ';')
                    i++;

                for (size_t j = i; j < line.size(); ++j)
                {
                    if (line[j] == ';')
                        break;
                    else if (j == i || line[j] == ' ')
                    {
                        uint32_t n = (uint32_t)strtoul(line.c_str()+j, 0, 16);
                        buffer[number++] = n;

                        if (with_part2)
                            set.insert(n);
                    }
                }

                std::u32string str;
                for (size_t j = 0; j < number; ++j)
                    str.push_back(buffer[j]);

                TESTX(column >= 1 && column <= 5);

                if (column == 1)
                    source = str;
                else if (column == 2)
                    NFC = str;
                else if (column == 3)
                    NFD = str;
                else if (column == 4)
                    NFKC = str;
                else if (column == 5)
                    NFKD = str;
            }
        }

        TESTX(!source.empty());
        TESTX(!NFC.empty());
        TESTX(!NFD.empty());
        TESTX(!NFKC.empty());
        TESTX(!NFKD.empty());

        // Because our implementation is buffered test the data with a prefix and a suffix
        // to make sure the buffer works.
        // Note: this actually doesn't test that the buffer works properly, it only tests
        // that it just works, don't rely much on these prefix/suffix tests.
        if (with_prefix)
        {
            std::u32string prefix(with_prefix, 'T');
            source = prefix + source;
            NFC = prefix + NFC;
            NFD = prefix + NFD;
            NFKC = prefix + NFKC;
            NFKD = prefix + NFKD;
        }
        if (with_suffix)
        {
            std::u32string suffix(with_suffix, 'T');
            source += suffix;
            NFC += suffix;
            NFD += suffix;
            NFKC += suffix;
            NFKD += suffix;
        }

        /* From: NormalizationTest.txt
        # Normalization Test Suite
        # Format:
        #
        #   Columns (c1, c2,...) are separated by semicolons
        #   They have the following meaning:
        #      source; NFC; NFD; NFKC; NFKD
        #   Comments are indicated with hash marks
        #   Each of the columns may have one or more code points.
        #
        # CONFORMANCE:
        # 1. The following invariants must be true for all conformant implementations
        #
        #    NFC
        #      c2 ==  toNFC(c1) ==  toNFC(c2) ==  toNFC(c3)
        #      c4 ==  toNFC(c4) ==  toNFC(c5)
        #
        #    NFD
        #      c3 ==  toNFD(c1) ==  toNFD(c2) ==  toNFD(c3)
        #      c5 ==  toNFD(c4) ==  toNFD(c5)
        #
        #    NFKC
        #      c4 == toNFKC(c1) == toNFKC(c2) == toNFKC(c3) == toNFKC(c4) == toNFKC(c5)
        #
        #    NFKD
        #      c5 == toNFKD(c1) == toNFKD(c2) == toNFKD(c3) == toNFKD(c4) == toNFKD(c5)
        */

        // UTF-16

        std::u16string c1_utf16 = uni::utf32to16u(source);
        std::u16string c2_utf16 = uni::utf32to16u(NFC);
        std::u16string c3_utf16 = uni::utf32to16u(NFD);
        std::u16string c4_utf16 = uni::utf32to16u(NFKC);
        std::u16string c5_utf16 = uni::utf32to16u(NFKD);

        // NFC
        TESTX(c2_utf16 == uni::norm::utf16_nfc(c1_utf16));
        TESTX(c2_utf16 == uni::norm::utf16_nfc(c2_utf16));
        TESTX(c2_utf16 == uni::norm::utf16_nfc(c3_utf16));

        TESTX(c4_utf16 == uni::norm::utf16_nfc(c4_utf16));
        TESTX(c4_utf16 == uni::norm::utf16_nfc(c5_utf16));

        // NFD
        TESTX(c3_utf16 == uni::norm::utf16_nfd(c1_utf16));
        TESTX(c3_utf16 == uni::norm::utf16_nfd(c2_utf16));
        TESTX(c3_utf16 == uni::norm::utf16_nfd(c3_utf16));

        TESTX(c5_utf16 == uni::norm::utf16_nfd(c4_utf16));
        TESTX(c5_utf16 == uni::norm::utf16_nfd(c5_utf16));

        // NFKC
        TESTX(c4_utf16 == uni::norm::utf16_nfkc(c1_utf16));
        TESTX(c4_utf16 == uni::norm::utf16_nfkc(c2_utf16));
        TESTX(c4_utf16 == uni::norm::utf16_nfkc(c3_utf16));
        TESTX(c4_utf16 == uni::norm::utf16_nfkc(c4_utf16));
        TESTX(c4_utf16 == uni::norm::utf16_nfkc(c5_utf16));

        // NFKD
        TESTX(c5_utf16 == uni::norm::utf16_nfkd(c1_utf16));
        TESTX(c5_utf16 == uni::norm::utf16_nfkd(c2_utf16));
        TESTX(c5_utf16 == uni::norm::utf16_nfkd(c3_utf16));
        TESTX(c5_utf16 == uni::norm::utf16_nfkd(c4_utf16));
        TESTX(c5_utf16 == uni::norm::utf16_nfkd(c5_utf16));

        // NFC and NFKC quick checks cannot be tested with this data
        // because there is some lines that return NFC/NFKC_Quick_Check=Maybe
        // our functions return true only if there is a clear NFC/NFKC_Quick_Check=Yes
        // NFD and NFKD can be tested, there is no Maybe answer for these normalization forms
        //TESTX(uni::norm::utf16_is_nfc(c2_utf16) == true);
        TESTX(uni::norm::utf16_is_nfd(c3_utf16) == true);
        //TESTX(uni::norm::utf16_is_nfkc(c4_utf16) == true);
        TESTX(uni::norm::utf16_is_nfkd(c5_utf16) == true);

        // Iterators

        // NFC
        TESTX(c2_utf16 == utf16_nfc(c1_utf16));
        TESTX(c2_utf16 == utf16_nfc(c2_utf16));
        TESTX(c2_utf16 == utf16_nfc(c3_utf16));

        TESTX(c4_utf16 == utf16_nfc(c4_utf16));
        TESTX(c4_utf16 == utf16_nfc(c5_utf16));

        // NFD
        TESTX(c3_utf16 == utf16_nfd(c1_utf16));
        TESTX(c3_utf16 == utf16_nfd(c2_utf16));
        TESTX(c3_utf16 == utf16_nfd(c3_utf16));

        TESTX(c5_utf16 == utf16_nfd(c4_utf16));
        TESTX(c5_utf16 == utf16_nfd(c5_utf16));

        // NFKC
        TESTX(c4_utf16 == utf16_nfkc(c1_utf16));
        TESTX(c4_utf16 == utf16_nfkc(c2_utf16));
        TESTX(c4_utf16 == utf16_nfkc(c3_utf16));
        TESTX(c4_utf16 == utf16_nfkc(c4_utf16));
        TESTX(c4_utf16 == utf16_nfkc(c5_utf16));

        // NFKD
        TESTX(c5_utf16 == utf16_nfkd(c1_utf16));
        TESTX(c5_utf16 == utf16_nfkd(c2_utf16));
        TESTX(c5_utf16 == utf16_nfkd(c3_utf16));
        TESTX(c5_utf16 == utf16_nfkd(c4_utf16));
        TESTX(c5_utf16 == utf16_nfkd(c5_utf16));

        // UTF-8

        std::string c1_utf8 = uni::utf32to8(source);
        std::string c2_utf8 = uni::utf32to8(NFC);
        std::string c3_utf8 = uni::utf32to8(NFD);
        std::string c4_utf8 = uni::utf32to8(NFKC);
        std::string c5_utf8 = uni::utf32to8(NFKD);

        // NFC
        TESTX(c2_utf8 == uni::norm::utf8_nfc(c1_utf8));
        TESTX(c2_utf8 == uni::norm::utf8_nfc(c2_utf8));
        TESTX(c2_utf8 == uni::norm::utf8_nfc(c3_utf8));

        TESTX(c4_utf8 == uni::norm::utf8_nfc(c4_utf8));
        TESTX(c4_utf8 == uni::norm::utf8_nfc(c5_utf8));

        // NFD
        TESTX(c3_utf8 == uni::norm::utf8_nfd(c1_utf8));
        TESTX(c3_utf8 == uni::norm::utf8_nfd(c2_utf8));
        TESTX(c3_utf8 == uni::norm::utf8_nfd(c3_utf8));

        TESTX(c5_utf8 == uni::norm::utf8_nfd(c4_utf8));
        TESTX(c5_utf8 == uni::norm::utf8_nfd(c5_utf8));

        // NFKC
        TESTX(c4_utf8 == uni::norm::utf8_nfkc(c1_utf8));
        TESTX(c4_utf8 == uni::norm::utf8_nfkc(c2_utf8));
        TESTX(c4_utf8 == uni::norm::utf8_nfkc(c3_utf8));
        TESTX(c4_utf8 == uni::norm::utf8_nfkc(c4_utf8));
        TESTX(c4_utf8 == uni::norm::utf8_nfkc(c5_utf8));

        // NFKD
        TESTX(c5_utf8 == uni::norm::utf8_nfkd(c1_utf8));
        TESTX(c5_utf8 == uni::norm::utf8_nfkd(c2_utf8));
        TESTX(c5_utf8 == uni::norm::utf8_nfkd(c3_utf8));
        TESTX(c5_utf8 == uni::norm::utf8_nfkd(c4_utf8));
        TESTX(c5_utf8 == uni::norm::utf8_nfkd(c5_utf8));

        //TESTX(uni::norm::utf8_is_nfc(c2_utf8) == true);
        TESTX(uni::norm::utf8_is_nfd(c3_utf8) == true);
        //TESTX(uni::norm::utf8_is_nfkc(c4_utf8) == true);
        TESTX(uni::norm::utf8_is_nfkd(c5_utf8) == true);

        // Iterators

        // NFC
        TESTX(c2_utf8 == utf8_nfc(c1_utf8));
        TESTX(c2_utf8 == utf8_nfc(c2_utf8));
        TESTX(c2_utf8 == utf8_nfc(c3_utf8));

        TESTX(c4_utf8 == utf8_nfc(c4_utf8));
        TESTX(c4_utf8 == utf8_nfc(c5_utf8));

        // NFD
        TESTX(c3_utf8 == utf8_nfd(c1_utf8));
        TESTX(c3_utf8 == utf8_nfd(c2_utf8));
        TESTX(c3_utf8 == utf8_nfd(c3_utf8));

        TESTX(c5_utf8 == utf8_nfd(c4_utf8));
        TESTX(c5_utf8 == utf8_nfd(c5_utf8));

        // NFKC
        TESTX(c4_utf8 == utf8_nfkc(c1_utf8));
        TESTX(c4_utf8 == utf8_nfkc(c2_utf8));
        TESTX(c4_utf8 == utf8_nfkc(c3_utf8));
        TESTX(c4_utf8 == utf8_nfkc(c4_utf8));
        TESTX(c4_utf8 == utf8_nfkc(c5_utf8));

        // NFKD
        TESTX(c5_utf8 == utf8_nfkd(c1_utf8));
        TESTX(c5_utf8 == utf8_nfkd(c2_utf8));
        TESTX(c5_utf8 == utf8_nfkd(c3_utf8));
        TESTX(c5_utf8 == utf8_nfkd(c4_utf8));
        TESTX(c5_utf8 == utf8_nfkd(c5_utf8));
    }

    if (!with_part2)
        return count_lines;

    /* From: NormalizationTest.txt
    # 2. For every code point X assigned in this version of Unicode that is not specifically
    #    listed in Part 1, the following invariants must be true for all conformant
    #    implementations:
    #
    #      X == toNFC(X) == toNFD(X) == toNFKC(X) == toNFKD(X)
    #
    */

    for (char32_t c = 0; c <= 0x10FFFF; ++c)
    {
        if (!set.count(c)) // Exclude code points from Part 1
        {
            std::u32string X(1, c);

            // UTF-8

            std::string X_utf8 = uni::utf32to8(X);

            TESTX(X_utf8 == uni::norm::utf8_nfc(X_utf8));
            TESTX(X_utf8 == uni::norm::utf8_nfd(X_utf8));
            TESTX(X_utf8 == uni::norm::utf8_nfkc(X_utf8));
            TESTX(X_utf8 == uni::norm::utf8_nfkd(X_utf8));

            TESTX(uni::norm::utf8_is_nfc(X_utf8) == true);
            TESTX(uni::norm::utf8_is_nfd(X_utf8) == true);
            TESTX(uni::norm::utf8_is_nfkc(X_utf8) == true);
            TESTX(uni::norm::utf8_is_nfkd(X_utf8) == true);

            // Iterators
            TESTX(X_utf8 == utf8_nfc(X_utf8));
            TESTX(X_utf8 == utf8_nfd(X_utf8));
            TESTX(X_utf8 == utf8_nfkc(X_utf8));
            TESTX(X_utf8 == utf8_nfkd(X_utf8));

            // UTF-16

            std::u16string X_utf16 = uni::utf32to16u(X);

            TESTX(X_utf16 == uni::norm::utf16_nfc(X_utf16));
            TESTX(X_utf16 == uni::norm::utf16_nfd(X_utf16));
            TESTX(X_utf16 == uni::norm::utf16_nfkc(X_utf16));
            TESTX(X_utf16 == uni::norm::utf16_nfkd(X_utf16));

            TESTX(uni::norm::utf16_is_nfc(X_utf16) == true);
            TESTX(uni::norm::utf16_is_nfd(X_utf16) == true);
            TESTX(uni::norm::utf16_is_nfkc(X_utf16) == true);
            TESTX(uni::norm::utf16_is_nfkd(X_utf16) == true);

            // Iterators
            TESTX(X_utf16 == utf16_nfc(X_utf16));
            TESTX(X_utf16 == utf16_nfd(X_utf16));
            TESTX(X_utf16 == utf16_nfkc(X_utf16));
            TESTX(X_utf16 == utf16_nfkd(X_utf16));
        }
    }

    return count_lines;
}

void test_norm()
{
    size_t count_lines = 0;

    count_lines = test_norm_impl();
    std::cout << "DONE: " << count_lines << " lines" << '\n';
    count_lines = test_norm_impl(false, 100, 0);
    std::cout << "DONE: " << count_lines << " lines with prefix" << '\n';
    count_lines = test_norm_impl(false, 0, 100);
    std::cout << "DONE: " << count_lines << " lines with suffix" << '\n';
}

void test_norm_detect()
{
    // Composition exclusion -> always false
    TESTX(uni::norm::utf8_is_nfc(uni::utf32to8(U"\x0344")) == false);
    TESTX(uni::norm::utf8_is_nfd(uni::utf32to8(U"\x0344")) == false);
    TESTX(uni::norm::utf8_is_nfkc(uni::utf32to8(U"\x0344")) == false);
    TESTX(uni::norm::utf8_is_nfkd(uni::utf32to8(U"\x0344")) == false);

    TESTX(uni::norm::utf16_is_nfc(uni::utf32to16u(U"\x0344")) == false);
    TESTX(uni::norm::utf16_is_nfd(uni::utf32to16u(U"\x0344")) == false);
    TESTX(uni::norm::utf16_is_nfkc(uni::utf32to16u(U"\x0344")) == false);
    TESTX(uni::norm::utf16_is_nfkd(uni::utf32to16u(U"\x0344")) == false);

    // Must be in the correct order
    TESTX(uni::norm::utf8_is_nfc(uni::utf32to8(U"\x0315\x0305")) == false);
    TESTX(uni::norm::utf8_is_nfd(uni::utf32to8(U"\x0315\x0305")) == false);
    TESTX(uni::norm::utf8_is_nfkc(uni::utf32to8(U"\x0315\x0305")) == false);
    TESTX(uni::norm::utf8_is_nfkd(uni::utf32to8(U"\x0315\x0305")) == false);

    TESTX(uni::norm::utf16_is_nfc(uni::utf32to16u(U"\x0315\x0305")) == false);
    TESTX(uni::norm::utf16_is_nfd(uni::utf32to16u(U"\x0315\x0305")) == false);
    TESTX(uni::norm::utf16_is_nfkc(uni::utf32to16u(U"\x0315\x0305")) == false);
    TESTX(uni::norm::utf16_is_nfkd(uni::utf32to16u(U"\x0315\x0305")) == false);

    // Must be decomposed in NFKC/NFKD
    TESTX(uni::norm::utf8_is_nfc(uni::utf32to8(U"\x00A0")) == true);
    TESTX(uni::norm::utf8_is_nfd(uni::utf32to8(U"\x00A0")) == true);
    TESTX(uni::norm::utf8_is_nfkc(uni::utf32to8(U"\x00A0")) == false);
    TESTX(uni::norm::utf8_is_nfkd(uni::utf32to8(U"\x00A0")) == false);

    TESTX(uni::norm::utf16_is_nfc(uni::utf32to16u(U"\x00A0")) == true);
    TESTX(uni::norm::utf16_is_nfd(uni::utf32to16u(U"\x00A0")) == true);
    TESTX(uni::norm::utf16_is_nfkc(uni::utf32to16u(U"\x00A0")) == false);
    TESTX(uni::norm::utf16_is_nfkd(uni::utf32to16u(U"\x00A0")) == false);

    // Must be decomposed in NFD/NFKD
    TESTX(uni::norm::utf8_is_nfc(uni::utf32to8(U"\x00C0")) == true);
    TESTX(uni::norm::utf8_is_nfd(uni::utf32to8(U"\x00C0")) == false);
    TESTX(uni::norm::utf8_is_nfkc(uni::utf32to8(U"\x00C0")) == true);
    TESTX(uni::norm::utf8_is_nfkd(uni::utf32to8(U"\x00C0")) == false);

    TESTX(uni::norm::utf16_is_nfc(uni::utf32to16u(U"\x00C0")) == true);
    TESTX(uni::norm::utf16_is_nfd(uni::utf32to16u(U"\x00C0")) == false);
    TESTX(uni::norm::utf16_is_nfkc(uni::utf32to16u(U"\x00C0")) == true);
    TESTX(uni::norm::utf16_is_nfkd(uni::utf32to16u(U"\x00C0")) == false);

    // Ill-formed -> always false
    TESTX(uni::norm::utf8_is_nfc("\x80") == false);
    TESTX(uni::norm::utf8_is_nfd("\x80") == false);
    TESTX(uni::norm::utf8_is_nfkc("\x80") == false);
    TESTX(uni::norm::utf8_is_nfkd("\x80") == false);

    TESTX(uni::norm::utf16_is_nfc(u"\xDC00") == false);
    TESTX(uni::norm::utf16_is_nfd(u"\xDC00") == false);
    TESTX(uni::norm::utf16_is_nfkc(u"\xDC00") == false);
    TESTX(uni::norm::utf16_is_nfkd(u"\xDC00") == false);

    std::cout << "DONE: Detecting Normalization Forms" << '\n';
}

void test_norm_stream_safe()
{
    // Note: 0x034F is U+034F COMBINING GRAPHEME JOINER (CGJ) that must be inserted within long sequences (30) of non-starters

    std::u32string NFC, NFD, NFKC, NFKD, NFC_CGJ, NFD_CGJ, NFKC_CGJ, NFKD_CGJ;

    // 0F77 = 0FB2 0F71 0F80 in NFKD, 0F71 0F80 both are trailing non-starters, 0300 is non-starter

    NFC  = U"\x0F77" + std::u32string(50, 0x0300);
    NFD  = U"\x0F77" + std::u32string(50, 0x0300);
    NFKC = U"\x0FB2\x0F71\x0F80" + std::u32string(50, 0x0300);
    NFKD = U"\x0FB2\x0F71\x0F80" + std::u32string(50, 0x0300);

    NFC_CGJ  = NFC;  NFC_CGJ.insert(1 + 30 - 2, 1, 0x034F);
    NFD_CGJ  = NFD;  NFD_CGJ.insert(1 + 30 - 2, 1, 0x034F);
    NFKC_CGJ = NFKC; NFKC_CGJ.insert(3 + 30 - 2, 1, 0x034F);
    NFKD_CGJ = NFKD; NFKD_CGJ.insert(3 + 30 - 2, 1, 0x034F);

    TESTX(uni::utf32to8(NFC_CGJ) == uni::norm::utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == uni::norm::utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == uni::norm::utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == uni::norm::utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == uni::norm::utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == uni::norm::utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == uni::norm::utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == uni::norm::utf16_nfkd(uni::utf32to16u(NFKD)));

    TESTX(uni::utf32to8(NFC_CGJ) == utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == utf16_nfkd(uni::utf32to16u(NFKD)));

    // Same but instead of 0300 use FF9E, this is starter in NFC/NFD, but non-starter (3099) in NFKD/NFKC

    NFC  = U"\x0F77" + std::u32string(50, 0xFF9E);
    NFD  = U"\x0F77" + std::u32string(50, 0xFF9E);
    NFKC = U"\x0020\x0335\x0335" + std::u32string(50, 0x3099);
    NFKD = U"\x0020\x0335\x0335" + std::u32string(50, 0x3099);

    NFC_CGJ  = NFC;  NFC_CGJ.insert(1 + 30 - 2, 1, 0x034F);
    NFD_CGJ  = NFD;  NFD_CGJ.insert(1 + 30 - 2, 1, 0x034F);
    NFKC_CGJ = NFKC; NFKC_CGJ.insert(3 + 30 - 2, 1, 0x034F);
    NFKD_CGJ = NFKD; NFKD_CGJ.insert(3 + 30 - 2, 1, 0x034F);

    TESTX(uni::utf32to8(NFC_CGJ) == uni::norm::utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == uni::norm::utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == uni::norm::utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == uni::norm::utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == uni::norm::utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == uni::norm::utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == uni::norm::utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == uni::norm::utf16_nfkd(uni::utf32to16u(NFKD)));

    TESTX(uni::utf32to8(NFC_CGJ) == utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == utf16_nfkd(uni::utf32to16u(NFKD)));

    // Same but instead of 0300 use 1161, this is Hangul V "non-starter" with CCC=0, so CGJ must not be produced

    // This also tests buffer exhaustion, remove "if (*size > 4 && buffer_ccc[*size - 1] == 0)" from norm_decomp_return
    // and this test fails because the string will be truncated by the second check. That check exists for such cases
    // so even if there is an error in generated data or a bug in the algorithm itself there won't be buffer exhaustion.
    // Note: the buffer exhaustion test only works for NFC/NFKC because in NFD/NFKD such cases are impossible.

    NFC  = U"\x0F77" + std::u32string(70, 0x1161);
    NFD  = U"\x0F77" + std::u32string(70, 0x1161);
    NFKC = U"\x0FB2\x0F71\x0F80" + std::u32string(70, 0x1161);
    NFKD = U"\x0FB2\x0F71\x0F80" + std::u32string(70, 0x1161);

    NFC_CGJ  = NFC;
    NFD_CGJ  = NFD;
    NFKC_CGJ = NFKC;
    NFKD_CGJ = NFKD;

    TESTX(uni::utf32to8(NFC_CGJ) == uni::norm::utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == uni::norm::utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == uni::norm::utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == uni::norm::utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == uni::norm::utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == uni::norm::utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == uni::norm::utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == uni::norm::utf16_nfkd(uni::utf32to16u(NFKD)));

    TESTX(uni::utf32to8(NFC_CGJ) == utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == utf16_nfkd(uni::utf32to16u(NFKD)));

    // Same but use 00C0, it has 1 trailing non-starter therefore 0 initial non-starters, so CGJ must not be produced
    // This test to make sure that generated data for initial/trailing non-starters counters is not messed up/swapped

    NFC  = U"\x0F77" + std::u32string(50, 0x00C0);
    NFD  = U"\x0F77"; for (int i = 0; i < 50; i++) NFD += U"\x0041\x0300";
    NFKC = U"\x0FB2\x0F71\x0F80" + std::u32string(50, 0x00C0);
    NFKD = U"\x0FB2\x0F71\x0F80"; for (int i = 0; i < 50; i++) NFKD += U"\x0041\x0300";

    NFC_CGJ  = NFC;
    NFD_CGJ  = NFD;
    NFKC_CGJ = NFKC;
    NFKD_CGJ = NFKD;

    TESTX(uni::utf32to8(NFC_CGJ) == uni::norm::utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == uni::norm::utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == uni::norm::utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == uni::norm::utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == uni::norm::utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == uni::norm::utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == uni::norm::utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == uni::norm::utf16_nfkd(uni::utf32to16u(NFKD)));

    TESTX(uni::utf32to8(NFC_CGJ) == utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == utf16_nfkd(uni::utf32to16u(NFKD)));

    // EXTRA: 2 CGJ

    // Same as first but produce 2 CGJ

    NFC  = U"\x0F77" + std::u32string(70, 0x0300);
    NFD  = U"\x0F77" + std::u32string(70, 0x0300);
    NFKC = U"\x0FB2\x0F71\x0F80" + std::u32string(70, 0x0300);
    NFKD = U"\x0FB2\x0F71\x0F80" + std::u32string(70, 0x0300);

    NFC_CGJ  = NFC;  NFC_CGJ.insert(1 + 30 - 2, 1, 0x034F);  NFC_CGJ.insert(1 + 30 - 2 + 1 + 30, 1, 0x034F);
    NFD_CGJ  = NFD;  NFD_CGJ.insert(1 + 30 - 2, 1, 0x034F);  NFD_CGJ.insert(1 + 30 - 2 + 1 + 30, 1, 0x034F);
    NFKC_CGJ = NFKC; NFKC_CGJ.insert(3 + 30 - 2, 1, 0x034F); NFKC_CGJ.insert(3 + 30 - 2 + 1 + 30, 1, 0x034F);
    NFKD_CGJ = NFKD; NFKD_CGJ.insert(3 + 30 - 2, 1, 0x034F); NFKD_CGJ.insert(3 + 30 - 2 + 1 + 30, 1, 0x034F);

    TESTX(uni::utf32to8(NFC_CGJ) == uni::norm::utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == uni::norm::utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == uni::norm::utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == uni::norm::utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == uni::norm::utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == uni::norm::utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == uni::norm::utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == uni::norm::utf16_nfkd(uni::utf32to16u(NFKD)));

    TESTX(uni::utf32to8(NFC_CGJ) == utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == utf16_nfkd(uni::utf32to16u(NFKD)));

    // Same as first but strings full of non-starters + produce 2 CGJ

    NFC  = std::u32string(70, 0x0300);
    NFD  = std::u32string(70, 0x0300);
    NFKC = std::u32string(70, 0x0300);
    NFKD = std::u32string(70, 0x0300);

    NFC_CGJ  = NFC;  NFC_CGJ.insert(30, 1, 0x034F);  NFC_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFD_CGJ  = NFD;  NFD_CGJ.insert(30, 1, 0x034F);  NFD_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFKC_CGJ = NFKC; NFKC_CGJ.insert(30, 1, 0x034F); NFKC_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFKD_CGJ = NFKD; NFKD_CGJ.insert(30, 1, 0x034F); NFKD_CGJ.insert(30 + 1 + 30, 1, 0x034F);

    TESTX(uni::utf32to8(NFC_CGJ) == uni::norm::utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == uni::norm::utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == uni::norm::utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == uni::norm::utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == uni::norm::utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == uni::norm::utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == uni::norm::utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == uni::norm::utf16_nfkd(uni::utf32to16u(NFKD)));

    TESTX(uni::utf32to8(NFC_CGJ) == utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == utf16_nfkd(uni::utf32to16u(NFKD)));

    // Same as previous but use 0344 non-starter (and composition exclusion) that decomposes into 2 non-starters 0308 0301

    std::u32string TEMP;
    for (int i = 0; i < 35; i++)
        TEMP += U"\x0308\x0301";

    NFC  = std::u32string(35, 0x0344);
    NFD  = std::u32string(35, 0x0344);
    NFKC = std::u32string(35, 0x0344);
    NFKD = std::u32string(35, 0x0344);

    NFC_CGJ  = TEMP; NFC_CGJ.insert(30, 1, 0x034F);  NFC_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFD_CGJ  = TEMP; NFD_CGJ.insert(30, 1, 0x034F);  NFD_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFKC_CGJ = TEMP; NFKC_CGJ.insert(30, 1, 0x034F); NFKC_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFKD_CGJ = TEMP; NFKD_CGJ.insert(30, 1, 0x034F); NFKD_CGJ.insert(30 + 1 + 30, 1, 0x034F);

    TESTX(uni::utf32to8(NFC_CGJ) == uni::norm::utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == uni::norm::utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == uni::norm::utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == uni::norm::utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == uni::norm::utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == uni::norm::utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == uni::norm::utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == uni::norm::utf16_nfkd(uni::utf32to16u(NFKD)));

    TESTX(uni::utf32to8(NFC_CGJ) == utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == utf16_nfkd(uni::utf32to16u(NFKD)));

    // Corner case
    // When non-starter decomposes into 2 non-starters in some cases we need to add CGJ after 29 non-starters instead of 30
    // This can only happen if there are Non-Starter Decompositions (0344, 0F73, 0F75, 0F81) in a text
    // Note that this is conformant with Stream-Safe Text Process algorithm
    // The result is exatly the same if we apply Stream-Safe Text Process first and then normalize

    std::u32string TEMP1 = std::u32string(29, 0xFF9E) + U"\x344" + std::u32string(28, 0xFF9E) + U"\x344";
    std::u32string TEMP2 = std::u32string(29, 0xFF9E) + U"\x0308\x0301" + std::u32string(28, 0xFF9E) + U"\x0308\x0301";
    std::u32string TEMP3 = std::u32string(29, 0x0300) + U"\x344" + std::u32string(28, 0x0300) + U"\x344";
    std::u32string TEMP4 = std::u32string(29, 0x0300) + U"\x0308\x0301" + std::u32string(28, 0x0300) + U"\x0308\x0301";

    NFC  = TEMP1;
    NFD  = TEMP1;
    NFKC = TEMP3;
    NFKD = TEMP3;

    NFC_CGJ  = TEMP2; NFC_CGJ.insert(29, 1, 0x034F);  NFC_CGJ.insert(29 + 1 + 30, 1, 0x034F);
    NFD_CGJ  = TEMP2; NFD_CGJ.insert(29, 1, 0x034F);  NFD_CGJ.insert(29 + 1 + 30, 1, 0x034F);
    NFKC_CGJ = TEMP4; NFKC_CGJ.insert(29, 1, 0x034F); NFKC_CGJ.insert(29 + 1 + 30, 1, 0x034F);
    NFKD_CGJ = TEMP4; NFKD_CGJ.insert(29, 1, 0x034F); NFKD_CGJ.insert(29 + 1 + 30, 1, 0x034F);

    TESTX(uni::utf32to8(NFC_CGJ) == uni::norm::utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == uni::norm::utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == uni::norm::utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == uni::norm::utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == uni::norm::utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == uni::norm::utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == uni::norm::utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == uni::norm::utf16_nfkd(uni::utf32to16u(NFKD)));

    TESTX(uni::utf32to8(NFC_CGJ) == utf8_nfc(uni::utf32to8(NFC)));
    TESTX(uni::utf32to8(NFD_CGJ) == utf8_nfd(uni::utf32to8(NFD)));
    TESTX(uni::utf32to8(NFKC_CGJ) == utf8_nfkc(uni::utf32to8(NFKC)));
    TESTX(uni::utf32to8(NFKD_CGJ) == utf8_nfkd(uni::utf32to8(NFKD)));

    TESTX(uni::utf32to16u(NFC_CGJ) == utf16_nfc(uni::utf32to16u(NFC)));
    TESTX(uni::utf32to16u(NFD_CGJ) == utf16_nfd(uni::utf32to16u(NFD)));
    TESTX(uni::utf32to16u(NFKC_CGJ) == utf16_nfkc(uni::utf32to16u(NFKC)));
    TESTX(uni::utf32to16u(NFKD_CGJ) == utf16_nfkd(uni::utf32to16u(NFKD)));

    std::cout << "DONE: Normalization Stream-Safe Text Format" << '\n';
}

std::string utf8_nfc(std::string_view str)
{
    //return str | uni::views::utf8 | uni::views::norm::nfc | uni::ranges::to_utf8<std::string>();

    uni::iter::utf8 it_begin{str.cbegin(), str.cend()};
    uni::iter::utf8 it_end{str.cend(), str.cend()};

    uni::iter::norm::nfc it_norm_begin{it_begin, it_end};
    uni::iter::norm::nfc it_norm_end{it_end, it_end};

    std::string result;
    uni::iter::output::utf8 it_out{std::back_inserter(result)};

    for (auto it = it_norm_begin; it != it_norm_end; ++it)
        it_out = *it;

    return result;
}

std::string utf8_nfd(std::string_view str)
{
    //return str | uni::views::utf8 | uni::views::norm::nfd | uni::ranges::to_utf8<std::string>();

    uni::iter::utf8 it_begin{str.cbegin(), str.cend()};
    uni::iter::utf8 it_end{str.cend(), str.cend()};

    uni::iter::norm::nfd it_norm_begin{it_begin, it_end};
    uni::iter::norm::nfd it_norm_end{it_end, it_end};

    std::string result;
    uni::iter::output::utf8 it_out{std::back_inserter(result)};

    for (auto it = it_norm_begin; it != it_norm_end; ++it)
        it_out = *it;

    return result;
}

std::string utf8_nfkc(std::string_view str)
{
    //return str | uni::views::utf8 | uni::views::norm::nfkc | uni::ranges::to_utf8<std::string>();

    uni::iter::utf8 it_begin{str.cbegin(), str.cend()};
    uni::iter::utf8 it_end{str.cend(), str.cend()};

    uni::iter::norm::nfkc it_norm_begin{it_begin, it_end};
    uni::iter::norm::nfkc it_norm_end{it_end, it_end};

    std::string result;
    uni::iter::output::utf8 it_out{std::back_inserter(result)};

    for (auto it = it_norm_begin; it != it_norm_end; ++it)
        it_out = *it;

    return result;
}

std::string utf8_nfkd(std::string_view str)
{
    //return str | uni::views::utf8 | uni::views::norm::nfkd | uni::ranges::to_utf8<std::string>();

    uni::iter::utf8 it_begin{str.cbegin(), str.cend()};
    uni::iter::utf8 it_end{str.cend(), str.cend()};

    uni::iter::norm::nfkd it_norm_begin{it_begin, it_end};
    uni::iter::norm::nfkd it_norm_end{it_end, it_end};

    std::string result;
    uni::iter::output::utf8 it_out{std::back_inserter(result)};

    for (auto it = it_norm_begin; it != it_norm_end; ++it)
        it_out = *it;

    return result;
}

std::u16string utf16_nfc(std::u16string_view str)
{
    //return str | uni::views::utf16 | uni::views::norm::nfc | uni::ranges::to_utf16<std::u16string>();

    uni::iter::utf16 it_begin{str.cbegin(), str.cend()};
    uni::iter::utf16 it_end{str.cend(), str.cend()};

    uni::iter::norm::nfc it_norm_begin{it_begin, it_end};
    uni::iter::norm::nfc it_norm_end{it_end, it_end};

    std::u16string result;
    uni::iter::output::utf16 it_out{std::back_inserter(result)};

    for (auto it = it_norm_begin; it != it_norm_end; ++it)
        it_out = *it;

    return result;
}

std::u16string utf16_nfd(std::u16string_view str)
{
    //return str | uni::views::utf16 | uni::views::norm::nfd | uni::ranges::to_utf16<std::u16string>();

    uni::iter::utf16 it_begin{str.cbegin(), str.cend()};
    uni::iter::utf16 it_end{str.cend(), str.cend()};

    uni::iter::norm::nfd it_norm_begin{it_begin, it_end};
    uni::iter::norm::nfd it_norm_end{it_end, it_end};

    std::u16string result;
    uni::iter::output::utf16 it_out{std::back_inserter(result)};

    for (auto it = it_norm_begin; it != it_norm_end; ++it)
        it_out = *it;

    return result;
}

std::u16string utf16_nfkc(std::u16string_view str)
{
    //return str | uni::views::utf16 | uni::views::norm::nfkc | uni::ranges::to_utf16<std::u16string>();

    uni::iter::utf16 it_begin{str.cbegin(), str.cend()};
    uni::iter::utf16 it_end{str.cend(), str.cend()};

    uni::iter::norm::nfkc it_norm_begin{it_begin, it_end};
    uni::iter::norm::nfkc it_norm_end{it_end, it_end};

    std::u16string result;
    uni::iter::output::utf16 it_out{std::back_inserter(result)};

    for (auto it = it_norm_begin; it != it_norm_end; ++it)
        it_out = *it;

    return result;
}

std::u16string utf16_nfkd(std::u16string_view str)
{
    //return str | uni::views::utf16 | uni::views::norm::nfkd | uni::ranges::to_utf16<std::u16string>();

    uni::iter::utf16 it_begin{str.cbegin(), str.cend()};
    uni::iter::utf16 it_end{str.cend(), str.cend()};

    uni::iter::norm::nfkd it_norm_begin{it_begin, it_end};
    uni::iter::norm::nfkd it_norm_end{it_end, it_end};

    std::u16string result;
    uni::iter::output::utf16 it_out{std::back_inserter(result)};

    for (auto it = it_norm_begin; it != it_norm_end; ++it)
        it_out = *it;

    return result;
}
