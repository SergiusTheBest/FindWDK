wdk_add_driver(MinifilterCppDriver
    Main.cpp
    )
target_link_libraries(MinifilterCppDriver WDK::FLTMGR)


set(SIGNTOOL "signtool.exe")

add_custom_command(TARGET MinifilterCppDriver
    POST_BUILD
    COMMAND ${SIGNTOOL} sign /v /fd sha256 /n "WDKTestCert" /t http://timestamp.digicert.com $<TARGET_FILE:MinifilterCppDriver>
    COMMENT "Signing (Test Sign) $<TARGET_FILE:MinifilterCppDriver> ..."
)