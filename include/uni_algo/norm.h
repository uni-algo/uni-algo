/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_NORM_H_UAIH
#define UNI_ALGO_NORM_H_UAIH

#ifdef UNI_ALGO_DISABLE_NORM
#  error "Normalization module is disabled via define UNI_ALGO_DISABLE_NORM"
#endif

#include <type_traits>
#include <string>
#include <string_view>
#include <iterator>

#include "config.h"
#include "version.h"
#include "internal/ranges_core.h"

#include "impl/impl_norm.h"

// For info about defines see uni_cpp_convert.h

namespace uni {

namespace detail {

template<typename Dst, typename Alloc, typename Src, size_t SizeX,
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    size_t(*FnNorm)(typename Src::const_pointer, typename Src::const_pointer, typename Dst::pointer)>
#else
    size_t(*FnNorm)(typename Src::const_iterator, typename Src::const_iterator, typename Dst::iterator)>
#endif
Dst t_norm(const Alloc& alloc, Src src)
{
    Dst dst{alloc};

    std::size_t length = src.size();

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

        dst.resize(length * SizeX);
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
        dst.resize(FnNorm(src.data(), src.data() + src.size(), dst.data()));
#else
        dst.resize(FnNorm(src.cbegin(), src.cend(), dst.begin()));
#endif
#ifndef UNI_ALGO_DISABLE_SHRINK_TO_FIT
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
    explicit proxy_it_out(Iter iter) : it(iter) {}
    decltype(*it) operator*() { return *it; }
    size_t operator-(const proxy_it_out&) { return 0; } // no-op
    proxy_it_out& operator++(int) { return *this; } // no-op (by default in C++ output iterators)
    // Test
    /*size_t operator-(const proxy_it_out& rhs) { return it - rhs.it; }
    proxy_it_out operator++(int)
    {
        proxy_it_out temp(*this);
        ++it;
        return temp;
    }*/
};

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

template<typename Dst, typename Alloc, typename Src,
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    size_t(*FnNorm)(typename Src::const_pointer, typename Src::const_pointer, proxy_it_out<std::back_insert_iterator<Dst>>)>
#else
    size_t(*FnNorm)(typename Src::const_iterator, typename Src::const_iterator, proxy_it_out<std::back_insert_iterator<Dst>>)>
#endif
Dst t_norm2(const Alloc& alloc, Src src)
{
    Dst dst{alloc};

    std::size_t length = src.size();

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

        proxy_it_out<std::back_insert_iterator<Dst>> it_out{std::back_inserter(dst)};

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
        FnNorm(src.data(), src.data() + src.size(), it_out);
#else
        FnNorm(src.cbegin(), src.cend(), it_out);
#endif
#ifndef UNI_ALGO_DISABLE_SHRINK_TO_FIT
        dst.shrink_to_fit();
#endif
    }

    return dst;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace detail

namespace norm {

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_nfc_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_norm_to_nfc_utf8, detail::impl_norm_to_nfc_utf8>(alloc, source);
}

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_nfd_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_norm_to_nfd_utf8, detail::impl_norm_to_nfd_utf8>(alloc, source);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_nfkc_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm2<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_norm_to_nfkc_utf8>(alloc, source);
}

template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_nfkd_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm2<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_norm_to_nfkd_utf8>(alloc, source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_PROP
template<typename UTF8, typename Alloc = std::allocator<UTF8>>
std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>
to_unaccent_utf8(std::basic_string_view<UTF8> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF8>);

    return detail::t_norm<std::basic_string<UTF8, std::char_traits<UTF8>, Alloc>, Alloc, std::basic_string_view<UTF8>,
            detail::impl_x_norm_to_unaccent_utf8, detail::impl_norm_to_unaccent_utf8>(alloc, source);
}
#endif // UNI_ALGO_DISABLE_PROP

template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_nfc_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_norm_to_nfc_utf16, detail::impl_norm_to_nfc_utf16>(alloc, source);
}

template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_nfd_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_norm_to_nfd_utf16, detail::impl_norm_to_nfd_utf16>(alloc, source);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_nfkc_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm2<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_norm_to_nfkc_utf16>(alloc, source);
}

