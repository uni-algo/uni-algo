/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_RANGES_CONV_H_UAIH
#define UNI_ALGO_RANGES_CONV_H_UAIH

#ifdef UNI_ALGO_LOG_CPP_ITER
#include <iostream>
#endif

#include "config.h"
#include "internal/ranges_core.h"

#include "impl/impl_iter.h"

namespace una {

namespace ranges {

template<class Range, char32_t Error = detail::impl_iter_replacement>
class utf8_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class utf8
    {
        static_assert(std::is_integral_v<detail::rng::iter_value_t<Iter>>,
                      "utf8 view requires integral UTF-8 range");

        // Error is only used for tests, do not document it
        static_assert(Error == detail::impl_iter_error || Error == detail::impl_iter_replacement);

    private:
        utf8_view* parent = nullptr;
        Iter it_pos = Iter{};
        Iter it_next = Iter{};
        detail::type_codept codepoint = 0;

        using iter_tag = detail::rng::iter_tag<Iter>;

        using is_bidirectional_or_better = std::is_convertible<iter_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<iter_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = detail::rng::iter_difference_t<Iter>;

        uaiw_constexpr utf8() = default;
        uaiw_constexpr explicit utf8(utf8_view& p, Iter begin, Sent end)
            : parent{std::addressof(p)}, it_pos{begin}, it_next{begin}
        {
            if (begin != end)
                it_next = detail::impl_iter_utf8(it_next, end, &codepoint, Error);
        }
        //uaiw_constexpr const Iter& base() const & noexcept { return it_pos; }
        //uaiw_constexpr Iter base() && { return std::move(it_pos); }
        uaiw_constexpr reference operator*() const noexcept { return codepoint; }
        uaiw_constexpr Iter begin() const noexcept { return it_pos; }
        uaiw_constexpr Iter end() const noexcept { return it_next; }
        uaiw_constexpr utf8& operator++()
        {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "una::views::utf8::operator++() " << this << '\n';
#endif
            it_pos = it_next;
            if (it_pos == std::end(parent->range))
                return *this;
            it_next = detail::impl_iter_utf8(it_next, std::end(parent->range), &codepoint, Error);
            return *this;
        }
        uaiw_constexpr utf8 operator++(int)
        {
            utf8 tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = utf8&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--()
        {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "una::views::utf8::operator--() " << this << '\n';
#endif
            it_next = it_pos;
            if (it_pos == std::begin(parent->range))
                return *this;
            it_pos = detail::impl_iter_rev_utf8(std::begin(parent->range), it_pos, &codepoint, Error);
            return *this;
        }
        template<class T = utf8> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--(int)
        {
            utf8 tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const utf8& x, const utf8& y) { return x.it_pos == y.it_pos; }
        friend uaiw_constexpr bool operator!=(const utf8& x, const utf8& y) { return x.it_pos != y.it_pos; }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const utf8& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend uaiw_constexpr bool operator==(const utf8& x, una::sentinel_t) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(const utf8& x, una::sentinel_t) { return !friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator==(una::sentinel_t, const utf8& x) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(una::sentinel_t, const utf8& x) { return !friend_compare_sentinel(x); }
    };

    using iter_t = detail::rng::iterator_t<Range>;
    using sent_t = detail::rng::sentinel_t<Range>;

    Range range = Range{};
    detail::rng::cache<utf8<iter_t, sent_t>> cached_begin;

public:
    uaiw_constexpr utf8_view() = default;
    uaiw_constexpr explicit utf8_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin.has_value())
            return cached_begin.get_value();

        cached_begin.set_value(utf8<iter_t, sent_t>{*this, std::begin(range), std::end(range)});

