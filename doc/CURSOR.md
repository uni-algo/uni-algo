**DRAFT**

## Text Segmentation Cursor

Text segmentation ranges (grapheme and word ranges) in the library support `cursor` function.<br>
The `cursor` function expects iterator of a range you are viewing and returns iterator to a text segmentation range.<br>
The cursor is used to find a text segmentation boundary
so you don't need to set it to the exact grapheme/word boundary,
you can set the cursor somewhere in the middle of a grapheme/word
or even in the middle of a UTF-8/16 sequence and it will always find the proper boundary.<br>
In the following example we will use the `cursor` function to find a word in the middle of a string
and iterate forward and backward from the cursor position.<br>
Note that spaces are words too according to Unicode text segmentation rules.

```cpp
#include "uni_algo/ranges_word.h"

std::string_view str = "one two three five six";

auto rg = una::views::word::utf8(str);

// Set cursor somewhere in the middle of the string
auto cur = rg.cursor(str.begin() + str.size() / 2);

// Output word at the current cursor position
std::cout << *cur << '\n';

// Output:
// three

// Iterate forward
for (auto it = cur; it != rg.end(); ++it)
    std::cout << *it << '|';
std::cout << '\n';

// Output:
// three| |five| |six|

// Iterate backward
for (auto it = cur; it != rg.begin();)
{
    --it;
    std::cout << *it << '|';
}
std::cout << '\n';

// Output:
//  |two| |one|
```

You can also use the cursor to split a string by a text segmentation boundary.<br>
In the following example we will use `cur.end()` as a boundary to split a string by a word boundary somewhere in the middle.

```cpp
#include "uni_algo/ranges_word.h"

std::string_view str = "one two three five six";

auto rg = una::views::word::utf8(str);

// Set cursor somewhere in the middle of the string
auto cur = rg.cursor(str.begin() + str.size() / 2);

// Output strings on the left and right side of the cursor
std::cout << std::string{str.begin(), cur.end()} << '\n';
std::cout << std::string{cur.end(), str.end()} << '\n';

// Output:
// one two three
//  five six
```
