# CVar: Console variable systems support library
# license: Apache, see LICENCE file
# file: InteractiveConsole.cmake - interactive console program CMake configuration
# author: Karl-Mihkel Ott

set(INTERACTIVE_CONSOLE_TARGET InteractiveConsole)
set(INTERACTIVE_CONSOLE_HEADERS)
set(INTERACTIVE_CONSOLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/InteractiveConsole.cpp)

add_executable(${INTERACTIVE_CONSOLE_TARGET} 
    ${INTERACTIVE_CONSOLE_HEADERS}
    ${INTERACTIVE_CONSOLE_SOURCES})

add_dependencies(${INTERACTIVE_CONSOLE_TARGET}
    ${CVAR_TARGET})

target_link_libraries(${INTERACTIVE_CONSOLE_TARGET}
    PRIVATE ${CVAR_TARGET})
