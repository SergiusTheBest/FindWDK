# FindWDK
CMake module for building drivers with Windows Development Kit (WDK) [![Build status](https://ci.appveyor.com/api/projects/status/o7cyircahkb6nv07/branch/master?svg=true)](https://ci.appveyor.com/project/SergiusTheBest/findwdk/branch/master)

- [Introduction](#introduction)
- [Usage](#usage)
  - [Kernel driver](#kernel-driver)
  - [Kernel library](#kernel-library)
- [Samples](#samples)
- [License](#license)
- [Version history](#version-history)

# Introduction
FindWDK makes it possible to build kernel drivers and kernel libraries with Windows Development Kit (WDK) and CMake.

Requirements:
- WDK10
- Visual Studio 2015 and higher
- CMake 3.0 and higher

# Usage
Add FindWDK to the module search path and call `find_package`:

```cmake
list(APPEND CMAKE_MODULE_PATH "<path_to_FindWDK>")
find_package(WDK REQUIRED)
```

FindWDK will search for the installed Windows Development Kit (WDK) and expose commands for creating kernel drivers and kernel libraries. Also the following variables will be defined:
- `WDK_FOUND` -- if false, do not try to use WDK
- `WDK_ROOT` -- where WDK is installed
- `WDK_VERSION` -- the version of the selected WDK
- `WDK_WINVER` -- the WINVER used for kernel drivers and libraries (default value is `0x0601` and can be changed per target or globally)

## Kernel driver
The following command adds a kernel driver target called `<name>` to be built from the source files listed in the command invocation:

```cmake
wdk_add_driver(<name> 
    [EXCLUDE_FROM_ALL]
    [KMDF <kmdf_version>]
    [WINVER <winver_version>]
    source1 [source2 ...]
    )
```

Options:
- `EXCLUDE_FROM_ALL` -- exclude from the default build target
- `KMDF <kmdf_version>` -- use KMDF and set KMDF version
- `WINVER <winver_version>` -- use specific WINVER version

Example:

```cmake
wdk_add_driver(KmdfCppDriver 
    KMDF 1.15 
    WINVER 0x0602
    Main.cpp
    )
```

## Kernel library
The following command adds a kernel library target called `<name>` to be built from the source files listed in the command invocation:

```cmake
wdk_add_library(<name> [STATIC | SHARED]
    [EXCLUDE_FROM_ALL]
    [KMDF <kmdf_version>]
    [WINVER <winver_version>]
    source1 [source2 ...]
    )
```

Options:
- `EXCLUDE_FROM_ALL` -- exclude from the default build target
- `KMDF <kmdf_version>` -- use KMDF and set KMDF version
- `WINVER <winver_version>` -- use specific WINVER version
- `STATIC or SHARED` -- specify the type of library to be created

Example:

```cmake
wdk_add_library(KmdfCppLib STATIC 
    KMDF 1.15
    WINVER 0x0602
    KmdfCppLib.h 
    KmdfCppLib.cpp
    )
```

# Samples
Take a look at the [samples](samples) folder to see how WMD and KMDF drivers and libraries are built.

# License
FindWDK is licensed under the OSI-approved 3-clause BSD license. You can freely use it in your commercial or opensource software.

# Version history

## Version 1.0.0 (03 Feb 2018)
- Initial public release