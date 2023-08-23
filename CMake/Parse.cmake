# CVar: Console variable systems support library
# license: Apache, see LICENCE file
# file: Parse.cmake - JSON parser test application
# author: Karl-Mihkel Ott

set(PARSE_TARGET Parse)
set(PARSE_HEADERS)
set(PARSE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Demos/Parse.cpp)

add_executable(${PARSE_TARGET}
    ${PARSE_HEADERS}
    ${PARSE_SOURCES})

add_dependencies(${PARSE_TARGET}
    ${CVAR_TARGET})

target_link_libraries(${PARSE_TARGET}
    PRIVATE ${CVAR_TARGET})
