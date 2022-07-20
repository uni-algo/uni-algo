/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_NORM_H_UAIX
#define CPP_UNI_NORM_H_UAIX

#ifdef UNI_ALGO_DISABLE_NORM
#  error "Normalization module is disabled via define UNI_ALGO_DISABLE_NORM"
#endif

#include <type_traits>
#include <string>
#include <string_view>
#include <iterator>

#include <uni/config.h>
#include <uni/version.h>

#include <uni/impl/norm.h>

// For info about defines see uni_cpp_convert.h

namespace uni {

namespace detail {

template<typename DST, typename SRC, size_t SZ,
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    size_t(*FNNORM)(typename SRC::const_pointer, typename SRC::const_pointer, typename DST::pointer)>
#else
    size_t(*FNNORM)(typename SRC::const_iterator, typename SRC::const_iterator, typename DST::iterator)>
#endif
DST t_norm(SRC source)
{
    DST destination;

    std::size_t length = source.size();

    if (length)
    {
        destination.resize(length * SZ);
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
        destination.resize(FNNORM(source.data(), source.data() + source.size(), destination.data()));
#else
        destination.resize(FNNORM(source.cbegin(), source.cend(), destination.begin()));
#endif
#ifndef UNI_ALGO_DISABLE_CPP_SHRINK_TO_FIT
        destination.shrink_to_fit();
#endif
    }

    return destination;
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

template<typename DST, typename SRC,
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    size_t(*FNNORM)(typename SRC::const_pointer, typename SRC::const_pointer, proxy_it_out<std::back_insert_iterator<DST>>)>
#else
    size_t(*FNNORM)(typename SRC::const_iterator, typename SRC::const_iterator, proxy_it_out<std::back_insert_iterator<DST>>)>
#endif
DST t_norm2(SRC source)
{
    DST destination;

    std::size_t length = source.size();

    //if (length && length <= destination.max_size() / 3)
    if (length)
    {
        destination.reserve(length * 3 / 2);

        proxy_it_out<std::back_insert_iterator<DST>> it_out(std::back_inserter(destination));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
        FNNORM(source.data(), source.data() + source.size(), it_out);
#else
        FNNORM(source.cbegin(), source.cend(), it_out);
#endif
#ifndef UNI_ALGO_DISABLE_CPP_SHRINK_TO_FIT
        destination.shrink_to_fit();
#endif
    }

    return destination;
}

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace detail

namespace norm {

template<typename UTF8>
std::basic_string<UTF8> utf8_nfc(std::basic_string_view<UTF8> source)
{
    return detail::t_norm<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_nfc, detail::impl_utf8_nfc>(source);
}

template<typename UTF8>
std::basic_string<UTF8> utf8_nfd(std::basic_string_view<UTF8> source)
{
    return detail::t_norm<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_nfd, detail::impl_utf8_nfd>(source);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF8>
std::basic_string<UTF8> utf8_nfkc(std::basic_string_view<UTF8> source)
{
    return detail::t_norm2<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_utf8_nfkc>(source);
}

template<typename UTF8>
std::basic_string<UTF8> utf8_nfkd(std::basic_string_view<UTF8> source)
{
    return detail::t_norm2<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_utf8_nfkd>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UNACCENT
template<typename UTF8>
std::basic_string<UTF8> utf8_unaccent(std::basic_string_view<UTF8> source)
{
    return detail::t_norm<std::basic_string<UTF8>, std::basic_string_view<UTF8>,
            detail::impl_x_utf8_unaccent, detail::impl_utf8_unaccent>(source);
}
#endif // UNI_ALGO_DISABLE_UNACCENT

template<typename UTF16>
std::basic_string<UTF16> utf16_nfc(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_nfc, detail::impl_utf16_nfc>(source);
}

template<typename UTF16>
std::basic_string<UTF16> utf16_nfd(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_nfd, detail::impl_utf16_nfd>(source);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF16>
std::basic_string<UTF16> utf16_nfkc(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm2<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_utf16_nfkc>(source);
}

template<typename UTF16>
std::basic_string<UTF16> utf16_nfkd(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm2<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_utf16_nfkd>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#ifndef UNI_ALGO_DISABLE_UNACCENT
template<typename UTF16>
std::basic_string<UTF16> utf16_unaccent(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

    return detail::t_norm<std::basic_string<UTF16>, std::basic_string_view<UTF16>,
            detail::impl_x_utf16_unaccent, detail::impl_utf16_unaccent>(source);
}
#endif // UNI_ALGO_DISABLE_UNACCENT

template<typename UTF8>
bool utf8_is_nfc(std::basic_string_view<UTF8> source)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_is_nfc(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_utf8_is_nfc(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

template<typename UTF8>
bool utf8_is_nfd(std::basic_string_view<UTF8> source)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_is_nfd(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_utf8_is_nfd(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF8>
bool utf8_is_nfkc(std::basic_string_view<UTF8> source)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_is_nfkc(source.data(), source.data() + source.size())  == detail::impl_norm_is_yes;
#else
    return detail::impl_utf8_is_nfkc(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

template<typename UTF8>
bool utf8_is_nfkd(std::basic_string_view<UTF8> source)
{
#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf8_is_nfkd(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_utf8_is_nfkd(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<typename UTF16>
bool utf16_is_nfc(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf16_is_nfc(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_utf16_is_nfc(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

template<typename UTF16>
bool utf16_is_nfd(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf16_is_nfd(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_utf16_is_nfd(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename UTF16>
bool utf16_is_nfkc(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf16_is_nfkc(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_utf16_is_nfkc(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}

template<typename UTF16>
bool utf16_is_nfkd(std::basic_string_view<UTF16> source)
{
    static_assert(sizeof(UTF16) >= sizeof(char16_t));

#ifdef UNI_ALGO_DISABLE_CPP_ITERATORS
    return detail::impl_utf16_is_nfkd(source.data(), source.data() + source.size()) == detail::impl_norm_is_yes;
#else
    return detail::impl_utf16_is_nfkd(source.cbegin(), source.cend()) == detail::impl_norm_is_yes;
#endif
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<typename Iter, typename Sent, typename Dest>
void utf8_nfc(Iter first, Sent last, Dest result)
{
    // TODO: static_assert for Dest is output iterator
    detail::proxy_it_out it_out(result);
    detail::impl_utf8_nfc(first, last, it_out);
}

template<typename Iter, typename Sent, typename Dest>
void utf8_nfd(Iter first, Sent last, Dest result)
{
    // TODO: static_assert for Dest is output iterator
    detail::proxy_it_out it_out(result);
    detail::impl_utf8_nfd(first, last, it_out);
}

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<typename Iter, typename Sent, typename Dest>
void utf8_nfkc(Iter first, Sent last, Dest result)
{
    // TODO: static_assert for Dest is output iterator
    detail::proxy_it_out it_out(result);
    detail::impl_utf8_nfkc(first, last, it_out);
}

template<typename Iter, typename Sent, typename Dest>
void utf8_nfkd(Iter first, Sent last, Dest result)
{
    // TODO: static_assert for Dest is output iterator
    detail::proxy_it_out it_out(result);
    detail::impl_utf8_nfkd(first, last, it_out);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline std::string utf8_nfc(std::string_view source)
{
    return utf8_nfc<char>(source);
}
inline std::string utf8_nfd(std::string_view source)
{
    return utf8_nfd<char>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline std::string utf8_nfkc(std::string_view source)
{
    return utf8_nfkc<char>(source);
}
inline std::string utf8_nfkd(std::string_view source)
{
    return utf8_nfkd<char>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_UNACCENT
inline std::string utf8_unaccent(std::string_view source)
{
    return utf8_unaccent<char>(source);
}
#endif // UNI_ALGO_DISABLE_UNACCENT

inline std::u16string utf16_nfc(std::u16string_view source)
{
    return utf16_nfc<char16_t>(source);
}
inline std::u16string utf16_nfd(std::u16string_view source)
{
    return utf16_nfd<char16_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline std::u16string utf16_nfkc(std::u16string_view source)
{
    return utf16_nfkc<char16_t>(source);
}
inline std::u16string utf16_nfkd(std::u16string_view source)
{
    return utf16_nfkd<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_UNACCENT
inline std::u16string utf16_unaccent(std::u16string_view source)
{
    return utf16_unaccent<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_UNACCENT

inline bool utf8_is_nfc(std::string_view source)
{
    return utf8_is_nfc<char>(source);
}
inline bool utf8_is_nfd(std::string_view source)
{
    return utf8_is_nfd<char>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline bool utf8_is_nfkc(std::string_view source)
{
    return utf8_is_nfkc<char>(source);
}
inline bool utf8_is_nfkd(std::string_view source)
{
    return utf8_is_nfkd<char>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

inline bool utf16_is_nfc(std::u16string_view source)
{
    return utf16_is_nfc<char16_t>(source);
}
inline bool utf16_is_nfd(std::u16string_view source)
{
    return utf16_is_nfd<char16_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline bool utf16_is_nfkc(std::u16string_view source)
{
    return utf16_is_nfkc<char16_t>(source);
}
inline bool utf16_is_nfkd(std::u16string_view source)
{
    return utf16_is_nfkd<char16_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

#if WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF // 16-bit wchar_t
inline std::wstring utf16_nfc(std::wstring_view source)
{
    return utf16_nfc<wchar_t>(source);
}
inline std::wstring utf16_nfd(std::wstring_view source)
{
    return utf16_nfd<wchar_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline std::wstring utf16_nfkc(std::wstring_view source)
{
    return utf16_nfkc<wchar_t>(source);
}
inline std::wstring utf16_nfkd(std::wstring_view source)
{
    return utf16_nfkd<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#ifndef UNI_ALGO_DISABLE_UNACCENT
inline std::wstring utf16_unaccent(std::wstring_view source)
{
    return utf16_unaccent<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_UNACCENT

inline bool utf16_is_nfc(std::wstring_view source)
{
    return utf16_is_nfc<wchar_t>(source);
}
inline bool utf16_is_nfd(std::wstring_view source)
{
    return utf16_is_nfd<wchar_t>(source);
}
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline bool utf16_is_nfkc(std::wstring_view source)
{
    return utf16_is_nfkc<wchar_t>(source);
}
inline bool utf16_is_nfkd(std::wstring_view source)
{
    return utf16_is_nfkd<wchar_t>(source);
}
#endif // UNI_ALGO_DISABLE_NFKC_NFKD
#endif // WCHAR_MAX >= 0x7FFF && WCHAR_MAX <= 0xFFFF

} // namespace norm

namespace iter::norm {

template<class Iter, class Sent = Iter>
class nfc
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;

    bool stream_end = false;

    detail::type_codept codepoint = 0;
    detail::impl_norm_iter_state state{};

    void iter_func_nfc()
    {
        /* TODO: Fun fact. If following functions doesn't have always inline attribute
         * GCC tries to inline these functions the not the way is should be
         * and it makes the code bigger and even a bit slower.
         * I tested and as I see GCC omits the function call here in this iterator but then reintroduce
         * the call in an output iterator because it doesn't inline the output iterator anymore.
         * But the function call here is much better than inlining these big functions
         * but not inlining smaller functions such as output iterator.
         * Actually with -Winline GCC sometimes even reports that it cannot inline these functions
         * so always inline must be used for these functions anyway if we want to test with -Winline.
         * Tested it again. With -flto GCC actually does much better job because it can see
         * the whole picture. It creates only this function (even if it used in different .cpp files)
         * this is exacly how it should be. Without -flto it does not.
         * And if these functions have always inline it works properly in both cases.
         * I don't even know how to document it properly. It's pretty complicated stuff.
         * I leave the todo for now as a reminder and just remove it later probably.
         * Maybe document it like this:
         * Note that these low-level functions only have inline version because
         * they are a part of one algorithm and must be inlined together on a higher level.
         * And move this note to the low-level?
         */
        if (!detail::unstable_norm_iter_ready(&state))
            while (it_pos != it_end && !detail::unstable_norm_iter_nfc(&state, *it_pos++));
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
    explicit nfc(Iter begin, Sent end)
        : it_begin{begin}, it_end{end}, it_pos{begin}
    {
        detail::impl_norm_iter_state_reset(&state);

        iter_func_nfc(); // Fn call must not be inlined
    }
    explicit nfc(Iter end)
        : it_begin{end}, it_end{end}, it_pos{end}, stream_end{true} {}
    reference operator*() const noexcept { return codepoint; }
    nfc& operator++()
    {
        iter_func_nfc(); // Fn call must not be inlined

        return *this;
    }
    nfc operator++(int)
    {
        nfc tmp = *this;
        operator++();
        return tmp;
    }
    friend bool operator==(const nfc& x, const nfc& y) { return x.stream_end == y.stream_end; }
    friend bool operator!=(const nfc& x, const nfc& y) { return x.stream_end != y.stream_end; }
    template<class S> friend bool operator==(const nfc& x, const S&) { return x.stream_end; }
    template<class S> friend bool operator!=(const nfc& x, const S&) { return !x.stream_end; }
    template<class S> friend bool operator==(const S&, const nfc& x) { return x.stream_end; }
    template<class S> friend bool operator!=(const S&, const nfc& x) { return !x.stream_end; }
};

template<class Iter, class Sent = Iter>
class nfd
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;

    bool stream_end = false;

    detail::type_codept codepoint = 0;
    detail::impl_norm_iter_state state{};

    void iter_func_nfd()
    {
        if (!detail::unstable_norm_iter_ready(&state))
            while (it_pos != it_end && !detail::unstable_norm_iter_nfd(&state, *it_pos++));
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
    explicit nfd(Iter begin, Sent end)
        : it_begin{begin}, it_end{end}, it_pos{begin}
    {
        detail::impl_norm_iter_state_reset(&state);

        iter_func_nfd();
    }
    explicit nfd(Iter end)
        : it_begin{end}, it_end{end}, it_pos{end}, stream_end{true} {}
    reference operator*() const noexcept { return codepoint; }
    nfd& operator++()
    {
        iter_func_nfd();

        return *this;
    }
    nfd operator++(int)
    {
        nfd tmp = *this;
        operator++();
        return tmp;
    }
    friend bool operator==(const nfd& x, const nfd& y) { return (x.stream_end == y.stream_end); }
    friend bool operator!=(const nfd& x, const nfd& y) { return (x.stream_end != y.stream_end); }
    template<class S> friend bool operator==(const nfd& x, const S&) { return x.stream_end; }
    template<class S> friend bool operator!=(const nfd& x, const S&) { return !x.stream_end; }
    template<class S> friend bool operator==(const S&, const nfd& x) { return x.stream_end; }
    template<class S> friend bool operator!=(const S&, const nfd& x) { return !x.stream_end; }
};

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

template<class Iter, class Sent = Iter>
class nfkc
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;

    bool stream_end = false;

    detail::type_codept codepoint = 0;
    detail::impl_norm_iter_state state{};

    void iter_func_nfkc()
    {
        if (!detail::unstable_norm_iter_ready(&state))
            while (it_pos != it_end && !detail::unstable_norm_iter_nfkc(&state, *it_pos++));
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
    explicit nfkc(Iter begin, Sent end)
        : it_begin{begin}, it_end{end}, it_pos{begin}
    {
        detail::impl_norm_iter_state_reset(&state);

        iter_func_nfkc();
    }
    explicit nfkc(Iter end)
        : it_begin{end}, it_end{end}, it_pos{end}, stream_end{true} {}
    reference operator*() const noexcept { return codepoint; }
    nfkc& operator++()
    {
        iter_func_nfkc();

        return *this;
    }
    nfkc operator++(int)
    {
        nfkc tmp = *this;
        operator++();
        return tmp;
    }
    friend bool operator==(const nfkc& x, const nfkc& y) { return (x.stream_end == y.stream_end); }
    friend bool operator!=(const nfkc& x, const nfkc& y) { return (x.stream_end != y.stream_end); }
    template<class S> friend bool operator==(const nfkc& x, const S&) { return x.stream_end; }
    template<class S> friend bool operator!=(const nfkc& x, const S&) { return !x.stream_end; }
    template<class S> friend bool operator==(const S&, const nfkc& x) { return x.stream_end; }
    template<class S> friend bool operator!=(const S&, const nfkc& x) { return !x.stream_end; }
};

template<class Iter, class Sent = Iter>
class nfkd
{
private:
    Iter it_begin;
    Sent it_end;
    Iter it_pos;

    bool stream_end = false;

    detail::type_codept codepoint = 0;
    detail::impl_norm_iter_state state{};

    void iter_func_nfkd()
    {
        if (!detail::unstable_norm_iter_ready(&state))
            while (it_pos != it_end && !detail::unstable_norm_iter_nfkd(&state, *it_pos++));
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
    explicit nfkd(Iter begin, Sent end)
        : it_begin{begin}, it_end{end}, it_pos{begin}
    {
        detail::impl_norm_iter_state_reset(&state);

        iter_func_nfkd();
    }
    explicit nfkd(Iter end)
        : it_begin{end}, it_end{end}, it_pos{end}, stream_end{true} {}
    reference operator*() const noexcept { return codepoint; }
    nfkd& operator++()
    {
        iter_func_nfkd();

        return *this;
    }
    nfkd operator++(int)
    {
        nfkd tmp = *this;
        operator++();
        return tmp;
    }
    friend bool operator==(const nfkd& x, const nfkd& y) { return (x.stream_end == y.stream_end); }
    friend bool operator!=(const nfkd& x, const nfkd& y) { return (x.stream_end != y.stream_end); }
    template<class S> friend bool operator==(const nfkd& x, const S&) { return x.stream_end; }
    template<class S> friend bool operator!=(const nfkd& x, const S&) { return !x.stream_end; }
    template<class S> friend bool operator==(const S&, const nfkd& x) { return x.stream_end; }
    template<class S> friend bool operator!=(const S&, const nfkd& x) { return !x.stream_end; }
};

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace iter::norm

} // namespace uni

#endif // CPP_UNI_NORM_H_UAIX

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
