# Options
option(USBTINGO_INSTALL "Enable the installation of the library." on)
option(USBTINGO_BUILD_SHARED_LIBS "Build as shared library. If set to OFF a static library is built." off)
option(USBTINGO_BUILD_EXAMPLES "Build the minimal examples." off)
option(USBTINGO_BUILD_UTILS "Build and install utility programs along with the library." on)
option(USBTINGO_BUILD_TESTS "Build the test utilities for the library. Requires Catch2." off)
option(USBTINGO_ENABLE_INTERACTIVE_TESTS "Enable tests that have to be confirmed manually." off)
option(USBTINGO_ENABLE_TESTS_WITH_OTHER_DEVICES "Enable tests that require other CAN devices to send and acknowledge CAN messages." off)
if(WIN32)
    option(USBTINGO_USE_WINAPI "Use the Windows API instead of libusb to interface the USBtingo. Requires the Windows SDK to be installed." on)
endif()


# Legacy deprecation
if(DEFINED BUILD_SHARED_LIBS)
    message(WARNING "BUILD_SHARED_LIBS is deprecated in USBTINGO build; use USBTINGO_BUILD_SHARED_LIBS instead")
        set(USBTINGO_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
endif()

if(DEFINED BUILD_EXAMPLES)
    message(WARNING "BUILD_EXAMPLES is deprecated in USBTINGO build; use USBTINGO_BUILD_EXAMPLES instead")
        set(USBTINGO_BUILD_EXAMPLES ${BUILD_EXAMPLES})
endif()

if(DEFINED BUILD_UTILS)
    message(WARNING "BUILD_UTILS is deprecated in USBTINGO build; use USBTINGO_BUILD_UTILS instead")
        set(USBTINGO_BUILD_UTILS ${BUILD_UTILS})
endif()

if(DEFINED BUILD_TESTS)
    message(WARNING "BUILD_TESTS is deprecated in USBTINGO build; use USBTINGO_BUILD_TESTS instead")
        set(USBTINGO_BUILD_TESTS ${BUILD_TESTS})
endif()

if(DEFINED ENABLE_INTERACTIVE_TESTS)
    message(WARNING "ENABLE_INTERACTIVE_TESTS is deprecated in USBTINGO build; use USBTINGO_ENABLE_INTERACTIVE_TESTS instead")
        set(USBTINGO_ENABLE_INTERACTIVE_TESTS ${ENABLE_INTERACTIVE_TESTS})
endif()

if(DEFINED ENABLE_TESTS_WITH_OTHER_DEVICE)
    message(WARNING "ENABLE_TESTS_WITH_OTHER_DEVICE is deprecated in USBTINGO build; use USBTINGO_ENABLE_TESTS_WITH_OTHER_DEVICES instead")
        set(USBTINGO_ENABLE_TESTS_WITH_OTHER_DEVICES ${ENABLE_TESTS_WITH_OTHER_DEVICE})
endif()

if(DEFINED USE_WINAPI)
    message(WARNING "USE_WINAPI is deprecated in USBTINGO build; use USBTINGO_USE_WINAPI instead")
        set(USBTINGO_USE_WINAPI ${USE_WINAPI})
endif()