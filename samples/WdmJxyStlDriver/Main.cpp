// currently the compiler crashes with an internal error if _KERNEL_MODE is set globally, define it locally if needed
#define _KERNEL_MODE 1

#include <ntifs.h> // include ntifs.h before wdm.h
#include <wdm.h>

#if !_HAS_EXCEPTIONS
#error _HAS_EXCEPTIONS should be not 0
#endif

#if !_KERNEL_MODE
#error _KERNEL_MODE should be not 0
#endif

///////////////////////////////////////////////////////////
// Implement CRT error reporting

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

///////////////////////////////////////////////////////////
// JXYSTL headers

#include <jxy/deque.hpp>
#include <jxy/list.hpp>
#include <jxy/map.hpp>
#include <jxy/memory.hpp>
#include <jxy/queue.hpp>
#include <jxy/set.hpp>
#include <jxy/stack.hpp>
#include <jxy/string.hpp>
#include <jxy/thread.hpp>

// #include <jxy/unordered_map.hpp> // not implemented
// #include <jxy/unordered_set.hpp> // not implemented

#include <jxy/vector.hpp>

///////////////////////////////////////////////////////////
// Tests

#define CHECK_AND_RETURN(res) { __pragma(warning(suppress: 4127)); if (!(res)) return false; }

bool dequeTest()
{
    jxy::deque<int, PagedPool, ' yxj'> d = { 1, 2, 3, 4, 5 };
    CHECK_AND_RETURN(d.size() == 5);
    CHECK_AND_RETURN(d.front() == 1);
    CHECK_AND_RETURN(d.back() == 5);

    return true;
}

bool listTest()
{
    jxy::list<int, PagedPool, ' yxj'> l = { 1, 2, 3, 4, 5 };
    CHECK_AND_RETURN(l.size() == 5);
    CHECK_AND_RETURN(l.front() == 1);
    CHECK_AND_RETURN(l.back() == 5);

    return true;
}

bool mapTest()
{
    jxy::map<int, int, PagedPool, ' yxj'> m = { { 1, 10 }, { 2, 20 } };
    CHECK_AND_RETURN(m[1] == 10);
    CHECK_AND_RETURN(m.count(3) == 0);

    return true;
}

bool memoryTest()
{
    auto u = jxy::make_unique<int, PagedPool, ' yxj'>(42);
    CHECK_AND_RETURN(u);
    CHECK_AND_RETURN(*u == 42);

    auto s = jxy::make_shared<int, PagedPool, ' yxj'>(38);
    CHECK_AND_RETURN(s);
    CHECK_AND_RETURN(*s == 38);

    return true;
}

bool queueTest()
{
    jxy::queue<int, PagedPool, ' yxj'> q;
    q.push(10);
    q.push(20);
    CHECK_AND_RETURN(q.size() == 2);
    CHECK_AND_RETURN(q.front() == 10);
    CHECK_AND_RETURN(q.back() == 20);

    return true;
}

bool setTest()
{
    jxy::set<int, PagedPool, ' yxj'> st = { 1, 2, 3, 4, 5 };
    CHECK_AND_RETURN(st.size() == 5);
    CHECK_AND_RETURN(!st.count(6));
    CHECK_AND_RETURN(st.count(1));

    return true;
}

bool stackTest()
{
    jxy::stack<int, PagedPool, ' yxj'> st;
    st.push(10);
    st.push(20);
    CHECK_AND_RETURN(st.size() == 2);
    CHECK_AND_RETURN(st.top() == 20);
    
    return true;
}

bool threadTest()
{
    bool isRun = false;
    jxy::thread th([&]() { isRun = true; });
   
    CHECK_AND_RETURN(th.joinable());
    th.join();

    CHECK_AND_RETURN(isRun);

    return true;
}

bool vectorTest()
{
    jxy::vector<int, PagedPool, ' yxj'> vec;
    vec.push_back(123);
    vec.resize(1000);
    CHECK_AND_RETURN(vec[0] == 123);
    CHECK_AND_RETURN(vec[999] == 0);

    try
    {
        int i = vec.at(2000);
        UNREFERENCED_PARAMETER(i);

        return false;
    }
    catch (const std::exception&)
    {
    }

    return true;
}

bool runTests() try
{
    CHECK_AND_RETURN(dequeTest());
    CHECK_AND_RETURN(listTest());
    CHECK_AND_RETURN(mapTest());
    CHECK_AND_RETURN(memoryTest());
    CHECK_AND_RETURN(queueTest());
    CHECK_AND_RETURN(setTest());
    CHECK_AND_RETURN(stackTest());
    CHECK_AND_RETURN(threadTest());
    CHECK_AND_RETURN(vectorTest());

    return true;
}
catch (const std::exception&)
{
    return false;
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
