/* Example for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include "cpp_uni_iterator.h"
#include "cpp_uni_norm.h"

// -------

// For example we need to check two strings for canonical equivalence
// it can be done using functions like this: uni::norm::utf8_nfc(str1) == uni::norm::utf8_nfc(str2)
// but it will allocate memory for both strings then normalize them and only then compare them with a binary
// comparison even if only the first code point is different so it will be slow especially for long strings
// but we can use iterator chains to implement the algorithm and as a bonus it never allocates memory too

bool canonical_equivalence(std::string_view str1, std::string_view str2)
{
    // UTF-8 -> NFC iterator chains for both strings
    uni::iter::utf8 begin1{str1.cbegin(), str1.cend()};
    uni::iter::utf8 begin2{str2.cbegin(), str2.cend()};

    auto it1 = uni::iter::norm::nfc{begin1, uni::sentinel};
    auto it2 = uni::iter::norm::nfc{begin2, uni::sentinel};

    // Compare the strings by code points
    for (; it1 != uni::sentinel && it2 != uni::sentinel; ++it1, ++it2)
    {
        if (*it1 != *it2)
            return false;
    }

    // Reached the end in both strings then the strings are equal
    if (it1 == uni::sentinel && it2 == uni::sentinel)
        return true;

    return false;
}

// "Z" with acute is canonically equivalent to "Ź"
// assert(canonical_equivalence("\x5A\xCC\x81", "\xC5\xB9") == true);

// Note that the algorithm is just an example,
// the algorithm is actually useless in most cases
// because you must never rely on canonical equivalence.
// This is very important for security reasons.
// See Unicode Standard: 2.12 Equivalent Sequences
//
// If you really need it you must understand that
// the only case when canonical equivalence algorithm
// produces exactly the same result is when both strings
// preprocessed with exactly the same normalization algorithm
// (for example stream-safe normalization can produce different result in some rare cases)
// with exactly the same Unicode version (or additional process for stabilized strings is needed)
// and strings doesn't contain ill-formed sequences
// (because different algorithms can produce different number of replacement characters).
// As you see there just too many ifs.

// -------

// Generic variant of remove_grave_and_acute_accents from README.md
// that can work with char (UTF-8) and char16_t (UTF-16)

template<typename T>
std::basic_string<T> remove_grave_and_acute_accents(std::basic_string_view<T> str)
{
    // Note that sizeof cannot be used to choose which UTF iterator to use
    // because if CHAR_BIT >= 16 then sizeof(char) == sizeof(char16_t)
    // so it won't work properly if sizeof is used.

    static_assert(std::is_same_v<T, char> || std::is_same_v<T, char16_t>);

    // Choose UTF-8 or UTF-16 iterator
    std::conditional_t<std::is_same_v<T, char>,
        typename uni::iter::utf8<decltype(str.cbegin())>,
        typename uni::iter::utf16<decltype(str.cbegin())>
    > begin{str.cbegin(), str.cend()};

    uni::iter::norm::nfd nfd_begin{begin, uni::sentinel};

    auto func = [](char32_t c) { return c != 0x0300 && c != 0x0301; };
    uni::iter::func::filter func_begin{func, nfd_begin, uni::sentinel};

    uni::iter::norm::nfc nfc_begin{func_begin, uni::sentinel};

    std::basic_string<T> result;

    // Choose UTF-8 or UTF-16 output iterator
    std::conditional_t<std::is_same_v<T, char>,
        typename uni::iter::output::utf8<decltype(std::back_inserter(result))>,
        typename uni::iter::output::utf16<decltype(std::back_inserter(result))>
    > out{std::back_inserter(result)};

    for (auto it = nfc_begin; it != uni::sentinel; ++it)
        out = *it;

    return result;
}

// assert(remove_grave_and_acute_accents<char>("bótte bòtte") == "botte botte")
// assert(remove_grave_and_acute_accents<char16_t>(u"bótte bòtte") == u"botte botte")

// -------
