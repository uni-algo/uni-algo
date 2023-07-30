/* Test for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

// Forward declaration for normalization functions that uses high-level ranges
// We must test the ranges together with fast functions
// See the end of the file for bodies
test_constexpr std::string to_nfc_utf8(std::string_view str);
test_constexpr std::string to_nfd_utf8(std::string_view str);
test_constexpr std::string to_nfkc_utf8(std::string_view str);
test_constexpr std::string to_nfkd_utf8(std::string_view str);
test_constexpr std::u16string to_nfc_utf16(std::u16string_view str);
test_constexpr std::u16string to_nfd_utf16(std::u16string_view str);
test_constexpr std::u16string to_nfkc_utf16(std::u16string_view str);
test_constexpr std::u16string to_nfkd_utf16(std::u16string_view str);

bool test_norm_impl(size_t& count_lines, bool with_part2 = true, size_t with_prefix = 0, size_t with_suffix = 0)
{
    std::ifstream input("NormalizationTest.txt", std::ios::binary);
    TESTX(input.is_open());
    // Latest version:
    // https://www.unicode.org/Public/UNIDATA/NormalizationTest.txt
    // Other versions:
    // https://www.unicode.org/Public/12.0.0/ucd/NormalizationTest.txt
    // https://www.unicode.org/Public/12.1.0/ucd/NormalizationTest.txt
    // etc.

    std::u32string source, NFC, NFD, NFKC, NFKD;
    std::set<char32_t> set;
    count_lines = 0;

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

        std::u16string c1_utf16 = una::utf32to16u(source);
        std::u16string c2_utf16 = una::utf32to16u(NFC);
        std::u16string c3_utf16 = una::utf32to16u(NFD);
        std::u16string c4_utf16 = una::utf32to16u(NFKC);
        std::u16string c5_utf16 = una::utf32to16u(NFKD);

        // NFC
        TESTX(c2_utf16 == una::norm::to_nfc_utf16(c1_utf16));
        TESTX(c2_utf16 == una::norm::to_nfc_utf16(c2_utf16));
        TESTX(c2_utf16 == una::norm::to_nfc_utf16(c3_utf16));

        TESTX(c4_utf16 == una::norm::to_nfc_utf16(c4_utf16));
        TESTX(c4_utf16 == una::norm::to_nfc_utf16(c5_utf16));

        // NFD
        TESTX(c3_utf16 == una::norm::to_nfd_utf16(c1_utf16));
        TESTX(c3_utf16 == una::norm::to_nfd_utf16(c2_utf16));
        TESTX(c3_utf16 == una::norm::to_nfd_utf16(c3_utf16));

        TESTX(c5_utf16 == una::norm::to_nfd_utf16(c4_utf16));
        TESTX(c5_utf16 == una::norm::to_nfd_utf16(c5_utf16));

        // NFKC
        TESTX(c4_utf16 == una::norm::to_nfkc_utf16(c1_utf16));
        TESTX(c4_utf16 == una::norm::to_nfkc_utf16(c2_utf16));
        TESTX(c4_utf16 == una::norm::to_nfkc_utf16(c3_utf16));
        TESTX(c4_utf16 == una::norm::to_nfkc_utf16(c4_utf16));
        TESTX(c4_utf16 == una::norm::to_nfkc_utf16(c5_utf16));

        // NFKD
        TESTX(c5_utf16 == una::norm::to_nfkd_utf16(c1_utf16));
        TESTX(c5_utf16 == una::norm::to_nfkd_utf16(c2_utf16));
        TESTX(c5_utf16 == una::norm::to_nfkd_utf16(c3_utf16));
        TESTX(c5_utf16 == una::norm::to_nfkd_utf16(c4_utf16));
        TESTX(c5_utf16 == una::norm::to_nfkd_utf16(c5_utf16));

        // NFC and NFKC quick checks cannot be tested with this data
        // because there is some lines that return NFC/NFKC_Quick_Check=Maybe
        // our functions return true only if there is a clear NFC/NFKC_Quick_Check=Yes
        // NFD and NFKD can be tested, there is no Maybe answer for these normalization forms
        //TESTX(una::norm::is_nfc_utf16(c2_utf16) == true);
        TESTX(una::norm::is_nfd_utf16(c3_utf16) == true);
        //TESTX(una::norm::is_nfkc_utf16(c4_utf16) == true);
        TESTX(una::norm::is_nfkd_utf16(c5_utf16) == true);

        // Ranges

        // NFC
        TESTX(c2_utf16 == to_nfc_utf16(c1_utf16));
        TESTX(c2_utf16 == to_nfc_utf16(c2_utf16));
        TESTX(c2_utf16 == to_nfc_utf16(c3_utf16));

        TESTX(c4_utf16 == to_nfc_utf16(c4_utf16));
        TESTX(c4_utf16 == to_nfc_utf16(c5_utf16));

        // NFD
        TESTX(c3_utf16 == to_nfd_utf16(c1_utf16));
        TESTX(c3_utf16 == to_nfd_utf16(c2_utf16));
        TESTX(c3_utf16 == to_nfd_utf16(c3_utf16));

        TESTX(c5_utf16 == to_nfd_utf16(c4_utf16));
        TESTX(c5_utf16 == to_nfd_utf16(c5_utf16));

        // NFKC
        TESTX(c4_utf16 == to_nfkc_utf16(c1_utf16));
        TESTX(c4_utf16 == to_nfkc_utf16(c2_utf16));
        TESTX(c4_utf16 == to_nfkc_utf16(c3_utf16));
        TESTX(c4_utf16 == to_nfkc_utf16(c4_utf16));
        TESTX(c4_utf16 == to_nfkc_utf16(c5_utf16));

        // NFKD
        TESTX(c5_utf16 == to_nfkd_utf16(c1_utf16));
        TESTX(c5_utf16 == to_nfkd_utf16(c2_utf16));
        TESTX(c5_utf16 == to_nfkd_utf16(c3_utf16));
        TESTX(c5_utf16 == to_nfkd_utf16(c4_utf16));
        TESTX(c5_utf16 == to_nfkd_utf16(c5_utf16));

        // UTF-8

        std::string c1_utf8 = una::utf32to8(source);
        std::string c2_utf8 = una::utf32to8(NFC);
        std::string c3_utf8 = una::utf32to8(NFD);
        std::string c4_utf8 = una::utf32to8(NFKC);
        std::string c5_utf8 = una::utf32to8(NFKD);

        // NFC
        TESTX(c2_utf8 == una::norm::to_nfc_utf8(c1_utf8));
        TESTX(c2_utf8 == una::norm::to_nfc_utf8(c2_utf8));
        TESTX(c2_utf8 == una::norm::to_nfc_utf8(c3_utf8));

        TESTX(c4_utf8 == una::norm::to_nfc_utf8(c4_utf8));
        TESTX(c4_utf8 == una::norm::to_nfc_utf8(c5_utf8));

        // NFD
        TESTX(c3_utf8 == una::norm::to_nfd_utf8(c1_utf8));
        TESTX(c3_utf8 == una::norm::to_nfd_utf8(c2_utf8));
        TESTX(c3_utf8 == una::norm::to_nfd_utf8(c3_utf8));

        TESTX(c5_utf8 == una::norm::to_nfd_utf8(c4_utf8));
        TESTX(c5_utf8 == una::norm::to_nfd_utf8(c5_utf8));

        // NFKC
        TESTX(c4_utf8 == una::norm::to_nfkc_utf8(c1_utf8));
        TESTX(c4_utf8 == una::norm::to_nfkc_utf8(c2_utf8));
        TESTX(c4_utf8 == una::norm::to_nfkc_utf8(c3_utf8));
        TESTX(c4_utf8 == una::norm::to_nfkc_utf8(c4_utf8));
        TESTX(c4_utf8 == una::norm::to_nfkc_utf8(c5_utf8));

        // NFKD
        TESTX(c5_utf8 == una::norm::to_nfkd_utf8(c1_utf8));
        TESTX(c5_utf8 == una::norm::to_nfkd_utf8(c2_utf8));
        TESTX(c5_utf8 == una::norm::to_nfkd_utf8(c3_utf8));
        TESTX(c5_utf8 == una::norm::to_nfkd_utf8(c4_utf8));
        TESTX(c5_utf8 == una::norm::to_nfkd_utf8(c5_utf8));

        //TESTX(una::norm::is_nfc_utf8(c2_utf8) == true);
        TESTX(una::norm::is_nfd_utf8(c3_utf8) == true);
        //TESTX(una::norm::is_nfkc_utf8(c4_utf8) == true);
        TESTX(una::norm::is_nfkd_utf8(c5_utf8) == true);

        // Ranges

        // NFC
        TESTX(c2_utf8 == to_nfc_utf8(c1_utf8));
        TESTX(c2_utf8 == to_nfc_utf8(c2_utf8));
        TESTX(c2_utf8 == to_nfc_utf8(c3_utf8));

        TESTX(c4_utf8 == to_nfc_utf8(c4_utf8));
        TESTX(c4_utf8 == to_nfc_utf8(c5_utf8));

        // NFD
        TESTX(c3_utf8 == to_nfd_utf8(c1_utf8));
        TESTX(c3_utf8 == to_nfd_utf8(c2_utf8));
        TESTX(c3_utf8 == to_nfd_utf8(c3_utf8));

        TESTX(c5_utf8 == to_nfd_utf8(c4_utf8));
        TESTX(c5_utf8 == to_nfd_utf8(c5_utf8));

        // NFKC
        TESTX(c4_utf8 == to_nfkc_utf8(c1_utf8));
        TESTX(c4_utf8 == to_nfkc_utf8(c2_utf8));
        TESTX(c4_utf8 == to_nfkc_utf8(c3_utf8));
        TESTX(c4_utf8 == to_nfkc_utf8(c4_utf8));
        TESTX(c4_utf8 == to_nfkc_utf8(c5_utf8));

        // NFKD
        TESTX(c5_utf8 == to_nfkd_utf8(c1_utf8));
        TESTX(c5_utf8 == to_nfkd_utf8(c2_utf8));
        TESTX(c5_utf8 == to_nfkd_utf8(c3_utf8));
        TESTX(c5_utf8 == to_nfkd_utf8(c4_utf8));
        TESTX(c5_utf8 == to_nfkd_utf8(c5_utf8));
    }

    if (!with_part2)
        return true;

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

            std::string X_utf8 = una::utf32to8(X);

            TESTX(X_utf8 == una::norm::to_nfc_utf8(X_utf8));
            TESTX(X_utf8 == una::norm::to_nfd_utf8(X_utf8));
            TESTX(X_utf8 == una::norm::to_nfkc_utf8(X_utf8));
            TESTX(X_utf8 == una::norm::to_nfkd_utf8(X_utf8));

            TESTX(una::norm::is_nfc_utf8(X_utf8) == true);
            TESTX(una::norm::is_nfd_utf8(X_utf8) == true);
            TESTX(una::norm::is_nfkc_utf8(X_utf8) == true);
            TESTX(una::norm::is_nfkd_utf8(X_utf8) == true);

            // Ranges
            TESTX(X_utf8 == to_nfc_utf8(X_utf8));
            TESTX(X_utf8 == to_nfd_utf8(X_utf8));
            TESTX(X_utf8 == to_nfkc_utf8(X_utf8));
            TESTX(X_utf8 == to_nfkd_utf8(X_utf8));

            // UTF-16

            std::u16string X_utf16 = una::utf32to16u(X);

            TESTX(X_utf16 == una::norm::to_nfc_utf16(X_utf16));
            TESTX(X_utf16 == una::norm::to_nfd_utf16(X_utf16));
            TESTX(X_utf16 == una::norm::to_nfkc_utf16(X_utf16));
            TESTX(X_utf16 == una::norm::to_nfkd_utf16(X_utf16));

            TESTX(una::norm::is_nfc_utf16(X_utf16) == true);
            TESTX(una::norm::is_nfd_utf16(X_utf16) == true);
            TESTX(una::norm::is_nfkc_utf16(X_utf16) == true);
            TESTX(una::norm::is_nfkd_utf16(X_utf16) == true);

            // Ranges
            TESTX(X_utf16 == to_nfc_utf16(X_utf16));
            TESTX(X_utf16 == to_nfd_utf16(X_utf16));
            TESTX(X_utf16 == to_nfkc_utf16(X_utf16));
            TESTX(X_utf16 == to_nfkd_utf16(X_utf16));
        }
    }

    return true;
}

void test_norm()
{
    size_t count_lines = 0;

    test_norm_impl(count_lines);
    std::cout << "DONE: " << count_lines << " lines" << '\n';
    test_norm_impl(count_lines, false, 100, 0);
    std::cout << "DONE: " << count_lines << " lines with prefix" << '\n';
    test_norm_impl(count_lines, false, 0, 100);
    std::cout << "DONE: " << count_lines << " lines with suffix" << '\n';
}

test_constexpr bool test_norm_detect()
{
    // Composition exclusion -> always false
    TESTX(una::norm::is_nfc_utf8(una::utf32to8(U"\x0344")) == false);
    TESTX(una::norm::is_nfd_utf8(una::utf32to8(U"\x0344")) == false);
    TESTX(una::norm::is_nfkc_utf8(una::utf32to8(U"\x0344")) == false);
    TESTX(una::norm::is_nfkd_utf8(una::utf32to8(U"\x0344")) == false);

    TESTX(una::norm::is_nfc_utf16(una::utf32to16u(U"\x0344")) == false);
    TESTX(una::norm::is_nfd_utf16(una::utf32to16u(U"\x0344")) == false);
    TESTX(una::norm::is_nfkc_utf16(una::utf32to16u(U"\x0344")) == false);
    TESTX(una::norm::is_nfkd_utf16(una::utf32to16u(U"\x0344")) == false);

    // Must be in the correct order
    TESTX(una::norm::is_nfc_utf8(una::utf32to8(U"\x0315\x0305")) == false);
    TESTX(una::norm::is_nfd_utf8(una::utf32to8(U"\x0315\x0305")) == false);
    TESTX(una::norm::is_nfkc_utf8(una::utf32to8(U"\x0315\x0305")) == false);
    TESTX(una::norm::is_nfkd_utf8(una::utf32to8(U"\x0315\x0305")) == false);

    TESTX(una::norm::is_nfc_utf16(una::utf32to16u(U"\x0315\x0305")) == false);
    TESTX(una::norm::is_nfd_utf16(una::utf32to16u(U"\x0315\x0305")) == false);
    TESTX(una::norm::is_nfkc_utf16(una::utf32to16u(U"\x0315\x0305")) == false);
    TESTX(una::norm::is_nfkd_utf16(una::utf32to16u(U"\x0315\x0305")) == false);

    // Must be decomposed in NFKC/NFKD
    TESTX(una::norm::is_nfc_utf8(una::utf32to8(U"\x00A0")) == true);
    TESTX(una::norm::is_nfd_utf8(una::utf32to8(U"\x00A0")) == true);
    TESTX(una::norm::is_nfkc_utf8(una::utf32to8(U"\x00A0")) == false);
    TESTX(una::norm::is_nfkd_utf8(una::utf32to8(U"\x00A0")) == false);

    TESTX(una::norm::is_nfc_utf16(una::utf32to16u(U"\x00A0")) == true);
    TESTX(una::norm::is_nfd_utf16(una::utf32to16u(U"\x00A0")) == true);
    TESTX(una::norm::is_nfkc_utf16(una::utf32to16u(U"\x00A0")) == false);
    TESTX(una::norm::is_nfkd_utf16(una::utf32to16u(U"\x00A0")) == false);

    // Must be decomposed in NFD/NFKD
    TESTX(una::norm::is_nfc_utf8(una::utf32to8(U"\x00C0")) == true);
    TESTX(una::norm::is_nfd_utf8(una::utf32to8(U"\x00C0")) == false);
    TESTX(una::norm::is_nfkc_utf8(una::utf32to8(U"\x00C0")) == true);
    TESTX(una::norm::is_nfkd_utf8(una::utf32to8(U"\x00C0")) == false);

    TESTX(una::norm::is_nfc_utf16(una::utf32to16u(U"\x00C0")) == true);
    TESTX(una::norm::is_nfd_utf16(una::utf32to16u(U"\x00C0")) == false);
    TESTX(una::norm::is_nfkc_utf16(una::utf32to16u(U"\x00C0")) == true);
    TESTX(una::norm::is_nfkd_utf16(una::utf32to16u(U"\x00C0")) == false);

    // Ill-formed -> always false
    TESTX(una::norm::is_nfc_utf8("\x80") == false);
    TESTX(una::norm::is_nfd_utf8("\x80") == false);
    TESTX(una::norm::is_nfkc_utf8("\x80") == false);
    TESTX(una::norm::is_nfkd_utf8("\x80") == false);

    TESTX(una::norm::is_nfc_utf16(u"\xDC00") == false);
    TESTX(una::norm::is_nfd_utf16(u"\xDC00") == false);
    TESTX(una::norm::is_nfkc_utf16(u"\xDC00") == false);
    TESTX(una::norm::is_nfkd_utf16(u"\xDC00") == false);

    // Not stream safe -> always false

    // NOTE: U+0305 non-starter is used below instead of U+0300 or something because it can appear
    // in all normalization forms, but for example U+0300 can only appear in NFD and NFKD.

    std::u32string not_stream_safe_utf32 = std::u32string(31, 0x0305); // 31 non-starters = not stream-safe
    std::string not_stream_safe_utf8 = una::utf32to8(not_stream_safe_utf32);
    std::u16string not_stream_safe_utf16 = una::utf32to16u(not_stream_safe_utf32);

    TESTX(una::norm::is_nfc_utf8(not_stream_safe_utf8) == false);
    TESTX(una::norm::is_nfd_utf8(not_stream_safe_utf8) == false);
    TESTX(una::norm::is_nfkc_utf8(not_stream_safe_utf8) == false);
    TESTX(una::norm::is_nfkd_utf8(not_stream_safe_utf8) == false);

    TESTX(una::norm::is_nfc_utf16(not_stream_safe_utf16) == false);
    TESTX(una::norm::is_nfd_utf16(not_stream_safe_utf16) == false);
    TESTX(una::norm::is_nfkc_utf16(not_stream_safe_utf16) == false);
    TESTX(una::norm::is_nfkd_utf16(not_stream_safe_utf16) == false);

    std::u32string stream_safe_utf32 = std::u32string(30, 0x0305); // 30 non-starters = stream-safe
    std::string stream_safe_utf8 = una::utf32to8(stream_safe_utf32);
    std::u16string stream_safe_utf16 = una::utf32to16u(stream_safe_utf32);

    TESTX(una::norm::is_nfc_utf8(stream_safe_utf8) == true);
    TESTX(una::norm::is_nfd_utf8(stream_safe_utf8) == true);
    TESTX(una::norm::is_nfkc_utf8(stream_safe_utf8) == true);
    TESTX(una::norm::is_nfkd_utf8(stream_safe_utf8) == true);

    TESTX(una::norm::is_nfc_utf16(stream_safe_utf16) == true);
    TESTX(una::norm::is_nfd_utf16(stream_safe_utf16) == true);
    TESTX(una::norm::is_nfkc_utf16(stream_safe_utf16) == true);
    TESTX(una::norm::is_nfkd_utf16(stream_safe_utf16) == true);

    return true;
}

test_constexpr bool test_norm_detect_error()
{
    // NOTE: Copy-paste of the previous test function test_norm_detect but with una::error parameter

    una::error error;

    // Composition exclusion -> always false
    TESTX(una::norm::is_nfc_utf8(una::utf32to8(U"\x0344"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfd_utf8(una::utf32to8(U"\x0344"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkc_utf8(una::utf32to8(U"\x0344"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkd_utf8(una::utf32to8(U"\x0344"), error) == false && error == una::error::code::not_normalized);

    TESTX(una::norm::is_nfc_utf16(una::utf32to16u(U"\x0344"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfd_utf16(una::utf32to16u(U"\x0344"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkc_utf16(una::utf32to16u(U"\x0344"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkd_utf16(una::utf32to16u(U"\x0344"), error) == false && error == una::error::code::not_normalized);

    // Must be in the correct order
    TESTX(una::norm::is_nfc_utf8(una::utf32to8(U"\x0315\x0305"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfd_utf8(una::utf32to8(U"\x0315\x0305"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkc_utf8(una::utf32to8(U"\x0315\x0305"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkd_utf8(una::utf32to8(U"\x0315\x0305"), error) == false && error == una::error::code::not_normalized);

    TESTX(una::norm::is_nfc_utf16(una::utf32to16u(U"\x0315\x0305"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfd_utf16(una::utf32to16u(U"\x0315\x0305"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkc_utf16(una::utf32to16u(U"\x0315\x0305"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkd_utf16(una::utf32to16u(U"\x0315\x0305"), error) == false && error == una::error::code::not_normalized);

    // Must be decomposed in NFKC/NFKD
    TESTX(una::norm::is_nfc_utf8(una::utf32to8(U"\x00A0"), error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfd_utf8(una::utf32to8(U"\x00A0"), error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfkc_utf8(una::utf32to8(U"\x00A0"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkd_utf8(una::utf32to8(U"\x00A0"), error) == false && error == una::error::code::not_normalized);

    TESTX(una::norm::is_nfc_utf16(una::utf32to16u(U"\x00A0"), error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfd_utf16(una::utf32to16u(U"\x00A0"), error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfkc_utf16(una::utf32to16u(U"\x00A0"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkd_utf16(una::utf32to16u(U"\x00A0"), error) == false && error == una::error::code::not_normalized);

    // Must be decomposed in NFD/NFKD
    TESTX(una::norm::is_nfc_utf8(una::utf32to8(U"\x00C0"), error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfd_utf8(una::utf32to8(U"\x00C0"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkc_utf8(una::utf32to8(U"\x00C0"), error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfkd_utf8(una::utf32to8(U"\x00C0"), error) == false && error == una::error::code::not_normalized);

    TESTX(una::norm::is_nfc_utf16(una::utf32to16u(U"\x00C0"), error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfd_utf16(una::utf32to16u(U"\x00C0"), error) == false && error == una::error::code::not_normalized);
    TESTX(una::norm::is_nfkc_utf16(una::utf32to16u(U"\x00C0"), error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfkd_utf16(una::utf32to16u(U"\x00C0"), error) == false && error == una::error::code::not_normalized);

    // Ill-formed -> always false
    TESTX(una::norm::is_nfc_utf8("\x80", error) == false && error == una::error::code::ill_formed_utf);
    TESTX(una::norm::is_nfd_utf8("\x80", error) == false && error == una::error::code::ill_formed_utf);
    TESTX(una::norm::is_nfkc_utf8("\x80", error) == false && error == una::error::code::ill_formed_utf);
    TESTX(una::norm::is_nfkd_utf8("\x80", error) == false && error == una::error::code::ill_formed_utf);

    TESTX(una::norm::is_nfc_utf16(u"\xDC00", error) == false && error == una::error::code::ill_formed_utf);
    TESTX(una::norm::is_nfd_utf16(u"\xDC00", error) == false && error == una::error::code::ill_formed_utf);
    TESTX(una::norm::is_nfkc_utf16(u"\xDC00", error) == false && error == una::error::code::ill_formed_utf);
    TESTX(una::norm::is_nfkd_utf16(u"\xDC00", error) == false && error == una::error::code::ill_formed_utf);

    // Not stream safe -> always false

    std::u32string not_stream_safe_utf32 = std::u32string(31, 0x0305); // 31 non-starters = not stream-safe
    std::string not_stream_safe_utf8 = una::utf32to8(not_stream_safe_utf32);
    std::u16string not_stream_safe_utf16 = una::utf32to16u(not_stream_safe_utf32);

    TESTX(una::norm::is_nfc_utf8(not_stream_safe_utf8, error) == false && error == una::error::code::not_stream_safe);
    TESTX(una::norm::is_nfd_utf8(not_stream_safe_utf8, error) == false && error == una::error::code::not_stream_safe);
    TESTX(una::norm::is_nfkc_utf8(not_stream_safe_utf8, error) == false && error == una::error::code::not_stream_safe);
    TESTX(una::norm::is_nfkd_utf8(not_stream_safe_utf8, error) == false && error == una::error::code::not_stream_safe);

    TESTX(una::norm::is_nfc_utf16(not_stream_safe_utf16, error) == false && error == una::error::code::not_stream_safe);
    TESTX(una::norm::is_nfd_utf16(not_stream_safe_utf16, error) == false && error == una::error::code::not_stream_safe);
    TESTX(una::norm::is_nfkc_utf16(not_stream_safe_utf16, error) == false && error == una::error::code::not_stream_safe);
    TESTX(una::norm::is_nfkd_utf16(not_stream_safe_utf16, error) == false && error == una::error::code::not_stream_safe);

    std::u32string stream_safe_utf32 = std::u32string(30, 0x0305); // 30 non-starters = stream-safe
    std::string stream_safe_utf8 = una::utf32to8(stream_safe_utf32);
    std::u16string stream_safe_utf16 = una::utf32to16u(stream_safe_utf32);

    TESTX(una::norm::is_nfc_utf8(stream_safe_utf8, error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfd_utf8(stream_safe_utf8, error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfkc_utf8(stream_safe_utf8, error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfkd_utf8(stream_safe_utf8, error) == true && error == una::error::code::success);

    TESTX(una::norm::is_nfc_utf16(stream_safe_utf16, error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfd_utf16(stream_safe_utf16, error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfkc_utf16(stream_safe_utf16, error) == true && error == una::error::code::success);
    TESTX(una::norm::is_nfkd_utf16(stream_safe_utf16, error) == true && error == una::error::code::success);

    return true;
}

test_constexpr bool test_norm_stream_safe()
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

    TESTX(una::utf32to8(NFC_CGJ) == una::norm::to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == una::norm::to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == una::norm::to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == una::norm::to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == una::norm::to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == una::norm::to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == una::norm::to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == una::norm::to_nfkd_utf16(una::utf32to16u(NFKD)));

    TESTX(una::utf32to8(NFC_CGJ) == to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == to_nfkd_utf16(una::utf32to16u(NFKD)));

    // Same but instead of 0300 use FF9E, this is starter in NFC/NFD, but non-starter (3099) in NFKD/NFKC

    NFC  = U"\x0F77" + std::u32string(50, 0xFF9E);
    NFD  = U"\x0F77" + std::u32string(50, 0xFF9E);
    NFKC = U"\x0020\x0335\x0335" + std::u32string(50, 0x3099);
    NFKD = U"\x0020\x0335\x0335" + std::u32string(50, 0x3099);

    NFC_CGJ  = NFC;  NFC_CGJ.insert(1 + 30 - 2, 1, 0x034F);
    NFD_CGJ  = NFD;  NFD_CGJ.insert(1 + 30 - 2, 1, 0x034F);
    NFKC_CGJ = NFKC; NFKC_CGJ.insert(3 + 30 - 2, 1, 0x034F);
    NFKD_CGJ = NFKD; NFKD_CGJ.insert(3 + 30 - 2, 1, 0x034F);

    TESTX(una::utf32to8(NFC_CGJ) == una::norm::to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == una::norm::to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == una::norm::to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == una::norm::to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == una::norm::to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == una::norm::to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == una::norm::to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == una::norm::to_nfkd_utf16(una::utf32to16u(NFKD)));

    TESTX(una::utf32to8(NFC_CGJ) == to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == to_nfkd_utf16(una::utf32to16u(NFKD)));

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

    TESTX(una::utf32to8(NFC_CGJ) == una::norm::to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == una::norm::to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == una::norm::to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == una::norm::to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == una::norm::to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == una::norm::to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == una::norm::to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == una::norm::to_nfkd_utf16(una::utf32to16u(NFKD)));

    TESTX(una::utf32to8(NFC_CGJ) == to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == to_nfkd_utf16(una::utf32to16u(NFKD)));

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

    TESTX(una::utf32to8(NFC_CGJ) == una::norm::to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == una::norm::to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == una::norm::to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == una::norm::to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == una::norm::to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == una::norm::to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == una::norm::to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == una::norm::to_nfkd_utf16(una::utf32to16u(NFKD)));

    TESTX(una::utf32to8(NFC_CGJ) == to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == to_nfkd_utf16(una::utf32to16u(NFKD)));

    // EXTRA: 2 CGJ

    // Same as first but produce 2 CGJ

    // NOTE: Use std::u32string(1, 0x0F77) instead of U"\x0F77" here to dodge GCC 12 bug
    // For details see commit: f739534d96a91ac01f32057f34e72abf06d6648c

    NFC  = std::u32string(1, 0x0F77) + std::u32string(70, 0x0300);
    NFD  = U"\x0F77" + std::u32string(70, 0x0300);
    NFKC = U"\x0FB2\x0F71\x0F80" + std::u32string(70, 0x0300);
    NFKD = U"\x0FB2\x0F71\x0F80" + std::u32string(70, 0x0300);

    NFC_CGJ  = NFC;  NFC_CGJ.insert(1 + 30 - 2, 1, 0x034F);  NFC_CGJ.insert(1 + 30 - 2 + 1 + 30, 1, 0x034F);
    NFD_CGJ  = NFD;  NFD_CGJ.insert(1 + 30 - 2, 1, 0x034F);  NFD_CGJ.insert(1 + 30 - 2 + 1 + 30, 1, 0x034F);
    NFKC_CGJ = NFKC; NFKC_CGJ.insert(3 + 30 - 2, 1, 0x034F); NFKC_CGJ.insert(3 + 30 - 2 + 1 + 30, 1, 0x034F);
    NFKD_CGJ = NFKD; NFKD_CGJ.insert(3 + 30 - 2, 1, 0x034F); NFKD_CGJ.insert(3 + 30 - 2 + 1 + 30, 1, 0x034F);

    TESTX(una::utf32to8(NFC_CGJ) == una::norm::to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == una::norm::to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == una::norm::to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == una::norm::to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == una::norm::to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == una::norm::to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == una::norm::to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == una::norm::to_nfkd_utf16(una::utf32to16u(NFKD)));

    TESTX(una::utf32to8(NFC_CGJ) == to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == to_nfkd_utf16(una::utf32to16u(NFKD)));

    // Same as first but strings full of non-starters + produce 2 CGJ

    NFC  = std::u32string(70, 0x0300);
    NFD  = std::u32string(70, 0x0300);
    NFKC = std::u32string(70, 0x0300);
    NFKD = std::u32string(70, 0x0300);

    NFC_CGJ  = NFC;  NFC_CGJ.insert(30, 1, 0x034F);  NFC_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFD_CGJ  = NFD;  NFD_CGJ.insert(30, 1, 0x034F);  NFD_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFKC_CGJ = NFKC; NFKC_CGJ.insert(30, 1, 0x034F); NFKC_CGJ.insert(30 + 1 + 30, 1, 0x034F);
    NFKD_CGJ = NFKD; NFKD_CGJ.insert(30, 1, 0x034F); NFKD_CGJ.insert(30 + 1 + 30, 1, 0x034F);

    TESTX(una::utf32to8(NFC_CGJ) == una::norm::to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == una::norm::to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == una::norm::to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == una::norm::to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == una::norm::to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == una::norm::to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == una::norm::to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == una::norm::to_nfkd_utf16(una::utf32to16u(NFKD)));

    TESTX(una::utf32to8(NFC_CGJ) == to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == to_nfkd_utf16(una::utf32to16u(NFKD)));

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

    TESTX(una::utf32to8(NFC_CGJ) == una::norm::to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == una::norm::to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == una::norm::to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == una::norm::to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == una::norm::to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == una::norm::to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == una::norm::to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == una::norm::to_nfkd_utf16(una::utf32to16u(NFKD)));

    TESTX(una::utf32to8(NFC_CGJ) == to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == to_nfkd_utf16(una::utf32to16u(NFKD)));

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

    TESTX(una::utf32to8(NFC_CGJ) == una::norm::to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == una::norm::to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == una::norm::to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == una::norm::to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == una::norm::to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == una::norm::to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == una::norm::to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == una::norm::to_nfkd_utf16(una::utf32to16u(NFKD)));

    TESTX(una::utf32to8(NFC_CGJ) == to_nfc_utf8(una::utf32to8(NFC)));
    TESTX(una::utf32to8(NFD_CGJ) == to_nfd_utf8(una::utf32to8(NFD)));
    TESTX(una::utf32to8(NFKC_CGJ) == to_nfkc_utf8(una::utf32to8(NFKC)));
    TESTX(una::utf32to8(NFKD_CGJ) == to_nfkd_utf8(una::utf32to8(NFKD)));

    TESTX(una::utf32to16u(NFC_CGJ) == to_nfc_utf16(una::utf32to16u(NFC)));
    TESTX(una::utf32to16u(NFD_CGJ) == to_nfd_utf16(una::utf32to16u(NFD)));
    TESTX(una::utf32to16u(NFKC_CGJ) == to_nfkc_utf16(una::utf32to16u(NFKC)));
    TESTX(una::utf32to16u(NFKD_CGJ) == to_nfkd_utf16(una::utf32to16u(NFKD)));

    // Extra test to test that the size of the internal normalization buffer is enough.
    // The following BUF_NFC when decomposes to BUF_NFKD uses the max size of the buffer 51.
    // U+AC01 is LVT hangul that decomposes to 3 starters, U+0300 is non-starter and U+FDFA decomposes to 18 code points.
    // The test only performed for NFKD because other forms require smaller buffer anyway.
    // REMINDER: To make this test fail change buffer size (norm_buffer_size variable) in impl/impl_norm.h to less than 51,
    // it will fail because norm_decomp_return function there has extra safe measures for this case and even without them
    // it still fail but in the safe layer so it should be impossible to get UB if the buffer size won't be enough by a mistake.
    // TODO: This function is not a good place for the test because it has nothing to do with stream-safe.

    std::u32string BUF_NFC = std::u32string(1, 0xAC01) + std::u32string(30, 0x0300) + std::u32string(1, 0xFDFA);
    std::u32string BUF_NFKD = U"\x1100\x1161\x11A8" + std::u32string(30, 0x0300) +
            U"\x0635\x0644\x0649\x0020\x0627\x0644\x0644\x0647\x0020\x0639\x0644\x064A\x0647\x0020\x0648\x0633\x0644\x0645";

    TESTX(una::utf32to8(BUF_NFKD) == una::norm::to_nfkd_utf8(una::utf32to8(BUF_NFC)));

    TESTX(una::utf32to16u(BUF_NFKD) == una::norm::to_nfkd_utf16(una::utf32to16u(BUF_NFC)));

    TESTX(una::utf32to8(BUF_NFKD) == to_nfkd_utf8(una::utf32to8(BUF_NFC)));

    TESTX(una::utf32to16u(BUF_NFKD) == to_nfkd_utf16(una::utf32to16u(BUF_NFC)));

    return true;
}

test_constexpr std::string to_nfc_utf8(std::string_view str)
{
    return str | una::views::utf8 | una::views::norm::nfc | una::ranges::to_utf8<std::string>();
}

test_constexpr std::string to_nfd_utf8(std::string_view str)
{
    return str | una::views::utf8 | una::views::norm::nfd | una::ranges::to_utf8<std::string>();
}

test_constexpr std::string to_nfkc_utf8(std::string_view str)
{
    return str | una::views::utf8 | una::views::norm::nfkc | una::ranges::to_utf8<std::string>();
}

test_constexpr std::string to_nfkd_utf8(std::string_view str)
{
    return str | una::views::utf8 | una::views::norm::nfkd | una::ranges::to_utf8<std::string>();
}

test_constexpr std::u16string to_nfc_utf16(std::u16string_view str)
{
    return str | una::views::utf16 | una::views::norm::nfc | una::ranges::to_utf16<std::u16string>();
}

test_constexpr std::u16string to_nfd_utf16(std::u16string_view str)
{
    return str | una::views::utf16 | una::views::norm::nfd | una::ranges::to_utf16<std::u16string>();
}

test_constexpr std::u16string to_nfkc_utf16(std::u16string_view str)
{
    return str | una::views::utf16 | una::views::norm::nfkc | una::ranges::to_utf16<std::u16string>();
}

test_constexpr std::u16string to_nfkd_utf16(std::u16string_view str)
{
    return str | una::views::utf16 | una::views::norm::nfkd | una::ranges::to_utf16<std::u16string>();
}
