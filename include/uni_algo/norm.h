/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_NORM_H_UAIH
#define UNI_ALGO_NORM_H_UAIH

#ifdef UNI_ALGO_DISABLE_NORM
#error "Normalization module is disabled via define UNI_ALGO_DISABLE_NORM"
#endif

#include <string>
#include <string_view>
#include <iterator> // std::back_insert_iterator

#include "config.h"
#include "internal/safe_layer.h"
#include "internal/error.h"

#include "impl/impl_norm.h"

namespace una {

namespace detail {

template<typename Dst, typename Alloc, typename Src, size_t SizeX,
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    size_t(*FnNorm)(typename Src::const_iterator, typename Src::const_iterator, typename Dst::iterator)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    size_t(*FnNorm)(typename Src::const_pointer, typename Src::const_pointer, typename Dst::pointer)>
#else // Safe layer
    size_t(*FnNorm)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, safe::out<typename Dst::pointer>)>
#endif
uaiw_constexpr Dst t_norm(const Alloc& alloc, const Src& src)
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
        dst.resize(FnNorm(src.cbegin(), src.cend(), dst.begin()));
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
        dst.resize(length * SizeX);
        dst.resize(FnNorm(src.data(), src.data() + src.size(), dst.data()));
#else // Safe layer
#  if !defined(__cpp_lib_string_resize_and_overwrite)
        dst.resize(length * SizeX);
        dst.resize(FnNorm(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, safe::out{dst.data(), dst.size()}));
#  else
        dst.resize_and_overwrite(length * SizeX, [&src](typename Dst::pointer p, std::size_t n) noexcept -> std::size_t {
            return FnNorm(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, safe::out{p, n});
        });
#  endif
#endif

#ifndef UNI_ALGO_NO_SHRINK_TO_FIT
        dst.shrink_to_fit();
#endif
    }

    return dst;
}

// For NFKC and NFKD it is ineffective to preallocate a string because max decomposition is 11/18
// in these forms, so we are using back_inserter for these normalization forms.
// Our functions are designed to work with most C++ iterators but sometimes proxy iterators are needed.
// In this case we need a simple proxy output iterator where operator-() is no-op.
// See impl/example/cpp_proxy_iterator.h
template<class Iter>
class proxy_it_out
{
private:
    Iter it;
public:
    uaiw_constexpr explicit proxy_it_out(Iter iter) : it{iter} {}
    uaiw_constexpr decltype(*it) operator*() { return *it; }
    uaiw_constexpr std::size_t operator-(const proxy_it_out&) { return 0; } // no-op
    uaiw_constexpr proxy_it_out& operator++(int) { return *this; } // no-op (by default in C++ output iterators)
    // Test
    /*uaiw_constexpr std::size_t operator-(const proxy_it_out& rhs) { return it - rhs.it; }
    uaiw_constexpr proxy_it_out operator++(int)
    {
        proxy_it_out temp(*this);
        ++it;
        return temp;
    }*/
};

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

template<typename Dst, typename Alloc, typename Src,
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    size_t(*FnNorm)(typename Src::const_iterator, typename Src::const_iterator, proxy_it_out<std::back_insert_iterator<Dst>>)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    size_t(*FnNorm)(typename Src::const_pointer, typename Src::const_pointer, proxy_it_out<std::back_insert_iterator<Dst>>)>
#else // Safe layer
    size_t(*FnNorm)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, proxy_it_out<std::back_insert_iterator<Dst>>)>
