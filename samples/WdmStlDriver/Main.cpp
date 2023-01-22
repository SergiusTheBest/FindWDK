#include <wdm.h>

#if _HAS_EXCEPTIONS
#error _HAS_EXCEPTIONS should be 0
#endif

#if !_KERNEL_MODE
#error _KERNEL_MODE should be not 0
#endif

///////////////////////////////////////////////////////////
// Implement CRT error reporting and STL checks 

extern "C" int _CrtDbgReport(
    _In_       int         /*_ReportType*/,
    _In_opt_z_ char const* /*_FileName*/,
    _In_       int         /*_Linenumber*/,
    _In_opt_z_ char const* /*_ModuleName*/,
    _In_opt_z_ char const* /*_Format*/,
    ...)
{
#pragma warning(suppress: 28159) // Consider using 'error logging or driver shutdown' instead of 'KeBugCheckEx'
    KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE, 0, 0, 0, 0);
}

namespace std
{
    [[noreturn]] void __cdecl _Xinvalid_argument(_In_z_ const char* /*What*/)
    {
#pragma warning(suppress: 28159) // Consider using 'error logging or driver shutdown' instead of 'KeBugCheckEx'
        KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE, 0, 0, 0, 0);
    }

    [[noreturn]] void __cdecl _Xlength_error(_In_z_ const char* /*What*/)
    {
#pragma warning(suppress: 28159) // Consider using 'error logging or driver shutdown' instead of 'KeBugCheckEx'
        KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE, 0, 0, 0, 0);
    }

    [[noreturn]] void __cdecl _Xout_of_range(_In_z_ const char* /*What*/)
    {
#pragma warning(suppress: 28159) // Consider using 'error logging or driver shutdown' instead of 'KeBugCheckEx'
        KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE, 0, 0, 0, 0);
    }
}

///////////////////////////////////////////////////////////
// STL headers and their statuses:
// 
// * without a comment - all/almost all functionality works
// * partially supported - some functionality works
// * not supported - no functionality works/not useful in the kernel

#include <algorithm>        // partially supported

// #include <any>           // not supported

#include <array>
#include <atomic>           // partially supported

// #include <barrier>       // not supported

#ifdef __cpp_lib_bitops
#include <bit>              // partially supported
#endif

#include <bitset>
#include <cassert>

// #include <ccomplex>      // not supported

#include <cctype>           // partially supported
#include <cerrno>

// #include <cfenv>         // not supported
// #include <cfloat>        // not supported

#ifdef __cpp_lib_to_chars
#include <charconv>
#endif

#pragma push_macro("_HAS_CXX20")
#undef _HAS_CXX20
#include <chrono>           // partially supported
#pragma pop_macro("_HAS_CXX20")

// #include <cinttypes>     // not supported
// #include <ciso646>       // not supported (deprecated)

#include <climits>

// #include <clocale>       // not supported
// #include <cmath>         // not supported
// #include <codecvt>       // not supported

#ifdef __cpp_lib_three_way_comparison
#include <compare>
#endif

// #include <complex>       // not supported

#ifdef __cpp_lib_concepts
#include <concepts>
#endif

// #include <condition_variable> // not supported
// #include <coroutine>     // not supported (needs /EHs-c- switch or exceptions support)
// #include <csetjmp>       // probably works (not tested)
// #include <csignal>       // not supported
// #include <cstdalign>     // not supported (deprecated)

#include <cstdarg>

// #include <cstdbool>      // not supported (deprecated)

#include <cstddef>
#include <cstdint>

// #include <cstdio>        // not supported

#include <cstdlib>          // partially supported
#include <cstring>          // partially supported

// #include <ctgmath>       // not supported (deprecated)
// #include <ctime>         // not supported
// #include <cuchar>        // not supported

#include <cwchar>           // partially supported
#include <cwctype>          // partially supported

// #include <deque>         // not supported
// #include <exception>     // not supported
// #include <execution>     // not supported

#ifdef __cpp_lib_expected
#include <expected>         // partially supported
#endif

// #include <filesystem>    // not supported
// #include <format>        // not supported
// #include <forward_list>  // not supported
// #include <fstream>       // not supported

#include <functional>       // partially supported

// #include <future>        // not supported
// #include <hash_map>      // not supported
// #include <hash_set>      // not supported

#include <initializer_list>

// #include <iomanip>       // not supported
// #include <ios>           // not supported
// #include <iosfwd>        // not supported
// #include <iostream>      // not supported
// #include <istream>       // not supported

