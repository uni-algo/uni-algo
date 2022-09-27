/* C++ Standard Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef UNI_ALGO_RANGES_H_UAIH
#define UNI_ALGO_RANGES_H_UAIH

#include <functional>
#ifdef UNI_ALGO_LOG_CPP_ITER
#include <iostream>
#endif

#include "config.h"
#include "version.h"
#include "internal/ranges_core.h"

#include "impl/impl_iterator.h"

namespace uni {

namespace ranges {

template<class Range, char32_t Error = detail::impl_iter_replacement>
class utf8_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class utf8
    {
        static_assert(std::is_integral_v<detail::ranges::iter_value_t<Iter>>,
                      "utf8 view requires integral UTF-8 range");

        // Error is only used for tests, do not document it
        static_assert(Error == detail::impl_iter_error || Error == detail::impl_iter_replacement);

    private:
        utf8_view* parent = nullptr;
        Iter it_pos;
        Iter it_next;
        detail::type_codept codepoint = 0;

        using iter_tag = detail::ranges::iter_tag<Iter>;

        using is_bidirectional_or_better = std::is_convertible<iter_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<iter_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

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
            std::cout << "uni::views::utf8::operator++() " << this << '\n';
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
            std::cout << "uni::views::utf8::operator--() " << this << '\n';
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
        if constexpr (std::is_same_v<iter_t, sent_t>) // std::ranges::common_range<Range>
            return utf8<iter_t, sent_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range, char32_t Error = detail::impl_iter_replacement>
class utf16_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class utf16
    {
        static_assert(std::is_integral_v<detail::ranges::iter_value_t<Iter>> &&
                      sizeof(detail::ranges::iter_value_t<Iter>) >= sizeof(char16_t),
                      "utf16 view requires integral UTF-16 range");

        // Error is only used for tests, do not document it
        static_assert(Error == detail::impl_iter_error || Error == detail::impl_iter_replacement);

    private:
        utf16_view* parent = nullptr;
        Iter it_pos;
        Iter it_next;
        detail::type_codept codepoint = 0;

        using iter_tag = detail::ranges::iter_tag<Iter>;

        using is_bidirectional_or_better = std::is_convertible<iter_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<iter_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = char32_t;
        using pointer           = void;
        using reference         = char32_t;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

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
            std::cout << "uni::views::utf16::operator++() " << this << '\n';
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
            std::cout << "uni::views::utf16::operator--() " << this << '\n';
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
        if constexpr (std::is_same_v<iter_t, sent_t>) // std::ranges::common_range<Range>
            return utf16<iter_t, sent_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
    //uaiw_constexpr bool empty() { return begin() == end(); }
    //explicit uaiw_constexpr operator bool() { return !empty(); }
};

template<class Range>
class reverse_view : public detail::ranges::view_base
{
    // There is a problem with std::views::reverse it is implemented using
    // std::reverse_iterator and operator* looks like this { Iterator tmp = current; return *--tmp; }
    // it is supper ineffective for complex bidirectional algos and that is what our utf8_view/utf16_view are
    // so we need O(1) operator* so we need to reimplement reverse view.
    // Note that our reverse view is always bidirectional so there are no optimizations
    // if Range is random access because we expect utf8_view/utf16_view before this view.
    // It means it still works even if Range is random access but with worse performance compared to std::views::reverse
private:
    template<class Iter, class Sent>
    class reverse
    {
        static_assert(detail::ranges::sa_iter_bidi_or_better<Iter>::value,
                      "reverse view requires bidirectional or better range");

    private:
        reverse_view* parent = nullptr;
        Iter it_pos;
        bool past_begin = true;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = detail::ranges::iter_value_t<Iter>;
        using pointer           = detail::ranges::iter_pointer_t<Iter>;
        using reference         = detail::ranges::iter_reference_t<Iter>;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        uaiw_constexpr reverse() = default;
        uaiw_constexpr explicit reverse(reverse_view& p, Iter begin, Sent end)
            : parent{std::addressof(p)}, it_pos{end}
        {
            if (begin != end)
            {
                --it_pos;
                past_begin = false;
            }
        }
        uaiw_constexpr reference operator*() const { return *it_pos; }
        uaiw_constexpr pointer operator->() const { return it_pos; }
        uaiw_constexpr reverse& operator++()
        {
            if (it_pos != std::begin(parent->range))
                --it_pos;
            else
                past_begin = true;

            return *this;
        }
        uaiw_constexpr reverse operator++(int)
        {
            reverse tmp = *this;
            operator++();
            return tmp;
        }
        uaiw_constexpr reverse& operator--()
        {
#if 0
            if (past_begin)
                past_begin = false;
            else
            {
                assert(it_pos != std::end(parent->range));
                if (it_pos != std::end(parent->range))
                    ++it_pos;
            }
#else
            if (past_begin)
                past_begin = false;
            else
                ++it_pos;
#endif
            return *this;
        }
        uaiw_constexpr reverse operator--(int)
        {
            reverse tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const reverse& x, const reverse& y)
        { return x.it_pos == y.it_pos && x.past_begin == y.past_begin; }
        friend uaiw_constexpr bool operator!=(const reverse& x, const reverse& y)
        { return x.it_pos != y.it_pos || x.past_begin != y.past_begin; }
        friend uaiw_constexpr bool operator==(const reverse& x, uni::sentinel_t) { return x.past_begin; }
        friend uaiw_constexpr bool operator!=(const reverse& x, uni::sentinel_t) { return !x.past_begin; }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const reverse& x) { return x.past_begin; }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const reverse& x) { return !x.past_begin; }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    reverse<iter_t, iter_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr reverse_view() = default;
    uaiw_constexpr explicit reverse_view(Range r) : range{std::move(r)} {}
    //uaiw_constexpr Range base() const& { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        if constexpr (std::is_same_v<iter_t, sent_t>)
            cached_begin_value = reverse<iter_t, iter_t>{*this, std::begin(range), std::end(range)};
        else
        {
            // This is to handle case when Range is bidirectional and not std::ranges::common_range
            // for example std::views::take(x) produces such range and our uni::views::take(x) too
            // this is the same how std::ranges handles such case
            // auto it = std::ranges::next(std::ranges::begin(range), std::ranges::end(range));
            auto it = std::begin(range);
            for (auto end = std::end(range); it != end; ++it);
            cached_begin_value = reverse<iter_t, iter_t>{*this, std::begin(range), it};

            // std::string_view{"12345678900"} | uni::views::utf8
            // | uni::views::reverse | std::views::take(7) | uni::views::reverse
            // | uni::views::drop(2) | uni::views::reverse -> 00987 (48 48 57 56 55)
        }
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        return reverse<iter_t, iter_t>{*this, std::begin(range), std::begin(range)};
    }
};

template<class Range, class Pred>
class filter_view : public detail::ranges::view_base
{
    // Our filter view is almost the same as std::views::filter
    // so the performance should be the same
private:
    template<class Iter, class Sent>
    class filter
    {
    private:
        filter_view* parent = nullptr;
        Iter it_pos;

        using iter_tag = detail::ranges::iter_tag<Iter>;

        using is_bidirectional_or_better = std::is_convertible<iter_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<iter_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = detail::ranges::iter_value_t<Iter>;
        using pointer           = detail::ranges::iter_pointer_t<Iter>;
        using reference         = detail::ranges::iter_reference_t<Iter>;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        uaiw_constexpr filter() = default;
        uaiw_constexpr explicit filter(filter_view& p, Iter begin, Sent end)
            : parent{std::addressof(p)}, it_pos{begin}
        {
            if (begin != end)
                while(!std::invoke(parent->func_filter, *it_pos) && ++it_pos != end);
        }
        uaiw_constexpr reference operator*() const { return *it_pos; }
        uaiw_constexpr pointer operator->() const { return it_pos; }
        uaiw_constexpr filter& operator++()
        {
            if (it_pos != std::end(parent->range))
                while(++it_pos != std::end(parent->range) && !std::invoke(parent->func_filter, *it_pos));

            return *this;
        }
        uaiw_constexpr filter operator++(int)
        {
            filter tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = filter&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--()
        {
#if 0
            assert(it_pos != std::begin(parent->range));
            if (it_pos != std::begin(parent->range))
                while (--it_pos != std::begin(parent->range) && !std::invoke(parent->func_filter, *it_pos));
#else
            do --it_pos;
            while (!std::invoke(parent->func_filter, *it_pos));
#endif
            return *this;
        }
        template<class T = filter> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--(int)
        {
            filter tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const filter& x, const filter& y) { return x.it_pos == y.it_pos; }
        friend uaiw_constexpr bool operator!=(const filter& x, const filter& y) { return x.it_pos != y.it_pos; }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const filter& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend uaiw_constexpr bool operator==(const filter& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(const filter& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const filter& x) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const filter& x) { return !friend_compare_sentinel(x); }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    Pred func_filter;
    filter<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr filter_view() = default;
    uaiw_constexpr filter_view(Range r, Pred pred) : range{std::move(r)}, func_filter{std::move(pred)} {}
    //uaiw_constexpr Range base() const& { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    //uaiw_constexpr const Pred& pred() const { return func_filter; }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = filter<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        if constexpr (std::is_same_v<iter_t, sent_t>)
            return filter<iter_t, sent_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
};

template<class Range, class Func>
class transform_view : public detail::ranges::view_base
{
    // Our transform_view view is always bidirectional or worse so there are no optimizations
    // if Range is random access because we expect utf8_view/utf16_view before this view.
    // It means it still works even if Range is random access but with worse performance compared to std::views::transform
private:
    template<class Iter, class Sent>
    class transform
    {
    private:
        transform_view* parent = nullptr;
        Iter it_pos;

        using iter_tag = detail::ranges::iter_tag<Iter>;

        using is_bidirectional_or_better = std::is_convertible<iter_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<iter_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        //using value_type        = std::remove_cvref_t<std::invoke_result_t<Func&, typename std::iterator_traits<Iter>::reference>>;
        using value_type        = std::remove_cv_t<std::remove_reference_t<
            std::invoke_result_t<Func&, detail::ranges::iter_reference_t<Iter>>>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        uaiw_constexpr transform() = default;
        uaiw_constexpr explicit transform(transform_view& p, Iter begin, Sent)
            : parent{std::addressof(p)}, it_pos{begin} {}
        uaiw_constexpr reference operator*() const { return std::invoke(parent->func_transform, *it_pos); }
        uaiw_constexpr transform& operator++()
        {
            if (it_pos != std::end(parent->range))
                ++it_pos;

            return *this;
        }
        uaiw_constexpr transform operator++(int)
        {
            transform tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = transform&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--()
        {
#if 0
            assert(it_pos != std::begin(parent->range));
            if (it_pos != std::begin(parent->range))
                --it_pos;
#else
            --it_pos;
#endif
            return *this;
        }
        template<class T = transform> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--(int)
        {
            transform tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const transform& x, const transform& y) { return x.it_pos == y.it_pos; }
        friend uaiw_constexpr bool operator!=(const transform& x, const transform& y) { return x.it_pos != y.it_pos; }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const transform& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend uaiw_constexpr bool operator==(const transform& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(const transform& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const transform& x) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const transform& x) { return !friend_compare_sentinel(x); }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    Func func_transform;

public:
    uaiw_constexpr transform_view() = default;
    uaiw_constexpr transform_view(Range r, Func fun) : range{std::move(r)}, func_transform{std::move(fun)} {}
    //uaiw_constexpr Range base() const& { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        return transform<iter_t, sent_t>{*this, std::begin(range), std::end(range)};
    }
    uaiw_constexpr auto end()
    {
        if constexpr (std::is_same_v<iter_t, sent_t>)
            return transform<iter_t, sent_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
};

template<class Range>
class take_view : public detail::ranges::view_base
{
    // Our take view is always bidirectional or worse so there are no optimizations
    // if Range is random access because we expect utf8_view/utf16_view before this view.
    // It means it still works even if Range is random access but with worse performance compared to std::views::take
    // Take iterator is simplified version of std::counted_iterator
private:
    template<class Iter, class Sent>
    class take
    {
    private:
        take_view* parent = nullptr;
        Iter it_pos;
        std::size_t count = 0;

        using iter_tag = detail::ranges::iter_tag<Iter>;

        using is_bidirectional_or_better = std::is_convertible<iter_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<iter_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = detail::ranges::iter_value_t<Iter>;
        using pointer           = detail::ranges::iter_pointer_t<Iter>;
        using reference         = detail::ranges::iter_reference_t<Iter>;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        uaiw_constexpr take() = default;
        uaiw_constexpr explicit take(take_view& p, Iter begin, Sent, std::size_t n)
            : parent{std::addressof(p)}, it_pos{begin}, count{n} {}
        uaiw_constexpr reference operator*() const { return *it_pos; }
        uaiw_constexpr pointer operator->() const { return it_pos; }
        uaiw_constexpr take& operator++()
        {
            if (count != 0 && it_pos != std::end(parent->range))
            {
                ++it_pos;
                --count;
            }

            return *this;
        }
        uaiw_constexpr take operator++(int)
        {
            take tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = take&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--()
        {
#if 0
            assert(it_pos != std::begin(parent->range));
            if (it_pos != std::begin(parent->range))
            {
                --it_pos;
                ++count;
            }
#else
            --it_pos;
            ++count;
#endif
            return *this;
        }
        template<class T = take> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--(int)
        {
            take tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const take& x, const take& y) { return x.it_pos == y.it_pos; }
        friend uaiw_constexpr bool operator!=(const take& x, const take& y) { return x.it_pos != y.it_pos; }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const take& x) { return x.count == 0 || x.it_pos == std::end(x.parent->range); }
    public:
        friend uaiw_constexpr bool operator==(const take& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(const take& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const take& x) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const take& x) { return !friend_compare_sentinel(x); }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    std::size_t count = 0;

public:
    uaiw_constexpr take_view() = default;
    uaiw_constexpr take_view(Range r, std::size_t n) : range{std::move(r)}, count{n} {}
    //uaiw_constexpr Range base() const& { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        return take<iter_t, sent_t>{*this, std::begin(range), std::end(range), count};
    }
    uaiw_constexpr auto end()
    {
        return uni::sentinel;
    }
};

template<class Range>
class drop_view : public detail::ranges::view_base
{
    // Our drop view is always bidirectional or worse so there are no optimizations
    // if Range is random access because we expect utf8_view/utf16_view before this view.
    // It means it still works even if Range is random access but with worse performance compared to std::views::drop
private:
    template<class Iter, class Sent>
    class drop
    {
    private:
        drop_view* parent = nullptr;
        Iter it_pos;

        using iter_tag = detail::ranges::iter_tag<Iter>;

        using is_bidirectional_or_better = std::is_convertible<iter_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<iter_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = detail::ranges::iter_value_t<Iter>;
        using pointer           = detail::ranges::iter_pointer_t<Iter>;
        using reference         = detail::ranges::iter_reference_t<Iter>;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        uaiw_constexpr drop() = default;
        uaiw_constexpr explicit drop(drop_view& p, Iter begin, Sent end, std::size_t cnt = 0)
            : parent{std::addressof(p)}, it_pos{begin}
        {
            if (begin != end && cnt > 0)
                //while (cnt--) ++it_pos;
                while (cnt-- && ++it_pos != end);
        }
        uaiw_constexpr reference operator*() const { return *it_pos; }
        uaiw_constexpr pointer operator->() const { return it_pos; }
        uaiw_constexpr drop& operator++()
        {
            if (it_pos != std::end(parent->range))
                ++it_pos;

            return *this;
        }
        uaiw_constexpr drop operator++(int)
        {
            drop tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = drop&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--()
        {
#if 0
            assert(it_pos != std::begin(parent->range));
            if (it_pos != std::begin(parent->range))
                --it_pos;
#else
            --it_pos;
#endif
            return *this;
        }
        template<class T = drop> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        uaiw_constexpr operator--(int)
        {
            drop tmp = *this;
            operator--();
            return tmp;
        }
        friend uaiw_constexpr bool operator==(const drop& x, const drop& y) { return x.it_pos == y.it_pos; }
        friend uaiw_constexpr bool operator!=(const drop& x, const drop& y) { return x.it_pos != y.it_pos; }
    private:
        static uaiw_constexpr bool friend_compare_sentinel(const drop& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend uaiw_constexpr bool operator==(const drop& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(const drop& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator==(uni::sentinel_t, const drop& x) { return friend_compare_sentinel(x); }
        friend uaiw_constexpr bool operator!=(uni::sentinel_t, const drop& x) { return !friend_compare_sentinel(x); }
    };

    using iter_t = detail::ranges::iterator_t<Range>;
    using sent_t = detail::ranges::sentinel_t<Range>;

    Range range = Range{};
    std::size_t count = 0;
    drop<iter_t, sent_t> cached_begin_value;
    bool cached_begin = false;

public:
    uaiw_constexpr drop_view() = default;
    uaiw_constexpr drop_view(Range r, std::size_t n) : range{std::move(r)}, count{n} {}
    //uaiw_constexpr Range base() const& { return range; }
    //uaiw_constexpr Range base() && { return std::move(range); }
    uaiw_constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = drop<iter_t, sent_t>{*this, std::begin(range), std::end(range), count};
        cached_begin = true;

        return cached_begin_value;
    }
    uaiw_constexpr auto end()
    {
        if constexpr (std::is_same_v<iter_t, sent_t>)
            return drop<iter_t, sent_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
};

} // namespace ranges

namespace detail {

/* UTF8_VIEW */

