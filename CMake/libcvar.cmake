# CVar: Console variable systems support library
# license: Apache, see LICENCE file
# file: libcvar.cmake - libcvar CMake configuration file
# author: Karl-Mihkel Ott

set(CVAR_TARGET cvar)
set(CVAR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/cvar/Api.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/cvar/CVarSystem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/cvar/CVarTypes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/cvar/ISerializer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/cvar/JSONSerializer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/cvar/JSONUnserializer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/cvar/SID.h)

set(CVAR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/CVarSystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/CVarTypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/JSONSerializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/JSONUnserializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Sources/SID.cpp)

if (NOT CVAR_STATIC)
    add_library(${CVAR_TARGET} SHARED
                ${CVAR_HEADERS}
                ${CVAR_SOURCES})
    target_compile_definitions(${CVAR_TARGET}
        PRIVATE CVAR_EXPORT_LIBRARY)
else()
    add_library(${CVAR_TARGET} STATIC
                ${CVAR_HEADERS}
                ${CVAR_SOURCES})
endif()

target_include_directories(${CVAR_TARGET}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
