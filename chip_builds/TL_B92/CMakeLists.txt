# Chip-specific configuration for TL_B92
set(CHIP_NAME TL_B92)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_B92/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_B92/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_B92/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_TL_B92)
