# Chip-specific configuration for TL_B91
set(CHIP_NAME TL_B91)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_B91/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_B91/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_B91/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_TL_B91)