struct adaptor_utf8
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::utf8_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_utf8& a) { return a(std::forward<R>(r)); }

/* UTF16_VIEW */

struct adaptor_utf16
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::utf16_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_utf16& a) { return a(std::forward<R>(r)); }

/* FILTER_VIEW */

template<class Pred>
struct adaptor_closure_filter
{
    Pred p;
    uaiw_constexpr explicit adaptor_closure_filter(Pred pred) : p{pred} {}
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::filter_view{std::forward<R>(r), p}; }
};
template<class R, class Pred>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_filter<Pred>& a) { return a(std::forward<R>(r)); }

struct adaptor_filter
{
    template<class R, class Pred>
    uaiw_constexpr auto operator()(R&& r, Pred pred) const { return uni::ranges::filter_view{std::forward<R>(r), std::move(pred)}; }
    template<class Pred>
    uaiw_constexpr auto operator()(Pred pred) const { return adaptor_closure_filter<Pred>{std::move(pred)}; }
};

/* REVERSE_VIEW */

struct adaptor_reverse
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::reverse_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_reverse& a) { return a(std::forward<R>(r)); }

/* DROP_VIEW */

struct adaptor_closure_drop
{
    std::size_t count = 0;
    uaiw_constexpr explicit adaptor_closure_drop(std::size_t n): count{n} {}

    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::drop_view{std::forward<R>(r), count}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, adaptor_closure_drop const &a) { return a(std::forward<R>(r)); }

