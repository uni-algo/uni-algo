**DRAFT**

Things that are not listed in this file:
- Functions and ranges that work with UTF starts with `utf8` and `utf16` only UTF-8 functions and ranges are listed
- Ranges come in pairs for example: `uni::ranges::word::utf8_view` / `uni::views::word::utf8` only views are listed

---

**`src/data.cpp`**

The only file that you need to compile.<br>
Contains Unicode data tables.<br>

---

**`uni_algo/all.h`**

Includes all enabled modules.

---

**`uni_algo/version.h`** - version (header-only)

Provides information about the library version and Unicode version.<br>
Other files always include this file.

---

**`uni_algo/config.h`** - configuration

See comments inside the file for more info.

---

**`uni_algo/convert.h`** - convertion functions (header-only)
```
uni::utf8to16 - convert a string from UTF-8 to UTF-16
uni::utf16to8
uni::utf8to32
uni::utf32to8
uni::utf16to32
uni::utf32to16
```
The same functions but when end with `u`<br>
then output `std::u8string` / `std::u16string` / `std::u32string`<br>
instead of `std::string` / `std::wstring`

The same functions but from `uni::strict` namespace<br>
then strict conversion instead of lenient.

---

**`uni_algo/case.h`** - case functions (requeries src/data.cpp)
```
uni::cases::utf8_lower - convert a string to lower case
uni::cases::utf8_upper - upper case
uni::cases::utf8_title - title case
uni::cases::utf8_fold  - case folding

uni::caseless::utf8_compare - case insensitive comparison
uni::caseless::utf8_collate - collation
uni::caseless::utf8_search  - search

uni::casesens::utf8_compare - case sensitive comparison
uni::casesens::utf8_collate - collation
uni::casesens::utf8_search  - search
```
Lower, upper, title case support `uni::locale`<br>
Case folding is always locale-independent and<br>
others are based on Default Case Matching and always locale-independent too.

---

**`uni_algo/locale.h`** - locale (requeries src/data.cpp)
```
uni::locale::system - get system locale (static function)

uni::locale class functions:

is_empty     - check if locale is empty
to_string    - output locale to a string
get_language - get language subtag
get_script   - get script subtag
get_region   - get region subtag

subtag classes:

uni::locale::language - language subtag
uni::locale::script   - script subtag
uni::locale::region   - region subtag
```
You can treat subtag classes the same as enum classes with a bit different syntax:<br>
`uni::locale::language{"en"}` instead of `uni::locale::language::en`<br>
They are constexpr so it's possible to use them in all places<br>
where enum classes can be used for example in switch case.

---

**`uni_algo/ranges.h`** - UTF and basic ranges (header-only)
```
uni::views::utf8 - requires integral UTF-8 range produces char32_t range

uni::ranges::to_utf8<type>() - requires char32_t range produces type range

uni::views::reverse   - always use this instead of std::views::reverse
uni::views::filter    - similar to std::views::filter
uni::views::transform - similar to std::views::transform
uni::views::drop      - similar to std::views::drop
uni::views::take      - similar to std::views::take
```
---

**`uni_algo/norm.h`** - normalization functions and ranges (requeries src/data.cpp)
```
uni::norm::utf8_nfc - normalize a string to NFC normalization form
uni::norm::utf8_nfd
uni::norm::utf8_nfkc
uni::norm::utf8_nfkd
uni::norm::utf8_is_nfc - check if a string in NFC normalization form
uni::norm::utf8_is_nfd
uni::norm::utf8_is_nfkc
uni::norm::utf8_is_nfkd
uni::norm::utf8_unaccent - remove all accents and normalize a string to NFC

uni::views::norm::nfc - requires char32_t range produces char32_t range
uni::views::norm::nfd
uni::views::norm::nfkc
uni::views::norm::nfkd
```
---

**`uni_algo/break_grapheme.h`** - grapheme ranges (requeries src/data.cpp)
```
uni::views::grapheme::utf8 - requires integral UTF-8 range produces UTF-8 std::string_view subranges
```
Uses UAX #29: Unicode Text Segmentation -> Grapheme Cluster Boundary Rules

---

**`uni_algo/break_word.h`** - word ranges (requeries src/data.cpp)
```
uni::views::word::utf8 - requires integral UTF-8 range produces UTF-8 std::string_view subranges
uni::views::word_only::utf8

word properties (iterator of word view provides these functions):

is_word             - excludes punctuation, new lines, spaces, and emojis (word_only view uses this property)
is_word_letter      - includes only words that contain letters (excludes kana, ideographic and numbers)
is_word_number      - includes only numbers
is_word_kana        - includes only kana (hiragana and katakana)
is_word_ideographic - includes only ideographic (hanzi, kanji, hanja etc.)
is_emoji            - includes only emojis
is_punctuation      - includes only punctuation
is_segspace         - includes only spaces
is_newline          - includes only new lines
```
Uses UAX #29: Unicode Text Segmentation -> Word Boundary Rules<br>
`word_only` is a special case of UAX #29 that takes only words and skips all punctuation.

---

**`uni_algo/prop.h`** - code point properties (requeries src/data.cpp)

