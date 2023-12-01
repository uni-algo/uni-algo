/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_CONV_H_UAIH
#define UNI_ALGO_CONV_H_UAIH

#include <string>
#include <string_view>
#include <cassert>

#include "config.h"
#include "internal/safe_layer.h"
#include "internal/error.h"

#include "impl/impl_conv.h"

namespace una {

namespace detail {

// Implementation details
// There were 2 possible ways to implement the conversion: using resize or using back_inserter,
// back_inserter was always 10-30% slower in tests even with reserve so it uses resize approach.
// Also it doesn't use 2-pass approach: calculate converted string size and then the actual conversion.
// This is obviously at least 50% slower, so it uses 1-pass with shrink_to_fit after.

template<typename Dst, typename Alloc, typename Src, size_t SizeX,
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    size_t(*FnUTF)(typename Src::const_iterator, typename Src::const_iterator, typename Dst::iterator, size_t*)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    size_t(*FnUTF)(typename Src::const_pointer, typename Src::const_pointer, typename Dst::pointer, size_t*)>
#else // Safe layer
    size_t(*FnUTF)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, safe::out<typename Dst::pointer>, size_t*)>
#endif
uaiw_constexpr Dst t_utf(const Alloc& alloc, const Src& src)
{
    Dst dst{alloc};

    const std::size_t length = src.size();

    if (length)
    {
        if (length > dst.max_size() / SizeX) // Overflow protection
        {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || (_HAS_EXCEPTIONS != 0)
            throw std::bad_alloc();
#else
            std::abort();
#endif
        }

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
        dst.resize(length * SizeX);
        dst.resize(FnUTF(src.cbegin(), src.cend(), dst.begin(), nullptr));
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
        dst.resize(length * SizeX);
        dst.resize(FnUTF(src.data(), src.data() + src.size(), dst.data(), nullptr));
#else // Safe layer
#  if !defined(__cpp_lib_string_resize_and_overwrite)
        dst.resize(length * SizeX);
        dst.resize(FnUTF(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, safe::out{dst.data(), dst.size()}, nullptr));
#  else
        dst.resize_and_overwrite(length * SizeX, [&src](typename Dst::pointer p, std::size_t n) noexcept -> std::size_t {
            return FnUTF(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, safe::out{p, n}, nullptr);
        });
#  endif
#endif

#ifndef UNI_ALGO_NO_SHRINK_TO_FIT
        dst.shrink_to_fit();
#endif
    }

    return dst;
}

template<typename Dst, typename Alloc, typename Src, size_t SizeX,
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    size_t(*FnUTF)(typename Src::const_iterator, typename Src::const_iterator, typename Dst::iterator, size_t*)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    size_t(*FnUTF)(typename Src::const_pointer, typename Src::const_pointer, typename Dst::pointer, size_t*)>
#else // Safe layer
    size_t(*FnUTF)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, safe::out<typename Dst::pointer>, size_t*)>
#endif
uaiw_constexpr Dst t_utf(const Alloc& alloc, const Src& src, una::error& error)
{
    error.reset();

    Dst dst{alloc};

    const std::size_t length = src.size();

    if (length)
    {
        if (length > dst.max_size() / SizeX) // Overflow protection
        {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || (_HAS_EXCEPTIONS != 0)
            throw std::bad_alloc();
#else
            std::abort();
#endif
        }

        std::size_t err = impl_npos;

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
        dst.resize(length * SizeX);
        dst.resize(FnUTF(src.cbegin(), src.cend(), dst.begin(), &err));
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
        dst.resize(length * SizeX);
        dst.resize(FnUTF(src.data(), src.data() + src.size(), dst.data(), &err));
#else // Safe layer
#  if !defined(__cpp_lib_string_resize_and_overwrite)
        dst.resize(length * SizeX);
        dst.resize(FnUTF(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, safe::out{dst.data(), dst.size()}, &err));
#  else
        dst.resize_and_overwrite(length * SizeX, [&src, &err](typename Dst::pointer p, std::size_t n) noexcept -> std::size_t {
            return FnUTF(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, safe::out{p, n}, &err);
        });
#  endif
#endif

        if (err != impl_npos)
        {
            error = una::error{una::error::code::ill_formed_utf, err};

            // Return valid part of a string if the define is defined
#ifndef UNI_ALGO_STRICT_CONV_RET_VALID_PART
            dst.clear();
#endif
        }

#ifndef UNI_ALGO_NO_SHRINK_TO_FIT
        dst.shrink_to_fit();
#endif
    }

    return dst;
}