#include <iterator>

// #include <latch>         // not supported

#include <limits>

// #include <list>          // not supported
// #include <locale>        // not supported
// #include <map>           // not supported

#include <memory>           // partially supported

// #include <memory_resource> // not supported
// #include <mutex>         // not supported

#include <new>              // partially supported

// #include <numbers>       // not supported

#include <numeric>

#ifdef __cpp_lib_optional
#include <optional>
#endif

// #include <ostream>       // not supported
// #include <queue>         // not supported

#include <random>           // partially supported

#ifdef __cpp_lib_ranges
#include <ranges>
#endif

#include <ratio>

// #include <regex>         // not supported
// #include <scoped_allocator> // not supported
// #include <semaphore>     // not supported
// #include <set>           // not supported
// #include <shared_mutex>  // not supported

#ifdef __cpp_lib_source_location
#include <source_location>
#endif

#ifdef __cpp_lib_span
#include <span>
#endif

// #include <spanstream>    // not supported
// #include <sstream>       // not supported
// #include <stack>         // not supported
// #include <stacktrace>    // not supported
// #include <stdexcept>     // not supported
// #include <stop_token>    // not supported
// #include <streambuf>     // not supported
// #include <string>        // not supported

#ifdef __cpp_lib_string_view	
#include <string_view>
#endif

// #include <strstream>     // not supported
// #include <syncstream>    // not supported
// #include <system_error>  // not supported
// #include <thread>        // not supported

#include <tuple>
#include <type_traits>

// #include <typeindex>     // not supported
// #include <typeinfo>      // not supported
// #include <unordered_map> // not supported
// #include <unordered_set> // not supported

#include <utility>

// #include <valarray>      // not supported

#ifdef __cpp_lib_variant
#include <variant>          // partially supported
#endif

// #include <vector>        // not supported

#include <version>

///////////////////////////////////////////////////////////
// Tests

#define CHECK_AND_RETURN(res) { __pragma(warning(suppress: 4127)); if (!(res)) return false; }

bool algorithmTest()
{
    const int arr[] = { 0, 1, 2, 3, 4 };
    int destArr[sizeof(arr)] = {};

    //////////////////////////////////////////////////////////////////////////
    // Non-modifying sequence operations

    CHECK_AND_RETURN(std::all_of(std::begin(arr), std::end(arr), [](auto& x) { return x >= 0; }));
    CHECK_AND_RETURN(std::any_of(std::begin(arr), std::end(arr), [](auto& x) { return x >= 0; }));
    CHECK_AND_RETURN(std::none_of(std::begin(arr), std::end(arr), [](auto& x) { return x < 0; }));
    std::for_each(std::begin(arr), std::end(arr), [](auto&) {});
    CHECK_AND_RETURN(std::count_if(std::begin(arr), std::end(arr), [](auto& x) { return x == 0; }) == 1);
    CHECK_AND_RETURN(std::find_if(std::begin(arr), std::end(arr), [](auto& x) { return x == 0; }) != std::end(arr));
    CHECK_AND_RETURN(std::search(std::begin(arr), std::end(arr), std::begin(arr), std::end(arr)) != std::end(arr));

    //////////////////////////////////////////////////////////////////////////
    // Modifying sequence operations

    CHECK_AND_RETURN(std::copy(std::begin(arr), std::end(arr), std::begin(destArr)) == std::begin(destArr) + std::size(arr));
    std::fill(std::begin(destArr), std::end(destArr), 0);
    std::transform(std::begin(arr), std::end(arr), std::begin(destArr), [](auto& x) { return x + 1; });
    std::generate(std::begin(destArr), std::end(destArr), []() { return 2; });
    std::reverse(std::begin(destArr), std::end(destArr));
    (void)std::unique(std::begin(destArr), std::end(destArr));

    //////////////////////////////////////////////////////////////////////////
    // Sorting operations

    std::sort(std::begin(destArr), std::end(destArr));
    CHECK_AND_RETURN(std::is_sorted(std::begin(destArr), std::end(destArr)));

    //////////////////////////////////////////////////////////////////////////
    // Binary search operations(on sorted ranges)

    CHECK_AND_RETURN(!std::binary_search(std::begin(destArr), std::end(destArr), 10));

    //////////////////////////////////////////////////////////////////////////
    // Minimum/maximum operations

    CHECK_AND_RETURN((std::max)({ 1, 3, 5 }) == 5);
    CHECK_AND_RETURN(*std::max_element(std::begin(arr), std::end(arr)) == 4);
    CHECK_AND_RETURN((std::min)({ 1, 3, 5 }) == 1);
    CHECK_AND_RETURN(*std::min_element(std::begin(arr), std::end(arr)) == 0);
    CHECK_AND_RETURN(*std::minmax(std::begin(arr), std::end(arr)).first == 0);

    //////////////////////////////////////////////////////////////////////////
    // Comparison operations

    (void)std::equal(std::begin(arr), std::end(arr), std::begin(destArr), std::end(destArr));

    return true;
}

