## Custom Allocator

This explains how to use a custom allocator with the library.

The library has strong guarantee about memory allocations this means a function always use the provided allocator
for all allocations any hidden allocations that use something other than the provided allocator are not allowed.
By default the library uses `std::allocator` for all allocations as long as a custom allocator for a specific function is not provided.<br>
Only functions that really need the memory allocation will allocate the memory and the default allocator for such functions can be changed
in all other cases the library will perform all operations on the stack.
Also note that range views never allocate the memory and always perform all operations on the stack.

The library uses `std::back_inserter` or `std::string::resize` (C++17, C++20) or `std::string::resize_and_overwrite` (C++23 or higher)
with `shrink_to_fit` call after for allocations. It is a good balance between the performance and the memory usage in most cases.
But for custom allocators when you want to achieve a better performance it can be a problem.
So with custom allocators when you want to control everything it can be better to use the library
with `UNI_ALGO_NO_SHRINK_TO_FIT` define and do `shrink_to_fit` call manually only when needed (see the difference in allocations in the examples below).<br>
Also note that only template functions support custom allocators.

```cpp
#include <iostream>
#include "uni_algo/case.h"

// Simple custom allocator that uses new/delete and reports allocated bytes
template<class T>
class my_alloc
{
public:
    using value_type = T;

    template <class U>
    my_alloc(const my_alloc<U>&) {}

    my_alloc() = default;
    T* allocate(std::size_t n)
    {
        T* p = new T[n];
        std::cout << "Alloc  : " << n << " bytes at " << static_cast<void*>(p) << '\n';
        return p;
    }
    void deallocate(T* p, std::size_t n)
    {
        std::cout << "Dealloc: " << n << " bytes at " << static_cast<void*>(p) << '\n';
        delete[] p;
    }
};

int main()
{
    my_alloc<char> alloc;

    using my_string_type = std::basic_string<char, std::char_traits<char>, decltype(alloc)>;

    // Use custom allocator with the uppercase function
    // Note that only template functions support custom allocators
    my_string_type result = una::cases::to_uppercase_utf8<char>("Long string that exceeds SSO", alloc);

    if (result == "LONG STRING THAT EXCEEDS SSO")
        std::cout << "OK" << '\n';
}

// Output:
// Alloc  : 85 bytes at 0x111111
// Alloc  : 29 bytes at 0x222222
// Dealloc: 85 bytes at 0x111111
// OK
// Dealloc: 29 bytes at 0x222222

// Output with UNI_ALGO_NO_SHRINK_TO_FIT define:
// Alloc  : 85 bytes at 0x111111
// OK
// Dealloc: 85 bytes at 0x111111
```

The similar way the library can be used with C++17 Polymorphic Memory Resources (PMR)
that will allow you to use the same string type (`std::pmr::string` and such) for all strings
with multiple custom allocators instead of introducing a new string type for every custom allocator like in the previous example.

```cpp
#include <memory_resource>
#include <iostream>
#include "uni_algo/case.h"

// Simple custom memory resource that uses new/delete and reports allocated bytes
class my_memory_resource : public std::pmr::memory_resource
{
    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
        void* p = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        std::cout << "Alloc  : " << bytes << " bytes at " << p << '\n';
        return p;
    }
    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override
    {
        std::cout << "Dealloc: " << bytes << " bytes at " << p << '\n';
        return std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        return std::pmr::new_delete_resource()->is_equal(other);
    }
};


int main()
{
    my_memory_resource mr;
    std::pmr::polymorphic_allocator<char> pa{&mr};

    std::pmr::string result = una::cases::to_uppercase_utf8<char>("Long string that exceeds SSO", pa);

    if (result == "LONG STRING THAT EXCEEDS SSO")
        std::cout << "OK" << '\n';
}

// Output:
// Alloc  : 85 bytes at 0x111111
// Alloc  : 29 bytes at 0x222222
// Dealloc: 85 bytes at 0x111111
// OK
// Dealloc: 29 bytes at 0x222222

// Output with UNI_ALGO_NO_SHRINK_TO_FIT define:
// Alloc  : 85 bytes at 0x111111
// OK
// Dealloc: 85 bytes at 0x111111
```
