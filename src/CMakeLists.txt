add_library(usbtingo SHARED
  usbtingo.cpp
)

target_include_directories(usbtingo
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(usbtingo

)