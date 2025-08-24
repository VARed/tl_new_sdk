cmake_minimum_required(VERSION 3.19)
project(tl_new_sdk)

# Include all submodules
include(cmake/toolchain.cmake)
include(cmake/kconfig.cmake)

# Enable chip-specific configurations
option(CHIP_TC321X "Enable TC321X chip support" OFF)
option(CHIP_TL321X "Enable TL321X chip support" OFF)
option(CHIP_B92 "Enable B92 chip support" OFF)

# Recursively add all subdirectories
file(GLOB_RECURSE SUB_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach(DIR ${SUB_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/CMakeLists.txt)
        add_subdirectory(${DIR})
    endif()
endforeach()

# Include Kconfig configuration
include(${CMAKE_BINARY_DIR}/kconfig.cmake)