// Validation

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
template<typename Src, bool(*FnValid)(typename Src::const_iterator, typename Src::const_iterator, size_t*)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
template<typename Src, bool(*FnValid)(typename Src::const_pointer, typename Src::const_pointer, size_t*)>
#else // Safe layer
template<typename Src, bool(*FnValid)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, size_t*)>
#endif
uaiw_constexpr bool t_valid(const Src& src)
{
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return FnValid(src.cbegin(), src.cend(), nullptr);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return FnValid(src.data(), src.data() + src.size(), nullptr);
#else // Safe layer
    return FnValid(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, nullptr);
#endif
}

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
template<typename Src, bool(*FnValid)(typename Src::const_iterator, typename Src::const_iterator, size_t*)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
template<typename Src, bool(*FnValid)(typename Src::const_pointer, typename Src::const_pointer, size_t*)>
#else // Safe layer
template<typename Src, bool(*FnValid)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, size_t*)>
#endif
uaiw_constexpr bool t_valid(const Src& src, una::error& error)
{
    size_t err = detail::impl_npos;

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    const bool ret = FnValid(src.cbegin(), src.cend(), &err);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    const bool ret = FnValid(src.data(), src.data() + src.size(), &err);
#else // Safe layer
    const bool ret = FnValid(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, &err);
#endif

    error = ret ? una::error{} : una::error{una::error::code::ill_formed_utf, err};

    return ret;
}

} // namespace detail

// Template functions

template<typename UTF8, typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
utf8to16(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_utf<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_utf8to16, detail::impl_utf8to16>(alloc, source);
}
template<typename UTF16, typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
utf16to8(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_utf<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_utf16to8, detail::impl_utf16to8>(alloc, source);
}
template<typename UTF8, typename UTF32, typename Alloc = std::allocator<UTF32>>
uaiw_constexpr std::basic_string<UTF32, std::char_traits<UTF32>, Alloc>
utf8to32(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF32, std::char_traits<UTF32>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_utf8to32, detail::impl_utf8to32>(alloc, source);
}
template<typename UTF32, typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
utf32to8(std::basic_string_view<UTF32> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF32>,
            detail::impl_x_utf32to8, detail::impl_utf32to8>(alloc, source);
}
template<typename UTF16, typename UTF32, typename Alloc = std::allocator<UTF32>>
uaiw_constexpr std::basic_string<UTF32, std::char_traits<UTF32>, Alloc>
utf16to32(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF32, std::char_traits<UTF32>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_utf16to32, detail::impl_utf16to32>(alloc, source);
}
template<typename UTF32, typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
utf32to16(std::basic_string_view<UTF32> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF32>,
            detail::impl_x_utf32to16, detail::impl_utf32to16>(alloc, source);
}

// Short non-template functions for std::string, std::wstring, std::u16string, std::u32string