struct adaptor_drop
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r, std::size_t count) const { return uni::ranges::drop_view{std::forward<R>(r), count}; }
    uaiw_constexpr auto operator()(std::size_t count) const { return adaptor_closure_drop{count}; }
};

/* TAKE_VIEW */

struct adaptor_closure_take
{
    std::size_t count = 0;
    uaiw_constexpr explicit adaptor_closure_take(std::size_t n): count{n} {}

    template <class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::take_view{std::forward<R>(r), count}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, adaptor_closure_take const &a) { return a(std::forward<R>(r)); }

struct adaptor_take
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r, std::size_t count) const { return uni::ranges::take_view{std::forward<R>(r), count}; }
    uaiw_constexpr auto operator()(std::size_t count) const { return adaptor_closure_take{count}; }
};

/* TRANSFORM_VIEW */

template<class Func>
struct adaptor_closure_transform
{
    Func f;
    uaiw_constexpr explicit adaptor_closure_transform(Func func) : f{func} {}
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::transform_view{std::forward<R>(r), f}; }
};
template<class R, class Func>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_transform<Func>& a) { return a(std::forward<R>(r)); }

struct adaptor_transform
{
    template<class R, class Func>
    uaiw_constexpr auto operator()(R&& r, Func f) const { return uni::ranges::transform_view{std::forward<R>(r), std::move(f)}; }
    template<class Func>
    uaiw_constexpr auto operator()(Func f) const { return adaptor_closure_transform<Func>{std::move(f)}; }
};

