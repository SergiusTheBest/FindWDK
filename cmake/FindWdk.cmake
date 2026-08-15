# Redistribution and use is allowed under the OSI-approved 3-clause BSD license.
# Copyright (c) 2018 Sergey Podobry (sergey.podobry at gmail.com). All rights reserved.

#.rst:
# FindWDK
# ----------
#
# This module searches for the installed Windows Development Kit (WDK) and
# exposes commands for creating kernel drivers and kernel libraries.
#
# Output variables:
# - `WDK_FOUND` -- if false, do not try to use WDK
# - `WDK_ROOT` -- where WDK is installed
# - `WDK_VERSION` -- the version of the selected WDK
# - `WDK_WINVER` -- the WINVER used for kernel drivers and libraries
#        (default value is `0x0601` and can be changed per target or globally)
# - `WDK_NTDDI_VERSION` -- the NTDDI_VERSION used for kernel drivers and libraries,
#                          if not set, the value will be automatically calculated by WINVER
#        (default value is left blank and can be changed per target or globally)
#
# Example usage:
#
#   find_package(WDK REQUIRED)
#
#   wdk_add_library(KmdfCppLib STATIC KMDF 1.15
#       KmdfCppLib.h
#       KmdfCppLib.cpp
#       )
#   target_include_directories(KmdfCppLib INTERFACE .)
#
#   wdk_add_driver(KmdfCppDriver KMDF 1.15
#       Main.cpp
#       )
#   target_link_libraries(KmdfCppDriver KmdfCppLib)
#

if(DEFINED ENV{WDKContentRoot})
    file(GLOB WDK_NTDDK_FILES
        "$ENV{WDKContentRoot}/Include/*/km/ntddk.h" # WDK 10
        "$ENV{WDKContentRoot}/Include/km/ntddk.h" # WDK 8.0, 8.1
    )
else()
    file(GLOB WDK_NTDDK_FILES
        "C:/Program Files*/Windows Kits/*/Include/*/km/ntddk.h" # WDK 10
        "C:/Program Files*/Windows Kits/*/Include/km/ntddk.h" # WDK 8.0, 8.1
    )
endif()

if(WDK_NTDDK_FILES)
    if (NOT CMAKE_VERSION VERSION_LESS 3.18.0)
        list(SORT WDK_NTDDK_FILES COMPARE NATURAL) # sort to use the latest available WDK
    endif()
    list(GET WDK_NTDDK_FILES -1 WDK_LATEST_NTDDK_FILE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WDK REQUIRED_VARS WDK_LATEST_NTDDK_FILE)

if(NOT WDK_LATEST_NTDDK_FILE)
    return()
endif()

get_filename_component(WDK_ROOT ${WDK_LATEST_NTDDK_FILE} DIRECTORY)
get_filename_component(WDK_ROOT ${WDK_ROOT} DIRECTORY)
get_filename_component(WDK_VERSION ${WDK_ROOT} NAME)
get_filename_component(WDK_ROOT ${WDK_ROOT} DIRECTORY)
if (NOT WDK_ROOT MATCHES ".*/[0-9][0-9.]*$") # WDK 10 has a deeper nesting level
    get_filename_component(WDK_ROOT ${WDK_ROOT} DIRECTORY) # go up once more
    set(WDK_LIB_VERSION "${WDK_VERSION}")
    set(WDK_INC_VERSION "${WDK_VERSION}")
else() # WDK 8.0, 8.1
    set(WDK_INC_VERSION "")
    foreach(VERSION winv6.3 win8 win7)
        if (EXISTS "${WDK_ROOT}/Lib/${VERSION}/")
            set(WDK_LIB_VERSION "${VERSION}")
            break()
        endif()
    endforeach()
    set(WDK_VERSION "${WDK_LIB_VERSION}")
endif()

if(NOT WDK_FIND_QUIETLY)
    message(STATUS "WDK_ROOT: " ${WDK_ROOT})
    message(STATUS "WDK_VERSION: " ${WDK_VERSION})
