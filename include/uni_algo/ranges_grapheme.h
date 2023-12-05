/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_RANGES_GRAPHEME_H_UAIH
#define UNI_ALGO_RANGES_GRAPHEME_H_UAIH

#ifdef UNI_ALGO_DISABLE_SEGMENT_GRAPHEME
#error "Grapheme segmentation module is disabled via define UNI_ALGO_DISABLE_SEGMENT_GRAPHEME"
#endif

#include <string_view>
#include <cassert>

#include "config.h"
#include "internal/safe_layer.h"
#include "internal/ranges_core.h"

#include "impl/impl_iter.h"
#include "impl/impl_segment_grapheme.h"

namespace una {

namespace ranges::grapheme {

template<class Range>
class utf8_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class utf8
    {
        static_assert(detail::rng::is_iter_bidi_or_better<Iter>::value &&
                      std::is_integral_v<detail::rng::iter_value_t<Iter>>,
                      "grapheme::utf8 view requires bidirectional or better UTF-8 range");

    private:
        utf8_view* parent = nullptr;
        Iter it_begin = Iter{};
        Iter it_pos = Iter{};
        Iter it_next = Iter{};

        detail::impl_segment_grapheme_state state{};

        uaiw_constexpr void iter_func_segment_grapheme_utf8()
        {
            it_begin = it_pos;

            while (it_next != std::end(parent->range))
            {
                it_pos = it_next;
                detail::type_codept codepoint = 0;
                it_next = detail::inline_iter_utf8(it_next, std::end(parent->range), &codepoint, detail::impl_iter_replacement);
                if (detail::inline_segment_grapheme(&state, codepoint))
                    return;
            }

            if (it_next == std::end(parent->range))
                it_pos = it_next;
        }
        uaiw_constexpr void iter_func_segment_grapheme_rev_utf8()
        {
            detail::impl_segment_grapheme_state_reset(&state);
            it_pos = it_begin;

            while (it_begin != std::begin(parent->range))
            {
                it_next = it_begin;
                detail::type_codept codepoint = 0;
                it_begin = detail::inline_iter_rev_utf8(std::begin(parent->range), it_begin, &codepoint, detail::impl_iter_replacement);
                if (detail::inline_segment_grapheme_rev_utf8(&state, codepoint, std::begin(parent->range), it_begin))
                {
                    it_begin = it_next;
                    break;
                }
            }

            it_next = it_pos;
            detail::impl_segment_grapheme_state_reset(&state);
        }
        uaiw_constexpr void iter_func_segment_grapheme_pos_utf8(Iter pos)
        {
            // Find UTF-8 boundary
            for (std::size_t i = 0; i < 3 && pos != std::end(parent->range); ++i, ++pos)
            {
                if (((*pos & 0xFF) & 0xC0) != 0x80)
                    break;
            }

            // NOTE: https://unicode.org/reports/tr29/#Random_Access

            it_begin = pos;

            iter_func_segment_grapheme_rev_utf8();

            it_pos = it_begin;
            it_next = it_begin;

            iter_func_segment_grapheme_utf8();
        }

        using is_contiguous = detail::rng::is_range_contiguous<Range>;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::conditional_t<is_contiguous::value,
            std::basic_string_view<detail::rng::iter_value_t<Iter>>, void>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::rng::iter_difference_t<Iter>;

        uaiw_constexpr utf8() = default;
        uaiw_constexpr explicit utf8(utf8_view& p, Iter begin, Sent end)
            : parent{std::addressof(p)}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_segment_grapheme_state_reset(&state);

