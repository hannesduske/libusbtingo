# libusbtingo
C++ API for the USBtingo - USB to CAN-FD Interface

# 1. Building and installing the library
## 1.1 Requirements for Windows
- CMake for Windows
- MSVC Compiler
- Windows SDK
- Windows Driver Kit

## 1.2 Requirements for Linux
The USB device functionality is currently only implemented for Windows devices using the Windows API.
A Linux variant is prepared but not implemented.

## 1.3 Building the library from source
The library is built with a standard CMake workflow.
Use the following commands to build the library.
```
mkdir libusbtingo/build
cd libusbtingo/build
cmake ..
cmake --build .
```
> Info: The build process is identical for Windows and Linux.

## 1.4 Installing the library

The library can be installed to CMakes default locating with the `--install` command.
The default install location is `C:/Program Files (x86)/libusbtingo` for Windows and `usr/local` for Linux.
```
cmake --install .
```

The library can be installed to a custom location by specifying a install path. Replace `<path>`with your desired install directory.

```
cmake --install . --prefix <path>
```

> Info: When installing the library to a non default location the path should also be added to the `CMAKE_PREFIX_PATH` environment variable for other packages to find this library.

## 1.5 CMake Options:

The build can be configured using CMake options.
Options can be set by calling `cmake ..` with the options flag `-D`.

For example:
```
cmake .. -DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=ON
```

| CMake Option | Default value | Description |
|---|---|---|
| BUILD_SHARED_LIBS | OFF | Choose between building a static library (default) or a shared library. |
| BUILD_TESTS | OFF | Build the test utilities for the library. |
| SKIP_INTERACTIVE_TESTS | ON | Configure the test utilities to skip any tests that require confirmation by the user |
| SKIP_TESTS_WITH_OTHER_DEVICES | ON | Configure the test utilities to skip any tests that require other devices to be connected on the Can bus. |



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
It is not recommended to instantiate `Device` objects manually.
Use the `DeviceFactory` to instantiate `Device` objects instead.
It makes sure that the specified USBtingo is physically connected and operational.
It also offers a method to list all available USBtingo devices.


# 3. Minimal example

## 3.1 Using the BasicBus

```
ToDo: Add example
```

## 3.2 Using the Bus

```
ToDo: Add example
```