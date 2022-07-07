// C++ proxy iterators example
// This example is usefull only if you need to write your own C++ wrapper
// or write a function that the current C++ wrapper does not provide.

// Note that it has nothing to do with high-level iterators from C++ wrapper: uni::iter::utf8/uni::iter::utf16
// so if you are not interested in the low-level implementation you can stop read here.

// Internal functions can work with most C++ iterators but sometimes proxy iterators are needed
// because these functions sometimes use operator-() to count a range, this range is always
// a return parameter and the range is not needed when you use C++ input or output iterators for example.
// So for a simple proxy iterator you need to overload operator-() that is all.

// For this example we will be using impl_convert.h module
#include "../impl_convert.h"

// Note that we use different namespace for user functions and do not add them to namespace uni.
// You must do it like this too if you want to extend functionality of the current C++ wrapper.
namespace uniex {

// Most internal functions look like this: impl_function(first, last, result)
// first, last is input iterators and result is output iterator

// Proxy iterator for the output iterator (result) look like this:

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
};

// And in most cases you don't need proxy iterator for input iterators (first, last)
// except converter functions that uses operator-() for error parameter.
// In this case, if you don't need error parameter for example you need to use
// converter functions for lineant conversion, proxy iterator look like this:
#if 1
template<class Iter>
class proxy_it_in
{
private:
    Iter it;
public:
    proxy_it_in() = default;
    explicit proxy_it_in(Iter iter) : it(iter) {}
    decltype(*it) operator*() const { return *it; }
    bool operator!=(const proxy_it_in& rhs) { return (it != rhs.it); }
    size_t operator-(const proxy_it_in&) { return 0; } // no-op
    proxy_it_in& operator++()
    {
        ++it;
        return *this;
    }
    proxy_it_in operator++(int)
    {
        proxy_it_in tmp = *this;
        operator++();
        return tmp;
    }
};
#endif
// And if you do need error parameter for example you need to use converter functions
// to convert a file with strict conversion and need to know the exact error location
// to the start of ill-formed sequence. Then proxy iterator look like this:
#if 0
template<class Iter>
class proxy_it_in
{
private:
    Iter it;
    size_t count = 0; // Need to count code units to know an error location
public:
    proxy_it_in() = default;
    explicit proxy_it_in(Iter iter) : it(iter) {}
    decltype(*it) operator*() const { return *it; }
    bool operator!=(const proxy_it_in& rhs) { return (it != rhs.it); }
    size_t operator-(const proxy_it_in& rhs) { return (count - rhs.count); } // Return the error location
    proxy_it_in& operator++()
    {
        ++count;
        ++it;
        return *this;
    }
    proxy_it_in operator++(int)
    {
        proxy_it_in tmp = *this;
        operator++();
        return tmp;
    }
};
#endif

// Don't ever use these proxy iterators for anything other than internal functions.
// The functions are designed to work this way.

// Let's use these proxy iterators to write a function that convert a file from UTF-8 to UTF-16
void example_fstream_and_back_inserter()
{
    // In this example we will be using test_utf8_broken.txt file from our tests.

    std::ifstream stream;
    stream.open("test_utf8_broken.txt", std::ios::binary);
    assert(stream.is_open());
    if (!stream.is_open())
        return;

    // Use proxy iterator for input iterators
    proxy_it_in<std::istreambuf_iterator<char>> it_in(stream.rdbuf());
    proxy_it_in<std::istreambuf_iterator<char>> it_in_end;

    std::vector<uint16_t> vec16;
    // Use proxy iterator for output iterator
    proxy_it_out<std::back_insert_iterator<std::vector<uint16_t>>> it_out(std::back_inserter(vec16));

    // Note that this function now always returns 0 because operator-() is no-op
    // but with back_inserter we don't need the result anyway so it's fine.
    uni::detail::impl_utf8to16(it_in, it_in_end, it_out, nullptr);

    stream.close();

    // Binary write our vector
    std::ofstream stream_out;
    stream_out.open("test_utf8to16_fixed.txt", std::ios::binary);
    stream_out.write(reinterpret_cast<const char*>(&vec16[0]), vec16.size() * sizeof(uint16_t));
    stream_out.close();

    // Now we have a valid UTF-16 file.
    // It must be exactly the same as test_utf8to16_result.txt file from tests.
}

// The same way you can make internal functions to work with a sentinel
// for example for null terminated strings to avoid counting the length, just overload operator!=()

