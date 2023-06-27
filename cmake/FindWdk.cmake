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
        "$ENV{WDKContentRoot}/Include/*/km/ntddk.h"
    )
else()
    file(GLOB WDK_NTDDK_FILES
        "C:/Program Files*/Windows Kits/10/Include/*/km/ntddk.h"
    )
endif()

if(WDK_NTDDK_FILES)
    list(GET WDK_NTDDK_FILES -1 WDK_LATEST_NTDDK_FILE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WDK REQUIRED_VARS WDK_LATEST_NTDDK_FILE)

if (NOT WDK_LATEST_NTDDK_FILE)
    return()
endif()

get_filename_component(WDK_ROOT ${WDK_LATEST_NTDDK_FILE} DIRECTORY)
get_filename_component(WDK_ROOT ${WDK_ROOT} DIRECTORY)
get_filename_component(WDK_VERSION ${WDK_ROOT} NAME)
get_filename_component(WDK_ROOT ${WDK_ROOT} DIRECTORY)
get_filename_component(WDK_ROOT ${WDK_ROOT} DIRECTORY)

message(STATUS "WDK_ROOT: " ${WDK_ROOT})
message(STATUS "WDK_VERSION: " ${WDK_VERSION})

set(WDK_WINVER "0x0601" CACHE STRING "Default WINVER for WDK targets")

set(WDK_ADDITIONAL_FLAGS_FILE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/wdkflags.h")
file(WRITE ${WDK_ADDITIONAL_FLAGS_FILE} "#pragma runtime_checks(\"suc\", off)")

set(WDK_COMPILE_FLAGS
    "/Zp8" # set struct alignment
    "/GF"  # enable string pooling
    "/GR-" # disable RTTI
    "/Gz" # __stdcall by default
    "/FIwarning.h" # disable warnings in WDK headers
    "/FI${WDK_ADDITIONAL_FLAGS_FILE}" # include file to disable RTC
    "/W4"   # last warning level 
    "/WX"   # warnings as errors
)

set(WDK_COMPILE_DEFINITIONS "WINNT=1")
set(WDK_COMPILE_DEFINITIONS_DEBUG "MSC_NOOPT;DEPRECATE_DDK_FUNCTIONS=1;DBG=1")

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    list(APPEND WDK_COMPILE_DEFINITIONS "_X86_=1;i386=1;STD_CALL")
    set(WDK_PLATFORM "x86")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    list(APPEND WDK_COMPILE_DEFINITIONS "_WIN64;_AMD64_;AMD64")
    set(WDK_PLATFORM "x64")
else()
    message(FATAL_ERROR "Unsupported architecture")
endif()

set(WINSDK ${WDK_ROOT}/bin/${WDK_VERSION}/${WDK_PLATFORM})
set(OPENSSL "openssl.exe")
set(MAKECERT "${WINSDK}/makecert.exe")
set(CERTUTIL "certutil.exe")
set(CERTMGR "certmgr.exe")
set(CERT2SPC "cert2spc.exe")
set(PVK2PFX "${WINSDK}/pvk2pfx.exe")
set(SIGNTOOL "${WINSDK}/signtool.exe")

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

# Generate imported targets for WDK lib files
file(GLOB WDK_LIBRARIES "${WDK_ROOT}/Lib/${WDK_VERSION}/km/${WDK_PLATFORM}/*.lib")    
foreach(LIBRARY IN LISTS WDK_LIBRARIES)
    get_filename_component(LIBRARY_NAME ${LIBRARY} NAME_WE)
    string(TOUPPER ${LIBRARY_NAME} LIBRARY_NAME)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
    if(NOT TARGET WDK::${LIBRARY_NAME})                     
        add_library(WDK::${LIBRARY_NAME} INTERFACE IMPORTED)
        set_property(TARGET WDK::${LIBRARY_NAME} PROPERTY INTERFACE_LINK_LIBRARIES  ${LIBRARY})
    endif()
endforeach(LIBRARY)
unset(WDK_LIBRARIES)

set(EXTENDED_CPP_FEATURES_LIST "/EHsc")

macro (EXTENDED_CPP_FEATURES_ON TARGET_NAME)
    list(REMOVE_ITEM WDK_COMPILE_FLAGS "/kernel")
    list(APPEND WDK_COMPILE_FLAGS ${EXTENDED_CPP_FEATURES_LIST})
    message(STATUS "Extended C++ features (exceptions, etc) are enabled for ${TARGET_NAME}")
endmacro (EXTENDED_CPP_FEATURES_ON)

macro (EXTENDED_CPP_FEATURES_OFF TARGET_NAME)
    list(REMOVE_ITEM WDK_COMPILE_FLAGS ${EXTENDED_CPP_FEATURES_LIST})
    list(APPEND WDK_COMPILE_FLAGS "/kernel")
    message(STATUS "Extended C++ features (exceptions, etc) are disabled for ${TARGET_NAME}")
endmacro (EXTENDED_CPP_FEATURES_OFF)

macro (TODAY RESULT)
    if(WIN32)
        execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..).(..).(....).*" "\\2/\\1/\\3" ${RESULT} ${${RESULT}})
    else()
        message(SEND_ERROR "FindWDK signing module supports only Windows systems")
        set(RESULT "00/00/0000")
    endif()
