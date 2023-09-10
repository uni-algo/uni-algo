// Describes Unicode version number in XYYYZZZ format such that:
// (VERSION % 1000) is the update version 0..255,
// (VERSION / 1000 % 1000) is the minor version 0..255,
// (VERSION / 1000000) is the major version 1..255.
// For reference see The Unicode Standard: 3.1 Versions of the Unicode Standard -> Version Numbering

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage, modernize-macro-to-enum)
#define UNI_ALGO_UNICODE_VERSION 15001000
