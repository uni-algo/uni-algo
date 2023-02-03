/* Example for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include "uni_algo/norm.h"
#include "uni_algo/ranges.h"
#include "uni_algo/ranges_norm.h"
#include "uni_algo/ranges_conv.h"
#include "uni_algo/ranges_word.h"

// -----------------------------
// Do not overuse ranges example
// -----------------------------

// For example you have a long string of words that uses different delimiters
// sometimes spaces sometimes a punctuation and you want to split them and
// rejoin with only one specific delimiter.
// And you want to normalize all words to NFC normalization form too.

// You can implement such function using only ranges like this:
std::string rejoin_words1(std::string_view str)
{
    std::string_view utf8_new_delimiter = ";";

    return str
            | una::views::word_only::utf8
            | std::views::join_with(utf8_new_delimiter)
            | una::views::utf8
            | una::views::norm::nfc
            | una::ranges::to_utf8<std::string>();
}

// But it has some problems:
// 1. It normalizes words after join so if your delimiter is some kind
//    of broken grapheme it may interfere with the normalization.
// 2. It normalizes words even if they are already in NFC normalization form
//    so it will affect the performance. Most of Unicode text is in NFC form.
// 3. Normalization range views are always slower than normalization functions
//    so it would be better to use them instead for this specific case.

// The best way to fix all these problems is to mix ranges and functions
// and reimplement the function like this:
std::string rejoin_words2(std::string_view str)
{
    std::string_view utf8_new_delimiter = ";";

    std::string utf8_result;

    auto view = str | una::views::word_only::utf8;

    for (auto it = view.begin(); it != una::sentinel; ++it)
    {
        if (it != view.begin()) // if not the first word
            utf8_result.append(utf8_new_delimiter);

        if (una::norm::is_nfc_utf8(*it))
            utf8_result.append(*it);
        else // normalize only if not in NFC form
            utf8_result.append(una::norm::to_nfc_utf8(*it));
    }

    return  utf8_result;
}

// assert(rejoin_words2(", word1. word2   word3!?") == "word1;word2;word3");

// SUMMARY
// Do not overuse ranges. In many cases it will be better to mix ranges with functions.
// In some cases it may not be possible for example remove_grave_and_acute_accents from README.md
// so it's fine to use only ranges for such cases.
// In some cases using ranges instead of functions may be better see an example below.

// -----------------------------
// Canonical equivalence example
// -----------------------------

// For example you need to check two strings for canonical equivalence
// it can be done using functions like this: una::norm::utf8_nfc(str1) == una::norm::utf8_nfc(str2)
// but it will allocate memory for both strings then normalize them and only then compare them with a binary
// comparison even if only the first code point is different so it will be slow especially for long strings
// but you can use ranges to implement the algorithm and as a bonus it will never allocate memory too.

bool canonical_equivalence(std::string_view str1, std::string_view str2)
{
    // UTF-8 -> NFC for both strings
    auto view1 = una::ranges::norm::nfc_view{una::ranges::utf8_view{str1}};
    auto view2 = una::ranges::norm::nfc_view{una::ranges::utf8_view{str2}};

    auto it1 = view1.begin();
    auto it2 = view2.begin();

    // Compare the strings by code points
    for (; it1 != una::sentinel && it2 != una::sentinel; ++it1, ++it2)
    {
        if (*it1 != *it2)
            return false;
    }

    // Reached the end in both strings then the strings are equal
    if (it1 == una::sentinel && it2 == una::sentinel)
        return true;

    return false;
}

// "W" with circumflex is canonically equivalent to "Ŵ"
// assert(canonical_equivalence("W\u0302", "Ŵ") == true);

// Unicode specific info below skip it if you are not interested.
//
// Note that the algorithm is just an example,
// the algorithm is actually useless in most cases
// because you must never rely on canonical equivalence.
// This is very important for security reasons.
// See The Unicode Standard: 2.12 Equivalent Sequences
//
// If you really need it you must understand that
// the only case when canonical equivalence algorithm
// produces exactly the same result is when both strings
// preprocessed with exactly the same normalization algorithm
// (for example stream-safe normalization can produce different result in some rare cases)
// with exactly the same Unicode version (or additional process for stabilized strings is needed)
// and strings doesn't contain any ill-formed sequences
// (because different algorithms can produce different number of replacement characters).
// As you see there just too many ifs involved.

// --------------------------
// Generic UTF ranges example
// --------------------------

// Generic variant of remove_grave_and_acute_accents from README.md
// that can work with char/char8_t (UTF-8) and char16_t (UTF-16)

template<typename T>
std::basic_string<T> remove_grave_and_acute_accents(std::basic_string_view<T> str)
{
    // Note that sizeof cannot be used to choose which UTF range to use
    // because if CHAR_BIT >= 16 then sizeof(char) == sizeof(char16_t)
    // so it won't work properly if sizeof is used.

    static_assert(std::is_same_v<T, char> || std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t>);

    using utf_view = std::conditional_t<std::is_same_v<T, char16_t>,
        una::ranges::utf16_view<std::basic_string_view<T>>,
        una::ranges::utf8_view<std::basic_string_view<T>>>;

    auto result_view = utf_view{str}
            | una::views::norm::nfd
            | una::views::filter([](char32_t c) { return c != 0x0300 && c != 0x0301; })
            | una::views::norm::nfc;

    if constexpr (std::is_same_v<T, char16_t>)
        return una::ranges::to_utf16<std::basic_string<T>>(result_view);
    else
        return una::ranges::to_utf8<std::basic_string<T>>(result_view);
}

// assert(remove_grave_and_acute_accents<char>("bótte bòtte") == "botte botte")
// assert(remove_grave_and_acute_accents<char8_t>(u8"bótte bòtte") == u8"botte botte")
// assert(remove_grave_and_acute_accents<char16_t>(u"bótte bòtte") == u"botte botte")
