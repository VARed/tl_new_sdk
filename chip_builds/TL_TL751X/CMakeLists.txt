# Chip-specific configuration for TL_TL751X
set(CHIP_NAME TL_TL751X)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL751X/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL751X/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL751X/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_TL_TL751X)