            iter_func_segment_grapheme_utf8();
        }
        uaiw_constexpr explicit utf8(utf8_view& p, Iter begin, Sent, Iter pos)
            : parent{std::addressof(p)}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            iter_func_segment_grapheme_pos_utf8(pos);
        }
        template<class T = reference> typename std::enable_if_t<is_contiguous::value, T>
        uaiw_constexpr operator*() const
        {
            return detail::rng::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        uaiw_constexpr utf8& operator++()
        {
            iter_func_segment_grapheme_utf8();

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
            iter_func_segment_grapheme_rev_utf8();

            return *this;
        }
        uaiw_constexpr utf8 operator--(int)
        {
            utf8 tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const utf8& x, const utf8& y) { return (x.it_begin == y.it_begin); }
        friend uaiw_constexpr bool operator!=(const utf8& x, const utf8& y) { return (x.it_begin != y.it_begin); }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const utf8& x) { return x.it_begin == std::end(x.parent->range); }
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
        return utf8<iter_t, sent_t>{*this, std::end(range), std::end(range)};
    }
    uaiw_constexpr auto cursor(iter_t pos)
    {
        return utf8<iter_t, sent_t>{*this, std::begin(range), std::end(range), pos};
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
class utf16_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class utf16
    {
        static_assert(detail::rng::is_iter_bidi_or_better<Iter>::value &&
                      std::is_integral_v<detail::rng::iter_value_t<Iter>> &&
                      sizeof(detail::rng::iter_value_t<Iter>) >= sizeof(char16_t),
                      "grapheme::utf16 view requires bidirectional or better UTF-16 range");

    private:
        utf16_view* parent = nullptr;
        Iter it_begin = Iter{};
        Iter it_pos = Iter{};
        Iter it_next = Iter{};

        detail::impl_segment_grapheme_state state{};

        uaiw_constexpr void iter_func_segment_grapheme_utf16()
        {
            it_begin = it_pos;

            while (it_next != std::end(parent->range))
            {
                it_pos = it_next;
                detail::type_codept codepoint = 0;
                it_next = detail::inline_iter_utf16(it_next, std::end(parent->range), &codepoint, detail::impl_iter_replacement);
                if (detail::inline_segment_grapheme(&state, codepoint))
                    return;
            }

            if (it_next == std::end(parent->range))
                it_pos = it_next;
        }
        uaiw_constexpr void iter_func_segment_grapheme_rev_utf16()
        {
            detail::impl_segment_grapheme_state_reset(&state);
            it_pos = it_begin;

            while (it_begin != std::begin(parent->range))
            {
                it_next = it_begin;
                detail::type_codept codepoint = 0;
                it_begin = detail::inline_iter_rev_utf16(std::begin(parent->range), it_begin, &codepoint, detail::impl_iter_replacement);
                if (detail::inline_segment_grapheme_rev_utf16(&state, codepoint, std::begin(parent->range), it_begin))
                {
                    it_begin = it_next;
                    break;
                }
            }

            it_next = it_pos;
            detail::impl_segment_grapheme_state_reset(&state);
        }
        uaiw_constexpr void iter_func_segment_grapheme_pos_utf16(Iter pos)
        {
            // Find UTF-16 boundary
            for (std::size_t i = 0; i < 1 && pos != std::end(parent->range); ++i, ++pos)
            {
                if (!((*pos & 0xFFFF) >= 0xDC00 && (*pos & 0xFFFF) <= 0xDFFF))
                    break;
            }

            // NOTE: https://unicode.org/reports/tr29/#Random_Access

            it_begin = pos;

            iter_func_segment_grapheme_rev_utf16();

            it_pos = it_begin;
            it_next = it_begin;

            iter_func_segment_grapheme_utf16();
        }

        using is_contiguous = detail::rng::is_range_contiguous<Range>;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::conditional_t<is_contiguous::value,
            std::basic_string_view<detail::rng::iter_value_t<Iter>>, void>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::rng::iter_difference_t<Iter>;

        uaiw_constexpr utf16() = default;
        uaiw_constexpr explicit utf16(utf16_view& p, Iter begin, Sent end)
            : parent{std::addressof(p)}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_segment_grapheme_state_reset(&state);

            iter_func_segment_grapheme_utf16();
        }
        uaiw_constexpr explicit utf16(utf16_view& p, Iter begin, Sent, Iter pos)
            : parent{std::addressof(p)}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            iter_func_segment_grapheme_pos_utf16(pos);
        }
        template<class T = reference> typename std::enable_if_t<is_contiguous::value, T>
        uaiw_constexpr operator*() const
        {
            return detail::rng::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        uaiw_constexpr utf16& operator++()
        {
            iter_func_segment_grapheme_utf16();

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
            iter_func_segment_grapheme_rev_utf16();

            return *this;
        }
        uaiw_constexpr utf16 operator--(int)
        {
            utf16 tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const utf16& x, const utf16& y) { return (x.it_begin == y.it_begin); }
        friend uaiw_constexpr bool operator!=(const utf16& x, const utf16& y) { return (x.it_begin != y.it_begin); }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const utf16& x) { return x.it_begin == std::end(x.parent->range); }
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
        return utf16<iter_t, sent_t>{*this, std::end(range), std::end(range)};
    }
    uaiw_constexpr auto cursor(iter_t pos)
    {
        return utf16<iter_t, sent_t>{*this, std::begin(range), std::end(range), pos};
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
utf8_view(Range&&) -> utf8_view<views::all_t<Range>>;
template<class Range>
utf16_view(Range&&) -> utf16_view<views::all_t<Range>>;

} // namespace ranges::grapheme

namespace detail::rng {

/* GRAPHEME_UTF8_VIEW */

struct adaptor_grapheme_utf8
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    { return ranges::grapheme::utf8_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_grapheme_utf8& a) { return a(std::forward<R>(r)); }

/* GRAPHEME_UTF16_VIEW */

struct adaptor_grapheme_utf16
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    { return ranges::grapheme::utf16_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_grapheme_utf16& a) { return a(std::forward<R>(r)); }

} // namespace detail::rng

namespace ranges::views::grapheme {

inline constexpr detail::rng::adaptor_grapheme_utf8 utf8;
inline constexpr detail::rng::adaptor_grapheme_utf16 utf16;

}

namespace views = ranges::views;

} // namespace una

#endif // UNI_ALGO_RANGES_GRAPHEME_H_UAIH

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
