# Chip-specific configuration for TC_TC321X
set(CHIP_NAME TC_TC321X)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TC_TC321X/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TC_TC321X/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/TC_TC321X/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_TC_TC321X)
