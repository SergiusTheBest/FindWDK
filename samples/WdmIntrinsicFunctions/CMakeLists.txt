wdk_add_driver(WdmIntrinsicFunctions
    Main.cpp
    )
target_link_libraries(WdmIntrinsicFunctions WdmCppLib)