template<class Iter, class Sent>
class proxy_it_in_sentinel
{
private:
    Iter it;
public:
    proxy_it_in_sentinel() = default;
    explicit proxy_it_in_sentinel(Iter iter) : it(iter) {}
    decltype(*it) operator*() const { return *it; }
    bool operator!=(const Sent& sentinel) { return *it != sentinel; } // sentinel
    size_t operator-(const proxy_it_in_sentinel&) { return 1; } // no-op but make it return smth to avoid incorrect usage
    proxy_it_in_sentinel& operator++()
    {
        ++it;
        return *this;
    }
    proxy_it_in_sentinel operator++(int)
    {
        proxy_it_in_sentinel tmp = *this;
        operator++();
        return tmp;
    }
};

void example_sentinel()
{
    const char str8[] = "\xD0\x90\xD0\x91\xD0\x92"; // Random UTF-8 string

    proxy_it_in_sentinel<decltype(std::cbegin(str8)), char> it_in(std::cbegin(str8));

    std::u32string str32;
    proxy_it_out<std::back_insert_iterator<std::u32string>> it_out(std::back_inserter(str32));

    // And for sentinel we can just use null character
    // Note that with this sentinel proxy iterator we don't need to count the length of str8 anymore
    uni::detail::impl_utf8to32(it_in, '\0', it_out, nullptr);

    if (str32 == U"\x0410\x0411\x0412")
        std::cout << "Sentinel test" << '\n';
}

// Also we can create "iterators" that not a proxy to something
// but can do things on their own to achieve something else
// especially when used together with converter functions

// Let's make no-op "iterator" to be able to just validate something

class it_out_noop
{
public:
    it_out_noop& operator*() { return *this; } // no-op
    size_t operator-(const it_out_noop&) { return 0; } // no-op
    it_out_noop& operator++(int) { return *this; } // no-op
    void operator=(char32_t) {} // no-op
};

void example_validate()
{
    std::string str8 = "\xD0\x90\xD0\x91\xD0\x92\x80"; // UTF8 string with invalid code unit at the end

    it_out_noop out_noop;
    size_t error = 0;
    uni::detail::impl_utf8to32(str8.cbegin(), str8.cend(), out_noop, &error);
    if (error)
        std::cout << "UTF-8 string is ill-formed" << '\n';
    else
        std::cout << "UTF-8 string is well-formed" << '\n';

    // Also it can be used together with proxy_it_in to validate a file with fstream
}

// Count "iterator" to count code points
// Also it can be used to count the number of code units needed for an output string

class it_out_count
{
private:
    size_t count = 0;
public:
    it_out_count& operator*() { return *this; } // no-op
    void operator=(char32_t) {} // no-op
    size_t operator-(const it_out_count& rhs) { return (count - rhs.count); }
    it_out_count& operator++(int)
    {
        count++;
        return *this;
    }
};

void example_count()
{
    std::string str8 = "\xD0\x90\xD0\x91\xD0\x92";

    it_out_count out_count;
    size_t count = uni::detail::impl_utf8to32(str8.cbegin(), str8.cend(), out_count, nullptr);
    std::cout << "Number of code points: " << count << '\n';
}

// And some useless "iterator" that can print code points and count them in a bit different way

class it_out32_print
{
private:
    size_t& count;
public:
    it_out32_print(size_t& c) : count(c) {}
    it_out32_print& operator*() { return *this; } // no-op
    size_t operator-(const it_out32_print&) { return 0; } // no-op
    it_out32_print& operator++(int) { return *this; } // no-op
    void operator=(char32_t codepoint)
    {
        std::cout << codepoint << '\n'; // Print code point
        count++; // And count the number of code points too
    }
};

void example_print()
{
    std::string str8 = "\xD0\x90\xD0\x91\xD0\x92";
    size_t count = 0;
    it_out32_print out32_print(count);
    uni::detail::impl_utf8to32(str8.cbegin(), str8.cend(), out32_print, nullptr);
    std::cout << "Number of code points: " << count << '\n';
}

// The main reason to use this approach is to achieve the maximum performance
// because the "iterators" will be inlined into a function with no-op stuff removed.
// For example count "iterator" will be faster than std::distance(uni::iter::utf8, uni::iter::utf8)
// Also this approach is very good to add support for LE BE encoding schemes
// by swapping bits inside proxy iterators.
// Maybe something else.
// Of course it should never be used like this in a user code, only inside a wrapper.

} // namespace uniex

int main()
{
    // Test everything
    uniex::example_fstream_and_back_inserter();
    uniex::example_sentinel();
    uniex::example_validate();
    uniex::example_count();
    uniex::example_print();

    return 0;
}
