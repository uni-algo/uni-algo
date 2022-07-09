/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_RANGES_H_UAIX
#define CPP_UNI_RANGES_H_UAIX

#include <type_traits>
#include <cassert>
#include <memory>
#include <functional>
#if defined(__cpp_lib_ranges) && !defined(__clang__)
#include <ranges>
#endif
#ifdef UNI_ALGO_LOG_CPP_ITER
#include <iostream>
#endif

#include "cpp_uni_config.h"
#include "cpp_uni_version.h"
#include "internal/cpp_uni_sentinel.h"

// TODO: Leave here the code for normalization ranges for now
// it must be moved to cpp_uni_norm.h later.
// All the normalization code protected with this define.
#ifdef UNI_ALGO_TEST_RANGES_NORM
#include "cpp_uni_norm.h"
#endif

#include "impl/impl_iterator.h"

namespace uni {

// We need to use public std::ranges::view_base for compatibility with std::ranges
// when a std::view on the right side of operator|
// https://tristanbrindle.com/posts/rvalue-ranges-and-views
namespace detail {
#if !defined(__cpp_lib_ranges) || defined(__clang__)
struct ranges_view_base {};
#else
using ranges_view_base = std::ranges::view_base;
#endif
}

namespace ranges {

template<class Range, char32_t Error = detail::impl_iter_replacement>
class utf8_view : public detail::ranges_view_base
{
private:
    template<class Iter, class Sent>
    class utf8
    {
    private:
        utf8_view* parent = nullptr;
        Iter it_pos;
        Iter it_next; // This makes the iterator FAT but no one ever pipe more than one utf8_view and the perf is better
        detail::type_codept codepoint = 0;

        // Error is only used for tests, do not document it
        static_assert(Error == detail::impl_iter_error || Error == detail::impl_iter_replacement);

        using base_iterator_tag = typename std::iterator_traits<Iter>::iterator_category;

        using is_random_access_or_better = std::is_convertible<base_iterator_tag, std::random_access_iterator_tag>;
        using is_bidirectional_or_better = std::is_convertible<base_iterator_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<base_iterator_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = char32_t;
        using pointer           = void; // pointer operator->() is not implemented
        using reference         = char32_t;
        using difference_type   = typename std::iterator_traits<Iter>::difference_type;