/* TO_UTF8 */

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1206r7.pdf

template<class Result>
struct adaptor_closure_to_utf8
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        using range_v = uni::detail::ranges::range_value_t<R>;
        using result_v = uni::detail::ranges::range_value_t<Result>;

        // Technically we want this static_assert for range_v:
        // static_assert(std::is_same_v<range_v, char32_t>);
        // but it makes it a bit clanky to use with transform view so use more permissive static_assert
        // See: test/test_ranges.h -> test_ranges()
        static_assert(std::is_integral_v<range_v> && sizeof(range_v) >= sizeof(char32_t),
                      "to_utf8 range requires char32_t range");
        static_assert(std::is_integral_v<result_v>,
                      "to_utf8 result type cannot store UTF-8");

        Result result;
        std::back_insert_iterator output{result};
        for (auto c : r)
            detail::impl_output_utf8(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf8<Result>& a) { return a(std::forward<R>(r)); }

template<class Result>
struct adaptor_to_utf8
{
    uaiw_constexpr auto operator()() const { return adaptor_closure_to_utf8<Result>{}; }
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return adaptor_closure_to_utf8<Result>{}(std::forward<R>(r)); }
};

/* TO_UTF16 */

template<class Result>
struct adaptor_closure_to_utf16
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        using range_v = uni::detail::ranges::range_value_t<R>;
        using result_v = uni::detail::ranges::range_value_t<Result>;

        // See comments in to_utf8 adaptor above
        // static_assert(std::is_same_v<range_v, char32_t>);
        static_assert(std::is_integral_v<range_v> && sizeof(range_v) >= sizeof(char32_t),
                      "to_utf16 range requires char32_t range");
        static_assert(std::is_integral_v<result_v> && sizeof(result_v) >= sizeof(char16_t),
                      "to_utf16 result type cannot store UTF-16");

        Result result;
        std::back_insert_iterator output{result};
        for (auto c : r)
            detail::impl_output_utf16(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf16<Result>& a) { return a(std::forward<R>(r)); }

