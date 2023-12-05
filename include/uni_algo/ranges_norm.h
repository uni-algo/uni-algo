/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_RANGES_NORM_H_UAIH
#define UNI_ALGO_RANGES_NORM_H_UAIH

#include "config.h"
#include "internal/safe_layer.h"
#include "internal/ranges_core.h"

#include "impl/impl_norm.h"

#ifdef UNI_ALGO_DISABLE_NORM
#error "Normalization module is disabled via define UNI_ALGO_DISABLE_NORM"
#endif

namespace una {

namespace ranges::norm {

template<class Range>
class nfc_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class nfc
    {
        static_assert(std::is_integral_v<detail::rng::iter_value_t<Iter>> &&
                      sizeof(detail::rng::iter_value_t<Iter>) >= sizeof(char32_t),
                      "norm::nfc view requires char32_t range");

    private:
        nfc_view* parent = nullptr;
        Iter it_pos = Iter{};

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        uaiw_constexpr void iter_func_norm_nfc()
        {
            if (!detail::inline_norm_iter_ready(&state))
            {
                for (bool stop = false; !stop && it_pos != std::end(parent->range); ++it_pos)
                    stop = detail::inline_norm_iter_nfc(&state, *it_pos);
            }
            if (!detail::inline_norm_iter_next_comp(&state, &codepoint))
                stream_end = true;
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = std::ptrdiff_t;

        uaiw_constexpr nfc() = default;
        uaiw_constexpr explicit nfc(nfc_view& p, Iter begin, Sent) : parent{std::addressof(p)}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_norm_nfc(); // Fn call must not be inlined
        }
        uaiw_constexpr reference operator*() const noexcept { return codepoint; }
        uaiw_constexpr nfc& operator++()
        {
            iter_func_norm_nfc(); // Fn call must not be inlined

            return *this;
        }
        uaiw_constexpr nfc operator++(int)
        {
            nfc tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const nfc& x, const nfc& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfc& x, const nfc& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const nfc& x, una::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfc& x, una::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(una::sentinel_t, const nfc& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(una::sentinel_t, const nfc& x) { return !x.stream_end; }
    };

    using iter_t = detail::rng::iterator_t<Range>;
    using sent_t = detail::rng::sentinel_t<Range>;

    Range range = Range{};
    detail::rng::cache<nfc<iter_t, sent_t>> cached_begin;

public:
    uaiw_constexpr nfc_view() = default;
    uaiw_constexpr explicit nfc_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin.has_value())
            return cached_begin.get_value();

        cached_begin.set_value(nfc<iter_t, sent_t>{*this, std::begin(range), std::end(range)});

