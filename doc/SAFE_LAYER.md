## Design

The safe layer is similar to [Debug Mode in GCC](https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode.html)
or [Debug Iterator Support in MSVC](https://learn.microsoft.com/en-us/cpp/standard-library/debug-iterator-support?view=msvc-170)
but without flaws of these tools ([read below](#other-tools)) and works with every compiler.<br>
The main job of the safe layer is to prevent a security vulnerability even if a critical bug exists in the library.

The safe layer is designed especially for this library to make it as lightweight as possible to achieve better performance
and make it possible to use it in release.

The safe layer is enabled in debug by default and it is possible
to enable it in release with define `UNI_ALGO_ENABLE_SAFE_LAYER` if you want to use the library in a high secure environment.<br>
The performance impact of the safe layer is around 0-20% in release.

## Implementation

The safe layer consists of 2 parts: safe arrays and safe iterators.

Safe arrays are implemented by simple bounds check in operator[] and because the library uses pregenerated Unicode data arrays
that means the size of the arrays is known at compile time that makes it simple for a compiler to optimize the checks out
so the performance impact of the safe arrays is 0% in most cases.<br>
Safe iterators are different the checks are done in operator* and it is not possible for a compiler
to optimize the checks out in many cases so the performance impact is around 0-20%.<br>
If the safe layer catches a problem it calls std::abort instead of try to read of write to a memory that is not supposed to.

Note that ranges only partially use the safe layer for now. They use only safe arrays part.
Because it is very ineffective to use safe iterators in every range view so it will be handled differently in the future.

## Security

Types of security vulnerabilities and how we handle them:

- *Memory safety errors*
  - *Buffer overflow* - constexpr + safe layer
  - *Dangling reference* - constexpr
- *Memory management errors* - constexpr + safe layer + very strict memory management (no new/delete etc.)
- *Logic errors* - tests

constexpr means the library is compiled in constexpr mode (where UB is not allowed) and try to pass the same tests that it does in runtime.
Almost all basic memory problems can be caught with this. Of course tests cannot provide all possible inputs
but the safe layer can catch almost all memory problems in runtime.
There are very few places where the safe layer is not involved and those places are very simple.<br>
Logic errors are the worst but we try to test all corner cases with tests.

## Other tools

For a reference other tools that was used in development and theirs flaws.<br>
Note that this text are not made to criticize the tools but to describe why they are not that good for our use cases.

**[Debug Mode in GCC](https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode.html)**<br>
No debug iterators for std::string and std::string_view ([details](https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode_semantics.html))
and we need them the most because this is Unicode library that works with strings most of the time.

**[Debug Iterator Support in MSVC](https://learn.microsoft.com/en-us/cpp/standard-library/debug-iterator-support?view=msvc-170)**<br>
Super slow in debug it needs like 10 minutes to pass tests that require 10 seconds aside from that it's good and better than Debug Mode in GCC.

**[C++ Buffer Hardening in Clang](https://discourse.llvm.org/t/rfc-c-buffer-hardening/65734)**
and
**[RFC Hardening in libc++](https://discourse.llvm.org/t/rfc-hardening-in-libc/73925)**<br>
Just for a reference. It doesn't exist at the time of writing this text.

The safe layer made these tools irrelevant because it is designed especially for the library
to handle cases the library needs the most instead of general cases these tools handle
and the most important the safe layer works in every compiler the same way.

**Sanitizers (ASan, UBSan) and such**<br>
They simply don't provide 100% result like if there is a bug exists a tool can catch it 100%. These tools cannot do this.
These tools are very good to find bugs in an old code base but the library was designed
with different things in mind from the very beginning.

Note that we still use all of these tools but do not rely on them anymore.

## Extra useful links
**[Compiler Options Hardening Guide for C and C++](https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html)**<br>
**[P2723R1 Zero-initialize objects of automatic storage duration](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2723r1.html)**<br>

