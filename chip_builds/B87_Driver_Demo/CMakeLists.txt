# Chip-specific configuration for B87_Driver_Demo
set(CHIP_NAME B87_Driver_Demo)

# Include chip-specific sources
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B87_Driver_Demo/boot)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B87_Driver_Demo/drivers)
add_subdirectory(${CMAKE_SOURCE_DIR}/chip/B87_Driver_Demo/link)

# Set chip-specific compiler definitions
add_definitions(-DCHIP_B87_DRIVER_DEMO)
