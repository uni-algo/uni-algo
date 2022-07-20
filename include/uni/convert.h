/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_CONVERT_H_UAIX
#define CPP_UNI_CONVERT_H_UAIX

#include <type_traits>
#include <string>
#include <string_view>
#include <cassert>

#include <uni/config.h>
#include <uni/version.h>
#include <uni/internal/error.h>

#include <uni/impl/convert.h>

namespace uni {

namespace detail {

// Implementation details
// There were 2 possible ways to implement the conversion: using resize or using back_inserter,
// back_inserter was always 10-30% slower in tests even with reserve so it uses resize approach.
// Also it doesn't use 2-pass approach: calculate converted string size and then the actual convertion.
// This is obviously at least 50% slower, so it uses 1-pass with shrink_to_fit after.

//#define UNI_ALGO_DISABLE_CPP_ITERATORS
// With this define pointers will be used instead of iterators.
// The only reason to use the define is to maximize performance in debug builds,
// for example MSVC debug iterators are very slow.
// Performance difference of pointers vs iterators in release builds
// is about 1-2% slower/faster in different compilers and must be considered irrelevant.
// The behaviour of pointers and iterators versions always absolutely the same.

//#define UNI_ALGO_DISABLE_CPP_SHRINK_TO_FIT
// With this define shrink_to_fit call will be omitted.
// Performance impact of shrink_to_fit call is 2-20% slower depends on the length of the string.
// This define must be used for test purposes only.

template<typename DST, typename SRC, size_t SZ,
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    size_t(*FNUTF)(typename SRC::const_pointer, typename SRC::const_pointer, typename DST::pointer, size_t*)>
#else
    size_t(*FNUTF)(typename SRC::const_iterator, typename SRC::const_iterator, typename DST::iterator, size_t*)>
#endif
DST t_utf(const SRC& source)
{
    DST destination;

    std::size_t length = source.size();

    //if (length && length <= destination.max_size() / SZ)
    if (length)
    {
        destination.resize(length * SZ);
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
        destination.resize(FNUTF(source.data(), source.data() + source.size(), destination.data(), nullptr));
#else
        destination.resize(FNUTF(source.cbegin(), source.cend(), destination.begin(), nullptr));
#endif
#ifndef UNI_ALGO_DISABLE_CPP_SHRINK_TO_FIT
        destination.shrink_to_fit();
#endif
    }

    return destination;
}

template<typename DST, typename SRC, size_t SZ,
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    size_t(*FNUTF)(typename SRC::const_pointer, typename SRC::const_pointer, typename DST::pointer, size_t*)>
#else
    size_t(*FNUTF)(typename SRC::const_iterator, typename SRC::const_iterator, typename DST::iterator, size_t*)>
#endif
DST t_utf(const SRC& source, uni::error& error)
{
    error.reset();

    DST destination;

    std::size_t length = source.size();

    if (length)
    {
        destination.resize(length * SZ);
        std::size_t err = impl_npos;
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
        std::size_t size = FNUTF(source.data(), source.data() + source.size(), destination.data(), &err);
#else
        std::size_t size = FNUTF(source.cbegin(), source.cend(), destination.begin(), &err);
#endif
        if (err == impl_npos)
            destination.resize(size);
        else
        {
            destination.clear();
            error = uni::error{true, err};
        }
#ifndef UNI_ALGO_DISABLE_CPP_SHRINK_TO_FIT
        destination.shrink_to_fit();
#endif
    }

    return destination;
}

} // namespace detail

// Template functions

template<typename UTF8, typename UTF16>
std::basic_string<UTF16> utf8to16(std::basic_string_view<UTF8> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_utf<std::basic_string<UTF16>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8to16, detail::impl_utf8to16>(source);
}
template<typename UTF16, typename UTF8>
std::basic_string<UTF8> utf16to8(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_utf<std::basic_string<UTF8>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16to8, detail::impl_utf16to8>(source);
}
template<typename UTF8, typename UTF32>
std::basic_string<UTF32> utf8to32(std::basic_string_view<UTF8> source)
{
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF32>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8to32, detail::impl_utf8to32>(source);
}
template<typename UTF32, typename UTF8>
std::basic_string<UTF8> utf32to8(std::basic_string_view<UTF32> source)
{
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF8>, std::basic_string_view<UTF32>,
            detail::impl_x_utf32to8, detail::impl_utf32to8>(source);
}
template<typename UTF16, typename UTF32>
std::basic_string<UTF32> utf16to32(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF32>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16to32, detail::impl_utf16to32>(source);
}
template<typename UTF32, typename UTF16>
std::basic_string<UTF16> utf32to16(std::basic_string_view<UTF32> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF16>, std::basic_string_view<UTF32>,
            detail::impl_x_utf32to16, detail::impl_utf32to16>(source);
}

// Short non-template functions for std::string, std::wstring, std::u16string, std::u32string

inline std::string utf16to8(std::u16string_view source)
{
    return utf16to8<char16_t, char>(source);
}
inline std::u16string utf8to16u(std::string_view source)
{
    return utf8to16<char, char16_t>(source);
}
inline std::string utf32to8(std::u32string_view source)
{
    return utf32to8<char32_t, char>(source);
}
inline std::u32string utf8to32u(std::string_view source)
{
    return utf8to32<char, char32_t>(source);
}
inline std::u16string utf32to16u(std::u32string_view source)
{
    return utf32to16<char32_t, char16_t>(source);
}
inline std::u32string utf16to32u(std::u16string_view source)
{
    return utf16to32<char16_t, char32_t>(source);
}

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::string utf16to8(std::wstring_view source)
{
    return utf16to8<wchar_t, char>(source);
}
inline std::wstring utf8to16(std::string_view source)
{
    return utf8to16<char, wchar_t>(source);
}
inline std::wstring utf32to16(std::u32string_view source)
{
    return utf32to16<char32_t, wchar_t>(source);
}
inline std::u32string utf16to32u(std::wstring_view source)
{
    return utf16to32<wchar_t, char32_t>(source);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline std::string utf32to8(std::wstring_view source)
{
    return utf32to8<wchar_t, char>(source);
}
inline std::wstring utf8to32(std::string_view source)
{
    return utf8to32<char, wchar_t>(source);
}
inline std::u16string utf32to16u(std::wstring_view source)
{
    return utf32to16<wchar_t, char16_t>(source);
}
inline std::wstring utf16to32(std::u16string_view source)
{
    return utf16to32<char16_t, wchar_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF

namespace strict {

// Template functions

template<typename UTF8, typename UTF16>
std::basic_string<UTF16> utf8to16(std::basic_string_view<UTF8> source, uni::error& error)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_utf<std::basic_string<UTF16>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8to16, detail::impl_utf8to16>(source, error);
}
template<typename UTF16, typename UTF8>
std::basic_string<UTF8> utf16to8(std::basic_string_view<UTF16> source, uni::error& error)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_utf<std::basic_string<UTF8>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16to8, detail::impl_utf16to8>(source, error);
}
template<typename UTF8, typename UTF32>
std::basic_string<UTF32> utf8to32(std::basic_string_view<UTF8> source, uni::error& error)
{
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF32>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8to32, detail::impl_utf8to32>(source, error);
}
template<typename UTF32, typename UTF8>
std::basic_string<UTF8> utf32to8(std::basic_string_view<UTF32> source, uni::error& error)
{
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF8>, std::basic_string_view<UTF32>,
            detail::impl_x_utf32to8, detail::impl_utf32to8>(source, error);
}
template<typename UTF16, typename UTF32>
std::basic_string<UTF32> utf16to32(std::basic_string_view<UTF16> source, uni::error& error)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF32>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16to32, detail::impl_utf16to32>(source, error);
}
template<typename UTF32, typename UTF16>
std::basic_string<UTF16> utf32to16(std::basic_string_view<UTF32> source, uni::error& error)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));
    static_assert(sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF16>, std::basic_string_view<UTF32>,
            detail::impl_x_utf32to16, detail::impl_utf32to16>(source, error);
}