#endif
uaiw_constexpr Dst t_norm2(const Alloc& alloc, const Src& src)
{
    Dst dst{alloc};

    const std::size_t length = src.size();

    if (length)
    {
        if (length > dst.max_size() / 3) // Overflow protection
        {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || (_HAS_EXCEPTIONS != 0)
            throw std::bad_alloc();
#else
            std::abort();
#endif
        }

        dst.reserve(length * 3 / 2);

        const proxy_it_out<std::back_insert_iterator<Dst>> it_out{std::back_inserter(dst)};

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
        FnNorm(src.cbegin(), src.cend(), it_out);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
        FnNorm(src.data(), src.data() + src.size(), it_out);
#else // Safe layer
        FnNorm(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, it_out);
#endif

#ifndef UNI_ALGO_NO_SHRINK_TO_FIT
        dst.shrink_to_fit();
#endif
    }

    return dst;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

// Normalization detection

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
template<typename Src, int(*FnDetect)(typename Src::const_iterator, typename Src::const_iterator, bool)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
template<typename Src, int(*FnDetect)(typename Src::const_pointer, typename Src::const_pointer, bool)>
#else // Safe layer
template<typename Src, int(*FnDetect)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, bool)>
#endif
uaiw_constexpr bool t_detect(const Src& src)
{
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    return FnDetect(src.cbegin(), src.cend(), true) == detail::impl_norm_is_yes;
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    return FnDetect(src.data(), src.data() + src.size(), true) == detail::impl_norm_is_yes;
#else // Safe layer
    return FnDetect(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, true) == detail::impl_norm_is_yes;
#endif
}

#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
template<typename Src, int(*FnDetect)(typename Src::const_iterator, typename Src::const_iterator, bool)>
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
template<typename Src, int(*FnDetect)(typename Src::const_pointer, typename Src::const_pointer, bool)>
#else // Safe layer
template<typename Src, int(*FnDetect)(safe::in<typename Src::const_pointer>, safe::end<typename Src::const_pointer>, bool)>
#endif
uaiw_constexpr bool t_detect(const Src& src, una::error& error)
{
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    const int ret = FnDetect(src.cbegin(), src.cend(), true);
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    const int ret = FnDetect(src.data(), src.data() + src.size(), true);
#else // Safe layer
    const int ret = FnDetect(safe::in{src.data(), src.size()}, safe::end{src.data() + src.size()}, true);
#endif

    if (ret == detail::impl_norm_is_yes)
    {
        error.reset();
        return true;
    }
    else if (ret == detail::impl_norm_is_ill_formed)
        error = una::error{una::error::code::ill_formed_utf};
    else if (ret == detail::impl_norm_is_not_stream_safe)
        error = una::error{una::error::code::not_stream_safe};
    else
        error = una::error{una::error::code::not_normalized};

    return false;
}

} // namespace detail

namespace norm {

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_nfc_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_norm_to_nfc_utf8, detail::impl_norm_to_nfc_utf8>(alloc, source);
}

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_nfd_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_norm_to_nfd_utf8, detail::impl_norm_to_nfd_utf8>(alloc, source);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_nfkc_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm2<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_norm_to_nfkc_utf8>(alloc, source);
}

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_nfkd_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm2<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_norm_to_nfkd_utf8>(alloc, source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_PROP
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
uaiw_constexpr std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_unaccent_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_norm_to_unaccent_utf8, detail::impl_norm_to_unaccent_utf8>(alloc, source);
}
#endif // UNI_ALGO_DISABLE_PROP

template<typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_nfc_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_norm_to_nfc_utf16, detail::impl_norm_to_nfc_utf16>(alloc, source);
}

template<typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_nfd_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_norm_to_nfd_utf16, detail::impl_norm_to_nfd_utf16>(alloc, source);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_nfkc_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm2<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_norm_to_nfkc_utf16>(alloc, source);
}

template<typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_nfkd_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm2<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_norm_to_nfkd_utf16>(alloc, source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_PROP
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
uaiw_constexpr std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_unaccent_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_norm_to_unaccent_utf16, detail::impl_norm_to_unaccent_utf16>(alloc, source);
}
#endif // UNI_ALGO_DISABLE_PROP

template<typename UTF8>
uaiw_constexpr bool is_nfc_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_detect<std::basic_string_view<UTF8>, detail::impl_norm_is_nfc_utf8>(source);
}

