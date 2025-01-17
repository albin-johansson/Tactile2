set(TACTILE_CXX_STANDARD 23)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  list(APPEND TACTILE_COMPILE_OPTIONS
       "/EHsc"
       "/MP"
       "/W4"
       "/bigobj"
       "/permissive-"
       "/Zc:preprocessor"
       "/Zc:__cplusplus"
       "/wd4251"
       "/wd4275"
       )
else ()
  list(APPEND TACTILE_COMPILE_OPTIONS
       "-fvisibility=hidden"
       "-Wall"
       "-Wextra"
       "-Wpedantic"
       "-Wconversion"
       "-Wsign-conversion"
       "-Wswitch-enum"
       "-Wold-style-cast"
       )

  if (TACTILE_BUILD_TYPE STREQUAL "asan")
    if (NOT MSVC)
      list(APPEND TACTILE_COMPILE_OPTIONS
           "-fsanitize=address"
           "-fno-sanitize-recover"
           "-fno-omit-frame-pointer"
           )
      list(APPEND TACTILE_LINK_OPTIONS
           "-fsanitize=address"
           "-fno-sanitize-recover"
           "-fno-omit-frame-pointer"
           )
    endif ()
  endif ()
endif ()

list(APPEND TACTILE_PRECOMPILED_HEADERS
     "<algorithm>"
     "<array>"
     "<charconv>"
     "<chrono>"
     "<codecvt>"
     "<concepts>"
     "<cstddef>"
     "<cstdint>"
     "<cstring>"
     "<cstdlib>"
     "<ctime>"
     "<cmath>"
     "<deque>"
     "<exception>"
     "<filesystem>"
     "<fstream>"
     "<functional>"
     "<iterator>"
     "<limits>"
     "<locale>"
     "<map>"
     "<memory>"
     "<numbers>"
     "<optional>"
     "<ostream>"
     "<queue>"
     "<random>"
     "<span>"
     "<sstream>"
     "<string>"
     "<string_view>"
     "<system_error>"
     "<type_traits>"
     "<unordered_map>"
     "<utility>"
     "<variant>"
     "<vector>"
     )

message(DEBUG "TACTILE_CXX_STANDARD: ${TACTILE_CXX_STANDARD}")
message(DEBUG "TACTILE_COMPILE_OPTIONS: ${TACTILE_COMPILE_OPTIONS}")
message(DEBUG "TACTILE_LINK_OPTIONS: ${TACTILE_LINK_OPTIONS}")
message(DEBUG "TACTILE_PRECOMPILED_HEADERS: ${TACTILE_PRECOMPILED_HEADERS}")

function(tactile_set_output_directory target directory)
  set_target_properties("${target}"
                        PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY "${directory}"
                        ARCHIVE_OUTPUT_DIRECTORY "${directory}"
                        LIBRARY_OUTPUT_DIRECTORY "${directory}"
                        )
endfunction()

function(tactile_set_properties target)
  set_target_properties(${target}
                        PROPERTIES
                        CXX_STANDARD "${TACTILE_CXX_STANDARD}"
                        CXX_EXTENSIONS "OFF"
                        CXX_STANDARD_REQUIRED "ON"
                        POSITION_INDEPENDENT_CODE "ON"
                        INTERPROCEDURAL_OPTIMIZATION "${TACTILE_USE_LTO}"
                        PREFIX ""
                        )
endfunction()

function(tactile_set_compiler_options target)
  target_compile_options(${target} PRIVATE "${TACTILE_COMPILE_OPTIONS}")
  target_link_options(${target} PRIVATE "${TACTILE_LINK_OPTIONS}")
endfunction()

function(tactile_prepare_target target)
  tactile_set_compiler_options(${target})
  tactile_set_properties(${target})
  tactile_set_output_directory(${target} "${TACTILE_BUILD_DIR}")

  if (TACTILE_USE_PRECOMPILED_HEADERS MATCHES "ON")
    target_precompile_headers(${target} PRIVATE "${TACTILE_PRECOMPILED_HEADERS}")
  endif ()

  if (TACTILE_BUILD_TYPE STREQUAL "asan" AND NOT MSVC)
    target_link_libraries(${target} PRIVATE "-fsanitize=address")
  endif ()
endfunction()