// Short non-template functions for std::string, std::wstring, std::u16string, std::u32string

inline std::string utf16to8(std::u16string_view source, uni::error& error)
{
    return utf16to8<char16_t, char>(source, error);
}
inline std::u16string utf8to16u(std::string_view source, uni::error& error)
{
    return utf8to16<char, char16_t>(source, error);
}
inline std::string utf32to8(std::u32string_view source, uni::error& error)
{
    return utf32to8<char32_t, char>(source, error);
}
inline std::u32string utf8to32u(std::string_view source, uni::error& error)
{
    return utf8to32<char, char32_t>(source, error);
}
inline std::u16string utf32to16u(std::u32string_view source, uni::error& error)
{
    return utf32to16<char32_t, char16_t>(source, error);
}
inline std::u32string utf16to32u(std::u16string_view source, uni::error& error)
{
    return utf16to32<char16_t, char32_t>(source, error);
}

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::string utf16to8(std::wstring_view source, uni::error& error)
{
    return utf16to8<wchar_t, char>(source, error);
}
inline std::wstring utf8to16(std::string_view source, uni::error& error)
{
    return utf8to16<char, wchar_t>(source, error);
}
inline std::wstring utf32to16(std::u32string_view source, uni::error& error)
{
    return utf32to16<char32_t, wchar_t>(source, error);
}
inline std::u32string utf16to32u(std::wstring_view source, uni::error& error)
{
    return utf16to32<wchar_t, char32_t>(source, error);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline std::string utf32to8(std::wstring_view source, uni::error& error)
{
    return utf32to8<wchar_t, char>(source, error);
}
inline std::wstring utf8to32(std::string_view source, uni::error& error)
{
    return utf8to32<char, wchar_t>(source, error);
}
inline std::u16string utf32to16u(std::wstring_view source, uni::error& error)
{
    return utf32to16<wchar_t, char16_t>(source, error);
}
inline std::wstring utf16to32(std::u16string_view source, uni::error& error)
{
    return utf16to32<char16_t, wchar_t>(source, error);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF

} // namespace strict

#if 0 // defined(__cpp_lib_char8_t) && __cpp_lib_char8_t >= 201907L // TODO: Test me
inline std::u8string utf16to8u(std::u16string_view source)
{
    return utf16to8<char16_t, char8_t>(source);
}
inline std::u16string utf8to16u(std::u8string_view source)
{
    return utf8to16<char8_t, char16_t>(source);
}
inline std::u8string utf32to8u(std::u32string_view source)
{
    return utf32to8<char32_t, char8_t>(source);
}
inline std::u32string utf8to32u(std::u8string_view source)
{
    return utf8to32<char8_t, char32_t>(source);
}

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::u8string utf16to8u(std::wstring_view source)
{
    return utf16to8<wchar_t, char8_t>(source);
}
inline std::wstring utf8to16(std::u8string_view source)
{
    return utf8to16<char8_t, wchar_t>(source);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline std::u8string utf32to8u(std::wstring_view source)
{
    return utf32to8<wchar_t, char8_t>(source);
}
inline std::wstring utf8to32(std::u8string_view source)
{
    return utf8to32<char8_t, wchar_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF

namespace strict {

inline std::u8string utf16to8u(std::u16string_view source, uni::error& error)
{
    return utf16to8<char16_t, char8_t>(source, error);
}
inline std::u16string utf8to16u(std::u8string_view source, uni::error& error)
{
    return utf8to16<char8_t, char16_t>(source, error);
}
inline std::u8string utf32to8u(std::u32string_view source, uni::error& error)
{
    return utf32to8<char32_t, char8_t>(source, error);
}
inline std::u32string utf8to32u(std::u8string_view source, uni::error& error)
{
    return utf8to32<char8_t, char32_t>(source, error);
}

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::u8string utf16to8u(std::wstring_view source, uni::error& error)
{
    return utf16to8<wchar_t, char8_t>(source, error);
}
inline std::wstring utf8to16(std::u8string_view source, uni::error& error)
{
    return utf8to16<char8_t, wchar_t>(source, error);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline std::u8string utf32to8u(std::wstring_view source, uni::error& error)
{
    return utf32to8<wchar_t, char8_t>(source, error);
}
inline std::wstring utf8to32(std::u8string_view source, uni::error& error)
{
    return utf8to32<char8_t, wchar_t>(source, error);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF

} // namespace strict
#endif // __cpp_lib_char8_t

} // namespace uni

#endif // CPP_UNI_CONVERT_H_UAIX

/* Public Domain License
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
 * software, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and to
 * the detriment of our heirs and successors. We intend this dedication to be an
 * overt act of relinquishment in perpetuity of all present and future rights to
 * this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