template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_nfkd_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm2<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_norm_to_nfkd_utf16>(alloc, source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_PROP
template<typename UTF16, typename Alloc = std::allocator<UTF16>>
std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>
to_unaccent_utf16(std::basic_string_view<UTF16> source, const Alloc& alloc = Alloc())
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16, std::char_traits<UTF16>, Alloc>, Alloc, std::basic_string_view<UTF16>,
            detail::impl_x_norm_to_unaccent_utf16, detail::impl_norm_to_unaccent_utf16>(alloc, source);
}
#endif // UNI_ALGO_DISABLE_PROP

template<typename UTF8>
bool is_nfc_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_norm_is_nfc_utf8(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_norm_is_nfc_utf8(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

template<typename UTF8>
bool is_nfd_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_norm_is_nfd_utf8(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_norm_is_nfd_utf8(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF8>
bool is_nfkc_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_norm_is_nfkc_utf8(source.data(), source.data() + source.size())  == detail::impl_norm_is_yes;
#else
    return detail::impl_norm_is_nfkc_utf8(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

template<typename UTF8>
bool is_nfkd_utf8(std::basic_string_view<UTF8> source)
{
    static_assert(std::is_integral_v<UTF8>);

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_norm_is_nfkd_utf8(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_norm_is_nfkd_utf8(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<typename UTF16>
bool is_nfc_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_norm_is_nfc_utf16(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_norm_is_nfc_utf16(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

template<typename UTF16>
bool is_nfd_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_norm_is_nfd_utf16(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_norm_is_nfd_utf16(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF16>
bool is_nfkc_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_norm_is_nfkc_utf16(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_norm_is_nfkc_utf16(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

template<typename UTF16>
bool is_nfkd_utf16(std::basic_string_view<UTF16> source)
{
    static_assert(std::is_integral_v<UTF16> && sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_norm_is_nfkd_utf16(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_norm_is_nfkd_utf16(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<typename Iter, typename Sent, typename Dest>
void to_nfc_utf8(Iter first, Sent last, Dest result)
{
    // TODO: static_assert for Dest is output iterator
    detail::proxy_it_out it_out(result);
    detail::impl_norm_to_nfc_utf8(first, last, it_out);
}

template<typename Iter, typename Sent, typename Dest>
void to_nfd_utf8(Iter first, Sent last, Dest result)
{
    // TODO: static_assert for Dest is output iterator
    detail::proxy_it_out it_out(result);
    detail::impl_norm_to_nfd_utf8(first, last, it_out);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename Iter, typename Sent, typename Dest>
void to_nfkc_utf8(Iter first, Sent last, Dest result)
{
    // TODO: static_assert for Dest is output iterator
    detail::proxy_it_out it_out(result);
    detail::impl_norm_to_nfkc_utf8(first, last, it_out);
}

template<typename Iter, typename Sent, typename Dest>
void to_nfkd_utf8(Iter first, Sent last, Dest result)
{
    // TODO: static_assert for Dest is output iterator
    detail::proxy_it_out it_out(result);
    detail::impl_norm_to_nfkd_utf8(first, last, it_out);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline std::string to_nfc_utf8(std::string_view source)
{
    return to_nfc_utf8<char>(source);
}
inline std::string to_nfd_utf8(std::string_view source)
{
    return to_nfd_utf8<char>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline std::string to_nfkc_utf8(std::string_view source)
{
    return to_nfkc_utf8<char>(source);
}
inline std::string to_nfkd_utf8(std::string_view source)
{
    return to_nfkd_utf8<char>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
inline std::string to_unaccent_utf8(std::string_view source)
{
    return to_unaccent_utf8<char>(source);
}
#endif // UNI_ALGO_DISABLE_PROP

inline std::u16string to_nfc_utf16(std::u16string_view source)
{
    return to_nfc_utf16<char16_t>(source);
}
inline std::u16string to_nfd_utf16(std::u16string_view source)
{
    return to_nfd_utf16<char16_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline std::u16string to_nfkc_utf16(std::u16string_view source)
{
    return to_nfkc_utf16<char16_t>(source);
}
inline std::u16string to_nfkd_utf16(std::u16string_view source)
{
    return to_nfkd_utf16<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
inline std::u16string to_unaccent_utf16(std::u16string_view source)
{
    return to_unaccent_utf16<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_PROP

inline bool is_nfc_utf8(std::string_view source)
{
    return is_nfc_utf8<char>(source);
}
inline bool is_nfd_utf8(std::string_view source)
{
    return is_nfd_utf8<char>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline bool is_nfkc_utf8(std::string_view source)
{
    return is_nfkc_utf8<char>(source);
}
inline bool is_nfkd_utf8(std::string_view source)
{
    return is_nfkd_utf8<char>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline bool is_nfc_utf16(std::u16string_view source)
{
    return is_nfc_utf16<char16_t>(source);
}
inline bool is_nfd_utf16(std::u16string_view source)
{
    return is_nfd_utf16<char16_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline bool is_nfkc_utf16(std::u16string_view source)
{
    return is_nfkc_utf16<char16_t>(source);
}
inline bool is_nfkd_utf16(std::u16string_view source)
{
    return is_nfkd_utf16<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring to_nfc_utf16(std::wstring_view source)
{
    return to_nfc_utf16<wchar_t>(source);
}
inline std::wstring to_nfd_utf16(std::wstring_view source)
{
    return to_nfd_utf16<wchar_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline std::wstring to_nfkc_utf16(std::wstring_view source)
{
    return to_nfkc_utf16<wchar_t>(source);
}
inline std::wstring to_nfkd_utf16(std::wstring_view source)
{
    return to_nfkd_utf16<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
inline std::wstring to_unaccent_utf16(std::wstring_view source)
{
    return to_unaccent_utf16<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_PROP

inline bool is_nfc_utf16(std::wstring_view source)
{
    return is_nfc_utf16<wchar_t>(source);
}
inline bool is_nfd_utf16(std::wstring_view source)
{
    return is_nfd_utf16<wchar_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline bool is_nfkc_utf16(std::wstring_view source)
{
    return is_nfkc_utf16<wchar_t>(source);
}
inline bool is_nfkd_utf16(std::wstring_view source)
{
    return is_nfkd_utf16<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

#ifdef __cpp_lib_char8_t

inline std::u8string to_nfc_utf8(std::u8string_view source)
{
    return to_nfc_utf8<char8_t>(source);
}
inline std::u8string to_nfd_utf8(std::u8string_view source)
{
    return to_nfd_utf8<char8_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline std::u8string to_nfkc_utf8(std::u8string_view source)
{
    return to_nfkc_utf8<char8_t>(source);
}
inline std::u8string to_nfkd_utf8(std::u8string_view source)
{
    return to_nfkd_utf8<char8_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_PROP
inline std::u8string to_unaccent_utf8(std::u8string_view source)
{
    return to_unaccent_utf8<char8_t>(source);
}
#endif // UNI_ALGO_DISABLE_PROP

inline bool is_nfc_utf8(std::u8string_view source)
{
    return is_nfc_utf8<char8_t>(source);
}
inline bool is_nfd_utf8(std::u8string_view source)
{
    return is_nfd_utf8<char8_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline bool is_nfkc_utf8(std::u8string_view source)
{
    return is_nfkc_utf8<char8_t>(source);
}
inline bool is_nfkd_utf8(std::u8string_view source)
{
    return is_nfkd_utf8<char8_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#endif // __cpp_lib_char8_t

} // namespace norm

// ------
// RANGES
// ------

namespace ranges::norm {

template<class Range>
class nfc_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class nfc
    {
        static_assert(std::is_integral_v<detail::ranges::iter_value_t<Iter>> &&
                      sizeof(detail::ranges::iter_value_t<Iter>) >= sizeof(char32_t),
                      "norm::nfc view requires char32_t range");

    private:
        nfc_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        void iter_func_norm_nfc()
        {
            if (!detail::unstable_norm_iter_ready(&state))
                while (it_pos != std::end(parent->range) && !detail::unstable_norm_iter_nfc(&state, *it_pos++));
            if (!detail::unstable_norm_iter_next_comp(&state, &codepoint))
                stream_end = true;
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = std::ptrdiff_t;

        nfc() = default;
        explicit nfc(nfc_view& p, Iter begin, Sent) : parent{std::addressof(p)}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_norm_nfc(); // Fn call must not be inlined
        }
        reference operator*() const noexcept { return codepoint; }
        nfc& operator++()
        {
            iter_func_norm_nfc(); // Fn call must not be inlined

            return *this;
        }
        nfc operator++(int)
        {
            nfc tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const nfc& x, const nfc& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfc& x, const nfc& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const nfc& x, uni::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfc& x, uni::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const nfc& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const nfc& x) { return !x.stream_end; }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    nfc<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr nfc_view() = default;
    uaiw_constexpr explicit nfc_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = nfc<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        return uni::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
class nfd_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class nfd
    {
        static_assert(std::is_integral_v<detail::ranges::iter_value_t<Iter>> &&
                      sizeof(detail::ranges::iter_value_t<Iter>) >= sizeof(char32_t),
                      "norm::nfd view requires char32_t range");

    private:
        nfd_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        void iter_func_norm_nfd()
        {
            if (!detail::unstable_norm_iter_ready(&state))
                while (it_pos != std::end(parent->range) && !detail::unstable_norm_iter_nfd(&state, *it_pos++));
            if (!detail::unstable_norm_iter_next_decomp(&state, &codepoint))
                stream_end = true;
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = std::ptrdiff_t;

        nfd() = default;
        explicit nfd(nfd_view& p, Iter begin, Sent) : parent{std::addressof(p)}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_norm_nfd(); // Fn call must not be inlined
        }
        reference operator*() const noexcept { return codepoint; }
        nfd& operator++()
        {
            iter_func_norm_nfd(); // Fn call must not be inlined

            return *this;
        }
        nfd operator++(int)
        {
            nfd tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const nfd& x, const nfd& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfd& x, const nfd& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const nfd& x, uni::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfd& x, uni::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const nfd& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const nfd& x) { return !x.stream_end; }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    nfd<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr nfd_view() = default;
    uaiw_constexpr explicit nfd_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = nfd<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        return uni::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

template<class Range>
class nfkc_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class nfkc
    {
        static_assert(std::is_integral_v<detail::ranges::iter_value_t<Iter>> &&
                      sizeof(detail::ranges::iter_value_t<Iter>) >= sizeof(char32_t),
                      "norm::nfkc view requires char32_t range");

    private:
        nfkc_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        void iter_func_norm_nfkc()
        {
            if (!detail::unstable_norm_iter_ready(&state))
                while (it_pos != std::end(parent->range) && !detail::unstable_norm_iter_nfkc(&state, *it_pos++));
            if (!detail::unstable_norm_iter_next_comp(&state, &codepoint))
                stream_end = true;
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = std::ptrdiff_t;

        nfkc() = default;
        explicit nfkc(nfkc_view& p, Iter begin, Sent) : parent{std::addressof(p)}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_norm_nfkc(); // Fn call must not be inlined
        }
        reference operator*() const noexcept { return codepoint; }
        nfkc& operator++()
        {
            iter_func_norm_nfkc(); // Fn call must not be inlined

            return *this;
        }
        nfkc operator++(int)
        {
            nfkc tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const nfkc& x, const nfkc& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfkc& x, const nfkc& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const nfkc& x, uni::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfkc& x, uni::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const nfkc& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const nfkc& x) { return !x.stream_end; }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    nfkc<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr nfkc_view() = default;
    uaiw_constexpr explicit nfkc_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = nfkc<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        return uni::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
class nfkd_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class nfkd
    {
        static_assert(std::is_integral_v<detail::ranges::iter_value_t<Iter>> &&
                      sizeof(detail::ranges::iter_value_t<Iter>) >= sizeof(char32_t),
                      "norm::nfkd view requires char32_t range");

    private:
        nfkd_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        void iter_func_norm_nfkd()
        {
            if (!detail::unstable_norm_iter_ready(&state))
                while (it_pos != std::end(parent->range) && !detail::unstable_norm_iter_nfkd(&state, *it_pos++));
            if (!detail::unstable_norm_iter_next_decomp(&state, &codepoint))
                stream_end = true;
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = std::ptrdiff_t;

        nfkd() = default;
        explicit nfkd(nfkd_view& p, Iter begin, Sent) : parent{std::addressof(p)}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_norm_nfkd(); // Fn call must not be inlined
        }
        reference operator*() const noexcept { return codepoint; }
        nfkd& operator++()
        {
            iter_func_norm_nfkd(); // Fn call must not be inlined

            return *this;
        }
        nfkd operator++(int)
        {
            nfkd tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const nfkd& x, const nfkd& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfkd& x, const nfkd& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const nfkd& x, uni::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfkd& x, uni::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const nfkd& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const nfkd& x) { return !x.stream_end; }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    nfkd<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr nfkd_view() = default;
    uaiw_constexpr explicit nfkd_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = nfkd<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        return uni::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<class Range>
nfc_view(Range&&) -> nfc_view<uni::views::all_t<Range>>;
template<class Range>
nfd_view(Range&&) -> nfd_view<uni::views::all_t<Range>>;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<class Range>
nfkc_view(Range&&) -> nfkc_view<uni::views::all_t<Range>>;
template<class Range>
nfkd_view(Range&&) -> nfkd_view<uni::views::all_t<Range>>;
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace ranges::norm

namespace detail {

/* NFC_VIEW */

struct adaptor_nfc
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::norm::nfc_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_nfc& a) { return a(std::forward<R>(r)); }

/* NFD_VIEW */

struct adaptor_nfd
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::norm::nfd_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_nfd& a) { return a(std::forward<R>(r)); }

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

/* NFKC_VIEW */

struct adaptor_nfkc
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::norm::nfkc_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_nfkc& a) { return a(std::forward<R>(r)); }

/* NFKD_VIEW */

struct adaptor_nfkd
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::norm::nfkd_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_nfkd& a) { return a(std::forward<R>(r)); }

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace detail

namespace ranges::views::norm {

inline constexpr detail::adaptor_nfc nfc;
inline constexpr detail::adaptor_nfd nfd;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline constexpr detail::adaptor_nfkc nfkc;
inline constexpr detail::adaptor_nfkd nfkd;
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace ranges::views::norm

namespace views = ranges::views;

// ----------
// PROPERTIES
// ----------

namespace codepoint {

class prop_norm
{
private:
    detail::type_codept data = 0;

public:
    prop_norm() = delete;
    explicit prop_norm(char32_t c) noexcept : data{detail::impl_norm_get_prop(c)} {}

    unsigned char Canonical_Combining_Class() const noexcept
    {
        // The Unicode Standard: UnicodeData.txt -> Canonical_Combining_Class
        return detail::impl_norm_get_ccc_prop(data);
    }
    bool NFC_Quick_Check_Yes() const noexcept
    {
        // The Unicode Standard: DerivedNormalizationProps.txt -> NFC_Quick_Check=Yes
        return detail::impl_norm_is_nfc_qc_yes_prop(data);
    }
    bool NFD_Quick_Check_Yes() const noexcept
    {
        // The Unicode Standard: DerivedNormalizationProps.txt -> NFD_Quick_Check=Yes
        return detail::impl_norm_is_nfd_qc_yes_prop(data);
    }
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
    bool NFKC_Quick_Check_Yes() const noexcept
    {
        // The Unicode Standard: DerivedNormalizationProps.txt -> NFKC_Quick_Check=Yes
        return detail::impl_norm_is_nfkc_qc_yes_prop(data);
    }
    bool NFKD_Quick_Check_Yes() const noexcept
    {
        // The Unicode Standard: DerivedNormalizationProps.txt -> NFKD_Quick_Check=Yes
        return detail::impl_norm_is_nfkd_qc_yes_prop(data);
    }
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
};

inline char32_t to_compose(char32_t c1, char32_t c2) noexcept
{
    return uni::detail::impl_norm_to_compose(c1, c2);
}

inline std::u32string to_decompose_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(uni::detail::impl_x_norm_to_nfd_utf16); // TODO: Better value
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    dst.resize(detail::impl_norm_to_decompose(c, dst.data()));
#else
    dst.resize(detail::impl_norm_to_decompose(c, dst.begin()));
#endif
    return dst;
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline std::u32string to_decompose_compat_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(uni::detail::impl_x_norm_to_nfkd_utf16); // TODO: Better value
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    dst.resize(detail::impl_norm_to_decompose_compat(c, dst.data()));
#else
    dst.resize(detail::impl_norm_to_decompose_compat(c, dst.begin()));
#endif
    return dst;
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline std::u32string to_decompose_hangul_u32(char32_t c)
{
    std::u32string dst;
    dst.resize(uni::detail::impl_x_norm_to_nfd_utf16); // TODO: Better value
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    dst.resize(detail::impl_norm_to_decompose_hangul(c, dst.data()));
#else
    dst.resize(detail::impl_norm_to_decompose_hangul(c, dst.begin()));
#endif
    return dst;
}

} // namespace codepoint

} // namespace uni

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
