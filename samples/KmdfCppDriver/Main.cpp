#include <wdm.h>
#include <wdf.h>
#include "KmdfCppLib.h"

EVT_WDF_DRIVER_UNLOAD evtDriverUnload;
VOID evtDriverUnload(_In_ WDFDRIVER /*driver*/)
{
    DbgPrint("Driver unloaded\n");
}

extern "C" DRIVER_INITIALIZE DriverEntry;
extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath)
{
    DbgPrint("Driver loaded\n");

    WDF_DRIVER_CONFIG config;
    WDF_DRIVER_CONFIG_INIT(&config, nullptr);
    config.EvtDriverUnload = evtDriverUnload;

    NTSTATUS status = WdfDriverCreate(driverObject, registryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    UNICODE_STRING answer = {};
    WdfStringGetUnicodeString(getAnswer(), &answer);
    DbgPrint("The answer is %wZ\n", answer);

    return STATUS_SUCCESS;
}