template<class Result>
struct adaptor_to_utf16
{
    uaiw_constexpr auto operator()() const { return adaptor_closure_to_utf16<Result>{}; }
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return adaptor_closure_to_utf16<Result>{}(std::forward<R>(r)); }
};

/* TO_UTF8_RESERVE */

template<class Result>
struct adaptor_closure_to_utf8_reserve
{
    std::size_t size = 0;
    uaiw_constexpr explicit adaptor_closure_to_utf8_reserve(std::size_t n): size{n} {}

    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        using range_v = uni::detail::ranges::range_value_t<R>;
        using result_v = uni::detail::ranges::range_value_t<Result>;

        static_assert(std::is_integral_v<range_v> && sizeof(range_v) >= sizeof(char32_t),
                      "to_utf8_reserve range requires char32_t range");
        static_assert(std::is_integral_v<result_v>,
                      "to_utf8_reserve result type cannot store UTF-8");

        Result result;
        result.reserve(size);
        std::back_insert_iterator output{result};
        for (auto c : r)
            detail::impl_output_utf8(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf8_reserve<Result>& a) { return a(std::forward<R>(r)); }

template<class Result>
struct adaptor_to_utf8_reserve
{
    uaiw_constexpr auto operator()(std::size_t size) const { return adaptor_closure_to_utf8_reserve<Result>{size}; }
    template<class R>
    uaiw_constexpr auto operator()(R&& r, std::size_t size) const { return adaptor_closure_to_utf8_reserve<Result>{size}(std::forward<R>(r)); }
};

/* TO_UTF16_RESERVE */

template<class Result>
struct adaptor_closure_to_utf16_reserve
{
    std::size_t size = 0;
    uaiw_constexpr explicit adaptor_closure_to_utf16_reserve(std::size_t n): size{n} {}

