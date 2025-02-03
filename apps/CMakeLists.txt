add_executable(usbtingo_detect
    USBtingoDetect.cpp
)

target_link_libraries(usbtingo_detect
    usbtingo
)

add_executable(usbtingo_can
    USBtingoCAN.cpp
)

target_link_libraries(usbtingo_can
    usbtingo
)