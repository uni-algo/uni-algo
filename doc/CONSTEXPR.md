## Constexpr Library

These conditions must be fulfilled for the constexpr library:
1. Header-only version of the library (it must be compiled with `UNI_ALGO_STATIC_DATA` config define)
2. Modern compiler that supports C++20 properly: GCC 12, Clang 15, MSVC 19.29 (MSVS 2019 16.10)
3. Compiler flags: `-std=c++20` (or higher) in GCC/Clang or `/std:c++20` (or higher) in MSVC

Define `UNI_ALGO_CONSTEXPR` can be used to check that the library is constexpr.

For example to enable constexpr library with CMake use `UNI_ALGO_HEADER_ONLY` CMake option and compile your project with
`target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)` with a modern compiler, similar for other build systems.

When constexpr library is enabled then almost every function in the library will be constexpr so it can be used in `static_assert` for example:

```cpp
#include "uni_algo/case.h"
#include "uni_algo/norm.h"
#include "uni_algo/ranges_conv.h"

#ifndef UNI_ALGO_CONSTEXPR
#error "The library is not constexpr"
#endif

static_assert(una::cases::to_uppercase_utf8("Test", una::locale{"en-US"}) == "TEST");
static_assert(una::norm::to_nfc_utf8("W\u0302") == "Ŵ");
static_assert((std::string_view{"Test"} | una::views::utf8 | una::ranges::to_utf16<std::u16string>()) == u"Test");
```
