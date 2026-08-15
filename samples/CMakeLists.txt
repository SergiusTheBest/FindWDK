cmake_minimum_required(VERSION 3.0)

project(FindWdk)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Suppress false positives clang emits on idiomatic WDK code:
    #   -Wsizeof-array-div  fires on RTL_CONSTANT_STRING, which uses
    #                       sizeof(s) / sizeof(char) on a WCHAR array
    #                       (intentionally giving size in bytes).
    #   -Wmissing-field-initializers  fires on partial-init structs MSVC
    #                                 accepts silently (KMDF callbacks).
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wno-sizeof-array-div -Wno-missing-field-initializers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sizeof-array-div -Wno-missing-field-initializers")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
find_package(WDK REQUIRED)

add_subdirectory(KmdfCppDriver)
add_subdirectory(KmdfCppLib)
add_subdirectory(MinifilterCppDriver)
add_subdirectory(WdmCppDriver)
add_subdirectory(WdmCppLib)
add_subdirectory(WdmDriver)
add_subdirectory(WdmLib)
add_subdirectory(WdmIntrinsicFunctions)