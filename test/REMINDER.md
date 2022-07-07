REMINDER

Download these 3 files first from unicode.org:
Latest version:
ftp://ftp.unicode.org/Public/UNIDATA/NormalizationTest.txt
ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/GraphemeBreakTest.txt
ftp://ftp.unicode.org/Public/UNIDATA/auxiliary/WordBreakTest.txt
Other versions:
ftp://ftp.unicode.org/Public/12.0.0/ucd/NormalizationTest.txt
ftp://ftp.unicode.org/Public/12.0.0/ucd/auxiliary/GraphemeBreakTest.txt
ftp://ftp.unicode.org/Public/12.0.0/ucd/auxiliary/WordBreakTest.txt
ftp://ftp.unicode.org/Public/12.1.0/ucd/NormalizationTest.txt
ftp://ftp.unicode.org/Public/12.1.0/ucd/auxiliary/GraphemeBreakTest.txt
ftp://ftp.unicode.org/Public/12.1.0/ucd/auxiliary/WordBreakTest.txt
etc.

GCC/Clang command lines:
g++ -O3 -std=c++17 -Werror -Wall -Wextra -Wpedantic -Wconversion -Wsign-compare -Wsign-conversion -Wshadow -Wmissing-braces -Warray-bounds=2 -Wstrict-overflow=2 -Wstack-usage=3000 -Wnull-dereference -Wcast-align -Wcast-qual -Wduplicated-branches -Wduplicated-cond test/main.cpp src/cpp_uni_data.cpp
clang++ -O3 -std=c++17 -Werror -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wsign-compare -Wshadow -Wmissing-braces -Warray-bounds -Wcast-align -Wcast-qual test/main.cpp src/cpp_uni_data.cpp

DEBUG: -g -O0 -D_GLIBCXX_DEBUG -DUNI_ALGO_TEST_ASSERT

MSVC: /W4 /WX

Must be 0 warnings and every warning must be fixed.

Tested in:
GCC 7.3.0 (x86/x64)
GCC 10.1.0 (x64)
GCC 12.1.0 (x64)
Clang 10.0.0
Clang 9.0.0
Clang 8.0.0
Clang 7.1.0 (x64)
MSVS 2017 (x86/x64)

Clang-Tidy command line:
clang-tidy test/clang-tidy/main.cpp -header-filter=.* -extra-arg=-std=c++17 --

Generate gen/nm_result.txt
nm --print-size --size-sort --radix=d cpp_uni_data.cpp.o >nm_result.txt

ASM:
objdump -d main.cpp.o >asm_result.txt

-------

https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
https://clang.llvm.org/docs/DiagnosticsReference.html

-Warray-bounds=2 -Wstrict-overflow > 2 may produce false positives.

-Winline can be enabled to check that inline is not used in wrong places.

-Wsign-conversion is enabled by -Wconversion in Clang but is not in GCC

-Warray-bounds=2 -Wstack-usage -Wduplicated-branches -Wduplicated-cond don't exist in Clang.

-Wmissing-braces is broken in GCC it produces warning even for simple case:
std::array<int, 3> = {1, 2, 3};
Can be only silenced with double braces, works fine in Clang.
If you ever need to silence it just do it in impl/internal_defines.h

-------

-D_GLIBCXX_DEBUG
https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode.html
https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode_semantics.html
Note that it doesn't provide safe iterators for std::basic_string
and that is what we need the most actually.
__gnu_debug::basic_string provides them but it is useless for us.
As I remember MSVC debug iterators work for basic_string so it can be better.

https://opensource.apple.com/source/gcc/gcc-1762/libstdc++-v3/docs/html/debug.html

-------

Some old and probably useless info below this line.

These are important but already enabled by -Wall -Wextra -Wpedantic:
-Wtype-limits -Wpointer-arith -Wuninitialized -Wmaybe-uninitialized

C99: -std=c99 -Wunused-const-variable=2

-------

Hey Microsoft you know how much time it takes to find and download VS Community 2017 in the year 2022
1 hour because your web site is such "user friendly", 1 hour because I need to register another outlook e-mail
1 hour because I got random number error that told me to wait 1 day. NANI?! Aho desu ne.
It is exactly 2.5 anime episodes that you took from me and I will never forget it.
For a reference to download and test in a random GCC/Clang version I need around 3 minutes.
DIRECT LINKS:
VS Comunity 2017: https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=15
VS Comunity 2019: https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=16
VS Comunity 2022: https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=17
Build Tools 2017: https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=BuildTools&rel=15
Build Tools 2019: https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=BuildTools&rel=16
Build Tools 2022: https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=BuildTools&rel=17
