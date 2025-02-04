#########################################################
# print summary
message(STATUS "")
if(CMAKE_COMPILER_IS_GNUCXX)
	set(MRPT_GCC_VERSION_STR "(GCC version: ${CMAKE_MRPT_GCC_VERSION})")
endif(CMAKE_COMPILER_IS_GNUCXX)

message(STATUS "+===============================================================+")
message(STATUS "       Resulting configuration for project ${PROJECT_NAME}       ")
message(STATUS "+===============================================================+")
message(STATUS "")
message(STATUS " ___________________________ PROJECT ___________________________")
message(STATUS " Name                            : " ${PROJECT_NAME})
message(STATUS " Version                         : " ${CMAKE_PROJECT_VERSION})
message(STATUS "")

message(STATUS " ___________________________ OPTIONS ___________________________")
message(STATUS " BUILD_SHARED_LIBS               : " ${BUILD_SHARED_LIBS})
message(STATUS " BUILD_EXAMPLES                  : " ${BUILD_EXAMPLES})
message(STATUS " BUILD_UTILS                     : " ${BUILD_UTILS})
message(STATUS " BUILD_TESTS                     : " ${BUILD_TESTS})
if(BUILD_TESTS)
message(STATUS " ENABLE_INTERACTIVE_TESTS        : " ${ENABLE_INTERACTIVE_TESTS})
message(STATUS " ENABLE_TESTS_WITH_OTHER_DEVICES : " ${ENABLE_TESTS_WITH_OTHER_DEVICES})
endif()
if(WIN32)
	message(STATUS " USE_WINAPI                      : " ${USE_WINAPI})
endif()
message(STATUS "")

message(STATUS " ___________________________ PLATFORM __________________________")
if(NOT ARCH_TEXT STREQUAL "")
	message(STATUS " Host                            : ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION} ${ARCH_TEXT}")
else()
	message(STATUS " Host                            : ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION} ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()
if(CMAKE_CROSSCOMPILING)
message(STATUS " Cross compile target            : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
endif(CMAKE_CROSSCOMPILING)
message(STATUS " CMake version                   : " ${CMAKE_VERSION})
message(STATUS " CMake generator                 : " ${CMAKE_GENERATOR})
message(STATUS " CMake build tool                : " ${CMAKE_BUILD_TOOL})
message(STATUS " Compiler                        : " ${CMAKE_CXX_COMPILER_ID})
if(MSVC)
	message(STATUS " MSVC                            : " ${MSVC_VERSION})
endif(MSVC)
if(CMAKE_GENERATOR MATCHES Xcode)
	message(STATUS " Xcode                           : " ${XCODE_VERSION})
endif(CMAKE_GENERATOR MATCHES Xcode)
if(NOT CMAKE_GENERATOR MATCHES "Xcode|Visual Studio")
	message(STATUS " Configuration                   : " ${CMAKE_BUILD_TYPE})
endif(NOT CMAKE_GENERATOR MATCHES "Xcode|Visual Studio")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	message( STATUS " C++ flags (Release)             : ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	message( STATUS " C++ flags (Debug)               : ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
else()
	message( STATUS " C++ flags                       : ${CMAKE_CXX_FLAGS}")
endif()
message(STATUS "")

message(STATUS " ___________________________ INSTALL ___________________________")
message(STATUS " Install prefix                  : ${CMAKE_INSTALL_PREFIX}")
if(CMAKE_BUILD_TYPE MATCHES Release)
	message(STATUS " Package generator               : ${GENERATORS_TEXT}")
else()
	message(STATUS " Package generator               : Debug build. No package generation.")
endif()
message(STATUS "")
message(STATUS "+===============================================================+")
message(STATUS "")