        return cached_begin.get_value();
    }
    uaiw_constexpr auto end()
    {
        if constexpr (std::is_same_v<iter_t, sent_t>) // std::ranges::common_range<Range>
            return utf8<iter_t, sent_t>{*this, std::end(range), std::end(range)};
        else
            return una::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range, char32_t Error = detail::impl_iter_replacement>
class utf16_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class utf16
    {
        static_assert(std::is_integral_v<detail::rng::iter_value_t<Iter>> &&
                      sizeof(detail::rng::iter_value_t<Iter>) >= sizeof(char16_t),
                      "utf16 view requires integral UTF-16 range");

        // Error is only used for tests, do not document it
        static_assert(Error == detail::impl_iter_error || Error == detail::impl_iter_replacement);

    private:
        utf16_view* parent = nullptr;
        Iter it_pos = Iter{};
        Iter it_next = Iter{};
        detail::type_codept codepoint = 0;

        using iter_tag = detail::rng::iter_tag<Iter>;

        using is_bidirectional_or_better = std::is_convertible<iter_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<iter_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = detail::rng::iter_difference_t<Iter>;

        uaiw_constexpr utf16() = default;
        uaiw_constexpr explicit utf16(utf16_view& p, Iter begin, Sent end)
            : parent{std::addressof(p)}, it_pos{begin}, it_next{begin}
        {
            if (begin != end)
                it_next = detail::impl_iter_utf16(it_next, end, &codepoint, Error);
        }
        //uaiw_constexpr const Iter& base() const & noexcept { return it_pos; }
        //uaiw_constexpr Iter base() && { return std::move(it_pos); }
        uaiw_constexpr reference operator*() const noexcept { return codepoint; }
        uaiw_constexpr Iter begin() const noexcept { return it_pos; }
        uaiw_constexpr Iter end() const noexcept { return it_next; }
        uaiw_constexpr utf16& operator++()
        {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "una::views::utf16::operator++() " << this << '\n';
#endif
            it_pos = it_next;
            if (it_pos == std::end(parent->range))
                return *this;
            it_next = detail::impl_iter_utf16(it_next, std::end(parent->range), &codepoint, Error);
            return *this;
        }
        uaiw_constexpr utf16 operator++(int)
        {
            utf16 tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = utf16&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--()
        {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "una::views::utf16::operator--() " << this << '\n';
#endif
            it_next = it_pos;
            if (it_pos == std::begin(parent->range))
                return *this;
            it_pos = detail::impl_iter_rev_utf16(std::begin(parent->range), it_pos, &codepoint, Error);
            return *this;
        }
        template<class T = utf16> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--(int)
        {
            utf16 tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const utf16& x, const utf16& y) { return x.it_pos == y.it_pos; }
        friend uaiw_constexpr bool operator!=(const utf16& x, const utf16& y) { return x.it_pos != y.it_pos; }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const utf16& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend uaiw_constexpr bool operator==(const utf16& x, una::sentinel_t) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(const utf16& x, una::sentinel_t) { return !friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator==(una::sentinel_t, const utf16& x) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(una::sentinel_t, const utf16& x) { return !friend_compare_sentinel(x); }
    };

    using iter_t = detail::rng::iterator_t<Range>;
    using sent_t = detail::rng::sentinel_t<Range>;

    Range range = Range{};
    detail::rng::cache<utf16<iter_t, sent_t>> cached_begin;

public:
    uaiw_constexpr utf16_view() = default;
    uaiw_constexpr explicit utf16_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin.has_value())
            return cached_begin.get_value();

        cached_begin.set_value(utf16<iter_t, sent_t>{*this, std::begin(range), std::end(range)});

        return cached_begin.get_value();
    }
    uaiw_constexpr auto end()
    {
        if constexpr (std::is_same_v<iter_t, sent_t>) // std::ranges::common_range<Range>
            return utf16<iter_t, sent_t>{*this, std::end(range), std::end(range)};
        else
            return una::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

} // namespace ranges

namespace detail::rng {

/* UTF8_VIEW */

struct adaptor_utf8
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    { return ranges::utf8_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_utf8& a) { return a(std::forward<R>(r)); }

/* UTF16_VIEW */

struct adaptor_utf16
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    { return ranges::utf16_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_utf16& a) { return a(std::forward<R>(r)); }

/* TO_UTF8 */

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1206r7.pdf

template<class Result, class Alloc = std::allocator<detail::rng::range_value_t<Result>>>
struct adaptor_closure_to_utf8
{
    Alloc alloc;
    uaiw_constexpr explicit adaptor_closure_to_utf8(const Alloc& a = Alloc()): alloc{a} {}

    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        using range_v = detail::rng::range_value_t<R>;
        using result_v = detail::rng::range_value_t<Result>;

        // Technically we want this static_assert for range_v:
        // static_assert(std::is_same_v<range_v, char32_t>);
        // but it makes it a bit clanky to use with transform view so use more permissive static_assert
        // See: test/test_ranges.h -> test_ranges()
        static_assert(std::is_integral_v<range_v> && sizeof(range_v) >= sizeof(char32_t),
                      "to_utf8 range requires char32_t range");
        static_assert(std::is_integral_v<result_v>,
                      "to_utf8 result type cannot store UTF-8");

        Result result{alloc};
        std::back_insert_iterator output{result};
        for (auto c : r)
            detail::impl_output_utf8(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result, class Alloc>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf8<Result, Alloc>& a) { return a(std::forward<R>(r)); }

template<class Result>
struct adaptor_to_utf8
{
    uaiw_constexpr auto operator()() const
    { return adaptor_closure_to_utf8<Result>{}; }
    template<class R, class = std::enable_if_t<detail::rng::has_member_begin<R>::value>>
    uaiw_constexpr auto operator()(R&& r) const
    { return adaptor_closure_to_utf8<Result>{}(std::forward<R>(r)); }
    template<class Alloc, class = std::enable_if_t<detail::rng::has_member_allocate<Alloc>::value>>
    uaiw_constexpr auto operator()(const Alloc& a) const
    { return adaptor_closure_to_utf8<Result, Alloc>{a}; }
    template<class R, class Alloc>
    uaiw_constexpr auto operator()(R&& r, const Alloc& a) const
    { return adaptor_closure_to_utf8<Result, Alloc>{a}(std::forward<R>(r)); }
};

/* TO_UTF16 */

template<class Result, class Alloc = std::allocator<detail::rng::range_value_t<Result>>>
struct adaptor_closure_to_utf16
{
    Alloc alloc;
    uaiw_constexpr explicit adaptor_closure_to_utf16(const Alloc& a = Alloc()): alloc{a} {}

    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        using range_v = detail::rng::range_value_t<R>;
        using result_v = detail::rng::range_value_t<Result>;

        // See comments in to_utf8 adaptor above
        // static_assert(std::is_same_v<range_v, char32_t>);
        static_assert(std::is_integral_v<range_v> && sizeof(range_v) >= sizeof(char32_t),
                      "to_utf16 range requires char32_t range");
        static_assert(std::is_integral_v<result_v> && sizeof(result_v) >= sizeof(char16_t),
                      "to_utf16 result type cannot store UTF-16");

        Result result{alloc};
        std::back_insert_iterator output{result};
        for (auto c : r)
            detail::impl_output_utf16(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result, class Alloc>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf16<Result, Alloc>& a) { return a(std::forward<R>(r)); }

template<class Result>
struct adaptor_to_utf16
{
    uaiw_constexpr auto operator()() const
    { return adaptor_closure_to_utf16<Result>{}; }
    template<class R, class = std::enable_if_t<detail::rng::has_member_begin<R>::value>>
    uaiw_constexpr auto operator()(R&& r) const
    { return adaptor_closure_to_utf16<Result>{}(std::forward<R>(r)); }
    template<class Alloc, class = std::enable_if_t<detail::rng::has_member_allocate<Alloc>::value>>
    uaiw_constexpr auto operator()(const Alloc& a) const
    { return adaptor_closure_to_utf16<Result, Alloc>{a}; }
    template<class R, class Alloc>
    uaiw_constexpr auto operator()(R&& r, const Alloc& a) const
    { return adaptor_closure_to_utf16<Result, Alloc>{a}(std::forward<R>(r)); }
};

/* TO_UTF8_RESERVE */

template<class Result, class Alloc = std::allocator<detail::rng::range_value_t<Result>>>
struct adaptor_closure_to_utf8_reserve
{
    std::size_t size = 0;
    Alloc alloc;
    uaiw_constexpr explicit adaptor_closure_to_utf8_reserve(std::size_t n, const Alloc& a = Alloc()): size{n}, alloc{a} {}

    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        using range_v = detail::rng::range_value_t<R>;
        using result_v = detail::rng::range_value_t<Result>;

        static_assert(std::is_integral_v<range_v> && sizeof(range_v) >= sizeof(char32_t),
                      "to_utf8_reserve range requires char32_t range");
        static_assert(std::is_integral_v<result_v>,
                      "to_utf8_reserve result type cannot store UTF-8");

        Result result{alloc};
        result.reserve(size);
        const std::back_insert_iterator output{result};
        for (auto c : r)
            detail::impl_output_utf8(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result, class Alloc>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf8_reserve<Result, Alloc>& a) { return a(std::forward<R>(r)); }

template<class Result>
struct adaptor_to_utf8_reserve
{
    uaiw_constexpr auto operator()(std::size_t n) const
    { return adaptor_closure_to_utf8_reserve<Result>{n}; }
    template<class R>
    uaiw_constexpr auto operator()(R&& r, std::size_t n) const
    { return adaptor_closure_to_utf8_reserve<Result>{n}(std::forward<R>(r)); }
    template<class Alloc>
    uaiw_constexpr auto operator()(std::size_t n, const Alloc& a) const
    { return adaptor_closure_to_utf8_reserve<Result, Alloc>{n, a}; }
    template<class R, class Alloc>
    uaiw_constexpr auto operator()(R&& r, std::size_t n, const Alloc& a) const
    { return adaptor_closure_to_utf8_reserve<Result, Alloc>{n, a}(std::forward<R>(r)); }
};

/* TO_UTF16_RESERVE */

template<class Result, class Alloc = std::allocator<detail::rng::range_value_t<Result>>>
struct adaptor_closure_to_utf16_reserve
{
    std::size_t size = 0;
    Alloc alloc;
    uaiw_constexpr explicit adaptor_closure_to_utf16_reserve(std::size_t n, const Alloc& a = Alloc()): size{n}, alloc{a} {}

    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        using range_v = detail::rng::range_value_t<R>;
        using result_v = detail::rng::range_value_t<Result>;

        static_assert(std::is_integral_v<range_v> && sizeof(range_v) >= sizeof(char32_t),
                      "to_utf16_reserve range requires char32_t range");
        static_assert(std::is_integral_v<result_v> && sizeof(result_v) >= sizeof(char16_t),
                      "to_utf16_reserve result type cannot store UTF-16");

        Result result{alloc};
        result.reserve(size);
        const std::back_insert_iterator output{result};
        for (auto c : r)
            detail::impl_output_utf16(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result, class Alloc>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf16_reserve<Result, Alloc>& a) { return a(std::forward<R>(r)); }

template<class Result>
struct adaptor_to_utf16_reserve
{
    uaiw_constexpr auto operator()(std::size_t n) const
    { return adaptor_closure_to_utf16_reserve<Result>{n}; }
    template<class R>
    uaiw_constexpr auto operator()(R&& r, std::size_t n) const
    { return adaptor_closure_to_utf16_reserve<Result>{n}(std::forward<R>(r)); }
    template<class Alloc>
    uaiw_constexpr auto operator()(std::size_t n, const Alloc& a) const
    { return adaptor_closure_to_utf16_reserve<Result, Alloc>{n, a}; }
    template<class R, class Alloc>
    uaiw_constexpr auto operator()(R&& r, std::size_t n, const Alloc& a) const
    { return adaptor_closure_to_utf16_reserve<Result, Alloc>{n, a}(std::forward<R>(r)); }
};

} // namespace detail::rng

namespace ranges::views {

inline constexpr detail::rng::adaptor_utf8 utf8;
inline constexpr detail::rng::adaptor_utf16 utf16;

} // namespace views

namespace ranges {

template<class Range>
utf8_view(Range&&) -> utf8_view<views::all_t<Range>>;

template<class Range>
utf16_view(Range&&) -> utf16_view<views::all_t<Range>>;

} // namespace ranges

namespace ranges {

// NOTE:
// If {} below is not used MSVC 19.16 and probably other versions produces this:
// error C2737: 'una::ranges::to_utf8': 'constexpr' object must be initialized
// It should not interfere with anything.

template<class Result>
inline constexpr detail::rng::adaptor_to_utf8<Result> to_utf8{};
template<class Result>
inline constexpr detail::rng::adaptor_to_utf16<Result> to_utf16{};
template<class Result>
inline constexpr detail::rng::adaptor_to_utf8_reserve<Result> to_utf8_reserve{};
template<class Result>
inline constexpr detail::rng::adaptor_to_utf16_reserve<Result> to_utf16_reserve{};

} // namespace ranges

namespace views = ranges::views;

} // namespace una

#endif // UNI_ALGO_RANGES_CONV_H_UAIH

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