        constexpr utf8() = default;
        constexpr explicit utf8(utf8_view& p, Iter begin, Sent end)
            : parent{&p}, it_pos{begin}, it_next{begin}
        {
            if (begin != end)
                it_next = detail::impl_utf8_iter(it_next, end, &codepoint, Error);
        }
        //constexpr const Iter& base() const & noexcept { return it_pos; }
        //constexpr Iter base() && { return std::move(it_pos); }
        constexpr reference operator*() const noexcept { return codepoint; }
        constexpr utf8& operator++()
        {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "uni::views::utf8::operator++() " << this << '\n';
#endif
            it_pos = it_next;
            if (it_pos == std::end(parent->range))
                return *this;
            it_next = detail::impl_utf8_iter(it_next, std::end(parent->range), &codepoint, Error);
            return *this;
        }
        constexpr utf8 operator++(int)
        {
            utf8 tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = utf8&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--()
        {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "uni::views::utf8::operator--() " << this << '\n';
#endif
            it_next = it_pos;
            if (it_pos == std::begin(parent->range))
                return *this;
            it_pos = detail::impl_utf8_iter_rev(std::begin(parent->range), it_pos, &codepoint, Error);
            return *this;
        }
        template<class T = utf8> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--(int)
        {
            utf8 tmp = *this;
            operator--();
            return tmp;
        }
        template<class T = difference_type> typename std::enable_if_t<is_random_access_or_better::value, T>
        friend constexpr operator-(const utf8& x, const utf8& y) { return x.it_pos - y.it_pos; } // Use for tests, do not document it
        friend constexpr bool operator==(const utf8& x, const utf8& y) { return x.it_pos == y.it_pos; }
        friend constexpr bool operator!=(const utf8& x, const utf8& y) { return x.it_pos != y.it_pos; }
    private:
        static constexpr bool friend_compare_sentinel(const utf8& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend constexpr bool operator==(const utf8& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(const utf8& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend constexpr bool operator==(uni::sentinel_t, const utf8& x) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(uni::sentinel_t, const utf8& x) { return !friend_compare_sentinel(x); }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>())); // std::ranges::iterator_t<Range>
    using base_sentinel_t = decltype(std::end(std::declval<Range&>())); // std::ranges::sentinel_t<Range>

    Range range = Range{};
    utf8<base_iterator_t, base_sentinel_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr utf8_view() = default;
    constexpr utf8_view(Range r) : range{std::move(r)} {}
    //constexpr Range base() const & { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = utf8<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>) // std::ranges::common_range<Range>
            return utf8<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
    //constexpr bool empty() { return begin() == end(); }
    //explicit constexpr operator bool() { return !empty(); }
};

template<class Range, char32_t Error = detail::impl_iter_replacement>
class utf16_view : public detail::ranges_view_base
{
private:
    template<class Iter, class Sent>
    class utf16
    {
    private:
        utf16_view* parent = nullptr;
        Iter it_pos;
        Iter it_next; // This makes the iterator FAT but no one ever pipe more than one utf16_view and the perf is better
        detail::type_codept codepoint = 0;

        // Error is only used for tests, do not document it
        static_assert(Error == detail::impl_iter_error || Error == detail::impl_iter_replacement);

        static_assert(sizeof(typename std::iterator_traits<Iter>::value_type) >= sizeof(char16_t));

        using base_iterator_tag = typename std::iterator_traits<Iter>::iterator_category;

        using is_random_access_or_better = std::is_convertible<base_iterator_tag, std::random_access_iterator_tag>;
        using is_bidirectional_or_better = std::is_convertible<base_iterator_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<base_iterator_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = char32_t;
        using pointer           = void; // pointer operator->() is not implemented
        using reference         = char32_t;
        using difference_type   = typename std::iterator_traits<Iter>::difference_type;

        constexpr utf16() = default;
        constexpr explicit utf16(utf16_view& p, Iter begin, Sent end)
            : parent{&p}, it_pos{begin}, it_next{begin}
        {
            if (begin != end)
                it_next = detail::impl_utf16_iter(it_next, end, &codepoint, Error);
        }
        //constexpr const Iter& base() const & noexcept { return it_pos; }
        //constexpr Iter base() && { return std::move(it_pos); }
        constexpr reference operator*() const noexcept { return codepoint; }
        constexpr utf16& operator++()
        {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "uni::views::utf16::operator++() " << this << '\n';
#endif
            it_pos = it_next;
            if (it_pos == std::end(parent->range))
                return *this;
            it_next = detail::impl_utf16_iter(it_next, std::end(parent->range), &codepoint, Error);
            return *this;
        }
        constexpr utf16 operator++(int)
        {
            utf16 tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = utf16&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--()
        {
#ifdef UNI_ALGO_LOG_CPP_ITER
            std::cout << "uni::views::utf16::operator--() " << this << '\n';
#endif
            it_next = it_pos;
            if (it_pos == std::begin(parent->range))
                return *this;
            it_pos = detail::impl_utf16_iter_rev(std::begin(parent->range), it_pos, &codepoint, Error);
            return *this;
        }
        template<class T = utf16> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--(int)
        {
            utf16 tmp = *this;
            operator--();
            return tmp;
        }
        template<class T = difference_type> typename std::enable_if_t<is_random_access_or_better::value, T>
        friend constexpr operator-(const utf16& x, const utf16& y) { return x.it_pos - y.it_pos; } // Use for tests, do not document it
        friend constexpr bool operator==(const utf16& x, const utf16& y) { return x.it_pos == y.it_pos; }
        friend constexpr bool operator!=(const utf16& x, const utf16& y) { return x.it_pos != y.it_pos; }
    private:
        static constexpr bool friend_compare_sentinel(const utf16& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend constexpr bool operator==(const utf16& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(const utf16& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend constexpr bool operator==(uni::sentinel_t, const utf16& x) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(uni::sentinel_t, const utf16& x) { return !friend_compare_sentinel(x); }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>())); // std::ranges::iterator_t<Range>
    using base_sentinel_t = decltype(std::end(std::declval<Range&>())); // std::ranges::sentinel_t<Range>

    Range range = Range{};
    utf16<base_iterator_t, base_sentinel_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr utf16_view() = default;
    constexpr utf16_view(Range r) : range{std::move(r)} {}
    //constexpr Range base() const & { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = utf16<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>) // std::ranges::common_range<Range>
            return utf16<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
    //constexpr bool empty() { return begin() == end(); }
    //explicit constexpr operator bool() { return !empty(); }
};

template<class Range>
class reverse_view : public detail::ranges_view_base
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
    private:
        reverse_view* parent = nullptr;
        Iter it_pos;
        bool past_begin = true;

        using base_iterator_tag = typename std::iterator_traits<Iter>::iterator_category;

        using is_bidirectional_or_better = std::is_convertible<base_iterator_tag, std::bidirectional_iterator_tag>;

        static_assert(is_bidirectional_or_better::value, "Bidirectional or better range is required");

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = typename std::iterator_traits<Iter>::value_type;
        using pointer           = typename std::iterator_traits<Iter>::pointer;
        using reference         = typename std::iterator_traits<Iter>::reference;
        using difference_type   = typename std::iterator_traits<Iter>::difference_type;

        constexpr reverse() = default;
        constexpr explicit reverse(reverse_view& p, Iter begin, Sent end)
            : parent{&p}, it_pos{end}
        {
            if (begin != end)
            {
                --it_pos;
                past_begin = false;
            }
        }
        constexpr reference operator*() const { return *it_pos; }
        constexpr pointer operator->() const { return it_pos; }
        constexpr reverse& operator++()
        {
            if (it_pos != std::begin(parent->range))
                --it_pos;
            else
                past_begin = true;

            return *this;
        }
        constexpr reverse operator++(int)
        {
            reverse tmp = *this;
            operator++();
            return tmp;
        }
        constexpr reverse& operator--()
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
        constexpr reverse operator--(int)
        {
            reverse tmp = *this;
            operator--();
            return tmp;
        }
        friend constexpr bool operator==(const reverse& x, const reverse& y)
        { return x.it_pos == y.it_pos && x.past_begin == y.past_begin; }
        friend constexpr bool operator!=(const reverse& x, const reverse& y)
        { return x.it_pos != y.it_pos || x.past_begin != y.past_begin; }
        friend constexpr bool operator==(const reverse& x, uni::sentinel_t) { return x.past_begin; }
        friend constexpr bool operator!=(const reverse& x, uni::sentinel_t) { return !x.past_begin; }
        friend constexpr bool operator==(uni::sentinel_t, const reverse& x) { return x.past_begin; }
        friend constexpr bool operator!=(uni::sentinel_t, const reverse& x) { return !x.past_begin; }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    reverse<base_iterator_t, base_iterator_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr reverse_view() = default;
    constexpr reverse_view(Range r) : range{std::move(r)} {}
    //constexpr Range base() const& { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>)
            cached_begin_value = reverse<base_iterator_t, base_iterator_t>{*this, std::begin(range), std::end(range)};
        else
        {
            // This is to handle case when Range is bidirectional and not std::ranges::common_range
            // for example std::views::take(x) produces such range and our uni::views::take(x) too
            // this is the same how std::ranges handles such case
            // auto it = std::ranges::next(std::ranges::begin(range), std::ranges::end(range));
            auto it = std::begin(range);
            for (auto end = std::end(range); it != end; ++it);
            cached_begin_value = reverse<base_iterator_t, base_iterator_t>{*this, std::begin(range), it};

            // std::string_view{"12345678900"} | uni::views::utf8
            // | uni::views::reverse | std::views::take(7) | uni::views::reverse
            // | uni::views::drop(2) | uni::views::reverse -> 00987 (48 48 57 56 55)
        }
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        return reverse<base_iterator_t, base_iterator_t>{*this, std::begin(range), std::begin(range)};
    }
};

template<class Range, class Pred>
class filter_view : public detail::ranges_view_base
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

        using base_iterator_tag = typename std::iterator_traits<Iter>::iterator_category;

        using is_bidirectional_or_better = std::is_convertible<base_iterator_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<base_iterator_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = typename std::iterator_traits<Iter>::value_type;
        using pointer           = typename std::iterator_traits<Iter>::pointer;
        using reference         = typename std::iterator_traits<Iter>::reference;
        using difference_type   = typename std::iterator_traits<Iter>::difference_type;

        constexpr filter() = default;
        constexpr explicit filter(filter_view& p, Iter begin, Sent end)
            : parent{&p}, it_pos{begin}
        {
            if (begin != end)
                while(!parent->func_filter(*it_pos) && ++it_pos != end);
        }
        constexpr reference operator*() const { return *it_pos; }
        constexpr pointer operator->() const { return it_pos; }
        constexpr filter& operator++()
        {
            if (it_pos != std::end(parent->range))
                while(++it_pos != std::end(parent->range) && !parent->func_filter(*it_pos));

            return *this;
        }
        constexpr filter operator++(int)
        {
            filter tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = filter&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--()
        {
#if 0
            assert(it_pos != std::begin(parent->range));
            if (it_pos != std::begin(parent->range))
                while (--it_pos != std::begin(parent->range) && !parent->func_filter(*it_pos));
#else
            do --it_pos;
            while (!parent->func_filter(*it_pos));
#endif
            return *this;
        }
        template<class T = filter> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--(int)
        {
            filter tmp = *this;
            operator--();
            return tmp;
        }
        friend constexpr bool operator==(const filter& x, const filter& y) { return x.it_pos == y.it_pos; }
        friend constexpr bool operator!=(const filter& x, const filter& y) { return x.it_pos != y.it_pos; }
    private:
        static constexpr bool friend_compare_sentinel(const filter& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend constexpr bool operator==(const filter& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(const filter& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend constexpr bool operator==(uni::sentinel_t, const filter& x) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(uni::sentinel_t, const filter& x) { return !friend_compare_sentinel(x); }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    Pred func_filter;
    filter<base_iterator_t, base_sentinel_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr filter_view() = default;
    constexpr filter_view(Range r, Pred pred) : range{std::move(r)}, func_filter{std::move(pred)} {}
    //constexpr Range base() const& { return range; }
    //constexpr Range base() && { return std::move(range); }
    //constexpr const Pred& pred() const { return func_filter; }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = filter<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>)
            return filter<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
};

template<class Range, class Func>
class transform_view : public detail::ranges_view_base
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

        using base_iterator_tag = typename std::iterator_traits<Iter>::iterator_category;

        using is_bidirectional_or_better = std::is_convertible<base_iterator_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<base_iterator_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        //using value_type        = std::remove_cvref_t<std::invoke_result_t<Func&, typename std::iterator_traits<Iter>::reference>>;
        using value_type        = std::remove_cv_t<std::remove_reference_t<
            std::invoke_result_t<Func&, typename std::iterator_traits<Iter>::reference>>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = typename std::iterator_traits<Iter>::difference_type;

        constexpr transform() = default;
        constexpr explicit transform(transform_view& p, Iter begin, Sent) : parent{&p}, it_pos{begin} {}
        constexpr reference operator*() const { return parent->func_transform(*it_pos); }
        constexpr transform& operator++()
        {
            if (it_pos != std::end(parent->range))
                ++it_pos;

            return *this;
        }
        constexpr transform operator++(int)
        {
            transform tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = transform&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--()
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
        constexpr operator--(int)
        {
            transform tmp = *this;
            operator--();
            return tmp;
        }
        friend constexpr bool operator==(const transform& x, const transform& y) { return x.it_pos == y.it_pos; }
        friend constexpr bool operator!=(const transform& x, const transform& y) { return x.it_pos != y.it_pos; }
    private:
        static constexpr bool friend_compare_sentinel(const transform& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend constexpr bool operator==(const transform& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(const transform& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend constexpr bool operator==(uni::sentinel_t, const transform& x) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(uni::sentinel_t, const transform& x) { return !friend_compare_sentinel(x); }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    Func func_transform;

public:
    constexpr transform_view() = default;
    constexpr transform_view(Range r, Func fun) : range{std::move(r)}, func_transform{std::move(fun)} {}
    //constexpr Range base() const& { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        return transform<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range)};
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>)
            return transform<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
};

template<class Range>
class take_view : public detail::ranges_view_base
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

        using base_iterator_tag = typename std::iterator_traits<Iter>::iterator_category;

        using is_bidirectional_or_better = std::is_convertible<base_iterator_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<base_iterator_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = typename std::iterator_traits<Iter>::value_type;
        using pointer           = typename std::iterator_traits<Iter>::pointer;
        using reference         = typename std::iterator_traits<Iter>::reference;
        using difference_type   = typename std::iterator_traits<Iter>::difference_type;

        constexpr take() = default;
        constexpr explicit take(take_view& p, Iter begin, Sent, std::size_t n) : parent{&p}, it_pos{begin}, count{n} {}
        constexpr reference operator*() const { return *it_pos; }
        constexpr pointer operator->() const { return it_pos; }
        constexpr take& operator++()
        {
            if (count != 0 && it_pos != std::end(parent->range))
            {
                ++it_pos;
                --count;
            }

            return *this;
        }
        constexpr take operator++(int)
        {
            take tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = take&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--()
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
        constexpr operator--(int)
        {
            take tmp = *this;
            operator--();
            return tmp;
        }
        friend constexpr bool operator==(const take& x, const take& y) { return x.it_pos == y.it_pos; }
        friend constexpr bool operator!=(const take& x, const take& y) { return x.it_pos != y.it_pos; }
    private:
        static constexpr bool friend_compare_sentinel(const take& x) { return x.count == 0 || x.it_pos == std::end(x.parent->range); }
    public:
        friend constexpr bool operator==(const take& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(const take& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend constexpr bool operator==(uni::sentinel_t, const take& x) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(uni::sentinel_t, const take& x) { return !friend_compare_sentinel(x); }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    std::size_t count = 0;

public:
    constexpr take_view() = default;
    constexpr take_view(Range r, std::size_t n) : range{std::move(r)}, count{n} {}
    //constexpr Range base() const& { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        return take<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range), count};
    }
    constexpr auto end()
    {
        return uni::sentinel;
    }
};

template<class Range>
class drop_view : public detail::ranges_view_base
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

        using base_iterator_tag = typename std::iterator_traits<Iter>::iterator_category;

        using is_bidirectional_or_better = std::is_convertible<base_iterator_tag, std::bidirectional_iterator_tag>;
        using is_forward_or_better       = std::is_convertible<base_iterator_tag, std::forward_iterator_tag>;

    public:
        using iterator_category = std::conditional_t<is_bidirectional_or_better::value,
            std::bidirectional_iterator_tag, std::conditional_t<is_forward_or_better::value,
            std::forward_iterator_tag, std::input_iterator_tag>>;
        using value_type        = typename std::iterator_traits<Iter>::value_type;
        using pointer           = typename std::iterator_traits<Iter>::pointer;
        using reference         = typename std::iterator_traits<Iter>::reference;
        using difference_type   = typename std::iterator_traits<Iter>::difference_type;

        constexpr drop() = default;
        constexpr explicit drop(drop_view& p, Iter begin, Sent end, std::size_t cnt = 0)
            : parent{&p}, it_pos{begin}
        {
            if (begin != end && cnt > 0)
                //while (cnt--) ++it_pos;
                while (cnt-- && ++it_pos != end);
        }
        constexpr reference operator*() const { return *it_pos; }
        constexpr pointer operator->() const { return it_pos; }
        constexpr drop& operator++()
        {
            if (it_pos != std::end(parent->range))
                ++it_pos;

            return *this;
        }
        constexpr drop operator++(int)
        {
            drop tmp = *this;
            operator++();
            return tmp;
        }
        template<class T = drop&> typename std::enable_if_t<is_bidirectional_or_better::value, T>
        constexpr operator--()
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
        constexpr operator--(int)
        {
            drop tmp = *this;
            operator--();
            return tmp;
        }
        friend constexpr bool operator==(const drop& x, const drop& y) { return x.it_pos == y.it_pos; }
        friend constexpr bool operator!=(const drop& x, const drop& y) { return x.it_pos != y.it_pos; }
    private:
        static constexpr bool friend_compare_sentinel(const drop& x) { return x.it_pos == std::end(x.parent->range); }
    public:
        friend constexpr bool operator==(const drop& x, uni::sentinel_t) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(const drop& x, uni::sentinel_t) { return !friend_compare_sentinel(x); }
        friend constexpr bool operator==(uni::sentinel_t, const drop& x) { return friend_compare_sentinel(x); }
        friend constexpr bool operator!=(uni::sentinel_t, const drop& x) { return !friend_compare_sentinel(x); }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    std::size_t count = 0;
    drop<base_iterator_t, base_sentinel_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr drop_view() = default;
    constexpr drop_view(Range r, std::size_t n) : range{std::move(r)}, count{n} {}
    //constexpr Range base() const& { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = drop<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range), count};
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>)
            return drop<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
};

#ifdef UNI_ALGO_TEST_RANGES_NORM
namespace norm {

template<class Range>
class nfc_view : public detail::ranges_view_base
{
private:
    template<class Iter, class Sent>
    class nfc
    {
    private:
        nfc_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        void iter_func_nfc()
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
        explicit nfc(nfc_view& p, Iter begin, Sent) : parent{&p}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_nfc(); // Fn call must not be inlined
        }
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
        friend constexpr bool operator==(const nfc& x, const nfc& y) { return x.stream_end == y.stream_end; }
        friend constexpr bool operator!=(const nfc& x, const nfc& y) { return x.stream_end != y.stream_end; }
        friend constexpr bool operator==(const nfc& x, uni::sentinel_t) { return x.stream_end; }
        friend constexpr bool operator!=(const nfc& x, uni::sentinel_t) { return !x.stream_end; }
        friend constexpr bool operator==(uni::sentinel_t, const nfc& x) { return x.stream_end; }
        friend constexpr bool operator!=(uni::sentinel_t, const nfc& x) { return !x.stream_end; }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    nfc<base_iterator_t, base_sentinel_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr nfc_view() = default;
    constexpr nfc_view(Range r) : range{std::move(r)} {}
    //constexpr Range base() const & { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = nfc<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>) // std::ranges::common_range<Range>
            return nfc<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
    //constexpr bool empty() { return begin() == end(); }
    //explicit constexpr operator bool() { return !empty(); }
};

template<class Range>
class nfd_view : public detail::ranges_view_base
{
private:
    template<class Iter, class Sent>
    class nfd
    {
    private:
        nfd_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        void iter_func_nfd()
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
        explicit nfd(nfd_view& p, Iter begin, Sent) : parent{&p}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_nfd(); // Fn call must not be inlined
        }
        reference operator*() const noexcept { return codepoint; }
        nfd& operator++()
        {
            iter_func_nfd(); // Fn call must not be inlined

            return *this;
        }
        nfd operator++(int)
        {
            nfd tmp = *this;
            operator++();
            return tmp;
        }
        friend constexpr bool operator==(const nfd& x, const nfd& y) { return x.stream_end == y.stream_end; }
        friend constexpr bool operator!=(const nfd& x, const nfd& y) { return x.stream_end != y.stream_end; }
        friend constexpr bool operator==(const nfd& x, uni::sentinel_t) { return x.stream_end; }
        friend constexpr bool operator!=(const nfd& x, uni::sentinel_t) { return !x.stream_end; }
        friend constexpr bool operator==(uni::sentinel_t, const nfd& x) { return x.stream_end; }
        friend constexpr bool operator!=(uni::sentinel_t, const nfd& x) { return !x.stream_end; }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    nfd<base_iterator_t, base_sentinel_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr nfd_view() = default;
    constexpr nfd_view(Range r) : range{std::move(r)} {}
    //constexpr Range base() const & { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = nfd<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>) // std::ranges::common_range<Range>
            return nfd<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
    //constexpr bool empty() { return begin() == end(); }
    //explicit constexpr operator bool() { return !empty(); }
};

template<class Range>
class nfkc_view : public detail::ranges_view_base
{
private:
    template<class Iter, class Sent>
    class nfkc
    {
    private:
        nfkc_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        void iter_func_nfkc()
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
        explicit nfkc(nfkc_view& p, Iter begin, Sent) : parent{&p}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_nfkc(); // Fn call must not be inlined
        }
        reference operator*() const noexcept { return codepoint; }
        nfkc& operator++()
        {
            iter_func_nfkc(); // Fn call must not be inlined

            return *this;
        }
        nfkc operator++(int)
        {
            nfkc tmp = *this;
            operator++();
            return tmp;
        }
        friend constexpr bool operator==(const nfkc& x, const nfkc& y) { return x.stream_end == y.stream_end; }
        friend constexpr bool operator!=(const nfkc& x, const nfkc& y) { return x.stream_end != y.stream_end; }
        friend constexpr bool operator==(const nfkc& x, uni::sentinel_t) { return x.stream_end; }
        friend constexpr bool operator!=(const nfkc& x, uni::sentinel_t) { return !x.stream_end; }
        friend constexpr bool operator==(uni::sentinel_t, const nfkc& x) { return x.stream_end; }
        friend constexpr bool operator!=(uni::sentinel_t, const nfkc& x) { return !x.stream_end; }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    nfkc<base_iterator_t, base_sentinel_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr nfkc_view() = default;
    constexpr nfkc_view(Range r) : range{std::move(r)} {}
    //constexpr Range base() const & { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = nfkc<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>) // std::ranges::common_range<Range>
            return nfkc<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
    //constexpr bool empty() { return begin() == end(); }
    //explicit constexpr operator bool() { return !empty(); }
};

template<class Range>
class nfkd_view : public detail::ranges_view_base
{
private:
    template<class Iter, class Sent>
    class nfkd
    {
    private:
        nfkd_view* parent = nullptr;
        Iter it_pos;

        bool stream_end = false;

        detail::type_codept codepoint = 0;
        detail::impl_norm_iter_state state{};

        void iter_func_nfkd()
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
        explicit nfkd(nfkd_view& p, Iter begin, Sent) : parent{&p}, it_pos{begin}
        {
            detail::impl_norm_iter_state_reset(&state);

            iter_func_nfkd(); // Fn call must not be inlined
        }
        reference operator*() const noexcept { return codepoint; }
        nfkd& operator++()
        {
            iter_func_nfkd(); // Fn call must not be inlined

            return *this;
        }
        nfkd operator++(int)
        {
            nfkd tmp = *this;
            operator++();
            return tmp;
        }
        friend constexpr bool operator==(const nfkd& x, const nfkd& y) { return x.stream_end == y.stream_end; }
        friend constexpr bool operator!=(const nfkd& x, const nfkd& y) { return x.stream_end != y.stream_end; }
        friend constexpr bool operator==(const nfkd& x, uni::sentinel_t) { return x.stream_end; }
        friend constexpr bool operator!=(const nfkd& x, uni::sentinel_t) { return !x.stream_end; }
        friend constexpr bool operator==(uni::sentinel_t, const nfkd& x) { return x.stream_end; }
        friend constexpr bool operator!=(uni::sentinel_t, const nfkd& x) { return !x.stream_end; }
    };

    using base_iterator_t = decltype(std::begin(std::declval<Range&>()));
    using base_sentinel_t = decltype(std::end(std::declval<Range&>()));

    Range range = Range{};
    nfkd<base_iterator_t, base_sentinel_t> cached_begin_value;
    bool cached_begin = false;

public:
    constexpr nfkd_view() = default;
    constexpr nfkd_view(Range r) : range{std::move(r)} {}
    //constexpr Range base() const & { return range; }
    //constexpr Range base() && { return std::move(range); }
    constexpr auto begin()
    {
        if (cached_begin)
            return cached_begin_value;

        cached_begin_value = nfkd<base_iterator_t, base_sentinel_t>{*this, std::begin(range), std::end(range)};
        cached_begin = true;

        return cached_begin_value;
    }
    constexpr auto end()
    {
        if constexpr (std::is_same_v<base_iterator_t, base_sentinel_t>) // std::ranges::common_range<Range>
            return nfkd<base_iterator_t, base_sentinel_t>{*this, std::end(range), std::end(range)};
        else
            return uni::sentinel;
    }
    //constexpr bool empty() { return begin() == end(); }
    //explicit constexpr operator bool() { return !empty(); }
};

} // namespace norm
#endif // UNI_ALGO_TEST_RANGES_NORM

// For C++17 we implement very simple ref_view that will be used together with uni::views::all_t/uni::views::all
// It has the similar design as std::views::ref_view so in C++20 we just use that
#if !defined(__cpp_lib_ranges) || defined(__clang__)
template<class Range>
class ref_view : public detail::ranges_view_base
{
private:
    Range* range = nullptr;
public:
    constexpr ref_view() = default;
    constexpr ref_view(Range& r) : range{std::addressof(r)} {}
    //constexpr Range& base() const { return *range; }
    constexpr auto begin() const { return std::begin(*range); }
    constexpr auto end() const { return std::end(*range); }
};
#else
template<class R>
using ref_view = std::ranges::ref_view<R>;
#endif

// std::owning_view is available in C++20 starting with __cpp_lib_ranges > 202106L
// but we still enable our owning_view only in C++17 so in C++20 inside all view sometimes
// std::owning_view will be used and sometimes std::ranges::subrange it shouldn't create any problems.
// C++ Commitee just thought it is a good idea to add std::ranges that is still WIP to C++20
// for God sake just make them experimental or something in C++20 and add them to C++23.
// Now I'm forced to deal with that crap. Thank you so much bros much appreciate it.
#if !defined(__cpp_lib_ranges) || __cpp_lib_ranges > 202106L
#if !defined(__cpp_lib_ranges) || defined(__clang__)
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2415r2.html
template<class Range>
class owning_view : public detail::ranges_view_base
{
private:
    Range range = Range{};
public:
    constexpr owning_view() = default;
    constexpr owning_view(Range&& r) : range{std::move(r)} {}
    //constexpr Range& base() & noexcept { return range; }
    //constexpr const Range& base() const & noexcept { return range; }
    //constexpr Range&& base() && noexcept { return std::move(range); }
    //constexpr const Range&& base() const && noexcept { return std::move(range); }
    constexpr owning_view(owning_view&&) = default;
    constexpr owning_view& operator=(owning_view&&) = default;
    constexpr auto begin() { return std::begin(range); }
    constexpr auto end() { return std::end(range); }
    //constexpr auto begin() const { return ranges::begin(range); }
    //constexpr auto end() const { return ranges::end(range); }
};
#else
template<class R>
using owning_view = std::ranges::owning_view<R>;
#endif
#endif

} // namespace ranges

namespace detail {

// For C++17 we implement very simply view adaptors that support operator|
// The design is based on this article:
// https://mariusbancila.ro/blog/2020/06/06/a-custom-cpp20-range-view/
// For adaptors without parameters adaptor_closure object from this article is enough so we just call them adaptor
// and for adaptors with parameters both adaptor and adaptor_closure objects are needed
// For C++20 we still use them because C++ Commitee just want people to suffer and custom adaptors still not available
// For C++23 where such adaptors will be available we want to use them
// They will be called: std::ranges::range_adaptor_closure
// https://stackoverflow.com/questions/71689137/what-is-the-best-way-to-drop-last-element-using-c20-ranges
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p2387r3.html
// Our adaptors support this:
// auto range = str | uni::views::utf8 | uni::views::filter(f)
// But do not support this:
// auto range = uni::views::utf8 | uni::views::filter(f)

/* ALL_VIEW */

#if !defined(__cpp_lib_ranges) || defined(__clang__)
struct adaptor_all
{
    //constexpr adaptor_closure_all() {}
    template<class R>
    constexpr auto operator()(R&& r) const
    {
        // The design of all view is "pretty simple"
        // for known ranges (our ranges) or std::string_view we just std::forward them directly
        // for unknown ranges (std::string etc.) we use ref_view as a proxy
        // and for rvalues we use owning_view as a proxy
        // this is the similar design as std::ranges

        using base_iterator_t = decltype(std::begin(std::declval<R&>()));
        using base_iterator_v = typename std::iterator_traits<base_iterator_t>::value_type;

        // Check if the range is our range or std::ranges::view
        // Note that the better check in C++20 will look like this:
        // if constexpr (std::ranges::view<std::decay_t<R>>)
        // but in C++20 it is just better to use all view/ref_view/owning_view provided by the standard library
        // std::decay_t is important here to handle some corner cases properly
        // see: test/test_ranges.h -> test_ranges_static_assert()
        if constexpr (std::is_base_of_v<detail::ranges_view_base, std::decay_t<R>> ||
                //std::is_base_of_v<std::ranges::view_interface<std::decay_t<R>>, std::decay_t<R>> || // view_interface check
                std::is_same_v<std::basic_string_view<base_iterator_v>, std::decay_t<R>>)
            return std::forward<R>(r);
        else if constexpr (std::is_lvalue_reference_v<R>)
            return uni::ranges::ref_view{std::forward<R>(r)};
        else
            return uni::ranges::owning_view{std::forward<R>(r)};

        // view_interface check is needed because std::ranges::view_interface in not derived from std::view_base anymore
        // https://cplusplus.github.io/LWG/issue3549
        // but we don't use this crappy all view in C++20 so we don't care
    }
};
template <class R>
constexpr auto operator|(R&& r, const adaptor_all& a) { return a(std::forward<R>(r)); }
#endif

/* UTF8_VIEW */

struct adaptor_utf8
{
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::utf8_view{std::forward<R>(r)}; }
};
template<class R>
constexpr auto operator|(R&& r, const adaptor_utf8& a) { return a(std::forward<R>(r)); }

/* UTF16_VIEW */

struct adaptor_utf16
{
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::utf16_view{std::forward<R>(r)}; }
};
template<class R>
constexpr auto operator|(R&& r, const adaptor_utf16& a) { return a(std::forward<R>(r)); }

/* FILTER_VIEW */

template<class Pred>
struct adaptor_closure_filter
{
    Pred p;
    constexpr adaptor_closure_filter(Pred pred) : p{pred} {}
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::filter_view{std::forward<R>(r), p}; }
};
template<class R, class Pred>
constexpr auto operator|(R&& r, const adaptor_closure_filter<Pred>& a) { return a(std::forward<R>(r)); }

struct adaptor_filter
{
    template<class R, class Pred>
    constexpr auto operator()(R&& r, Pred pred) const { return ranges::filter_view{std::forward<R>(r), std::move(pred)}; }
    template<class Pred>
    constexpr auto operator()(Pred pred) const { return adaptor_closure_filter<Pred>{std::move(pred)}; }
};

/* REVERSE_VIEW */

struct adaptor_reverse
{
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::reverse_view{std::forward<R>(r)}; }
};
template<class R>
constexpr auto operator|(R&& r, const adaptor_reverse& a) { return a(std::forward<R>(r)); }

/* DROP_VIEW */

struct adaptor_closure_drop
{
    std::size_t count = 0;
    constexpr adaptor_closure_drop(std::size_t n): count(n) {}

    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::drop_view{std::forward<R>(r), count}; }
};
template<class R>
constexpr auto operator|(R&& r, adaptor_closure_drop const &a) { return a(std::forward<R>(r)); }

struct adaptor_drop
{
    template<class R>
    constexpr auto operator()(R&& r, std::size_t count) const { return ranges::drop_view{std::forward<R>(r), count}; }
    constexpr auto operator()(std::size_t count) const { return adaptor_closure_drop{count}; }
};

/* TAKE_VIEW */

struct adaptor_closure_take
{
    std::size_t count = 0;
    constexpr adaptor_closure_take(std::size_t n): count(n) {}

    template <class R>
    constexpr auto operator()(R&& r) const { return ranges::take_view{std::forward<R>(r), count}; }
};
template<class R>
constexpr auto operator|(R&& r, adaptor_closure_take const &a) { return a(std::forward<R>(r)); }

struct adaptor_take
{
    template<class R>
    constexpr auto operator()(R&& r, std::size_t count) const { return ranges::take_view{std::forward<R>(r), count}; }
    constexpr auto operator()(std::size_t count) const { return adaptor_closure_take{count}; }
};

/* TRANSFORM_VIEW */

template<class Func>
struct adaptor_closure_transform
{
    Func f;
    constexpr adaptor_closure_transform(Func func) : f{func} {}
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::transform_view{std::forward<R>(r), f}; }
};
template<class R, class Func>
constexpr auto operator|(R&& r, const adaptor_closure_transform<Func>& a) { return a(std::forward<R>(r)); }

struct adaptor_transform
{
    template<class R, class Func>
    constexpr auto operator()(R&& r, Func f) const { return ranges::transform_view{std::forward<R>(r), std::move(f)}; }
    template<class Func>
    constexpr auto operator()(Func f) const { return adaptor_closure_transform<Func>{std::move(f)}; }
};

#ifdef UNI_ALGO_TEST_RANGES_NORM
/* NFC_VIEW */

struct adaptor_nfc
{
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::norm::nfc_view{std::forward<R>(r)}; }
};
template<class R>
constexpr auto operator|(R&& r, const adaptor_nfc& a) { return a(std::forward<R>(r)); }

/* NFD_VIEW */

struct adaptor_nfd
{
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::norm::nfd_view{std::forward<R>(r)}; }
};
template<class R>
constexpr auto operator|(R&& r, const adaptor_nfd& a) { return a(std::forward<R>(r)); }

/* NFKC_VIEW */

struct adaptor_nfkc
{
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::norm::nfkc_view{std::forward<R>(r)}; }
};
template<class R>
constexpr auto operator|(R&& r, const adaptor_nfkc& a) { return a(std::forward<R>(r)); }

/* NFKD_VIEW */

struct adaptor_nfkd
{
    template<class R>
    constexpr auto operator()(R&& r) const { return ranges::norm::nfkd_view{std::forward<R>(r)}; }
};
template<class R>
constexpr auto operator|(R&& r, const adaptor_nfkd& a) { return a(std::forward<R>(r)); }
#endif // UNI_ALGO_TEST_RANGES_NORM

#if 0
/* TO_STRING */

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1206r7.pdf

struct adaptor_closure_to_string
{
    // TODO: clang -std=c++17 doesn't like constexpr here GCC/MSVC are fine
    template<class R>
    /*constexpr*/ auto operator()(R&& r) const
    {
        //using base_iterator_t = decltype(std::begin(std::declval<R&>())); // std::ranges::iterator_t<R>
        //static_assert(std::is_same_v<typename std::iterator_traits<base_iterator_t>::value_type, char32_t>);

        std::string result;
        std::back_insert_iterator output{result};
        for (char32_t c : r)
            detail::codepoint_to_utf8(c, output);
        return result;
    }
};
// TODO: clang -std=c++17 doesn't like constexpr here GCC/MSVC are fine
template<class R>
/*constexpr*/ auto operator|(R&& r, const adaptor_closure_to_string& a) { return a(std::forward<R>(r)); }

struct adaptor_to_string
{
    template<class R>
    constexpr auto operator()(R&& r) const { return adaptor_closure_to_string{}(r); }
    constexpr auto operator()() const { return adaptor_closure_to_string{}; }
};
#endif

/* TO_UTF8 */

// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1206r7.pdf

template<class Result>
struct adaptor_closure_to_utf8
{
    template<class R>
    constexpr auto operator()(R&& r) const
    {
        using base_iterator_t = decltype(std::begin(std::declval<R&>())); // std::ranges::iterator_t<R>
        using base_iterator_v = typename std::iterator_traits<base_iterator_t>::value_type; // std::ranges::range_value_t<R>
        using result_iterator_t = decltype(std::begin(std::declval<Result&>())); // std::ranges::iterator_t<Result>
        using result_iterator_v = typename std::iterator_traits<result_iterator_t>::value_type; // std::ranges::range_value_t<Result>

        // Technically we want this static_assert for base_iterator_v:
        // static_assert(std::is_same_v<base_iterator_v, char32_t>, "Must be char32_t range");
        // but it makes it a bit clanky to use with transform view so use more permissive static_assert
        // See: test/test_ranges.h -> test_ranges()
        static_assert(std::is_integral_v<base_iterator_v> && !std::is_same_v<base_iterator_v, bool>);
        static_assert(std::is_integral_v<result_iterator_v> && !std::is_same_v<result_iterator_v, bool>);

        Result result;
        std::back_insert_iterator output{result};
        for (char32_t c : r)
            detail::impl_utf8_output(c, output);
        return result;
    }
};
template<class R, class Result>
constexpr auto operator|(R&& r, const adaptor_closure_to_utf8<Result>& a) { return a(std::forward<R>(r)); }

/* TO_UTF16 */

template<class Result>
struct adaptor_closure_to_utf16
{
    template<class R>
    constexpr auto operator()(R&& r) const
    {
        using base_iterator_t = decltype(std::begin(std::declval<R&>())); // std::ranges::iterator_t<R>
        using base_iterator_v = typename std::iterator_traits<base_iterator_t>::value_type; // std::ranges::range_value_t<R>
        using result_iterator_t = decltype(std::begin(std::declval<Result&>())); // std::ranges::iterator_t<Result>
        using result_iterator_v = typename std::iterator_traits<result_iterator_t>::value_type; // std::ranges::range_value_t<Result>

        // See comments in to_utf8 adaptor above
        // static_assert(std::is_same_v<base_iterator_v, char32_t>, "Must be char32_t range");
        static_assert(std::is_integral_v<base_iterator_v> && !std::is_same_v<base_iterator_v, bool>);
        static_assert(std::is_integral_v<result_iterator_v> && sizeof(result_iterator_v) >= sizeof(char16_t));

        Result result;
        std::back_insert_iterator output{result};
        for (char32_t c : r)
            detail::impl_utf16_output(c, output);
        return result;
    }
};
template<class R, class Result>
constexpr auto operator|(R&& r, const adaptor_closure_to_utf16<Result>& a) { return a(std::forward<R>(r)); }

} // namespace detail