inline uaiw_constexpr std::u16string utf8to16u(std::string_view source)
{
    return utf8to16<char, char16_t>(source);
}
inline uaiw_constexpr std::string utf16to8(std::u16string_view source)
{
    return utf16to8<char16_t, char>(source);
}
inline uaiw_constexpr std::u32string utf8to32u(std::string_view source)
{
    return utf8to32<char, char32_t>(source);
}
inline uaiw_constexpr std::string utf32to8(std::u32string_view source)
{
    return utf32to8<char32_t, char>(source);
}
inline uaiw_constexpr std::u32string utf16to32u(std::u16string_view source)
{
    return utf16to32<char16_t, char32_t>(source);
}
inline uaiw_constexpr std::u16string utf32to16u(std::u32string_view source)
{
    return utf32to16<char32_t, char16_t>(source);
}

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline uaiw_constexpr std::wstring utf8to16(std::string_view source)
{
    return utf8to16<char, wchar_t>(source);
}
inline uaiw_constexpr std::string utf16to8(std::wstring_view source)
{
    return utf16to8<wchar_t, char>(source);
}
inline uaiw_constexpr std::u32string utf16to32u(std::wstring_view source)
{
    return utf16to32<wchar_t, char32_t>(source);
}
inline uaiw_constexpr std::wstring utf32to16(std::u32string_view source)
{
    return utf32to16<char32_t, wchar_t>(source);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline uaiw_constexpr std::wstring utf8to32(std::string_view source)
{
    return utf8to32<char, wchar_t>(source);
}
inline uaiw_constexpr std::string utf32to8(std::wstring_view source)
{
    return utf32to8<wchar_t, char>(source);
}
inline uaiw_constexpr std::wstring utf16to32(std::u16string_view source)
{
    return utf16to32<char16_t, wchar_t>(source);
}
inline uaiw_constexpr std::u16string utf32to16u(std::wstring_view source)
{
    return utf32to16<wchar_t, char16_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF

namespace strict {

// Template functions

template<typename UTF8, typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
utf8to16(std::basic_string_view<UTF8> source, una::error& error, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_utf<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_utf8to16, detail::impl_utf8to16>(alloc, source, error);
}
template<typename UTF16, typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
utf16to8(std::basic_string_view<UTF16> source, una::error& error, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_utf<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_utf16to8, detail::impl_utf16to8>(alloc, source, error);
}
template<typename UTF8, typename UTF32, typename Alloc = std::allocator<UTF32>>
uaiw_constexpr std::basic_string<UTF32, std::char_traits<UTF32>, Alloc>
utf8to32(std::basic_string_view<UTF8> source, una::error& error, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF32, std::char_traits<UTF32>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_utf8to32, detail::impl_utf8to32>(alloc, source, error);
}
template<typename UTF32, typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
utf32to8(std::basic_string_view<UTF32> source, una::error& error, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF32>,
            detail::impl_x_utf32to8, detail::impl_utf32to8>(alloc, source, error);
}
template<typename UTF16, typename UTF32, typename Alloc = std::allocator<UTF32>>
uaiw_constexpr std::basic_string<UTF32, std::char_traits<UTF32>, Alloc>
utf16to32(std::basic_string_view<UTF16> source, una::error& error, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF32, std::char_traits<UTF32>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_utf16to32, detail::impl_utf16to32>(alloc, source, error);
}
template<typename UTF32, typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
utf32to16(std::basic_string_view<UTF32> source, una::error& error, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_utf<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF32>,
            detail::impl_x_utf32to16, detail::impl_utf32to16>(alloc, source, error);
}

// Short non-template functions for std::string, std::wstring, std::u16string, std::u32string

inline uaiw_constexpr std::u16string utf8to16u(std::string_view source, una::error& error)
{
    return utf8to16<char, char16_t>(source, error);
}
inline uaiw_constexpr std::string utf16to8(std::u16string_view source, una::error& error)
{
    return utf16to8<char16_t, char>(source, error);
}
inline uaiw_constexpr std::u32string utf8to32u(std::string_view source, una::error& error)
{
    return utf8to32<char, char32_t>(source, error);
}
inline uaiw_constexpr std::string utf32to8(std::u32string_view source, una::error& error)
{
    return utf32to8<char32_t, char>(source, error);
}
inline uaiw_constexpr std::u32string utf16to32u(std::u16string_view source, una::error& error)
{
    return utf16to32<char16_t, char32_t>(source, error);
}
inline uaiw_constexpr std::u16string utf32to16u(std::u32string_view source, una::error& error)
{
    return utf32to16<char32_t, char16_t>(source, error);
}

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline uaiw_constexpr std::wstring utf8to16(std::string_view source, una::error& error)
{
    return utf8to16<char, wchar_t>(source, error);
}
inline uaiw_constexpr std::string utf16to8(std::wstring_view source, una::error& error)
{
    return utf16to8<wchar_t, char>(source, error);
}
inline uaiw_constexpr std::u32string utf16to32u(std::wstring_view source, una::error& error)
{
    return utf16to32<wchar_t, char32_t>(source, error);
}
inline uaiw_constexpr std::wstring utf32to16(std::u32string_view source, una::error& error)
{
    return utf32to16<char32_t, wchar_t>(source, error);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline uaiw_constexpr std::wstring utf8to32(std::string_view source, una::error& error)
{
    return utf8to32<char, wchar_t>(source, error);
}
inline uaiw_constexpr std::string utf32to8(std::wstring_view source, una::error& error)
{
    return utf32to8<wchar_t, char>(source, error);
}
inline uaiw_constexpr std::wstring utf16to32(std::u16string_view source, una::error& error)
{
    return utf16to32<char16_t, wchar_t>(source, error);
}
inline uaiw_constexpr std::u16string utf32to16u(std::wstring_view source, una::error& error)
{
    return utf32to16<wchar_t, char16_t>(source, error);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF

} // namespace strict

template<typename UTF8>
uaiw_constexpr bool is_valid_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_valid<std::basic_string_view<UTF8>, detail::impl_is_valid_utf8>(source);
}

template<typename UTF16>
uaiw_constexpr bool is_valid_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_valid<std::basic_string_view<UTF16>, detail::impl_is_valid_utf16>(source);
}

template<typename UTF32>
uaiw_constexpr bool is_valid_utf32(std::basic_string_view<UTF32> source)
{
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_valid<std::basic_string_view<UTF32>, detail::impl_is_valid_utf32>(source);
}

template<typename UTF8>
uaiw_constexpr bool is_valid_utf8(std::basic_string_view<UTF8> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_valid<std::basic_string_view<UTF8>, detail::impl_is_valid_utf8>(source, error);
}

template<typename UTF16>
uaiw_constexpr bool is_valid_utf16(std::basic_string_view<UTF16> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_valid<std::basic_string_view<UTF16>, detail::impl_is_valid_utf16>(source, error);
}

template<typename UTF32>
uaiw_constexpr bool is_valid_utf32(std::basic_string_view<UTF32> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF32> && sizeof(UTF32) >= sizeof(char32_t));

    return detail::t_valid<std::basic_string_view<UTF32>, detail::impl_is_valid_utf32>(source, error);
}

inline uaiw_constexpr bool is_valid_utf8(std::string_view source)
{
    return is_valid_utf8<char>(source);
}
inline uaiw_constexpr bool is_valid_utf16(std::u16string_view source)
{
    return is_valid_utf16<char16_t>(source);
}
inline uaiw_constexpr bool is_valid_utf32(std::u32string_view source)
{
    return is_valid_utf32<char32_t>(source);
}
inline uaiw_constexpr bool is_valid_utf8(std::string_view source, una::error& error)
{
    return is_valid_utf8<char>(source, error);
}
inline uaiw_constexpr bool is_valid_utf16(std::u16string_view source, una::error& error)
{
    return is_valid_utf16<char16_t>(source, error);
}
inline uaiw_constexpr bool is_valid_utf32(std::u32string_view source, una::error& error)
{
    return is_valid_utf32<char32_t>(source, error);
}
#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline uaiw_constexpr bool is_valid_utf16(std::wstring_view source)
{
    return is_valid_utf16<wchar_t>(source);
}
inline uaiw_constexpr bool is_valid_utf16(std::wstring_view source, una::error& error)
{
    return is_valid_utf16<wchar_t>(source, error);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline uaiw_constexpr bool is_valid_utf32(std::wstring_view source)
{
    return is_valid_utf32<wchar_t>(source);
}
inline uaiw_constexpr bool is_valid_utf32(std::wstring_view source, una::error& error)
{
    return is_valid_utf32<wchar_t>(source, error);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF


#ifdef __cpp_lib_char8_t

inline uaiw_constexpr std::u16string utf8to16u(std::u8string_view source)
{
    return utf8to16<char8_t, char16_t>(source);
}
inline uaiw_constexpr std::u8string utf16to8u(std::u16string_view source)
{
    return utf16to8<char16_t, char8_t>(source);
}
inline uaiw_constexpr std::u32string utf8to32u(std::u8string_view source)
{
    return utf8to32<char8_t, char32_t>(source);
}
inline uaiw_constexpr std::u8string utf32to8u(std::u32string_view source)
{
    return utf32to8<char32_t, char8_t>(source);
}

inline uaiw_constexpr bool is_valid_utf8(std::u8string_view source)
{
    return is_valid_utf8<char8_t>(source);
}
inline uaiw_constexpr bool is_valid_utf8(std::u8string_view source, una::error& error)
{
    return is_valid_utf8<char8_t>(source, error);
}

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline uaiw_constexpr std::wstring utf8to16(std::u8string_view source)
{
    return utf8to16<char8_t, wchar_t>(source);
}
inline uaiw_constexpr std::u8string utf16to8u(std::wstring_view source)
{
    return utf16to8<wchar_t, char8_t>(source);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline uaiw_constexpr std::wstring utf8to32(std::u8string_view source)
{
    return utf8to32<char8_t, wchar_t>(source);
}
inline uaiw_constexpr std::u8string utf32to8u(std::wstring_view source)
{
    return utf32to8<wchar_t, char8_t>(source);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF

namespace strict {

inline uaiw_constexpr std::u16string utf8to16u(std::u8string_view source, una::error& error)
{
    return utf8to16<char8_t, char16_t>(source, error);
}
inline uaiw_constexpr std::u8string utf16to8u(std::u16string_view source, una::error& error)
{
    return utf16to8<char16_t, char8_t>(source, error);
}
inline uaiw_constexpr std::u32string utf8to32u(std::u8string_view source, una::error& error)
{
    return utf8to32<char8_t, char32_t>(source, error);
}
inline uaiw_constexpr std::u8string utf32to8u(std::u32string_view source, una::error& error)
{
    return utf32to8<char32_t, char8_t>(source, error);
}

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline uaiw_constexpr std::wstring utf8to16(std::u8string_view source, una::error& error)
{
    return utf8to16<char8_t, wchar_t>(source, error);
}
inline uaiw_constexpr std::u8string utf16to8u(std::wstring_view source, una::error& error)
{
    return utf16to8<wchar_t, char8_t>(source, error);
}
#elif WCHAR_MAX >= 0x7FFFFFFF // 32-bit wchar_t
inline uaiw_constexpr std::wstring utf8to32(std::u8string_view source, una::error& error)
{
    return utf8to32<char8_t, wchar_t>(source, error);
}
inline uaiw_constexpr std::u8string utf32to8u(std::wstring_view source, una::error& error)
{
    return utf32to8<wchar_t, char8_t>(source, error);
}
#endif // WCHAR_MAX >= 0x7FFFFFFF

} // namespace strict

#endif // __cpp_lib_char8_t

} // namespace una

#endif // UNI_ALGO_CONV_H_UAIH

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
