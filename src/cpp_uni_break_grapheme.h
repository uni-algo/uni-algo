/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_BREAK_GRAPHEME_H_UAIX
#define CPP_UNI_BREAK_GRAPHEME_H_UAIX

#ifdef UNI_ALGO_DISABLE_BREAK_GRAPHEME
#  error "Break Grapheme module is disabled via define UNI_ALGO_DISABLE_BREAK_GRAPHEME"
#endif

#include <type_traits>
#include <cassert>

#include "cpp_uni_config.h"
#include "cpp_uni_version.h"
#include "internal/cpp_uni_ranges_core.h"

#include "impl/impl_iterator.h"
#include "impl/impl_break_grapheme.h"

namespace uni {

namespace ranges::grapheme {

template<class Range>
class utf8_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class utf8
    {
        static_assert(detail::ranges::sa_iter_contiguous<Iter>::value &&
                      std::is_integral_v<detail::ranges::iter_value_t<Iter>>,
                      "grapheme::utf8 view requires contiguous UTF-8 range");

    private:
        utf8_view* parent = nullptr;
        Iter it_begin;
        Iter it_pos;
        Iter it_next;

        detail::impl_break_grapheme_state state{};

        uaiw_constexpr void iter_func_utf8_break_grapheme()
        {
            it_begin = it_pos;

            while (it_next != std::end(parent->range))
            {
                it_pos = it_next;
                uni::detail::type_codept codepoint = 0;
                it_next = detail::inline_utf8_iter(it_next, std::end(parent->range), &codepoint, detail::impl_iter_replacement);
                if (detail::inline_break_grapheme(&state, codepoint))
                    return;
            }

            if (it_next == std::end(parent->range))
                it_pos = it_next;
        }
        uaiw_constexpr void iter_func_utf8_break_grapheme_rev()
        {
            detail::impl_break_grapheme_state_reset(&state);
            it_pos = it_begin;

            while (it_begin != std::begin(parent->range))
            {
                it_next = it_begin;
                uni::detail::type_codept codepoint = 0;
                it_begin = detail::inline_utf8_iter_rev(std::begin(parent->range), it_begin, &codepoint, detail::impl_iter_replacement);
                if (detail::inline_utf8_break_grapheme_rev(&state, codepoint, std::begin(parent->range), it_begin))
                {
                    it_begin = it_next;
                    break;
                }
            }

            it_next = it_pos;
            detail::impl_break_grapheme_state_reset(&state);
        }

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::basic_string_view<detail::ranges::iter_value_t<Iter>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        utf8() = default;
        explicit utf8(utf8_view& p, Iter begin, Sent end)
            : parent{std::addressof(p)}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_break_grapheme_state_reset(&state);

            iter_func_utf8_break_grapheme();
        }
        uaiw_constexpr reference operator*() const
        {
            return detail::ranges::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        uaiw_constexpr utf8& operator++()
        {
            iter_func_utf8_break_grapheme();

            return *this;
        }
        uaiw_constexpr utf8 operator++(int)
        {
            utf8 tmp = *this;
            operator++();
            return tmp;
        }
        uaiw_constexpr utf8& operator--()
        {
            iter_func_utf8_break_grapheme_rev();

            return *this;
        }
        uaiw_constexpr utf8 operator--(int)
        {
            utf8 tmp = *this;
            operator--();
            return tmp;
        }
        friend bool operator==(const utf8& x, const utf8& y) { return (x.it_begin == y.it_begin); }
        friend bool operator!=(const utf8& x, const utf8& y) { return (x.it_begin != y.it_begin); }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const utf8& x) { return x.it_begin == std::end(x.parent->range); }
    public:
        friend uaiw_constexpr bool operator==(const utf8& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(const utf8& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const utf8& x) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const utf8& x) { return !friend_compare_sentinel(x); }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    utf8<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr utf8_view() = default;
    uaiw_constexpr explicit utf8_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = utf8<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        return utf8<iter_t, sent_t>{*this, std::end(range), std::end(range)};
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
class utf16_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class utf16
    {
        static_assert(detail::ranges::sa_iter_contiguous<Iter>::value &&
                      std::is_integral_v<detail::ranges::iter_value_t<Iter>> &&
                      sizeof(detail::ranges::iter_value_t<Iter>) >= sizeof(char16_t),
                      "grapheme::utf16 view requires contiguous UTF-16 range");

