if(CMAKE_VERSION VERSION_LESS 3.8.0)
    return()
endif()

cmake_minimum_required(VERSION 3.8)

# support VS2019 and higher
if(MSVC_VERSION LESS 1920)
    return()
endif()

foreach(cxx_std IN ITEMS cxx_std_14 cxx_std_17 cxx_std_20 cxx_std_23)
    if(${cxx_std} IN_LIST CMAKE_CXX_COMPILE_FEATURES)
        wdk_add_driver(WdmStlDriver_${cxx_std} STL
            Main.cpp
            )
        target_compile_features(WdmStlDriver_${cxx_std} PRIVATE ${cxx_std})
    endif()
endforeach()