bool arrayTest()
{
    std::array<int, 5> arr;
    arr[0] = 1;
    arr.at(4) = 0;
    CHECK_AND_RETURN(!arr.empty());
    CHECK_AND_RETURN(arr.size() == 5);
    CHECK_AND_RETURN(arr.front() == 1);
    CHECK_AND_RETURN(arr.back() == 0);
    CHECK_AND_RETURN(*arr.data() == 1);
    arr.fill(-1);

    std::array<int, 5> arr2 = { 1, 2, 3, 4, 5 };
    CHECK_AND_RETURN(arr != arr2);

#ifdef __cpp_lib_to_array
    auto arr3 = std::to_array("foo");
    CHECK_AND_RETURN(arr3.size() == 4);
#endif

#ifdef __cpp_deduction_guides
    std::array arr4 = { 1, 2, 3, 4, 5 };
    CHECK_AND_RETURN(arr4.size() == 5);
#endif

    return true;
}

bool atomicTest()
{
    std::atomic_int i1;
    i1++;

    std::atomic<long long> l1;
    long long l2 = l1;
    l1 = l2;
    l1.store(l2);
    l1.compare_exchange_strong(l2, 10);
    std::atomic_exchange(&l1, l2);

    std::atomic_flag fl;
    fl.clear();
    fl.test_and_set();

    // not supported
    // 
    // fl.notify_one();
    // fl.wait(true);

    return true;
}

bool bitTest()
{
#ifdef __cpp_lib_bitops
    static_assert(std::endian::native == std::endian::little);

    struct char2 { char a; char b; };
    auto casted = std::bit_cast<short>(char2{});
    UNREFERENCED_PARAMETER(casted);

    unsigned int val = 2;
    CHECK_AND_RETURN(std::has_single_bit(val));
    //CHECK_AND_RETURN(popcount(val) == 1); // not supported
#endif

    return true;
}

bool bitsetTest()
{
    std::bitset<100> bset;

    bset[10] = true;
    CHECK_AND_RETURN(bset.test(10));
    CHECK_AND_RETURN(bset.any());
    CHECK_AND_RETURN(!bset.all());
    CHECK_AND_RETURN(!bset.none());
    bset.flip(20);
    //CHECK_AND_RETURN(bset.count() == 2); // not supported

    return true;
}

bool cassertTest()
{
    assert(true);

    return true;
}

bool cctypeTest()
{
    //CHECK_AND_RETURN(std::isalnum('Y')); // not supported
    //CHECK_AND_RETURN(std::isalpha('Y')); // not supported
    CHECK_AND_RETURN(std::islower('y'));
    CHECK_AND_RETURN(std::isupper('Y'));
    CHECK_AND_RETURN(std::isdigit('0'));
    CHECK_AND_RETURN(std::isxdigit('A'));
    CHECK_AND_RETURN(std::tolower('Y') == 'y');
    CHECK_AND_RETURN(std::toupper('y') == 'Y');

    return true;
}

bool cerrnoTest()
{
    int errors[] = { EACCES, EINVAL, ENOMEM, ENOTSUP };

    return true;
}

bool charconvTest()
{
#ifdef __cpp_lib_to_chars
    const char arr[] = "100";

    int res;
    std::from_chars(std::begin(arr), std::end(arr), res);
    CHECK_AND_RETURN(res == 100);

    char buff[10] = {};
    std::to_chars(std::begin(buff), std::end(buff), res);
#endif

    return true;
}

bool chronoTest()
{
    // clocks, time point - not supported

    using namespace std::literals::chrono_literals;

    std::chrono::milliseconds t1(15);

    auto t2 = 10ns;
    CHECK_AND_RETURN(t1 > t2);

    return true;
}

bool climitsTest()
{
    CHECK_AND_RETURN(CHAR_BIT == 8);
    CHECK_AND_RETURN(INT_MAX == 0x7fffffff);

    return true;
}

