cmake_minimum_required(VERSION 3.0)

project(FindWdk)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

foreach(WARN_FLAG
    CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/W3" "/W4 /WX" ${WARN_FLAG} "${${WARN_FLAG}}")
endforeach()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")
find_package(WDK REQUIRED)

add_subdirectory(KmdfCppDriver)
add_subdirectory(KmdfCppLib)
add_subdirectory(WdmCppDriver)
add_subdirectory(WdmCppLib)
add_subdirectory(WdmDriver)
add_subdirectory(WdmLib)