template<typename UTF8>
uaiw_constexpr bool is_nfd_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_detect<std::basic_string_view<UTF8>, detail::impl_norm_is_nfd_utf8>(source);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF8>
uaiw_constexpr bool is_nfkc_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_detect<std::basic_string_view<UTF8>, detail::impl_norm_is_nfkc_utf8>(source);
}

template<typename UTF8>
uaiw_constexpr bool is_nfkd_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_detect<std::basic_string_view<UTF8>, detail::impl_norm_is_nfkd_utf8>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<typename UTF16>
uaiw_constexpr bool is_nfc_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_detect<std::basic_string_view<UTF16>, detail::impl_norm_is_nfc_utf16>(source);
}

template<typename UTF16>
uaiw_constexpr bool is_nfd_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_detect<std::basic_string_view<UTF16>, detail::impl_norm_is_nfd_utf16>(source);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF16>
uaiw_constexpr bool is_nfkc_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_detect<std::basic_string_view<UTF16>, detail::impl_norm_is_nfkc_utf16>(source);
}

template<typename UTF16>
uaiw_constexpr bool is_nfkd_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_detect<std::basic_string_view<UTF16>, detail::impl_norm_is_nfkd_utf16>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<typename UTF8>
uaiw_constexpr bool is_nfc_utf8(std::basic_string_view<UTF8> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_detect<std::basic_string_view<UTF8>, detail::impl_norm_is_nfc_utf8>(source, error);
}

