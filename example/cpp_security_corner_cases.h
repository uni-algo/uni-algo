/* Example for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See LICENSE.md */

#include "cpp_uni_convert.h"
#include "cpp_uni_case.h"

// --------
// Example: REPLACEMENT CHARACTER (U+FFFD)
// --------

// Most of Unicode algorithms work with code points so every code unit sequence
// will be converted into a code point for an algorithm.
//
// This means even an ill-formed sequence will be converted to a code point.
// The library follows W3C Standard for ill-formed sequences in this case.
//
// This means that for example comparison by code points can produce
// a different result than a binary comparison (comparison by code units).
//
// Let's use "\x80" and "\x81" ill-formed UTF-8 sequences in this example:

assert(uni::casesens::utf8compare("\x80", "\x81") == 0);
// Note that they are equal because we actually compare two U+FFFD code points
// and the result is not the same as a binary comparison
// even though we use case-sensitive comparison.

// All functions in the library are consistent in this case and produce the same result.
// For example it is guaranteed that these 2 variants are equivalent.
assert(uni::casesens::utf8compare("\x80", "\x81") == 0);
assert(uni::utf8to16u("\x80") == uni::utf8to16u("\x81"));

// The same with these 2 etc.
assert(uni::casesens::utf8search("ABC\x80", "\x81").pos() == 3);
assert(uni::utf8to16u("ABC\x80").find(uni::utf8to16u("\x81")) == 3);

// Note that this is not a feature of this particular library this is how Unicode works,
// but the consistency in all cases is the feature of this library.
// You must understand why comparison by code points is different than a binary comparison.
// This is important for security reasons.

// Even aside from this case, a binary comparison of the same
// UTF-8 and UTF-16 text can produce a different result.
// See Unicode Standard: 5.17 Binary Order
//
// But if you compare the same text with UTF-8 and UTF-16 functions
// in this library the result always be the same and consistent.
// The consistency is important part in the design of the library.

// --------
// Example: CHAR_BIT = 16
// --------

// Note that some other use cases can cause the same problem but CHAR_BIT = 16 demonstrates it best.

// Let's imagine we work on a system where CHAR_BIT == 16
// this means char is 16-bit too so this code doesn't even produce a warning:
std::string str;
str.resize(3);
str[0] = 0xFF + 1 + 'A';
str[1] = 0xFF + 1 + 'B';
str[2] = 0xFF + 1 + 'C';

// It is guaranteed that the library will follow the same overflow rules always
// so all functions produce the same result when CHAR_BIT == 8 and CHAR_BIT > 8
assert(uni::casesens::utf8compare(str, "ABC") == 0);
assert(uni::utf8to16u(str) == uni::utf8to16u("ABC"));

// Note that the string that wasn't "ABC" is "ABC" now.
// You must understand why this happens. This is important for security reasons.
// The library doesn't help you to prevent such issues because
// it can't even detect if an overflow happened somewhere if CHAR_BIT = 8.
// The library just guarantees the consistent behaviour of overflows
// and that the behaviour is not undefined with such data.
