# Chip-specific configuration for B85_Driver_Demo
set(CHIP_NAME B85_Driver_Demo)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B85_Driver_Demo/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B85_Driver_Demo/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B85_Driver_Demo/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_B85_DRIVER_DEMO)