    template<class R>
    uaiw_constexpr auto operator()(R&& r) const
    {
        using range_v = uni::detail::ranges::range_value_t<R>;
        using result_v = uni::detail::ranges::range_value_t<Result>;

        static_assert(std::is_integral_v<range_v> && sizeof(range_v) >= sizeof(char32_t),
                      "to_utf16_reserve range requires char32_t range");
        static_assert(std::is_integral_v<result_v> && sizeof(result_v) >= sizeof(char16_t),
                      "to_utf16_reserve result type cannot store UTF-16");

        Result result;
        result.reserve(size);
        std::back_insert_iterator output{result};
        for (auto c : r)
            detail::impl_output_utf16(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf16_reserve<Result>& a) { return a(std::forward<R>(r)); }

template<class Result>
struct adaptor_to_utf16_reserve
{
    uaiw_constexpr auto operator()(std::size_t size) const { return adaptor_closure_to_utf16_reserve<Result>{size}; }
    template<class R>
    uaiw_constexpr auto operator()(R&& r, std::size_t size) const { return adaptor_closure_to_utf16_reserve<Result>{size}(std::forward<R>(r)); }
};

} // namespace detail

namespace ranges::views {

inline constexpr detail::adaptor_utf8 utf8;
inline constexpr detail::adaptor_utf16 utf16;
inline constexpr detail::adaptor_reverse reverse;
inline constexpr detail::adaptor_filter filter;
inline constexpr detail::adaptor_transform transform;
inline constexpr detail::adaptor_take take;
inline constexpr detail::adaptor_drop drop;

} // namespace views

namespace ranges {

// These user-defined CTAD guides are important
// the problem the code compiles in most cases and works perfectly fine
// even if there is a mistake here but there will be extra move/copy operations
// for the object we are viewing so the performance will be much worse
// It is handled by test/test_ranges.h -> test_ranges_ctad()

template<class Range>
utf8_view(Range&&) -> utf8_view<uni::views::all_t<Range>>;

template<class Range>
utf16_view(Range&&) -> utf16_view<uni::views::all_t<Range>>;

template<class Range, class Pred>
filter_view(Range&&, Pred) -> filter_view<uni::views::all_t<Range>, Pred>;

template<class Range>
reverse_view(Range&&) -> reverse_view<uni::views::all_t<Range>>;

template<class Range>
drop_view(Range&&, std::size_t) -> drop_view<uni::views::all_t<Range>>;

template<class Range, class Func>
transform_view(Range&&, Func) -> transform_view<uni::views::all_t<Range>, Func>;

template<class Range>
take_view(Range&&, std::size_t) -> take_view<uni::views::all_t<Range>>;

} // namespace ranges

namespace ranges {

// NOTE:
// If {} below is not used MSVC 19.16 and probably other versions produces this:
// error C2737: 'uni::ranges::to_utf8': 'constexpr' object must be initialized
// It should not interfere with anything.

template<class Result>
inline constexpr detail::adaptor_to_utf8<Result> to_utf8{};
template<class Result>
inline constexpr detail::adaptor_to_utf16<Result> to_utf16{};
template<class Result>
inline constexpr detail::adaptor_to_utf8_reserve<Result> to_utf8_reserve{};
template<class Result>
inline constexpr detail::adaptor_to_utf16_reserve<Result> to_utf16_reserve{};

} // namespace ranges

namespace views = ranges::views;

} // namespace uni

#endif // UNI_ALGO_RANGES_H_UAIH

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
