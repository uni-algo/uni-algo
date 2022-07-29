/* C++ Standart Library wrapper for Unicode Algorithms Implementation.
 * License: Public Domain or MIT - choose whatever you want.
 * See notice at the end of this file. */

#ifndef CPP_UNI_RANGES_H_UAIX
#define CPP_UNI_RANGES_H_UAIX

#ifdef UNI_ALGO_LOG_CPP_ITER
#include <iostream>
#endif

#include "cpp_uni_config.h"
#include "cpp_uni_version.h"
#include "internal/cpp_uni_ranges_core.h"

// TODO: Leave here the code for normalization ranges for now
// it must be moved to cpp_uni_norm.h later.
// All the normalization code protected with this define.
#ifdef UNI_ALGO_TEST_RANGES_NORM
#include "cpp_uni_norm.h"
#endif
// TODO: The same for break ranges
#ifdef UNI_ALGO_TEST_RANGES_BREAK
#include "cpp_uni_break_grapheme.h"
#include "cpp_uni_break_word.h"
#endif

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
            : parent{&p}, it_pos{begin}, it_next{begin}
        {
            if (begin != end)
                it_next = detail::impl_utf8_iter(it_next, end, &codepoint, Error);
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
            it_next = detail::impl_utf8_iter(it_next, std::end(parent->range), &codepoint, Error);
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
            it_pos = detail::impl_utf8_iter_rev(std::begin(parent->range), it_pos, &codepoint, Error);
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
    uaiw_constexpr utf8_view(Range r) : range{std::move(r)} {}
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
            : parent{&p}, it_pos{begin}, it_next{begin}
        {
            if (begin != end)
                it_next = detail::impl_utf16_iter(it_next, end, &codepoint, Error);
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
            it_next = detail::impl_utf16_iter(it_next, std::end(parent->range), &codepoint, Error);
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
            it_pos = detail::impl_utf16_iter_rev(std::begin(parent->range), it_pos, &codepoint, Error);
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
    uaiw_constexpr utf16_view(Range r) : range{std::move(r)} {}
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
            : parent{&p}, it_pos{end}
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
    uaiw_constexpr reverse_view(Range r) : range{std::move(r)} {}
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
            : parent{&p}, it_pos{begin}
        {
            if (begin != end)
                while(!parent->func_filter(*it_pos) && ++it_pos != end);
        }
        uaiw_constexpr reference operator*() const { return *it_pos; }
        uaiw_constexpr pointer operator->() const { return it_pos; }
        uaiw_constexpr filter& operator++()
        {
            if (it_pos != std::end(parent->range))
                while(++it_pos != std::end(parent->range) && !parent->func_filter(*it_pos));

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
                while (--it_pos != std::begin(parent->range) && !parent->func_filter(*it_pos));
#else
            do --it_pos;
            while (!parent->func_filter(*it_pos));
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
        uaiw_constexpr explicit transform(transform_view& p, Iter begin, Sent) : parent{&p}, it_pos{begin} {}
        uaiw_constexpr reference operator*() const { return parent->func_transform(*it_pos); }
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
        uaiw_constexpr explicit take(take_view& p, Iter begin, Sent, std::size_t n) : parent{&p}, it_pos{begin}, count{n} {}
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
            : parent{&p}, it_pos{begin}
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

#ifdef UNI_ALGO_TEST_RANGES_NORM
namespace norm {

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
    uaiw_constexpr nfc_view(Range r) : range{std::move(r)} {}
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
    uaiw_constexpr nfd_view(Range r) : range{std::move(r)} {}
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
    uaiw_constexpr nfkc_view(Range r) : range{std::move(r)} {}
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
    uaiw_constexpr nfkd_view(Range r) : range{std::move(r)} {}
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

} // namespace norm
#endif // UNI_ALGO_TEST_RANGES_NORM

#ifdef UNI_ALGO_TEST_RANGES_BREAK
namespace grapheme {

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

        void iter_func_grapheme_utf8()
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

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::basic_string_view<detail::ranges::iter_value_t<Iter>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        utf8() = default;
        explicit utf8(utf8_view& p, Iter begin, Sent end)
            : parent{&p}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_break_grapheme_state_reset(&state);

            iter_func_grapheme_utf8();
        }
        uaiw_constexpr reference operator*() const
        {
            return detail::ranges::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        utf8& operator++()
        {
            iter_func_grapheme_utf8();

            return *this;
        }
        utf8 operator++(int)
        {
            utf8 tmp = *this;
            operator++();
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
    uaiw_constexpr utf8_view(Range r) : range{std::move(r)} {}
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

        void iter_func_grapheme_utf16()
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

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::basic_string_view<detail::ranges::iter_value_t<Iter>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        utf16() = default;
        explicit utf16(utf16_view& p, Iter begin, Sent end)
            : parent{&p}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_break_grapheme_state_reset(&state);

            iter_func_grapheme_utf16();
        }
        uaiw_constexpr reference operator*() const
        {
            return detail::ranges::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        utf16& operator++()
        {
            iter_func_grapheme_utf16();

            return *this;
        }
        utf16 operator++(int)
        {
            utf16 tmp = *this;
            operator++();
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
    uaiw_constexpr utf16_view(Range r) : range{std::move(r)} {}
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

} // namespace grapheme

namespace word {

template<class Range>
class utf8_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class utf8
    {
        static_assert(detail::ranges::sa_iter_contiguous<Iter>::value &&
                      std::is_integral_v<detail::ranges::iter_value_t<Iter>>,
                      "word::utf8 view requires contiguous UTF-8 range");

    private:
        utf8_view* parent = nullptr;
        Iter it_begin;
        Iter it_pos;
        Iter it_next;
        detail::type_codept word_prop = 0;
        detail::type_codept next_word_prop = 0;

        detail::impl_break_word_state state{};

        void iter_func_word_utf8()
        {
            it_begin = it_pos;

            while (it_next != std::end(parent->range))
            {
                it_pos = it_next;
                word_prop = next_word_prop;
                detail::type_codept codepoint = 0;
                it_next = detail::inline_utf8_iter(it_next, std::end(parent->range), &codepoint, detail::impl_iter_replacement);
                if (detail::inline_utf8_break_word(&state, codepoint, &next_word_prop, it_next, std::end(parent->range)))
                    return;
            }

            if (it_next == std::end(parent->range))
            {
                it_pos = it_next;
                word_prop = next_word_prop;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::basic_string_view<detail::ranges::iter_value_t<Iter>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        utf8() = default;
        explicit utf8(utf8_view& p, Iter begin, Sent end)
            : parent{&p}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_break_word_state_reset(&state);

            iter_func_word_utf8();
        }
        uaiw_constexpr reference operator*() const
        {
            return detail::ranges::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        utf8& operator++()
        {
            iter_func_word_utf8();

            return *this;
        }
        utf8 operator++(int)
        {
            utf8 tmp = *this;
            operator++();
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
    uaiw_constexpr utf8_view(Range r) : range{std::move(r)} {}
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
                      "word::utf16 view requires contiguous UTF-16 range");

    private:
        utf16_view* parent = nullptr;
        Iter it_begin;
        Iter it_pos;
        Iter it_next;
        detail::type_codept word_prop = 0;
        detail::type_codept next_word_prop = 0;

        detail::impl_break_word_state state{};

        void iter_func_word_utf16()
        {
            it_begin = it_pos;

            while (it_next != std::end(parent->range))
            {
                it_pos = it_next;
                word_prop = next_word_prop;
                detail::type_codept codepoint = 0;
                it_next = detail::inline_utf16_iter(it_next, std::end(parent->range), &codepoint, detail::impl_iter_replacement);
                if (detail::inline_utf16_break_word(&state, codepoint, &next_word_prop, it_next, std::end(parent->range)))
                    return;
            }

            if (it_next == std::end(parent->range))
            {
                it_pos = it_next;
                word_prop = next_word_prop;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::basic_string_view<detail::ranges::iter_value_t<Iter>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        utf16() = default;
        explicit utf16(utf16_view& p, Iter begin, Sent end)
            : parent{&p}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_break_word_state_reset(&state);

            iter_func_word_utf16();
        }
        uaiw_constexpr reference operator*() const
        {
            return detail::ranges::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        utf16& operator++()
        {
            iter_func_word_utf16();

            return *this;
        }
        utf16 operator++(int)
        {
            utf16 tmp = *this;
            operator++();
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
    uaiw_constexpr utf16_view(Range r) : range{std::move(r)} {}
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

} // namespace word

namespace word_only {

template<class Range>
class utf8_view : public detail::ranges::view_base
{
private:
    template<class Iter, class Sent>
    class utf8
    {
        static_assert(detail::ranges::sa_iter_contiguous<Iter>::value &&
                      std::is_integral_v<detail::ranges::iter_value_t<Iter>>,
                      "word_only::utf8 view requires contiguous UTF-8 range");

    private:
        utf8_view* parent = nullptr;
        Iter it_begin;
        Iter it_pos;
        Iter it_next;
        detail::type_codept word_prop = 0;
        detail::type_codept next_word_prop = 0;

        detail::impl_break_word_state state{};

        void iter_func_word_only_utf8()
        {
            it_begin = it_pos;

            while (it_next != std::end(parent->range))
            {
                it_pos = it_next;
                word_prop = next_word_prop;
                detail::type_codept codepoint = 0;
                it_next = detail::inline_utf8_iter(it_next, std::end(parent->range), &codepoint, detail::impl_iter_replacement);
                if (detail::inline_utf8_break_word(&state, codepoint, &next_word_prop, it_next, std::end(parent->range)))
                {
                    if (detail::impl_break_is_word(word_prop))
                        return;

                    it_begin = it_pos;
                    continue;
                }
            }

            if (it_next == std::end(parent->range))
            {
                it_pos = it_next;
                word_prop = next_word_prop;
                if (!detail::impl_break_is_word(word_prop))
                    it_begin = it_next;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::basic_string_view<detail::ranges::iter_value_t<Iter>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        utf8() = default;
        explicit utf8(utf8_view& p, Iter begin, Sent end)
            : parent{&p}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_break_word_state_reset(&state);

            iter_func_word_only_utf8();
        }
        uaiw_constexpr reference operator*() const
        {
            return detail::ranges::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        utf8& operator++()
        {
            iter_func_word_only_utf8();

            return *this;
        }
        utf8 operator++(int)
        {
            utf8 tmp = *this;
            operator++();
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
    uaiw_constexpr utf8_view(Range r) : range{std::move(r)} {}
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
                      "word_only::utf16 view requires contiguous UTF-16 range");

    private:
        utf16_view* parent = nullptr;
        Iter it_begin;
        Iter it_pos;
        Iter it_next;
        detail::type_codept word_prop = 0;
        detail::type_codept next_word_prop = 0;

        detail::impl_break_word_state state{};

        void iter_func_word_only_utf16()
        {
            it_begin = it_pos;

            while (it_next != std::end(parent->range))
            {
                it_pos = it_next;
                word_prop = next_word_prop;
                detail::type_codept codepoint = 0;
                it_next = detail::inline_utf16_iter(it_next, std::end(parent->range), &codepoint, detail::impl_iter_replacement);
                if (detail::inline_utf16_break_word(&state, codepoint, &next_word_prop, it_next, std::end(parent->range)))
                {
                    if (detail::impl_break_is_word(word_prop))
                        return;

                    it_begin = it_pos;
                    continue;
                }
            }

            if (it_next == std::end(parent->range))
            {
                it_pos = it_next;
                word_prop = next_word_prop;
                if (!detail::impl_break_is_word(word_prop))
                    it_begin = it_next;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = std::basic_string_view<detail::ranges::iter_value_t<Iter>>;
        using pointer           = void;
        using reference         = value_type;
        using difference_type   = detail::ranges::iter_difference_t<Iter>;

        utf16() = default;
        explicit utf16(utf16_view& p, Iter begin, Sent end)
            : parent{&p}, it_begin{begin}, it_pos{begin}, it_next{begin}
        {
            if (begin == end)
                return;

            detail::impl_break_word_state_reset(&state);

            iter_func_word_only_utf16();
        }
        uaiw_constexpr reference operator*() const
        {
            return detail::ranges::to_string_view<reference>(parent->range, it_begin, it_pos);
        }
        uaiw_constexpr Iter begin() const noexcept { return it_begin; }
        uaiw_constexpr Iter end() const noexcept { return it_pos; }
        utf16& operator++()
        {
            iter_func_word_only_utf16();

            return *this;
        }
        utf16 operator++(int)
        {
            utf16 tmp = *this;
            operator++();
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
    uaiw_constexpr utf16_view(Range r) : range{std::move(r)} {}
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

} // namespace word_only
#endif // UNI_ALGO_TEST_RANGES_BREAK

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
    uaiw_constexpr adaptor_closure_filter(Pred pred) : p{pred} {}
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
    uaiw_constexpr adaptor_closure_drop(std::size_t n): count(n) {}

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
    uaiw_constexpr adaptor_closure_take(std::size_t n): count(n) {}

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
    uaiw_constexpr adaptor_closure_transform(Func func) : f{func} {}
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

#ifdef UNI_ALGO_TEST_RANGES_NORM
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
#endif // UNI_ALGO_TEST_RANGES_NORM

#ifdef UNI_ALGO_TEST_RANGES_BREAK
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

/* WORD_UTF8_VIEW */

struct adaptor_word_utf8
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::word::utf8_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_word_utf8& a) { return a(std::forward<R>(r)); }

/* WORD_UTF16_VIEW */

struct adaptor_word_utf16
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::word::utf16_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_word_utf16& a) { return a(std::forward<R>(r)); }

/* WORD_ONLY_UTF8_VIEW */

struct adaptor_word_only_utf8
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::word_only::utf8_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_word_only_utf8& a) { return a(std::forward<R>(r)); }

/* WORD_ONLY_UTF16_VIEW */

struct adaptor_word_only_utf16
{
    template<class R>
    uaiw_constexpr auto operator()(R&& r) const { return uni::ranges::word_only::utf16_view{std::forward<R>(r)}; }
};
template<class R>
uaiw_constexpr auto operator|(R&& r, const adaptor_word_only_utf16& a) { return a(std::forward<R>(r)); }
#endif // UNI_ALGO_TEST_RANGES_BREAK

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
            detail::impl_utf8_output(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf8<Result>& a) { return a(std::forward<R>(r)); }

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
            detail::impl_utf16_output(static_cast<char32_t>(c), output);
        return result;
    }
};
template<class R, class Result>
uaiw_constexpr auto operator|(R&& r, const adaptor_closure_to_utf16<Result>& a) { return a(std::forward<R>(r)); }

} // namespace detail

namespace ranges::views {

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
#ifdef UNI_ALGO_TEST_RANGES_BREAK
namespace grapheme {
inline constexpr detail::adaptor_grapheme_utf8 utf8;
inline constexpr detail::adaptor_grapheme_utf16 utf16;
}
namespace word {
inline constexpr detail::adaptor_word_utf8 utf8;
inline constexpr detail::adaptor_word_utf16 utf16;
}
namespace word_only {
inline constexpr detail::adaptor_word_only_utf8 utf8;
inline constexpr detail::adaptor_word_only_utf16 utf16;
}
#endif // UNI_ALGO_TEST_RANGES_BREAK

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

#ifdef UNI_ALGO_TEST_RANGES_BREAK
namespace grapheme {
template<class Range>
utf8_view(Range&&) -> utf8_view<uni::views::all_t<Range>>;
template<class Range>
utf16_view(Range&&) -> utf16_view<uni::views::all_t<Range>>;
}
namespace word {
template<class Range>
utf8_view(Range&&) -> utf8_view<uni::views::all_t<Range>>;
template<class Range>
utf16_view(Range&&) -> utf16_view<uni::views::all_t<Range>>;
}
namespace word_only {
template<class Range>
utf8_view(Range&&) -> utf8_view<uni::views::all_t<Range>>;
template<class Range>
utf16_view(Range&&) -> utf16_view<uni::views::all_t<Range>>;
}
#endif // UNI_ALGO_TEST_RANGES_BREAK

} // namespace ranges

namespace ranges {

template<class Result>
using to_utf8 = detail::adaptor_closure_to_utf8<Result>;
template<class Result>
using to_utf16 = detail::adaptor_closure_to_utf16<Result>;

} // namespace ranges

namespace views = ranges::views;

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