namespace views {

inline constexpr detail::adaptor_utf8 utf8;
inline constexpr detail::adaptor_utf16 utf16;
inline constexpr detail::adaptor_reverse reverse;
inline constexpr detail::adaptor_filter filter;
inline constexpr detail::adaptor_transform transform;
inline constexpr detail::adaptor_take take;
inline constexpr detail::adaptor_drop drop;
#ifdef UNI_ALGO_TEST_RANGES_NORM
namespace norm {
inline constexpr detail::adaptor_nfc nfc;
inline constexpr detail::adaptor_nfd nfd;
inline constexpr detail::adaptor_nfkc nfkc;
inline constexpr detail::adaptor_nfkd nfkd;
} // namespace norm
#endif // UNI_ALGO_TEST_RANGES_NORM

// In C++17 use our simple all view that uses our simple ref_view/owning_view
// In C++20 use facilities provided by the standard library
#if !defined(__cpp_lib_ranges) || defined(__clang__)
inline constexpr detail::adaptor_all all;
template<class Range>
using all_t = decltype(all(std::declval<Range>()));
#else
inline constexpr auto all = std::views::all;
template<class Range>
using all_t = std::views::all_t<Range>;
#endif

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

#ifdef UNI_ALGO_TEST_RANGES_NORM
namespace norm {
template<class Range>
nfc_view(Range&&) -> nfc_view<uni::views::all_t<Range>>;
template<class Range>
nfd_view(Range&&) -> nfd_view<uni::views::all_t<Range>>;
template<class Range>
nfkc_view(Range&&) -> nfkc_view<uni::views::all_t<Range>>;
template<class Range>
nfkd_view(Range&&) -> nfkd_view<uni::views::all_t<Range>>;
} // namespace norm
#endif // UNI_ALGO_TEST_RANGES_NORM

} // namespace ranges

namespace ranges {

//inline constexpr detail::adaptor_to_string to_string;
template<class Result>
using to_utf8 = detail::adaptor_closure_to_utf8<Result>;
template<class Result>
using to_utf16 = detail::adaptor_closure_to_utf16<Result>;

} // namespace ranges

// TODO: I messed up namespaces, views must be inside ranges and then use this:
//namespace views = uni::ranges::views;

} // namespace uni

#endif // CPP_UNI_RANGES_H_UAIX

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
