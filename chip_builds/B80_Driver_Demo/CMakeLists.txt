# Chip-specific configuration for B80_Driver_Demo
set(CHIP_NAME B80_Driver_Demo)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B80_Driver_Demo/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B80_Driver_Demo/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B80_Driver_Demo/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_B80_DRIVER_DEMO)
