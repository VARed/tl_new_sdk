# Chip-specific configuration for TL_TL721X
set(CHIP_NAME TL_TL721X)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL721X/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL721X/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL721X/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_TL_TL721X)
