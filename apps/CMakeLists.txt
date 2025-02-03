add_executable(USBtingoDetect
    USBtingoDetect.cpp
)

target_link_libraries(USBtingoDetect
    usbtingo
)

add_executable(USBtingoCansend
    USBtingoCansend.cpp
)

target_link_libraries(USBtingoCansend
    usbtingo
)

# add_executable(USBtingoCandump
#     USBtingoCandump.cpp
# )

# target_link_libraries(USBtingoCandump
#     usbtingo
# )

install(TARGETS
    USBtingoDetect
    USBtingoCansend
    #USBtingoCandump

    EXPORT usbtingoTargets
    RUNTIME DESTINATION bin
)