template<typename UTF8>
uaiw_constexpr bool is_nfd_utf8(std::basic_string_view<UTF8> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_detect<std::basic_string_view<UTF8>, detail::impl_norm_is_nfd_utf8>(source, error);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF8>
uaiw_constexpr bool is_nfkc_utf8(std::basic_string_view<UTF8> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_detect<std::basic_string_view<UTF8>, detail::impl_norm_is_nfkc_utf8>(source, error);
}

template<typename UTF8>
uaiw_constexpr bool is_nfkd_utf8(std::basic_string_view<UTF8> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_detect<std::basic_string_view<UTF8>, detail::impl_norm_is_nfkd_utf8>(source, error);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<typename UTF16>
uaiw_constexpr bool is_nfc_utf16(std::basic_string_view<UTF16> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_detect<std::basic_string_view<UTF16>, detail::impl_norm_is_nfc_utf16>(source, error);
}

template<typename UTF16>
uaiw_constexpr bool is_nfd_utf16(std::basic_string_view<UTF16> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_detect<std::basic_string_view<UTF16>, detail::impl_norm_is_nfd_utf16>(source, error);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF16>
uaiw_constexpr bool is_nfkc_utf16(std::basic_string_view<UTF16> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_detect<std::basic_string_view<UTF16>, detail::impl_norm_is_nfkc_utf16>(source, error);
}

template<typename UTF16>
uaiw_constexpr bool is_nfkd_utf16(std::basic_string_view<UTF16> source, una::error& error)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_detect<std::basic_string_view<UTF16>, detail::impl_norm_is_nfkd_utf16>(source, error);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef UNI_ALGO_EXPERIMENTAL

template<typename Iter, typename Sent, typename Dest>
uaiw_constexpr void to_nfc_utf8(Iter first, Sent last, Dest result)
{
    detail::proxy_it_out it_out(result);
    detail::impl_norm_to_nfc_utf8(first, last, it_out);
}

template<typename Iter, typename Sent, typename Dest>
uaiw_constexpr void to_nfd_utf8(Iter first, Sent last, Dest result)
{
    detail::proxy_it_out it_out(result);
    detail::impl_norm_to_nfd_utf8(first, last, it_out);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename Iter, typename Sent, typename Dest>
uaiw_constexpr void to_nfkc_utf8(Iter first, Sent last, Dest result)
{
    detail::proxy_it_out it_out(result);
    detail::impl_norm_to_nfkc_utf8(first, last, it_out);
}

template<typename Iter, typename Sent, typename Dest>
uaiw_constexpr void to_nfkd_utf8(Iter first, Sent last, Dest result)
{
    detail::proxy_it_out it_out(result);
    detail::impl_norm_to_nfkd_utf8(first, last, it_out);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // UNI_ALGO_EXPERIMENTAL

inline uaiw_constexpr std::string to_nfc_utf8(std::string_view source)
{
    return to_nfc_utf8<char>(source);
}
inline uaiw_constexpr std::string to_nfd_utf8(std::string_view source)
{
    return to_nfd_utf8<char>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr std::string to_nfkc_utf8(std::string_view source)
{
    return to_nfkc_utf8<char>(source);
}
inline uaiw_constexpr std::string to_nfkd_utf8(std::string_view source)
{
    return to_nfkd_utf8<char>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
inline uaiw_constexpr std::string to_unaccent_utf8(std::string_view source)
{
    return to_unaccent_utf8<char>(source);
}
#endif // UNI_ALGO_DISABLE_PROP

inline uaiw_constexpr std::u16string to_nfc_utf16(std::u16string_view source)
{
    return to_nfc_utf16<char16_t>(source);
}
inline uaiw_constexpr std::u16string to_nfd_utf16(std::u16string_view source)
{
    return to_nfd_utf16<char16_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr std::u16string to_nfkc_utf16(std::u16string_view source)
{
    return to_nfkc_utf16<char16_t>(source);
}
inline uaiw_constexpr std::u16string to_nfkd_utf16(std::u16string_view source)
{
    return to_nfkd_utf16<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
inline uaiw_constexpr std::u16string to_unaccent_utf16(std::u16string_view source)
{
    return to_unaccent_utf16<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_PROP

inline uaiw_constexpr bool is_nfc_utf8(std::string_view source)
{
    return is_nfc_utf8<char>(source);
}
inline uaiw_constexpr bool is_nfd_utf8(std::string_view source)
{
    return is_nfd_utf8<char>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr bool is_nfkc_utf8(std::string_view source)
{
    return is_nfkc_utf8<char>(source);
}
inline uaiw_constexpr bool is_nfkd_utf8(std::string_view source)
{
    return is_nfkd_utf8<char>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline uaiw_constexpr bool is_nfc_utf16(std::u16string_view source)
{
    return is_nfc_utf16<char16_t>(source);
}
inline uaiw_constexpr bool is_nfd_utf16(std::u16string_view source)
{
    return is_nfd_utf16<char16_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr bool is_nfkc_utf16(std::u16string_view source)
{
    return is_nfkc_utf16<char16_t>(source);
}
inline uaiw_constexpr bool is_nfkd_utf16(std::u16string_view source)
{
    return is_nfkd_utf16<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline uaiw_constexpr bool is_nfc_utf8(std::string_view source, una::error& error)
{
    return is_nfc_utf8<char>(source, error);
}
inline uaiw_constexpr bool is_nfd_utf8(std::string_view source, una::error& error)
{
    return is_nfd_utf8<char>(source, error);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr bool is_nfkc_utf8(std::string_view source, una::error& error)
{
    return is_nfkc_utf8<char>(source, error);
}
inline uaiw_constexpr bool is_nfkd_utf8(std::string_view source, una::error& error)
{
    return is_nfkd_utf8<char>(source, error);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline uaiw_constexpr bool is_nfc_utf16(std::u16string_view source, una::error& error)
{
    return is_nfc_utf16<char16_t>(source, error);
}
inline uaiw_constexpr bool is_nfd_utf16(std::u16string_view source, una::error& error)
{
    return is_nfd_utf16<char16_t>(source, error);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr bool is_nfkc_utf16(std::u16string_view source, una::error& error)
{
    return is_nfkc_utf16<char16_t>(source, error);
}
inline uaiw_constexpr bool is_nfkd_utf16(std::u16string_view source, una::error& error)
{
    return is_nfkd_utf16<char16_t>(source, error);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline uaiw_constexpr std::wstring to_nfc_utf16(std::wstring_view source)
{
    return to_nfc_utf16<wchar_t>(source);
}
inline uaiw_constexpr std::wstring to_nfd_utf16(std::wstring_view source)
{
    return to_nfd_utf16<wchar_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr std::wstring to_nfkc_utf16(std::wstring_view source)
{
    return to_nfkc_utf16<wchar_t>(source);
}
inline uaiw_constexpr std::wstring to_nfkd_utf16(std::wstring_view source)
{
    return to_nfkd_utf16<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
inline uaiw_constexpr std::wstring to_unaccent_utf16(std::wstring_view source)
{
    return to_unaccent_utf16<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_PROP

inline uaiw_constexpr bool is_nfc_utf16(std::wstring_view source)
{
    return is_nfc_utf16<wchar_t>(source);
}
inline uaiw_constexpr bool is_nfd_utf16(std::wstring_view source)
{
    return is_nfd_utf16<wchar_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr bool is_nfkc_utf16(std::wstring_view source)
{
    return is_nfkc_utf16<wchar_t>(source);
}
inline uaiw_constexpr bool is_nfkd_utf16(std::wstring_view source)
{
    return is_nfkd_utf16<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline uaiw_constexpr bool is_nfc_utf16(std::wstring_view source, una::error& error)
{
    return is_nfc_utf16<wchar_t>(source, error);
}
inline uaiw_constexpr bool is_nfd_utf16(std::wstring_view source, una::error& error)
{
    return is_nfd_utf16<wchar_t>(source, error);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr bool is_nfkc_utf16(std::wstring_view source, una::error& error)
{
    return is_nfkc_utf16<wchar_t>(source, error);
}
inline uaiw_constexpr bool is_nfkd_utf16(std::wstring_view source, una::error& error)
{
    return is_nfkd_utf16<wchar_t>(source, error);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef __cpp_lib_char8_t

inline uaiw_constexpr std::u8string to_nfc_utf8(std::u8string_view source)
{
    return to_nfc_utf8<char8_t>(source);
}
inline uaiw_constexpr std::u8string to_nfd_utf8(std::u8string_view source)
{
    return to_nfd_utf8<char8_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr std::u8string to_nfkc_utf8(std::u8string_view source)
{
    return to_nfkc_utf8<char8_t>(source);
}
inline uaiw_constexpr std::u8string to_nfkd_utf8(std::u8string_view source)
{
    return to_nfkd_utf8<char8_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
inline uaiw_constexpr std::u8string to_unaccent_utf8(std::u8string_view source)
{
    return to_unaccent_utf8<char8_t>(source);
}
#endif // UNI_ALGO_DISABLE_PROP

inline uaiw_constexpr bool is_nfc_utf8(std::u8string_view source)
{
    return is_nfc_utf8<char8_t>(source);
}
inline uaiw_constexpr bool is_nfd_utf8(std::u8string_view source)
{
    return is_nfd_utf8<char8_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr bool is_nfkc_utf8(std::u8string_view source)
{
    return is_nfkc_utf8<char8_t>(source);
}
inline uaiw_constexpr bool is_nfkd_utf8(std::u8string_view source)
{
    return is_nfkd_utf8<char8_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline uaiw_constexpr bool is_nfc_utf8(std::u8string_view source, una::error& error)
{
    return is_nfc_utf8<char8_t>(source, error);
}
inline uaiw_constexpr bool is_nfd_utf8(std::u8string_view source, una::error& error)
{
    return is_nfd_utf8<char8_t>(source, error);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr bool is_nfkc_utf8(std::u8string_view source, una::error& error)
{
    return is_nfkc_utf8<char8_t>(source, error);
}
inline uaiw_constexpr bool is_nfkd_utf8(std::u8string_view source, una::error& error)
{
    return is_nfkd_utf8<char8_t>(source, error);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // __cpp_lib_char8_t

} // namespace norm

// ----------
// PROPERTIES
// ----------

namespace codepoint {

class prop_norm
{
private:
    detail::type_codept data = 0;

public:
    uaiw_constexpr prop_norm() = delete;
    uaiw_constexpr explicit prop_norm(char32_t c) noexcept : data{detail::impl_norm_get_prop(c)} {}

    uaiw_constexpr unsigned char Canonical_Combining_Class() const noexcept
    {
        // The Unicode Standard: UnicodeData.txt -> Canonical_Combining_Class
        return detail::impl_norm_get_ccc_prop(data);
    }
    uaiw_constexpr bool NFC_Quick_Check_Yes() const noexcept
    {
        // The Unicode Standard: DerivedNormalizationProps.txt -> NFC_Quick_Check=Yes
        return detail::impl_norm_is_nfc_qc_yes_prop(data);
    }
    uaiw_constexpr bool NFD_Quick_Check_Yes() const noexcept
    {
        // The Unicode Standard: DerivedNormalizationProps.txt -> NFD_Quick_Check=Yes
        return detail::impl_norm_is_nfd_qc_yes_prop(data);
    }
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    uaiw_constexpr bool NFKC_Quick_Check_Yes() const noexcept
    {
        // The Unicode Standard: DerivedNormalizationProps.txt -> NFKC_Quick_Check=Yes
        return detail::impl_norm_is_nfkc_qc_yes_prop(data);
    }
    uaiw_constexpr bool NFKD_Quick_Check_Yes() const noexcept
    {
        // The Unicode Standard: DerivedNormalizationProps.txt -> NFKD_Quick_Check=Yes
        return detail::impl_norm_is_nfkd_qc_yes_prop(data);
    }
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
};

inline uaiw_constexpr char32_t to_compose(char32_t c1, char32_t c2) noexcept
{
    return detail::impl_norm_to_compose(c1, c2);
}

inline uaiw_constexpr std::u32string to_decompose_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(detail::impl_max_norm_decomp_canon);
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    dst.resize(detail::impl_norm_to_decompose(c, dst.begin()));
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    dst.resize(detail::impl_norm_to_decompose(c, dst.data()));
#else // Safe layer
    dst.resize(detail::impl_norm_to_decompose(c, detail::safe::out{dst.data(), dst.size()}));
#endif
    return dst;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline uaiw_constexpr std::u32string to_decompose_compat_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(detail::impl_max_norm_decomp_compat);
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    dst.resize(detail::impl_norm_to_decompose_compat(c, dst.begin()));
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    dst.resize(detail::impl_norm_to_decompose_compat(c, dst.data()));
#else // Safe layer
    dst.resize(detail::impl_norm_to_decompose_compat(c, detail::safe::out{dst.data(), dst.size()}));
#endif
    return dst;
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifdef UNI_ALGO_EXPERIMENTAL
inline uaiw_constexpr std::u32string to_decompose_hangul_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(detail::impl_max_norm_decomp_canon);
#if defined(UNI_ALGO_FORCE_CPP_ITERATORS)
    dst.resize(detail::impl_norm_to_decompose_hangul(c, dst.begin()));
#elif defined(UNI_ALGO_FORCE_C_POINTERS)
    dst.resize(detail::impl_norm_to_decompose_hangul(c, dst.data()));
#else // Safe layer
    dst.resize(detail::impl_norm_to_decompose_hangul(c, detail::safe::out{dst.data(), dst.size()}));
#endif
    return dst;
}
#endif // UNI_ALGO_EXPERIMENTAL

} // namespace codepoint

} // namespace una

#endif // UNI_ALGO_NORM_H_UAIH

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
