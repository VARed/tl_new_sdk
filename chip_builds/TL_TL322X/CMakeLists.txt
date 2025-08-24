# Chip-specific configuration for TL_TL322X
set(CHIP_NAME TL_TL322X)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL322X/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL322X/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TL_TL322X/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_TL_TL322X)
