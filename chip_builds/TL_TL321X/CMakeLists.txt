# Chip-specific configuration for TL_TL321X
set(CHIP_NAME TL_TL321X)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL321X/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL321X/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL321X/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_TL_TL321X)
