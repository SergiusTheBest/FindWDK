#include "KmdfCppLib.h"

WDFSTRING getAnswer()
{
    static const UNICODE_STRING str = RTL_CONSTANT_STRING(L"42");

    WDFSTRING wdfstr = nullptr;
    NTSTATUS status = WdfStringCreate(&str, WDF_NO_OBJECT_ATTRIBUTES, &wdfstr);
    if (!NT_SUCCESS(status))
    {
        return nullptr;
    }

    return wdfstr;
}