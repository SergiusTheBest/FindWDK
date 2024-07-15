wdk_add_driver(WdmCppDriver
    Main.cpp
    )
target_link_libraries(WdmCppDriver WdmCppLib)


set(SIGNTOOL "signtool.exe")

add_custom_command(TARGET WdmCppDriver
    POST_BUILD
    COMMAND ${SIGNTOOL} sign /v /fd sha256 /n "WDKTestCert" /t http://timestamp.digicert.com $<TARGET_FILE:WdmCppDriver>
    COMMENT "Signing (Test Sign) $<TARGET_FILE:WdmCppDriver> ..."
)