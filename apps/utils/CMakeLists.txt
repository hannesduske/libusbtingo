add_executable(USBtingoDetect
    USBtingoDetect.cpp
)

target_link_libraries(USBtingoDetect
    usbtingo
)

if(BUILD_SHARED_LIBS)
    set_target_properties(USBtingoDetect PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib"
        BUILD_RPATH "${CMAKE_BINARY_DIR}/lib"
    )
endif()

add_executable(USBtingoCansend
    USBtingoCansend.cpp
)

target_link_libraries(USBtingoCansend
    usbtingo
)

if(BUILD_SHARED_LIBS)
    set_target_properties(USBtingoCansend PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib"
        BUILD_RPATH "${CMAKE_BINARY_DIR}/lib"
    )
endif()

add_executable(USBtingoCandump
    USBtingoCandump.cpp
)

target_link_libraries(USBtingoCandump
    usbtingo
)

if(BUILD_SHARED_LIBS)
    set_target_properties(USBtingoCandump PROPERTIES
        INSTALL_RPATH "$ORIGIN/../lib"
        BUILD_RPATH "${CMAKE_BINARY_DIR}/lib"
    )
endif()

install(TARGETS
    USBtingoDetect
    USBtingoCansend
    USBtingoCandump

    EXPORT usbtingoTargets
    RUNTIME DESTINATION bin
)