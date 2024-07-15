wdk_add_driver(KmdfCppDriver
    KMDF 1.15
    Main.cpp
    )
target_link_libraries(KmdfCppDriver KmdfCppLib)


set(SIGNTOOL "signtool.exe")

add_custom_command(TARGET KmdfCppDriver
    POST_BUILD
    COMMAND ${SIGNTOOL} sign /v /fd sha256 /n "WDKTestCert" /t http://timestamp.digicert.com $<TARGET_FILE:KmdfCppDriver>
    COMMENT "Signing (Test Sign) $<TARGET_FILE:KmdfCppDriver> ..."
)