# CVar: Console variable systems support library
# license: Apache, see LICENCE file
# file: CMakeLists.txt - main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(cvar)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

option(CVAR_BUILD_DEMOS "Build demo CVar applications" ON)
option(CVAR_STATIC "Build CVar systems library as static library" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/libcvar.cmake)

if (CVAR_BUILD_DEMOS)
    message(STATUS "Adding demo build configurations")
    set(DEMO_APPS_DIR DemoApps)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/InteractiveConsole.cmake)
endif()