bool compareTest()
{
#ifdef __cpp_lib_three_way_comparison
    struct C
    {
        auto operator<=>(const C& other) const
        {
            if (value > other.value)
            {
                return std::weak_ordering::greater;
            }
            else if (value < other.value)
            {
                return std::weak_ordering::less;
            }

            return std::weak_ordering::equivalent;
        }

        int value;
    };

    CHECK_AND_RETURN(C{ 1 } < C{ 2 });
    CHECK_AND_RETURN(C{ 2 } > C{ 1 });
    CHECK_AND_RETURN(C{ 1 } >= C{ 1 });
    CHECK_AND_RETURN(C{ 1 } <= C{ 2 });
#endif

    return true;
}

#ifdef __cpp_lib_concepts
auto conceptFunc1(std::integral auto i) { return i + 1; }
auto conceptFunc1(std::same_as<nullptr_t> auto p) { return p; }

template <typename T>
concept hasStart = requires(T c) { c.start(); };
#endif

bool conceptTest()
{
#ifdef __cpp_lib_concepts
    struct A
    {
        void foo();
    };

    struct B
    {
        void start();
    };

    CHECK_AND_RETURN(conceptFunc1(10) == 11);
    CHECK_AND_RETURN(conceptFunc1(nullptr) == nullptr);
    CHECK_AND_RETURN(!hasStart<A>);
    CHECK_AND_RETURN(hasStart<B>);
#endif

    return true;
}

