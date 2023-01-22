if(CMAKE_VERSION VERSION_LESS 3.8.0)
    return()
endif()

cmake_minimum_required(VERSION 3.8)

# support VS2019 and higher
if(MSVC_VERSION LESS 1920)
    return()
endif()

include(FetchContent)

FetchContent_Declare(
  jxystl
  GIT_REPOSITORY https://github.com/jxy-s/stlkrn.git
  GIT_TAG        cee8fe7825c2c8e88b689a2152140bc5ac12a492
)

FetchContent_MakeAvailable(jxystl)

foreach(cxx_std IN ITEMS cxx_std_14 cxx_std_17 cxx_std_20 cxx_std_23)
    if(${cxx_std} IN_LIST CMAKE_CXX_COMPILE_FEATURES)
        wdk_add_driver(WdmJxyStlDriver_${cxx_std} STL EXCEPTIONS
            Main.cpp
            "${jxystl_SOURCE_DIR}/jxystl/msvcfill.cpp"
            "${jxystl_SOURCE_DIR}/jxystl/thread.cpp"
            "${jxystl_SOURCE_DIR}/jxystl/alloc.cpp"
            )
        target_include_directories(WdmJxyStlDriver_${cxx_std} PRIVATE "${jxystl_SOURCE_DIR}/include")

        # vcrtl_driver.lib provides exceptions support
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            target_link_libraries(WdmJxyStlDriver_${cxx_std} "${jxystl_SOURCE_DIR}/third-party/vcrtl/x86/vcrtl_driver.lib")
        else()
            target_link_libraries(WdmJxyStlDriver_${cxx_std} "${jxystl_SOURCE_DIR}/third-party/vcrtl/x64/vcrtl_driver.lib")
        endif()

        target_compile_features(WdmJxyStlDriver_${cxx_std} PRIVATE ${cxx_std})

        # jxystl fix for a new WDK
        target_compile_options(WdmJxyStlDriver_${cxx_std} PRIVATE /wd4996) # ExAllocatePoolWithTag is deprecated

        # jxystl fix for VS2022
        if(MSVC_VERSION GREATER_EQUAL 1930)
            target_compile_definitions(WdmJxyStlDriver_${cxx_std} PRIVATE _CONSTEXPR20_DYNALLOC=_CONSTEXPR20)
        endif()
    endif()
endforeach()
