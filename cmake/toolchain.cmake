# Shared toolchain configuration for all chips
# This file contains common settings shared across all chip platforms

# Common compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Common warning flags
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
    -Wno-unused-function
)

# Common optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g3)
else()
    add_compile_options(-O2 -g)
endif()

# Include chip-specific toolchain configuration
if(CHIP_TC_TC321X)
    include(toolchain_tc_tc321x.cmake)
endif()
if(CHIP_TL_B91)
    include(toolchain_tl_b91.cmake)
endif()
if(CHIP_TL_B92)
    include(toolchain_tl_b92.cmake)
endif()
if(CHIP_TL_TL321X)
    include(toolchain_tl_tl321x.cmake)
endif()
if(CHIP_TL_TL322X)
    include(toolchain_tl_tl322x.cmake)
endif()
if(CHIP_TL_TL721X)
    include(toolchain_tl_tl721x.cmake)
endif()
if(CHIP_TL_TL751X)
    include(toolchain_tl_tl751x.cmake)
endif()
