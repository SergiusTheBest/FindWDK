#include <wdm.h>
#include <intrin.h>

DRIVER_UNLOAD driverUnload;
VOID driverUnload(_In_ PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);

    DbgPrint("Driver unloaded\n");
}

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath)
{
    UNREFERENCED_PARAMETER(registryPath);

    DbgPrint("Driver loaded\n");

    _disable(); //use intrinsic function
    ULONG64 cr3 = __readcr3(); //use intrinsic function
    _enable(); //use intrinsic function

    DbgPrint("CR3: %p\n", cr3);
    driverObject->DriverUnload = driverUnload;
    return STATUS_SUCCESS;
}