    private:
        utf16_view* parent = nullptr;
        Iter it_begin;
        Iter it_pos;
        Iter it_next;

        detail::impl_break_grapheme_state state{};

        uaiw_constexpr void iter_func_utf16_break_grapheme()
        {
            it_begin = it_pos;

            while (it_next != std::end(parent->range))
            {
                it_pos = it_next;
                uni::detail::type_codept codepoint = 0;
                it_next = detail::inline_utf16_iter(it_next, std::end(parent->range), &codepoint, detail::impl_iter_replacement);
                if (detail::inline_break_grapheme(&state, codepoint))
                    return;
            }

            if (it_next == std::end(parent->range))
                it_pos = it_next;
        }
        uaiw_constexpr void iter_func_utf16_break_grapheme_rev()
        {
            detail::impl_break_grapheme_state_reset(&state);
            it_pos = it_begin;

            while (it_begin != std::begin(parent->range))
            {
                it_next = it_begin;
                uni::detail::type_codept codepoint = 0;
                it_begin = detail::inline_utf16_iter_rev(std::begin(parent->range), it_begin, &codepoint, detail::impl_iter_replacement);
                if (detail::inline_utf16_break_grapheme_rev(&state, codepoint, std::begin(parent->range), it_begin))
                {
                    it_begin = it_next;
                    break;
                }
            }

            it_next = it_pos;
            detail::impl_break_grapheme_state_reset(&state);
        }

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::basic_string_view<detail::ranges::iter_value_t<Iter>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        utf16() = default;
        explicit utf16(utf16_view& p, Iter begin, Sent end)
            : parent{std::addressof(p)}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_break_grapheme_state_reset(&state);

            iter_func_utf16_break_grapheme();
        }
        uaiw_constexpr reference operator*() const
        {
            return detail::ranges::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        uaiw_constexpr utf16& operator++()
        {
            iter_func_utf16_break_grapheme();

            return *this;
        }
        uaiw_constexpr utf16 operator++(int)
        {
            utf16 tmp = *this;
            operator++();
            return tmp;
        }
        uaiw_constexpr utf16& operator--()
        {
            iter_func_utf16_break_grapheme_rev();

            return *this;
        }
        uaiw_constexpr utf16 operator--(int)
        {
            utf16 tmp = *this;
            operator--();
            return tmp;
        }
        friend bool operator==(const utf16& x, const utf16& y) { return (x.it_begin == y.it_begin); }
        friend bool operator!=(const utf16& x, const utf16& y) { return (x.it_begin != y.it_begin); }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const utf16& x) { return x.it_begin == std::end(x.parent->range); }
    public:
        friend uaiw_constexpr bool operator==(const utf16& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(const utf16& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const utf16& x) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const utf16& x) { return !friend_compare_sentinel(x); }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    utf16<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr utf16_view() = default;
    uaiw_constexpr explicit utf16_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = utf16<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        return utf16<iter_t, sent_t>{*this, std::end(range), std::end(range)};
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
utf8_view(Range&&) -> utf8_view<uni::views::all_t<Range>>;
template<class Range>
utf16_view(Range&&) -> utf16_view<uni::views::all_t<Range>>;

} // namespace ranges::grapheme

namespace detail {

/* GRAPHEME_UTF8_VIEW */

struct adaptor_grapheme_utf8
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::grapheme::utf8_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_grapheme_utf8& a) { return a(std::forward<R>(r)); }

/* GRAPHEME_UTF16_VIEW */

struct adaptor_grapheme_utf16
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::grapheme::utf16_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_grapheme_utf16& a) { return a(std::forward<R>(r)); }

} // namespace detail

namespace ranges::views::grapheme {

inline constexpr detail::adaptor_grapheme_utf8 utf8;
inline constexpr detail::adaptor_grapheme_utf16 utf16;

}

namespace views = ranges::views;

} // namespace uni

#endif // CPP_UNI_BREAK_GRAPHEME_H_UAIX

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
