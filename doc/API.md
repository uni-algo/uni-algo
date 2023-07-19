## API Reference

The uni-algo library API consists of the following parts:
- **[src/data.cpp](#anchor-data)**
- **[uni_algo/all.h](#anchor-all)**
- **[uni_algo/version.h](#anchor-version)** - version
- **[uni_algo/config.h](#anchor-config)** - configuration
- **[uni_algo/conv.h](#anchor-conv)** - conversion functions
- **[uni_algo/case.h](#anchor-case)** - case functions
- **[uni_algo/locale.h](#anchor-locale)** - locale class
- **[uni_algo/norm.h](#anchor-norm)** - normalization functions
- **[uni_algo/ranges.h](#anchor-ranges)** - basic ranges
- **[uni_algo/ranges_conv.h](#anchor-ranges-conv)** - conversion ranges
- **[uni_algo/ranges_norm.h](#anchor-ranges-norm)** - normalization ranges
- **[uni_algo/ranges_grapheme.h](#anchor-ranges-grapheme)** - grapheme ranges
- **[uni_algo/ranges_word.h](#anchor-ranges-word)** - word ranges
- **[uni_algo/script.h](#anchor-script)** - code point script property
- **[uni_algo/prop.h](#anchor-prop)** - code point properties
- **[other classes](#anchor-other)**

Things that are not listed in this file:
- Functions and ranges that work with UTF end with `utf8` and `utf16` only UTF-8 functions and ranges are listed
- Range views come in pairs for example: `una::ranges::utf8_view` / `una::views::utf8` only second one is listed

---

<a id="anchor-data"></a>
#### `src/data.cpp`

The only file that you need to compile.<br>
Contains Unicode data tables.

If you are using a package manager<br>
or header-only version then ignore this file.

---

<a id="anchor-all"></a>
#### `uni_algo/all.h`

Includes all enabled modules.

---

<a id="anchor-version"></a>
#### `uni_algo/version.h` - version (header-only)

Provides information about the library version and Unicode version.<br>
See comments inside the file for more info.

---

<a id="anchor-config"></a>
#### `uni_algo/config.h` - configuration

See comments inside the file for more info.

---

<a id="anchor-conv"></a>
#### `uni_algo/conv.h` - conversion functions (header-only)

```
una::utf8to16 - convert a string from UTF-8 to UTF-16
una::utf16to8
una::utf8to32
una::utf32to8
una::utf16to32
una::utf32to16

una::is_valid_utf8 - check if UTF-8 string is well-formed
una::is_valid_utf16
una::is_valid_utf32
```

The same functions but when end with `u`<br>
then output `std::u8string` / `std::u16string` / `std::u32string`<br>
instead of `std::string` / `std::wstring`

The same functions but from `una::strict` namespace<br>
then strict conversion instead of lenient.

Validation and strict functions support `una::error` that<br>
can be used to find out the position where an error occured.

---

<a id="anchor-case"></a>
#### `uni_algo/case.h` - case functions (requeries src/data.cpp)

```
una::cases::to_lowercase_utf8 - convert a string to lower case
una::cases::to_uppercase_utf8 - upper case
una::cases::to_titlecase_utf8 - title case
una::cases::to_casefold_utf8  - case folding

una::caseless::compare_utf8 - case insensitive comparison
una::caseless::collate_utf8 - collation
una::caseless::find_utf8    - search

una::casesens::compare_utf8 - case sensitive comparison
una::casesens::collate_utf8 - collation
una::casesens::find_utf8    - search
```

Lower, upper, title case support `una::locale`<br>
Case folding is always locale-independent and<br>
others are based on Default Case Matching and always locale-independent too.<br>
Find functions return result in `una::found` class.

---

<a id="anchor-locale"></a>
#### `uni_algo/locale.h` - locale class (requeries src/data.cpp)

```
una::locale::system - get system locale (static function)

una::locale class functions:

is_empty     - check if locale is empty
to_string    - output locale to a string
get_language - get language subtag
get_script   - get script subtag
get_region   - get region subtag

subtag classes:

una::locale::language - language subtag
una::locale::script   - script subtag
una::locale::region   - region subtag
```

You can treat subtag classes the same as enum classes with a bit different syntax:<br>
`una::locale::language{"en"}` instead of `una::locale::language::en`<br>
They are constexpr so it's possible to use them in all places<br>
where enum classes can be used for example in switch case.

---

<a id="anchor-norm"></a>
#### `uni_algo/norm.h` - normalization functions (requeries src/data.cpp)

```
una::norm::to_nfc_utf8 - normalize a string to NFC
una::norm::to_nfd_utf8
una::norm::to_nfkc_utf8
una::norm::to_nfkd_utf8
una::norm::is_nfc_utf8 - detect if a string is NFC normalized
una::norm::is_nfd_utf8
una::norm::is_nfkc_utf8
una::norm::is_nfkd_utf8
una::norm::to_unaccent_utf8 - remove all accents and normalize a string to NFC
```

Normalization detection functions support `una::error`<br>
that can be used to get extended error information.

---

<a id="anchor-ranges"></a>
#### `uni_algo/ranges.h` - basic ranges (header-only)

```
una::views::reverse   - always use this instead of std::views::reverse
una::views::filter    - similar to std::views::filter
una::views::transform - similar to std::views::transform
una::views::drop      - similar to std::views::drop
una::views::take      - similar to std::views::take
```

---

<a id="anchor-ranges-conv"></a>
#### `uni_algo/ranges_conv.h` - conversion ranges (header-only)

```
una::views::utf8 - requires integral UTF-8 range produces char32_t range

una::ranges::to_utf8<type>() - requires char32_t range produces type range
una::ranges::to_utf8_reserve<type>(n) - same as previous but with reserve(n) call
```

---

<a id="anchor-ranges-norm"></a>
#### `uni_algo/ranges_norm.h` - normalization ranges (requeries src/data.cpp)

```
una::views::norm::nfc - requires char32_t range produces char32_t NFC normalized range
una::views::norm::nfd
una::views::norm::nfkc
una::views::norm::nfkd
```

---

<a id="anchor-ranges-grapheme"></a>
#### `uni_algo/ranges_grapheme.h` - grapheme ranges (requeries src/data.cpp)

```
una::views::grapheme::utf8 - requires integral UTF-8 range produces UTF-8 std::string_view subranges of graphemes
```

The algorithm uses [UAX #29: Unicode Text Segmentation -> Grapheme Cluster Boundary Rules](https://unicode.org/reports/tr29/#Grapheme_Cluster_Boundaries)<br>
The grapheme range supports cursor. For details see [doc/CURSOR.md](CURSOR.md)

---

<a id="anchor-ranges-word"></a>
#### `uni_algo/ranges_word.h` - word ranges (requeries src/data.cpp)

```
una::views::word::utf8 - requires integral UTF-8 range produces UTF-8 std::string_view subranges of words
una::views::word_only::utf8

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

The algorithm uses [UAX #29: Unicode Text Segmentation -> Word Boundary Rules](https://unicode.org/reports/tr29/#Word_Boundaries)<br>
`word_only` is a special case of UAX #29 that takes only words and skips all punctuation and such.<br>
The word range supports cursor. For details see [doc/CURSOR.md](CURSOR.md)

---

<a id="anchor-script"></a>
#### `uni_algo/script.h` - code point script property (requeries src/data.cpp)

```
una::codepoint::get_script - get code point script property
una::codepoint::has_script - check if a code point has script extensions property
```
The functions use `una::locale::script` class for script property and `char32_t` for code point.<br>
Example: `assert(una::codepoint::get_script(U'W') == una::locale::script{"Latn"})`<br>
The algorithm uses [UAX #24: Unicode Script Property](https://www.unicode.org/reports/tr24)<br>
List of all scripts: https://en.wikipedia.org/wiki/Script_(Unicode)#List_of_scripts_in_Unicode

---

<a id="anchor-prop"></a>
#### `uni_algo/prop.h` - code point properties (requeries src/data.cpp)

The documentation contains links to:<br>
Unicode character database: https://www.unicode.org/reports/tr44 <br>
and Unicode glossary: https://www.unicode.org/glossary <br>
for better understanding what these functions do.<br>
All these functions work with code point `char32_t` <br>
functions that end with `u32` return result in `std::u32string`

<pre><code>una::codepoint::max_value        - the last possible code point U+10FFFF
una::codepoint::total_number     - total number of code points 0x110000
una::codepoint::replacement_char - REPLACEMENT CHARACTER U+FFFD

una::codepoint::is_valid         - check if valid <a href="https://www.unicode.org/glossary/#code_point">code point</a>
una::codepoint::is_valid_scalar  - check if valid <a href="https://www.unicode.org/glossary/#unicode_scalar_value">scalar value</a>

una::codepoint::is_alphabetic    - <a href="https://www.unicode.org/reports/tr44/#Alphabetic">Alphabetic</a>
una::codepoint::is_numeric       - <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a>=Number
una::codepoint::is_alphanumeric  - <a href="https://www.unicode.org/reports/tr44/#Alphabetic">Alphabetic</a> or <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a>=Number
una::codepoint::is_whitespace    - <a href="https://www.unicode.org/reports/tr44/#White_Space">White_Space</a>

una::codepoint::is_reserved      - <a href="https://www.unicode.org/glossary/#reserved_code_point">reserved code point</a>
una::codepoint::is_noncharacter  - <a href="https://www.unicode.org/glossary/#noncharacter">noncharacter</a>
una::codepoint::is_control       - <a href="https://www.unicode.org/glossary/#control_codes">control codes</a>
una::codepoint::is_surrogate     - <a href="https://www.unicode.org/glossary/#surrogate_code_point">surrogate code point</a>
una::codepoint::is_supplementary - <a href="https://www.unicode.org/glossary/#supplementary_code_point">supplementary code point</a>
una::codepoint::is_private_use   - <a href="https://www.unicode.org/glossary/#private_use_code_point">private use code point</a>

class una::codepoint::prop       - provides <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a> and <a href="https://www.unicode.org/glossary/#code_point_type">code point type</a> functions

enum una::codepoint::general_category - provides <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a> values
una::codepoint::get_general_category  - get <a href="https://www.unicode.org/reports/tr44/#General_Category_Values">General_Category</a> value

<i>other modules can provide more properties they are listed below</i>

<b>uni_algo/case.h</b>

una::codepoint::is_lowercase        - <a href="https://www.unicode.org/reports/tr44/#Lowercase">Lowercase</a>
una::codepoint::is_uppercase        - <a href="https://www.unicode.org/reports/tr44/#Uppercase">Uppercase</a>

una::codepoint::to_simple_lowercase - performs <a href="https://unicode.org/reports/tr44/#Simple_Lowercase_Mapping">Simple_Lowercase_Mapping</a> on a code point
una::codepoint::to_simple_uppercase - <a href="https://www.unicode.org/reports/tr44/#Simple_Uppercase_Mapping">Simple_Uppercase_Mapping</a>
una::codepoint::to_simple_titlecase - <a href="https://www.unicode.org/reports/tr44/#Simple_Titlecase_Mapping">Simple_Titlecase_Mapping</a>
una::codepoint::to_simple_casefold  - <a href="https://www.unicode.org/reports/tr44/#Simple_Case_Folding">Simple_Case_Folding</a>

una::codepoint::to_lowercase_u32    - performs full <a href="https://unicode.org/reports/tr44/#Lowercase_Mapping">Lowercase_Mapping</a> on a code point
una::codepoint::to_uppercase_u32    - <a href="https://www.unicode.org/reports/tr44/#Uppercase_Mapping">Uppercase_Mapping</a>
una::codepoint::to_titlecase_u32    - <a href="https://www.unicode.org/reports/tr44/#Titlecase_Mapping">Titlecase_Mapping</a>
una::codepoint::to_casefold_u32     - <a href="https://www.unicode.org/reports/tr44/#Case_Folding">Case_Folding</a>

class una::codepoint::prop_case     - provides <a href="https://www.unicode.org/reports/tr44/#Cased">Cased</a>, <a href="https://www.unicode.org/reports/tr44/#Case_Ignorable">Case_Ignorable</a> and <a href="https://www.unicode.org/reports/tr44/#Soft_Dotted">Soft_Dotted</a> functions

<b>uni_algo/norm.h</b>

una::codepoint::to_decompose_u32        - full canonical decomposition
una::codepoint::to_decompose_compat_u32 - full compatibility decomposition

una::codepoint::to_compose              - composes two code points

class una::codepoint::prop_norm         - provides <a href="https://www.unicode.org/reports/tr44/#Canonical_Combining_Class_Values">Canonical_Combining_Class</a> and <a href="https://www.unicode.org/reports/tr44/#QC_Values_Table">Quick_Check</a> functions
</code></pre>

---

<a id="anchor-other"></a>
#### `other classes`

```
class una::error    - error status, used by validation functions
class una::found    - search result, used by find functions
class una::sentinel - sentinel, used by ranges
```
