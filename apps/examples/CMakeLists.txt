add_executable(MinimalExampleBasicBus
    MinimalExampleBasicBus.cpp
)

target_link_libraries(MinimalExampleBasicBus
    usbtingo
)

add_executable(MinimalExampleBus
    MinimalExampleBus.cpp
)

target_link_libraries(MinimalExampleBus
    usbtingo
)

add_executable(MinimalExampleLogic
    MinimalExampleLogicStream.cpp
)

target_link_libraries(MinimalExampleLogic
    usbtingo
)