# libusbtingo
C++ API for the USBtingo - USB to CAN-FD Interface

# 1. Building and installing the library
## 1.1 Requirements for Windows
- CMake
- MSVC Compiler
- Windows SDK

> Note:
It is possible to use libusb on Windows platforms instead of the Windows API.
Refer to the [USE_WINAPI](#15-cmake-options) option for further details.
<br>This option has not been tested and might require some additional configuration of the CMake files.

## 1.2 Requirements for Linux

- CMake
- libusb-1.0-0
- Some C++ compiler

## 1.3 Building the library from source
The library is built with a standard CMake workflow which is identical for Windows and Linux.
Use the following commands to build the library.
```
git clone https://github.com/hannesduske/libusbtingo.git

mkdir libusbtingo/build
cd libusbtingo/build
cmake ..
cmake --build .
```

## 1.4 Installing the library

The library can be installed to CMakes default locating with the `cmake --install` command.
The default install location is `C:/Program Files (x86)/libusbtingo` for Windows and `/usr/local` for Linux.
```
cmake --install .
```

The library can be installed to a custom location by specifying an install path.
Replace `<path>` with your desired install directory.

```
cmake --install . --prefix <path>
```

> Note:
Custom install paths should be added to the `CMAKE_PREFIX_PATH` environment variable if the library is installed to a non default location.
This enables other packages to find this library.

## 1.5 CMake Options:

The build can be configured with CMake options.
Options can be set by calling `cmake ..` with the flag `-D`.
For example, the following command builds the library as a shared library and disables tests.
```
cmake .. -DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF
```

| CMake Option | Default value | Description |
|---|---|---|
| BUILD_SHARED_LIBS | OFF | Build libusbtingo as shared library. If set to OFF a static library is built. |
| BUILD_UTILS | ON | Build and install utility programs along with the library. |
| BUILD_TESTS | ON | Build the test utilities for the library. Requires Catch2. |
| ENABLE_INTERACTIVE_TESTS | OFF | Enable tests that have to be confirmed manually. |
| ENABLE_TESTS_WITH_OTHER_DEVICES | OFF | Enable tests that require other CAN devices to send and acknowledge CAN messages. |
| USE_WINAPI | ON | This option is only available on Windows platforms. Choose which USB backend is used. The default backend is the Windows API. When this option is turned OFF, libusb is used instead. This requires libusb to be installed.


# 2. How to use the library

This library has two interfaces to access a CAN Bus with a USBtingo: The `BasicBus` and the `Bus`. Both interfaces use the same underlying implementation and each manage one USBtingo device. They differ in the level of raw data accessibility and ease of use.

<img src="doc/class_diagram_simple.png" width="800"/>

## 2.1 BasicBus
The `BasicBus` is a simple, easy to use interface with reduced functionality.
It is recommended for all applications that exchange simple CAN or CAN FD data messages and do not rely on advanced features of the USBtingo.
The BasicBus automatically chooses the first USBtingo device it discovers and does not require manual configuration.

A `BasicBus` object can be directly instantiated using its static `create()` method.
The returned `BasicBus` object is operational without any additional configuration, provided that a working USBtingo device is connected to the system.

## 2.2 Bus
The `Bus` interface offers full control over the USBtingo device and grants access to the raw data buffers that are exchanged with the USBtingo.
This interface is more complex and is recommended in cases where simplified the BasicBus does not meet the application requirements.

`Bus` objects require a valid `Device` that has to be configured before passing it to a `Bus`.
The `Device` configuration includes all CAN bus parameters, i.e. its protocol, baudrate and all advanced options.
Refer to [DeviceFactory](#24-devicefactory) for how to safely instantiate `Device` objects.

## 2.3 Device
The `Device` represents the connected USBtingo and implements all necessary interface methods.
After creating a valid `Deivce` with the `DeviceFactory` it has to be configured with the desired CAN parameters.
After the configuration is complete, a `Device` can be used to instantiate a `BasicBus` or a `Bus` which handles all further communication with the USBtingo.

> Note: One physical USBtingo can only be managed by one `Device` at the same time.

## 2.4 DeviceFactory

### 2.4.1 Enumerating devices
The `DeviceFactory` offers a method to enumerate all connected USBtingo devices which returns a vector of the corresponding serial numbers. The serial numbers can be used in the factory method `DeviceFactory::create()` to instantiate a specific USBtingo `Device`.

### 2.4.2 Creating devices
The `Device` is an abstract interface and cannot be instantiated directly.
Use the `DeviceFactory` to create `Device` objects instead.
The device factory chooses the correct `Device` implementation (libusb or WinApi) for the current system.
In addition, the Factory makes sure that the specified USBtingo is physically connected and operational before returning the object.
If the device does not operate correctly a `nullptr` is returned instead.


# 3. Utility applications

The library comes with three small utility applications that illustrate the basic functionality and serve as examples on how to use the library.

**USBtingoDetect**<br>
Minimal example of a command line program that lists the serial numbers of all connected USBtingo devices.
At the start all currently connected USBtingo serial numbers are printed.
Subsequently, all connection and disconnection events of USBtingo devices are printed.

**USBtingoCansend**<br>
Minimal example of a command line program that sends CAN messages.
After the configuration, the program sends all entered messages on the CAN Bus.

**USBtingoCandump**<br>
 Minimal example of a command line program that prints out all received CAN messages.
 After the configuration, a listener is registered as an observer of the CAN Bus instance that gets notified asynchronously when new messages arrive.

>Note:
 Only one of the utility application can access a USBtingo device at a time. It is currently not possible to run the USBtingoCansend and USBtingoCandump example side by side.
 
# 4. Minimal example

Refer to the utility applications `USBtingoDetect`, `USBtingoCansend` and `USBtingoCandump` in the `apps` directory for examples on how to use this library. Below are two additional minimal examples on how to use the `BasicBus` and the `Bus`.

## 4.1 Using the BasicBus

```
ToDo: Add example
```

## 4.2 Using the Bus

```
ToDo: Add example
```