endif()

set(WDK_WINVER "0x0601" CACHE STRING "Default WINVER for WDK targets")
set(WDK_NTDDI_VERSION "" CACHE STRING "Specified NTDDI_VERSION for WDK targets if needed")

set(WDK_ADDITIONAL_FLAGS_FILE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wdkflags.h")
file(WRITE ${WDK_ADDITIONAL_FLAGS_FILE} "#pragma runtime_checks(\"suc\", off)")

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Force-included shim that papers over two gaps in clang's MSVC
    # kernel-intrinsic support:
    #   1. clang < 20.1.0 declared __readcr4/__readcr8/__writecr4/__writecr8
    #      with `unsigned long` instead of ULONG64, conflicting with wdm.h.
    #      Fix landed via https://github.com/llvm/llvm-project/pull/122238.
    #   2. _disable, _enable, __readcr0, __writecr0 are declared in
    #      <intrin.h> but never inlined to instructions, so the linker hits
    #      undefined symbols on KM code that uses them.
    # Strategy: macro-rename clang's broken/missing-body decls aside, then
    # provide our own static-inline asm bodies on the canonical names.
    set(WDK_CLANG_COMPAT_FILE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wdk_clang_compat.h")
    set(_wdk_shim_contents "")
    string(APPEND _wdk_shim_contents
"#ifdef __clang__\n"
"/* Hide clang's intrin.h decls (wrong types pre-20.1.0 and/or no body) so\n"
" * wdm.h's prototypes are unchallenged; provide inline-asm bodies below. */\n"
"#define _disable   __wdk_unused_clang_disable\n"
"#define _enable    __wdk_unused_clang_enable\n"
"#define __readcr0  __wdk_unused_clang_readcr0\n"
"#define __readcr4  __wdk_unused_clang_readcr4\n"
"#define __readcr8  __wdk_unused_clang_readcr8\n"
"#define __writecr0 __wdk_unused_clang_writecr0\n"
"#define __writecr4 __wdk_unused_clang_writecr4\n"
"#define __writecr8 __wdk_unused_clang_writecr8\n"
"#include <intrin.h>\n"
"#undef _disable\n"
"#undef _enable\n"
"#undef __readcr0\n"
"#undef __readcr4\n"
"#undef __readcr8\n"
"#undef __writecr0\n"
"#undef __writecr4\n"
"#undef __writecr8\n"
"#ifdef __cplusplus\n"
"extern \"C\" {\n"
"#endif\n"
"#define _WDK_DEF_RDCR(N) static __forceinline unsigned __int64 __readcr##N(void) { unsigned __int64 v; __asm__ __volatile__(\"movq %%cr\" #N \", %0\" : \"=r\"(v)); return v; }\n"
"#define _WDK_DEF_WRCR(N) static __forceinline void __writecr##N(unsigned __int64 v) { __asm__ __volatile__(\"movq %0, %%cr\" #N :: \"r\"(v)); }\n"
"static __forceinline void _disable(void) { __asm__ __volatile__(\"cli\" ::: \"memory\"); }\n"
"static __forceinline void _enable(void)  { __asm__ __volatile__(\"sti\" ::: \"memory\"); }\n"
"_WDK_DEF_RDCR(0) _WDK_DEF_RDCR(4) _WDK_DEF_RDCR(8)\n"
"_WDK_DEF_WRCR(0) _WDK_DEF_WRCR(4) _WDK_DEF_WRCR(8)\n"
"#undef _WDK_DEF_RDCR\n"
"#undef _WDK_DEF_WRCR\n"
"#ifdef __cplusplus\n"
"}\n"
"#endif\n"
"#endif\n")
    file(WRITE ${WDK_CLANG_COMPAT_FILE} "${_wdk_shim_contents}")

    # clang-cl has no /kernel; approximate it: SIMD off (xmm/ymm in KM
    # require explicit Ke{Save,Restore}FloatingPointState), security
    # cookie + stack probes off (KM has no __chkstk / __security_cookie).
    set(WDK_COMPILE_FLAGS
        "/Zp8" "/GF" "/GR-" "/Gz" "/Oi"
        "/GS-" "/Gs999999"
        "/FIwarning.h"
        -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3
        -mno-sse4.1 -mno-sse4.2
        -mno-avx -mno-avx2 -mno-avx512f
        -mno-mmx -mno-aes -mno-pclmul -mno-fma
        -Wno-unused-command-line-argument
    )
    list(APPEND WDK_COMPILE_FLAGS "/FI${WDK_CLANG_COMPAT_FILE}")
else()
    set(WDK_COMPILE_FLAGS
        "/Zp8"          # set struct alignment
        "/GF"           # enable string pooling
        "/GR-"          # disable RTTI
        "/Gz"           # __stdcall by default
        "/kernel"       # create kernel mode binary
        "/FIwarning.h"  # disable warnings in WDK headers
        "/FI${WDK_ADDITIONAL_FLAGS_FILE}" # include file to disable RTC
        "/Oi"           # intrinsic functions so _disable/_enable work
    )
endif()

set(WDK_COMPILE_DEFINITIONS "WINNT=1")
set(WDK_COMPILE_DEFINITIONS_DEBUG "MSC_NOOPT;DEPRECATE_DDK_FUNCTIONS=1;DBG=1")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    list(APPEND WDK_COMPILE_DEFINITIONS "_X86_=1;i386=1;STD_CALL")
    set(WDK_PLATFORM "x86")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "ARM64")
    list(APPEND WDK_COMPILE_DEFINITIONS "_ARM64_;ARM64;_USE_DECLSPECS_FOR_SAL=1;STD_CALL")
    set(WDK_PLATFORM "arm64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    list(APPEND WDK_COMPILE_DEFINITIONS "_AMD64_;AMD64")
    set(WDK_PLATFORM "x64")
else()
    message(FATAL_ERROR "Unsupported architecture")
endif()

string(CONCAT WDK_LINK_FLAGS
    "/MANIFEST:NO " #
    "/DRIVER " #
    "/OPT:REF " #
    "/INCREMENTAL:NO " #
    "/OPT:ICF " #
    "/SUBSYSTEM:NATIVE " #
    "/MERGE:_TEXT=.text;_PAGE=PAGE " #
    "/NODEFAULTLIB " # do not link default CRT
    "/SECTION:INIT,d " #
    "/VERSION:10.0 " #
    )

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    string(CONCAT WDK_LINK_FLAGS
        "/HIGHENTROPYVA:NO " # Strip some user-space only flags so the kernel loader accepts our driver
        "/TSAWARE:NO " # ^
        "/BASE:0x1C0000000 " # Set an acceptable base address for a kernel driver
        "/SECTION:INIT,DER " # Make the INIT section executable
        )
else()
    string(CONCAT WDK_LINK_FLAGS
        "/SECTION:INIT,d " #
        )
endif()

# Generate imported targets for WDK lib files
file(GLOB WDK_LIBRARIES "${WDK_ROOT}/Lib/${WDK_LIB_VERSION}/km/${WDK_PLATFORM}/*.lib")
foreach(LIBRARY IN LISTS WDK_LIBRARIES)
    get_filename_component(LIBRARY_NAME ${LIBRARY} NAME_WE)
    string(TOUPPER ${LIBRARY_NAME} LIBRARY_NAME)
    add_library(WDK::${LIBRARY_NAME} INTERFACE IMPORTED)
    set_property(TARGET WDK::${LIBRARY_NAME} PROPERTY INTERFACE_LINK_LIBRARIES ${LIBRARY})
endforeach(LIBRARY)
unset(WDK_LIBRARIES)

function(wdk_add_driver _target)
    cmake_parse_arguments(WDK "" "KMDF;WINVER;NTDDI_VERSION" "" ${ARGN})

    add_executable(${_target} ${WDK_UNPARSED_ARGUMENTS})

    set_target_properties(${_target} PROPERTIES SUFFIX ".sys")
    set_target_properties(${_target} PROPERTIES COMPILE_OPTIONS "${WDK_COMPILE_FLAGS}")
    set_target_properties(${_target} PROPERTIES COMPILE_DEFINITIONS
        "${WDK_COMPILE_DEFINITIONS};$<$<CONFIG:Debug>:${WDK_COMPILE_DEFINITIONS_DEBUG}>;_WIN32_WINNT=${WDK_WINVER}"
        )
    set_target_properties(${_target} PROPERTIES LINK_FLAGS "${WDK_LINK_FLAGS}")
    if(WDK_NTDDI_VERSION)
        target_compile_definitions(${_target} PRIVATE NTDDI_VERSION=${WDK_NTDDI_VERSION})
    endif()

    target_include_directories(${_target} SYSTEM PRIVATE
        "${WDK_ROOT}/Include/${WDK_INC_VERSION}/shared"
        "${WDK_ROOT}/Include/${WDK_INC_VERSION}/km"
        "${WDK_ROOT}/Include/${WDK_INC_VERSION}/km/crt"
        )

    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_link_libraries(${_target} WDK::MEMCMP)
    endif()

    target_link_libraries(${_target} WDK::NTOSKRNL WDK::HAL WDK::WMILIB)

    if(WDK_WINVER LESS "0x0602") # If WINVER < 0x0602 (Windows 7 or lower)
        target_link_libraries(${_target} WDK::BUFFEROVERFLOWK)
    else() # Windows 8 and above
        target_link_libraries(${_target} WDK::BUFFEROVERFLOWFASTFAILK)
    endif()

    if(CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "ARM64")
        target_link_libraries(${_target} "arm64rt.lib")
    endif()

    if(DEFINED WDK_KMDF)
        target_include_directories(${_target} SYSTEM PRIVATE "${WDK_ROOT}/Include/wdf/kmdf/${WDK_KMDF}")
        target_link_libraries(${_target}
            "${WDK_ROOT}/Lib/wdf/kmdf/${WDK_PLATFORM}/${WDK_KMDF}/WdfDriverEntry.lib"
            "${WDK_ROOT}/Lib/wdf/kmdf/${WDK_PLATFORM}/${WDK_KMDF}/WdfLdr.lib"
            )

        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:FxDriverEntry@8")
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:FxDriverEntry")
        endif()
    else()
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:GsDriverEntry@8")
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:GsDriverEntry")
        endif()
    endif()
endfunction()

function(wdk_add_library _target)
    cmake_parse_arguments(WDK "" "KMDF;WINVER;NTDDI_VERSION" "" ${ARGN})

    add_library(${_target} ${WDK_UNPARSED_ARGUMENTS})

    set_target_properties(${_target} PROPERTIES COMPILE_OPTIONS "${WDK_COMPILE_FLAGS}")
    set_target_properties(${_target} PROPERTIES COMPILE_DEFINITIONS
        "${WDK_COMPILE_DEFINITIONS};$<$<CONFIG:Debug>:${WDK_COMPILE_DEFINITIONS_DEBUG};>_WIN32_WINNT=${WDK_WINVER}"
        )
    if(WDK_NTDDI_VERSION)
        target_compile_definitions(${_target} PRIVATE NTDDI_VERSION=${WDK_NTDDI_VERSION})
    endif()

    target_include_directories(${_target} SYSTEM PRIVATE
        "${WDK_ROOT}/Include/${WDK_INC_VERSION}/shared"
        "${WDK_ROOT}/Include/${WDK_INC_VERSION}/km"
        "${WDK_ROOT}/Include/${WDK_INC_VERSION}/km/crt"
        )

    if(DEFINED WDK_KMDF)
        target_include_directories(${_target} SYSTEM PRIVATE "${WDK_ROOT}/Include/wdf/kmdf/${WDK_KMDF}")
    endif()
endfunction()