endmacro (TODAY)

set(COMPILATION_DATE "")
TODAY(COMPILATION_DATE)
message(STATUS "Compilation date: ${COMPILATION_DATE}")

function(wdk_add_driver _target)
    cmake_parse_arguments(WDK "EXTENDED_CPP_FEATURES" "CUSTOM_ENTRY_POINT;KMDF;WINVER" "" ${ARGN})

    if(WDK_EXTENDED_CPP_FEATURES)
        EXTENDED_CPP_FEATURES_ON(${_target})
    else()
        EXTENDED_CPP_FEATURES_OFF(${_target})
    endif()

    add_executable(${_target} ${WDK_UNPARSED_ARGUMENTS})

    set_target_properties(${_target} PROPERTIES SUFFIX ".sys")
    set_target_properties(${_target} PROPERTIES COMPILE_OPTIONS "${WDK_COMPILE_FLAGS}")
    set_target_properties(${_target} PROPERTIES COMPILE_DEFINITIONS
        "${WDK_COMPILE_DEFINITIONS};$<$<CONFIG:Debug>:${WDK_COMPILE_DEFINITIONS_DEBUG}>;_WIN32_WINNT=${WDK_WINVER}"
        )
    set_target_properties(${_target} PROPERTIES LINK_FLAGS "${WDK_LINK_FLAGS}")

    target_include_directories(${_target} SYSTEM PRIVATE
        "${WDK_ROOT}/Include/${WDK_VERSION}/shared"
        "${WDK_ROOT}/Include/${WDK_VERSION}/km"
        "${WDK_ROOT}/Include/${WDK_VERSION}/km/crt"
        )

    target_link_libraries(${_target} WDK::NTOSKRNL WDK::HAL WDK::BUFFEROVERFLOWK WDK::WMILIB)

    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_link_libraries(${_target} WDK::MEMCMP)
    endif()

    if(DEFINED WDK_CUSTOM_ENTRY_POINT)
        set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:${WDK_CUSTOM_ENTRY_POINT}")
    elseif(DEFINED WDK_KMDF)
        target_include_directories(${_target} SYSTEM PRIVATE "${WDK_ROOT}/Include/wdf/kmdf/${WDK_KMDF}")
        target_link_libraries(${_target}
            "${WDK_ROOT}/Lib/wdf/kmdf/${WDK_PLATFORM}/${WDK_KMDF}/WdfDriverEntry.lib"
            "${WDK_ROOT}/Lib/wdf/kmdf/${WDK_PLATFORM}/${WDK_KMDF}/WdfLdr.lib"
            )
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:FxDriverEntry@8")
        elseif(CMAKE_SIZEOF_VOID_P  EQUAL 8)
            set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:FxDriverEntry")
        endif()
    else()
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:GsDriverEntry@8")
        elseif(CMAKE_SIZEOF_VOID_P  EQUAL 8)
            set_property(TARGET ${_target} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:GsDriverEntry")
        endif()
    endif()
endfunction()

function(wdk_add_library _target)
    cmake_parse_arguments(WDK "EXTENDED_CPP_FEATURES" "KMDF;WINVER" "" ${ARGN})
    
    if(WDK_EXTENDED_CPP_FEATURES)
        EXTENDED_CPP_FEATURES_ON(${_target})
    else()
        EXTENDED_CPP_FEATURES_OFF(${_target})
    endif()

    add_library(${_target} ${WDK_UNPARSED_ARGUMENTS})

    set_target_properties(${_target} PROPERTIES COMPILE_OPTIONS "${WDK_COMPILE_FLAGS}")
    set_target_properties(${_target} PROPERTIES COMPILE_DEFINITIONS 
        "${WDK_COMPILE_DEFINITIONS};$<$<CONFIG:Debug>:${WDK_COMPILE_DEFINITIONS_DEBUG}>;_WIN32_WINNT=${WDK_WINVER}"
        )

    target_include_directories(${_target} SYSTEM PRIVATE
        "${WDK_ROOT}/Include/${WDK_VERSION}/shared"
        "${WDK_ROOT}/Include/${WDK_VERSION}/km"
        "${WDK_ROOT}/Include/${WDK_VERSION}/km/crt"
        )

    if(DEFINED WDK_KMDF)
        target_include_directories(${_target} SYSTEM PRIVATE "${WDK_ROOT}/Include/wdf/kmdf/${WDK_KMDF}")
    endif()
endfunction()

function(wdk_make_certificate _target _certificate_name)
    cmake_parse_arguments(WDK "" "CERTIFICATE_PATH;COMPANY" "" ${ARGN})

    if(NOT DEFINED WDK_CERTIFICATE_PATH)
        set(WDK_CERTIFICATE_PATH ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    if(NOT DEFINED WDK_COMPANY)
        set(WDK_COMPANY "NoCompany")
    endif()

    add_custom_command(OUTPUT ${_certificate_name}.pfx
	    COMMAND "${CMAKE_COMMAND}" -E remove ${_certificate_name}.pvk ${_certificate_name}.cer ${_certificate_name}.pfx ${_certificate_name}.spc
	    COMMAND "${MAKECERT}" -b ${COMPILATION_DATE} -r -n \"CN=${WDK_COMPANY}\" -sv ${_certificate_name}.pvk ${_certificate_name}.cer
	    COMMAND "${CERTMGR}" -add ${_certificate_name}.cer -s -r currentUser ROOT
	    COMMAND "${CERTMGR}" -add ${_certificate_name}.cer -s -r currentUser TRUSTEDPUBLISHER
	    COMMAND "${CERT2SPC}" ${_certificate_name}.cer ${_certificate_name}.spc
	    COMMAND "${PVK2PFX}" -pvk ${_certificate_name}.pvk -spc ${_certificate_name}.spc -pfx ${_certificate_name}.pfx
	    WORKING_DIRECTORY "${WDK_CERTIFICATE_PATH}"
	    COMMENT "Generating SSL certificates to sign the drivers and executable ..."
   )
    add_custom_target(${_certificate_name}
	    DEPENDS ${_certificate_name}.pfx)
    add_dependencies(${_target} ${_certificate_name})
endfunction()

function(wdk_sign_driver _target _certificate_name)
    cmake_parse_arguments(WDK "" "CERTIFICATE_PATH;TIMESTAMP_SERVER" "" ${ARGN})

    if(NOT DEFINED WDK_CERTIFICATE_PATH)
       set(WDK_CERTIFICATE_PATH ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    if(NOT DEFINED WDK_TIMESTAMP_SERVER)
        set(WDK_TIMESTAMP_SERVER http://timestamp.verisign.com/scripts/timstamp.dll)
    endif()

    add_custom_command(TARGET ${_target}
    COMMAND "${SIGNTOOL}" sign /v /f "${WDK_CERTIFICATE_PATH}/${_certificate_name}.pfx" /t ${WDK_TIMESTAMP_SERVER} $<TARGET_FILE:${_target}>
        COMMENT "Signing $<TARGET_FILE:${_target} ..."
    )
endfunction()