#if 0 // needs /EHs-c- switch or exceptions support
int coroutineTest()
{
    // temp buffer instead of real memory allocation
    static char buf[100];

    struct task
    {
        struct promise_type
        {
            // using class operators new/delete
            void* __cdecl operator new(size_t) noexcept { return buf; }
            void __cdecl operator delete(void*) noexcept {}

            task get_return_object() noexcept { return {}; }
            std::suspend_never initial_suspend() noexcept { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void return_void() noexcept {}
            void unhandled_exception() noexcept {}
        };
    };

    struct awaitable
    {
        bool await_ready() noexcept { return false; }
        bool await_suspend(std::coroutine_handle<>) noexcept { return false; }
        void await_resume() noexcept {}
    };

    auto asyncRoutine = []() noexcept -> awaitable { return {}; };
    auto coroutine = [&]() noexcept -> task { co_await asyncRoutine(); };

    coroutine();

    return true;
}
#endif

bool cstdargsTest()
{
    auto vsum = [](int count, ...)
    {
        int result = 0;

        std::va_list args;
        va_start(args, count);

        for (int i = 0; i < count; ++i)
        {
            result += va_arg(args, int);
        }

        va_end(args);

        return result;
    };

    CHECK_AND_RETURN(vsum(4, 1, 2, 3, 4) == 1 + 2 + 3 + 4);

    return true;
}

bool cstddefTest()
{
#ifdef __cpp_lib_byte
    [](std::byte) {};
#endif

    [](std::size_t, std::ptrdiff_t, std::nullptr_t, std::max_align_t) {};

    struct A
    {
        int a;
    };

    CHECK_AND_RETURN(offsetof(A, a) == 0);

    return true;
}

bool cstdintTest()
{
    [](std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::intptr_t) {};
    [](std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, std::uintptr_t) {};

    return true;
}

bool cstdlibTest()
{
    CHECK_AND_RETURN(std::atoi("-58") == -58);
    CHECK_AND_RETURN(std::atol("-58") == -58);

    int arr[] = { -2, 99, 0, -743, INT_MAX, 2, INT_MIN, 4 };

    std::qsort(
        std::data(arr),
        std::size(arr),
        sizeof(arr[0]),
        [](const void* x, const void* y)
        {
            const int arg1 = *static_cast<const int*>(x);
            const int arg2 = *static_cast<const int*>(y);

            if (arg1 > arg2)
            {
                return 1;
            }
            else if (arg1 < arg2)
            {
                return -1;
            }

            return 0;
        });
    CHECK_AND_RETURN(std::is_sorted(std::begin(arr), std::end(arr)));

    return true;
}

bool cstringTest()
{
    const char str[] = "hello";

    char buf2[sizeof(str)];

#pragma warning(suppress: 4996) // This function or variable may be unsafe.Consider using xxxx_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    std::strcpy(buf2, str);
    CHECK_AND_RETURN(buf2[0] == 'h' && buf2[1] == 'e');

#pragma warning(suppress: 4996) // This function or variable may be unsafe.Consider using xxxx_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    std::strncpy(buf2, "test", 3);
    CHECK_AND_RETURN(buf2[0] == 't' && buf2[1] == 'e');

    char buf3[10] = "ab";

#pragma warning(suppress: 4996) // This function or variable may be unsafe.Consider using xxxx_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    std::strcat(buf3, "cd");
    CHECK_AND_RETURN(buf3[0] == 'a' && buf3[3] == 'd');

#pragma warning(suppress: 4996) // This function or variable may be unsafe.Consider using xxxx_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    std::strncat(buf3, "efg", 1);
    CHECK_AND_RETURN(buf3[4] == 'e' && buf3[5] == 0);

    CHECK_AND_RETURN(std::strlen(str) == sizeof(str) -1);
    CHECK_AND_RETURN(std::strcmp(str, str) == 0);
    CHECK_AND_RETURN(std::strncmp(str, str, 2) == 0);
    CHECK_AND_RETURN(std::strchr(str, 'e') == str + 1);
    CHECK_AND_RETURN(std::strrchr(str, 'l') == str + 3);
    CHECK_AND_RETURN(std::strspn(str, "eh") == 2);
    CHECK_AND_RETURN(std::strstr(str, "llo") == str + 2);

    CHECK_AND_RETURN(std::memchr(str, 'e', sizeof(str)) == str + 1);
    CHECK_AND_RETURN(std::memcmp(str, str, sizeof(str)) == 0);

    char buf[] = "11111";
    std::memset(buf, 0, sizeof(buf));
    CHECK_AND_RETURN(buf[0] == 0 && buf[1] == 0);

    std::memcpy(buf, str, 2);
    CHECK_AND_RETURN(buf[0] == 'h' && buf[1] == 'e');

    std::memmove(buf, buf + 1, 2);
    CHECK_AND_RETURN(buf[0] == 'e' && buf[1] == 0);

    return true;
}

bool cwcharTest()
{
    const wchar_t str[] = L"hello";

    wchar_t buf2[sizeof(str)];

#pragma warning(suppress: 4996) // This function or variable may be unsafe.Consider using xxxx_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    std::wcscpy(buf2, str);
    CHECK_AND_RETURN(buf2[0] == L'h' && buf2[1] == L'e');

#pragma warning(suppress: 4996) // This function or variable may be unsafe.Consider using xxxx_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    std::wcsncpy(buf2, L"test", 3);
    CHECK_AND_RETURN(buf2[0] == L't' && buf2[1] == L'e');

    wchar_t buf3[10] = L"ab";

#pragma warning(suppress: 4996) // This function or variable may be unsafe.Consider using xxxx_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    std::wcscat(buf3, L"cd");
    CHECK_AND_RETURN(buf3[0] == L'a' && buf3[3] == L'd');

#pragma warning(suppress: 4996) // This function or variable may be unsafe.Consider using xxxx_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.
    std::wcsncat(buf3, L"efg", 1);
    CHECK_AND_RETURN(buf3[4] == L'e' && buf3[5] == 0);

    CHECK_AND_RETURN(std::wcslen(str) == sizeof(str) / 2 - 1);
    CHECK_AND_RETURN(std::wcscmp(str, str) == 0);
    CHECK_AND_RETURN(std::wcsncmp(str, str, 2) == 0);
    CHECK_AND_RETURN(std::wcschr(str, L'e') == str + 1);
    CHECK_AND_RETURN(std::wcsrchr(str, L'l') == str + 3);
    CHECK_AND_RETURN(std::wcsspn(str, L"eh") == 2);
    CHECK_AND_RETURN(std::wcsstr(str, L"llo") == str + 2);

    CHECK_AND_RETURN(std::wmemchr(str, L'e', sizeof(str) / 2) == str + 1);
    CHECK_AND_RETURN(std::wmemcmp(str, str, sizeof(str) / 2) == 0);

    wchar_t buf[] = L"11111";
    std::wmemset(buf, L'\0', sizeof(buf) / 2);
    CHECK_AND_RETURN(buf[0] == 0 && buf[1] == 0);

    std::wmemcpy(buf, str, 2);
    CHECK_AND_RETURN(buf[0] == 'h' && buf[1] == 'e');

    std::wmemmove(buf, buf + 1, 2);
    CHECK_AND_RETURN(buf[0] == 'e' && buf[1] == 0);

    return true;
}

bool cwctypeTest()
{
    //CHECK_AND_RETURN(std::iswalnum(L'Y')); // supported only on Windows 10
    //CHECK_AND_RETURN(std::iswalpha(L'Y')); // not supported
    //CHECK_AND_RETURN(std::iswlower(L'y')); // not supported
    //CHECK_AND_RETURN(std::iswupper(L'Y')); // not supported
    //CHECK_AND_RETURN(std::iswdigit(L'0')); // supported only on Windows 10
    //CHECK_AND_RETURN(std::iswxdigit(L'A')); // not supported
    CHECK_AND_RETURN(std::towlower(L'Y') == L'y');
    CHECK_AND_RETURN(std::towupper(L'y') == L'Y');

    return true;
}

bool initializer_listTest()
{
    std::initializer_list<int> il{ 1, 2, 3 };

    CHECK_AND_RETURN(il.size() == 3);

    int sum = 0;
    for (int i : il)
    {
        sum += i;
    }
    CHECK_AND_RETURN(sum == 1 + 2 + 3);

    return true;
}

bool iteratorTest()
{
    int arr[] = { 1, 2, 3, 4, 5 };

    CHECK_AND_RETURN(std::distance(std::begin(arr), std::end(arr)) == 5);

    auto it = std::begin(arr);
    std::advance(it, 2);
    CHECK_AND_RETURN(*it == 3);
    CHECK_AND_RETURN(*std::prev(it) == 2);

    return true;
}

bool expectedTest()
{
#ifdef __cpp_lib_expected
    auto good = []() -> std::expected<int, NTSTATUS>
    {
        return 42;
    };

    auto bad = []() -> std::expected<int, NTSTATUS>
    {
        return std::unexpected(STATUS_INVALID_PARAMETER);
    };

    auto result = good();
    CHECK_AND_RETURN(result);
    CHECK_AND_RETURN(result.has_value());
    // CHECK_AND_RETURN(result.value() == 42); // not supported
    CHECK_AND_RETURN(*result == 42);
    CHECK_AND_RETURN(result.value_or(0) == 42);

    result = bad();
    CHECK_AND_RETURN(!result);
    CHECK_AND_RETURN(!result.has_value());
    CHECK_AND_RETURN(result.error() == STATUS_INVALID_PARAMETER);
#endif

    return true;
}

bool functionalTest()
{
    // function, boyer_moore_searcher - not supported

    auto hashValue = std::hash<int>()(100);
    CHECK_AND_RETURN(hashValue != 0);

    CHECK_AND_RETURN(std::less<int>()(1, 5));

#ifdef __cpp_lib_not_fn
    CHECK_AND_RETURN(!std::not_fn(std::less<int>())(1, 5));
#endif

    return true;
}

bool limitsTest()
{
    CHECK_AND_RETURN((std::numeric_limits<int>::max)() == 0x7fffffff);
    CHECK_AND_RETURN((std::numeric_limits<int>::min)() == 0x80000000);

    return true;
}

bool memoryTest()
{
    // shared_ptr, weak_ptr - not supported

    bool deleterCalled = false;
    auto deleter = [&](void*) { deleterCalled = true; };

    {
        std::unique_ptr<void, decltype(deleter)> uptr(reinterpret_cast<void*>(0x1234), deleter);
        CHECK_AND_RETURN(uptr);
    }

    CHECK_AND_RETURN(deleterCalled);

    return true;
}

bool newTest()
{
    static bool constructorCalled = false;
    static bool destructorCalled = false;

    struct C
    {
        C() { constructorCalled = true; }
        ~C() { destructorCalled = true; }
    };

    char buf[sizeof(C)];

    C* c = new(buf) C; // call built-in placement new
    //c->~C(); // unfortunately you should define placement delete to uncomment this line

    CHECK_AND_RETURN(c == static_cast<void*>(buf));
    CHECK_AND_RETURN(constructorCalled);
    //CHECK_AND_RETURN(destructorCalled);

    return true;
}

bool numericTest()
{
    int arr[5] = {};

    std::iota(std::begin(arr), std::end(arr), 1);
    CHECK_AND_RETURN(std::accumulate(std::begin(arr), std::end(arr), 0) == 1 + 2 + 3 + 4 + 5);

#if _HAS_CXX17
    CHECK_AND_RETURN(std::reduce(std::begin(arr), std::end(arr)) == 1 + 2 + 3 + 4 + 5);
    CHECK_AND_RETURN(std::reduce(std::begin(arr), std::end(arr), 1, std::multiplies<int>{}) == 1 * 2 * 3 * 4 * 5);
#endif

    return true;
}

bool optionalTest()
{
#ifdef __cpp_lib_optional
    std::optional<int> op(10);

    CHECK_AND_RETURN(op.has_value());
    CHECK_AND_RETURN(op == 10);

    op = std::nullopt;
#endif

    return true;
}

bool randomTest()
{
    std::mt19937 gen(123456);
    std::uniform_int_distribution<> distrib(1, 6);

    int total = 0;
    const int count = 100;
    for (int i = 0; i < count; ++i)
    {
        total += distrib(gen);
    }

    int avg = total / count;
    CHECK_AND_RETURN(avg == (1 + 6) / 2);

    return true;
}

bool rangesTest()
{
#ifdef __cpp_lib_ranges
    const int ints[] = { 0, 1, 2, 3, 4, 5 };

    int sum = 0;
    std::ranges::for_each(ints, [&](auto x) { sum += x; });
    CHECK_AND_RETURN(sum == 0 + 1 + 2 + 3 + 4 + 5);

    auto even = [](int i) { return 0 == i % 2; };
    auto square = [](int i) { return i * i; };

    int sum2 = 0;
    for (int i : ints | std::views::filter(even) | std::views::transform(square))
    {
        sum2 += i;
    }
    CHECK_AND_RETURN(sum2 == 2 * 2 + 4 * 4);

    int sum3 = 0;
    for (int i : ints | std::views::drop(2) | std::views::take(3))
    {
        sum3 += i;
    }
    CHECK_AND_RETURN(sum3 == 2 + 3 + 4);

    int sum4 = 0;
    for (int i : std::views::iota(3, 5))
    {
        sum4 += i;
    }
    CHECK_AND_RETURN(sum4 == 3 + 4);

    std::string_view text = "Lorem ipsum dolor sit amet";
    auto count = std::ranges::distance(text | std::views::split(' '));
    CHECK_AND_RETURN(count == 5);
#endif

    return true;
}

bool ratioTest()
{
    using two_third = std::ratio<2, 3>;
    using one_sixth = std::ratio<1, 6>;

    using sum = std::ratio_add<two_third, one_sixth>;
    CHECK_AND_RETURN(sum::num == 5);
    CHECK_AND_RETURN(sum::den == 6);

    return true;
}

bool source_locationTest()
{
#ifdef __cpp_lib_source_location
    auto loc = std::source_location::current();
    (void)loc.function_name();
#endif

    return true;
}

bool spanTest()
{
#ifdef __cpp_lib_span
    int arr[] = { 1, 2, 3, 4, 5 };
    std::span<int> sp1(arr);

    CHECK_AND_RETURN(sp1.size() == 5);
    CHECK_AND_RETURN(sp1.size_bytes() == 5 * sizeof(int));
    CHECK_AND_RETURN(sp1.first(3).size() == 3);
    CHECK_AND_RETURN(sp1.subspan(3).size() == 2);
    CHECK_AND_RETURN(sp1[0] == 1);
    CHECK_AND_RETURN(*sp1.data() == 1);

    std::span<const std::byte> sp2 = std::as_bytes(sp1);
    std::span<const std::byte, 2> sp3 = sp2.first<2>();
#endif

    return true;
}

bool string_viewTest()
{
#ifdef __cpp_lib_string_view	
    std::string_view view1("test");

    CHECK_AND_RETURN(view1.size() == 4);
    CHECK_AND_RETURN(!view1.empty());

    using namespace std::string_view_literals;

    auto view2 = L"test"sv;

    CHECK_AND_RETURN(view2.size() == 4);
    CHECK_AND_RETURN(!view2.empty());
#endif

    return true;
}

bool tupleTest()
{
    std::tuple<int, void*> t(100, nullptr);

    CHECK_AND_RETURN(std::get<0>(t) == 100);
    CHECK_AND_RETURN(std::get<1>(t) == nullptr);

    auto t2 = std::make_tuple(10, "test");

    return true;
}

bool type_traitsTest()
{
    CHECK_AND_RETURN(std::is_integral_v<int>);
    CHECK_AND_RETURN(!std::is_integral_v<void*>);
    CHECK_AND_RETURN(std::is_pointer_v<void*>);
    CHECK_AND_RETURN(!std::is_abstract_v<int*>);

    return true;
}

bool utilityTest()
{
    int a = 1;
    int b = 2;
    std::swap(a, b);
    CHECK_AND_RETURN(a == 2);
    CHECK_AND_RETURN(b == 1);

    auto p1 = std::make_pair(100, &b);
    std::pair<int, int*> p2 = std::move(p1);
    CHECK_AND_RETURN(p1.first == 100);

    return true;
}

bool variantTest()
{
#ifdef __cpp_lib_variant
    std::variant<int, void*> v;
    v = 100;
    CHECK_AND_RETURN(std::holds_alternative<int>(v));
    CHECK_AND_RETURN(!std::get_if<void*>(&v));
    CHECK_AND_RETURN(*std::get_if<int>(&v) == 100);
    v = &v;
    CHECK_AND_RETURN(std::holds_alternative<void*>(v));
#endif

    return true;
}

// Check that C functions work too
bool crtTest()
{
    char buf[16] = {};
    _snprintf_s(buf, _TRUNCATE, "%d", 10);
    CHECK_AND_RETURN(buf[0] == '1' && buf[1] == '0');

    wchar_t wbuf[16] = {};
    _snwprintf_s(wbuf, _TRUNCATE, L"%d", 10);
    CHECK_AND_RETURN(wbuf[0] == L'1' && wbuf[1] == L'0');

    CHECK_AND_RETURN(_stricmp("test", "TEST") == 0);
    CHECK_AND_RETURN(_wcsicmp(L"test", L"TEST") == 0);
    CHECK_AND_RETURN(memcmp("test", "test", 4) == 0);
    CHECK_AND_RETURN(strlen("test") == 4);
    CHECK_AND_RETURN(wcslen(L"test") == 4);
    CHECK_AND_RETURN(strstr("test", "st"));
    CHECK_AND_RETURN(wcsstr(L"test", L"st"));

    return true;
}

bool runTests()
{
    CHECK_AND_RETURN(algorithmTest());
    CHECK_AND_RETURN(arrayTest());
    CHECK_AND_RETURN(atomicTest());
    CHECK_AND_RETURN(bitTest());
    CHECK_AND_RETURN(bitsetTest());
    CHECK_AND_RETURN(cassertTest());
    CHECK_AND_RETURN(cctypeTest());
    CHECK_AND_RETURN(cerrnoTest());
    CHECK_AND_RETURN(charconvTest());
    CHECK_AND_RETURN(chronoTest());
    CHECK_AND_RETURN(climitsTest());
    CHECK_AND_RETURN(compareTest());
    CHECK_AND_RETURN(conceptTest());
    CHECK_AND_RETURN(cstdargsTest());
    CHECK_AND_RETURN(cstddefTest());
    CHECK_AND_RETURN(cstdintTest());
    CHECK_AND_RETURN(cstdlibTest());
    CHECK_AND_RETURN(cstringTest());
    CHECK_AND_RETURN(cwcharTest());
    CHECK_AND_RETURN(cwctypeTest());
    CHECK_AND_RETURN(initializer_listTest());
    CHECK_AND_RETURN(iteratorTest());
    CHECK_AND_RETURN(expectedTest());
    CHECK_AND_RETURN(functionalTest());
    CHECK_AND_RETURN(limitsTest());
    CHECK_AND_RETURN(memoryTest());
    CHECK_AND_RETURN(newTest());
    CHECK_AND_RETURN(numericTest());
    CHECK_AND_RETURN(optionalTest());
    CHECK_AND_RETURN(randomTest());
    CHECK_AND_RETURN(rangesTest());
    CHECK_AND_RETURN(ratioTest());
    CHECK_AND_RETURN(source_locationTest());
    CHECK_AND_RETURN(spanTest());
    CHECK_AND_RETURN(string_viewTest());
    CHECK_AND_RETURN(tupleTest());
    CHECK_AND_RETURN(type_traitsTest());
    CHECK_AND_RETURN(utilityTest());
    CHECK_AND_RETURN(variantTest());

    CHECK_AND_RETURN(crtTest());

    return true;
}

///////////////////////////////////////////////////////////
// Entry point

DRIVER_UNLOAD driverUnload;
VOID driverUnload(_In_ PDRIVER_OBJECT /*driverObject*/)
{
    DbgPrint("Driver unloaded\n");
}

extern "C" DRIVER_INITIALIZE DriverEntry;
extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING /*registryPath*/)
{
    DbgPrint("Driver loaded\n");

    if (!runTests())
    {
        return STATUS_FAIL_CHECK;
    }

    driverObject->DriverUnload = driverUnload;
    return STATUS_SUCCESS;
}
