include_guard(GLOBAL)

include(FetchContent)
include(CMakeParseArguments)

function(fetchcontent_declare_compat name)
  set(options)
  set(oneValueArgs URL)
  set(multiValueArgs)

  cmake_parse_arguments(
    FCC
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
  )

  if(NOT FCC_URL)
    message(FATAL_ERROR
      "fetchcontent_declare_compat(${name}): URL is required"
    )
  endif()

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    FetchContent_Declare(
      ${name}
      URL ${FCC_URL}
      DOWNLOAD_EXTRACT_TIMESTAMP OFF
    )
  else()
    FetchContent_Declare(
      ${name}
      URL ${FCC_URL}
    )
  endif()
endfunction()