The documentation contains links to:<br>
Unicode character database: https://www.unicode.org/reports/tr44 <br>
and Unicode glossary: https://www.unicode.org/glossary <br>
for better understanding what these functions do.<br>
All these functions work with code point `char32_t` <br>
functions that end with `u32` return result in `std::u32string`

<pre><code>uni::codepoint::max_value        - the last possible code point U+10FFFF
uni::codepoint::total_number     - total number of code points 0x110000
uni::codepoint::replacement_char - REPLACEMENT CHARACTER U+FFFD

uni::codepoint::is_valid         - check if valid <a href="https://www.unicode.org/glossary/#code_point">code point</a>
uni::codepoint::is_valid_scalar  - check if valid <a href="https://www.unicode.org/glossary/#unicode_scalar_value">scalar value</a>

uni::codepoint::is_alphabetic    - <a href="https://www.unicode.org/reports/tr44/#Alphabetic">Alphabetic</a>
uni::codepoint::is_numeric       - <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a>=Number
uni::codepoint::is_alphanumeric  - <a href="https://www.unicode.org/reports/tr44/#Alphabetic">Alphabetic</a> or <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a>=Number
uni::codepoint::is_whitespace    - <a href="https://www.unicode.org/reports/tr44/#White_Space">White_Space</a>

uni::codepoint::is_reserved      - <a href="https://www.unicode.org/glossary/#reserved_code_point">reserved code point</a>
uni::codepoint::is_noncharacter  - <a href="https://www.unicode.org/glossary/#noncharacter">noncharacter</a>
uni::codepoint::is_control       - <a href="https://www.unicode.org/glossary/#control_codes">control codes</a>
uni::codepoint::is_surrogate     - <a href="https://www.unicode.org/glossary/#surrogate_code_point">surrogate code point</a>
uni::codepoint::is_supplementary - <a href="https://www.unicode.org/glossary/#supplementary_code_point">supplementary code point</a>
uni::codepoint::is_private_use   - <a href="https://www.unicode.org/glossary/#private_use_code_point">private use code point</a>

class uni::codepoint::prop       - provides <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a> and <a href="https://www.unicode.org/glossary/#code_point_type">code point type</a> functions

enum uni::codepoint::general_category - provides <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a> values
uni::codepoint::get_general_category  - get <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a> value

<i>other modules can provide more properties they are listed below</i>

<b>uni_algo/case.h</b>

uni::codepoint::is_lowercase        - <a href="https://www.unicode.org/reports/tr44/#Lowercase">Lowercase</a>
uni::codepoint::is_uppercase        - <a href="https://www.unicode.org/reports/tr44/#Uppercase">Uppercase</a>

uni::codepoint::to_simple_lowercase - performs <a href="https://unicode.org/reports/tr44/#Simple_Lowercase_Mapping">Simple_Lowercase_Mapping</a> on a code point
uni::codepoint::to_simple_uppercase - <a href="https://www.unicode.org/reports/tr44/#Simple_Uppercase_Mapping">Simple_Uppercase_Mapping</a>
uni::codepoint::to_simple_titlecase - <a href="https://www.unicode.org/reports/tr44/#Simple_Titlecase_Mapping">Simple_Titlecase_Mapping</a>
uni::codepoint::to_simple_casefold  - <a href="https://www.unicode.org/reports/tr44/#Simple_Case_Folding">Simple_Case_Folding</a>

uni::codepoint::to_lowercase_u32    - performs full <a href="https://unicode.org/reports/tr44/#Lowercase_Mapping">Lowercase_Mapping</a> on a code point
uni::codepoint::to_uppercase_u32    - <a href="https://www.unicode.org/reports/tr44/#Uppercase_Mapping">Uppercase_Mapping</a>
uni::codepoint::to_titlecase_u32    - <a href="https://www.unicode.org/reports/tr44/#Titlecase_Mapping">Titlecase_Mapping</a>
uni::codepoint::to_casefold_u32     - <a href="https://www.unicode.org/reports/tr44/#Case_Folding">Case_Folding</a>

class uni::codepoint::prop_case     - provides <a href="https://www.unicode.org/reports/tr44/#Cased">Cased</a>, <a href="https://www.unicode.org/reports/tr44/#Case_Ignorable">Case_Ignorable</a> and <a href="https://www.unicode.org/reports/tr44/#Soft_Dotted">Soft_Dotted</a> functions

<b>uni_algo/norm.h</b>

uni::codepoint::to_decompose_u32        - full canonical decomposition (includes hangul decomposition)
uni::codepoint::to_decompose_compat_u32 - full compatibility decomposition (includes hangul decomposition)
uni::codepoint::to_decompose_hangul_u32 - hangul decomposition

uni::codepoint::to_compose              - composes two code points (includes hangul composition)

class uni::codepoint::prop_norm         - provides <a href="https://www.unicode.org/reports/tr44/#Canonical_Combining_Class">Canonical_Compining_Class</a> and <a href="https://www.unicode.org/reports/tr44/#QC_Values_Table">Quick_Check</a> functions
</code></pre>

---

**`other classes`**
```
class uni::error    - used by validation functions
class uni::search   - used by search functions
class uni::sentinel - used by ranges
```
