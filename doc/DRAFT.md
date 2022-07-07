DRAFT

---

cpp_uni_data.cpp
The only file that you need to compile.
Contains Unicode data tables.
Never compile this file into a dynamic library the ABI is not stable for now.

---

cpp_uni_all.h
Includes all enabled modules.

---

cpp_uni_version.h - Version
Provides information about the library version and Unicode version.
Other modules always include this module.

---

cpp_uni_config.h - Configuration
See comments inside the file for more info.

---

cpp_uni_convert.h - Convertion operations
Header only.

uni::utf8to16 - convert a string from UTF-8 to UTF-16
uni::utf16to8
uni::utf8to32
uni::utf32to8
uni::utf16to32
uni::utf32to16

The same functions but when end with u
then output std::u8string/std::u16string/std::u32string

The same functions but from uni::strict namespace
then strict conversion instead of lenient

---

cpp_uni_case.h - Case operations
Requeries cpp_uni_data.cpp

uni::cases::utf8_lower - convert a string to lower case
uni::cases::utf8_upper - upper case
uni::cases::utf8_title - title case
uni::cases::utf8_fold - case folding
uni::caseless::utf8_compare - case insensitive comparison
uni::caseless::utf8_collate - collation
uni::caseless::utf8_search - search
uni::casesens::utf8_compare - case sensitive comparison
uni::casesens::utf8_collate - collation
uni::casesens::utf8_search - search

Lower, upper, title case support uni::locale
Case folding is always locale-independant and
others are based on Default Case Matching and always locale-independant too.

---

cpp_uni_norm.h - Normalization
Requeries cpp_uni_data.cpp

uni::norm::utf8_nfc - normalize a string to NFC normalization form
uni::norm::utf8_nfd
uni::norm::utf8_nfkc
uni::norm::utf8_nfkd
uni::norm::utf8_is_nfc - check if a string in NFC normalization form
uni::norm::utf8_is_nfd
uni::norm::utf8_is_nfkc
uni::norm::utf8_is_nfkd
uni::norm::utf8_unaccent - remove all accents and normalize a string to NFC

class uni::iter::norm::nfc
class uni::iter::norm::nfd
class uni::iter::norm::nfkc
class uni::iter::norm::nfkd

---

cpp_uni_iterator.h
Header only.

class uni::iter::utf8
class uni::iter::utf16

class uni::iter::output::utf8
class uni::iter::output::utf16

class uni::iter::func::filter
class uni::iter::func::transform

---

cpp_uni_break_grapheme.h
Requeries cpp_uni_data.cpp

class uni::breaks::grapheme::utf8
class uni::breaks::grapheme::utf16

---

cpp_uni_break_word.h
Requeries cpp_uni_data.cpp

class uni::breaks::word::utf8
class uni::breaks::word::utf16

---

Other classes

class uni::error - used by validation functions
class uni::locale - used by Case module
class uni::sentinel - used by iterators