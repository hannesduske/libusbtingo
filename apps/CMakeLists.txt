add_executable(USBtingoDetect
    USBtingoDetect.cpp
)

target_link_libraries(USBtingoDetect
    usbtingo
)

add_executable(USBtingoCan
    USBtingoCAN.cpp
)

target_link_libraries(USBtingoCan
    usbtingo
)