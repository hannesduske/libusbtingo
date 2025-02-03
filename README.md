# libusbtingo
C++ API for the USBtingo - USB to CAN-FD Interface

# 1. Building and installing the library
## 1.1 Requirements for Windows
- CMake
- MSVC Compiler
- Windows SDK
- Windows Driver Kit

> Note:
It is possible to use libusb on Windows platforms instead of the Windows API.
This requires libusb to be installed.
Refer to the [USE_WINAPI](#15-cmake-options) option for further details.

## 1.2 Requirements for Linux

- CMake
- GCC/ Clang compiler
- libusb-1.0-0

## 1.3 Building the library from source
The library is built with a standard CMake workflow which is identical for Windows and Linux.
Use the following commands to build the library.
```
mkdir libusbtingo/build
cd libusbtingo/build
cmake ..
cmake --build .
```

## 1.4 Installing the library

The library can be installed to CMakes default locating with the `--install` command.
The default install location is `C:/Program Files (x86)/libusbtingo` for Windows and `usr/local` for Linux.
```
cmake --install .
```

The library can be installed to a custom location by specifying a install path.
Replace `<path>`with your desired install directory.

```
cmake --install . --prefix <path>
```

> Info:
Custom install paths should also be added to the `CMAKE_PREFIX_PATH` environment variable if the library is installed to a non default location.
This enables other packages to find this library.

## 1.5 CMake Options:

The build can be configured using CMake options.
Options can be set by calling `cmake ..` with the options flag `-D`.

For example:
```
cmake .. -DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=ON
```

| CMake Option | Default value | Description |
|---|---|---|
| BUILD_SHARED_LIBS | OFF | Build as shared library. If set to OFF a static library is built. |
| BUILD_UTILS | ON | Build and install utility programs along with the library. |
| BUILD_TESTS | ON | Build the test utilities for the library. Requires Catch2. |
| ENABLE_INTERACTIVE_TESTS | OFF | Enable tests that have to be confirmed manually. |
| ENABLE_TESTS_WITH_OTHER_DEVICES | OFF | Enable tests that require other CAN devices to send and acknowledge CAN messages. |
| USE_WINAPI | ON | This option is only available on Windows platforms. Choose which USB backend is used. The default backend is the Windows API. When this option is turned OFF, libusb is used instead. This requires libusb to be installed.


# 2. How to use the library

This library has two interfaces to access a Can Bus with a USBtingo, the `BasicBus` and the `Bus`. Both interfaces use the same underlying implementation and each manage one USBtingo device. They differ in the level of raw data accessibility and ease of use.

<img src="doc/class_diagram_simple.png" width="800"/>

## 2.1 BasicBus
The `BasicBus` is a simple, easy to use interface with reduced functionality.
It is recommended for all applications that exchange simple Can or Can FD data messages and do not rely on advanced features of the USBtingo.
The BasicBus automatically chooses the first USBtingo device it discovers and does not require manual configuration.

A `BasicBus` object can be directly instantiated using its static `create()` mehtod.
The returned `BasicBus` object is operational without any additional configuration, provided that a working USBtingo device is connected to the system.

## 2.2 Bus
The `Bus` interface offers full control over the USBtingo device and allows access to the raw data buffers that are exchanged with the USBtingo.
This interface is more complex and is recommended in cases where simplified the BasicBus does not meet the application requirements.

`Bus` objects require a valid `Device` that has to be configured before using it to instantiate a `Bus`.
The `Device` configuration includes all Can bus parameters, i.e. its protocol, baudrate and all advanced options.

## 2.3 Device
The `Device` represents the connected USBtingo and implements all necessary interface methods.
After creating a valid `Deivce` with the `DeviceFactory` it has to be configured with the desired Can properties.
After the configuration a `Device` is used to instantiate a `BasicBus` or a `Bus` which handles all further communication with the USBtingo.

> Warning: One physical USBtingo can only be managed by one `Device` at the same time.

## 2.4 DeviceFactory
The `Device` is an abstract interface class and cannot be instantiated directly.
Use the `DeviceFactory` to create `Device` objects instead.
The device factory chooses the correct Device implementation for the current system.
In addition the Factory makes sure that the specified USBtingo is physically connected and operational before returning the object.

# 3. Minimal example

## 3.1 Using the BasicBus

```
ToDo: Add example
```

## 3.2 Using the Bus

```
ToDo: Add example
```