        return cached_begin.get_value();
    }
    uaiw_constexpr auto end()
    {
        return una::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
class nfd_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class nfd
    {
        static_assert(std::is_integral_v<detail::rng::iter_value_t<Iter>> &&
                      sizeof(detail::rng::iter_value_t<Iter>) >= sizeof(char32_t),
                      "norm::nfd view requires char32_t range");

    private:
        nfd_view* parent = nullptr;
        Iter it_pos = Iter{};

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        uaiw_constexpr void iter_func_norm_nfd()
        {
            if (!detail::inline_norm_iter_ready(&state))
            {
                for (bool stop = false; !stop && it_pos != std::end(parent->range); ++it_pos)
                    stop = detail::inline_norm_iter_nfd(&state, *it_pos);
            }
            if (!detail::inline_norm_iter_next_decomp(&state, &codepoint))
                stream_end = true;
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = std::ptrdiff_t;

        uaiw_constexpr nfd() = default;
        uaiw_constexpr explicit nfd(nfd_view& p, Iter begin, Sent) : parent{std::addressof(p)}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_norm_nfd(); // Fn call must not be inlined
        }
        uaiw_constexpr reference operator*() const noexcept { return codepoint; }
        uaiw_constexpr nfd& operator++()
        {
            iter_func_norm_nfd(); // Fn call must not be inlined

            return *this;
        }
        uaiw_constexpr nfd operator++(int)
        {
            nfd tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const nfd& x, const nfd& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfd& x, const nfd& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const nfd& x, una::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfd& x, una::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(una::sentinel_t, const nfd& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(una::sentinel_t, const nfd& x) { return !x.stream_end; }
    };

    using iter_t = detail::rng::iterator_t<Range>;
    using sent_t = detail::rng::sentinel_t<Range>;

    Range range = Range{};
    detail::rng::cache<nfd<iter_t, sent_t>> cached_begin;

public:
    uaiw_constexpr nfd_view() = default;
    uaiw_constexpr explicit nfd_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin.has_value())
            return cached_begin.get_value();

        cached_begin.set_value(nfd<iter_t, sent_t>{*this, std::begin(range), std::end(range)});

        return cached_begin.get_value();
    }
    uaiw_constexpr auto end()
    {
        return una::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

template<class Range>
class nfkc_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class nfkc
    {
        static_assert(std::is_integral_v<detail::rng::iter_value_t<Iter>> &&
                      sizeof(detail::rng::iter_value_t<Iter>) >= sizeof(char32_t),
                      "norm::nfkc view requires char32_t range");

    private:
        nfkc_view* parent = nullptr;
        Iter it_pos = Iter{};

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        uaiw_constexpr void iter_func_norm_nfkc()
        {
            if (!detail::inline_norm_iter_ready(&state))
            {
                for (bool stop = false; !stop && it_pos != std::end(parent->range); ++it_pos)
                    stop = detail::inline_norm_iter_nfkc(&state, *it_pos);
            }
            if (!detail::inline_norm_iter_next_comp(&state, &codepoint))
                stream_end = true;
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = std::ptrdiff_t;

        uaiw_constexpr nfkc() = default;
        uaiw_constexpr explicit nfkc(nfkc_view& p, Iter begin, Sent) : parent{std::addressof(p)}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_norm_nfkc(); // Fn call must not be inlined
        }
        uaiw_constexpr reference operator*() const noexcept { return codepoint; }
        uaiw_constexpr nfkc& operator++()
        {
            iter_func_norm_nfkc(); // Fn call must not be inlined

            return *this;
        }
        uaiw_constexpr nfkc operator++(int)
        {
            nfkc tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const nfkc& x, const nfkc& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfkc& x, const nfkc& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const nfkc& x, una::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfkc& x, una::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(una::sentinel_t, const nfkc& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(una::sentinel_t, const nfkc& x) { return !x.stream_end; }
    };

    using iter_t = detail::rng::iterator_t<Range>;
    using sent_t = detail::rng::sentinel_t<Range>;

    Range range = Range{};
    detail::rng::cache<nfkc<iter_t, sent_t>> cached_begin;

public:
    uaiw_constexpr nfkc_view() = default;
    uaiw_constexpr explicit nfkc_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin.has_value())
            return cached_begin.get_value();

        cached_begin.set_value(nfkc<iter_t, sent_t>{*this, std::begin(range), std::end(range)});

        return cached_begin.get_value();
    }
    uaiw_constexpr auto end()
    {
        return una::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
class nfkd_view : public detail::rng::view_base
{
private:
    template<class Iter, class Sent>
    class nfkd
    {
        static_assert(std::is_integral_v<detail::rng::iter_value_t<Iter>> &&
                      sizeof(detail::rng::iter_value_t<Iter>) >= sizeof(char32_t),
                      "norm::nfkd view requires char32_t range");

    private:
        nfkd_view* parent = nullptr;
        Iter it_pos = Iter{};

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        uaiw_constexpr void iter_func_norm_nfkd()
        {
            if (!detail::inline_norm_iter_ready(&state))
            {
                for (bool stop = false; !stop && it_pos != std::end(parent->range); ++it_pos)
                    stop = detail::inline_norm_iter_nfkd(&state, *it_pos);
            }
            if (!detail::inline_norm_iter_next_decomp(&state, &codepoint))
                stream_end = true;
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = std::ptrdiff_t;

        uaiw_constexpr nfkd() = default;
        uaiw_constexpr explicit nfkd(nfkd_view& p, Iter begin, Sent) : parent{std::addressof(p)}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_norm_nfkd(); // Fn call must not be inlined
        }
        uaiw_constexpr reference operator*() const noexcept { return codepoint; }
        uaiw_constexpr nfkd& operator++()
        {
            iter_func_norm_nfkd(); // Fn call must not be inlined

            return *this;
        }
        uaiw_constexpr nfkd operator++(int)
        {
            nfkd tmp = *this;
            operator++();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const nfkd& x, const nfkd& y) { return x.stream_end == y.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfkd& x, const nfkd& y) { return x.stream_end != y.stream_end; }
        friend uaiw_constexpr bool operator==(const nfkd& x, una::sentinel_t) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(const nfkd& x, una::sentinel_t) { return !x.stream_end; }
        friend uaiw_constexpr bool operator==(una::sentinel_t, const nfkd& x) { return x.stream_end; }
        friend uaiw_constexpr bool operator!=(una::sentinel_t, const nfkd& x) { return !x.stream_end; }
    };

    using iter_t = detail::rng::iterator_t<Range>;
    using sent_t = detail::rng::sentinel_t<Range>;

    Range range = Range{};
    detail::rng::cache<nfkd<iter_t, sent_t>> cached_begin;

public:
    uaiw_constexpr nfkd_view() = default;
    uaiw_constexpr explicit nfkd_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const & { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin.has_value())
            return cached_begin.get_value();

        cached_begin.set_value(nfkd<iter_t, sent_t>{*this, std::begin(range), std::end(range)});

        return cached_begin.get_value();
    }
    uaiw_constexpr auto end()
    {
        return una::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

template<class Range>
nfc_view(Range&&) -> nfc_view<views::all_t<Range>>;
template<class Range>
nfd_view(Range&&) -> nfd_view<views::all_t<Range>>;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
template<class Range>
nfkc_view(Range&&) -> nfkc_view<views::all_t<Range>>;
template<class Range>
nfkd_view(Range&&) -> nfkd_view<views::all_t<Range>>;
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace ranges::norm

namespace detail::rng {

/* NFC_VIEW */

struct adaptor_nfc
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    { return ranges::norm::nfc_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_nfc& a) { return a(std::forward<R>(r)); }

/* NFD_VIEW */

struct adaptor_nfd
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    { return ranges::norm::nfd_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_nfd& a) { return a(std::forward<R>(r)); }

#ifndef UNI_ALGO_DISABLE_NFKC_NFKD

/* NFKC_VIEW */

struct adaptor_nfkc
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    { return ranges::norm::nfkc_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_nfkc& a) { return a(std::forward<R>(r)); }

/* NFKD_VIEW */

struct adaptor_nfkd
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    { return ranges::norm::nfkd_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_nfkd& a) { return a(std::forward<R>(r)); }

#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace detail::rng

namespace ranges::views::norm {

inline constexpr detail::rng::adaptor_nfc nfc;
inline constexpr detail::rng::adaptor_nfd nfd;
#ifndef UNI_ALGO_DISABLE_NFKC_NFKD
inline constexpr detail::rng::adaptor_nfkc nfkc;
inline constexpr detail::rng::adaptor_nfkd nfkd;
#endif // UNI_ALGO_DISABLE_NFKC_NFKD

} // namespace ranges::views::norm

namespace views = ranges::views;

} // namespace una

#endif // UNI_ALGO_RANGES_NORM_H_UAIH

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
