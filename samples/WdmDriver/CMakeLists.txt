wdk_add_driver(WdmDriver
    Main.c
    )
target_link_libraries(WdmDriver WdmLib)


set(SIGNTOOL "signtool.exe")

add_custom_command(TARGET WdmDriver
    POST_BUILD
    COMMAND ${SIGNTOOL} sign /v /fd sha256 /n "WDKTestCert" /t http://timestamp.digicert.com $<TARGET_FILE:WdmDriver>
    COMMENT "Signing (Test Sign) $<TARGET_FILE:WdmDriver> ..."
)