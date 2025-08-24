# Auto-generated DHRYSTONE demo CMake configuration
project(DHRYSTONE)

set(SOURCES
    chip/B91/boot
    chip/B91/drivers
    chip/B91/link
    common
    demo/vendor/Dhrystone_Demo
    demo/vendor/common/B91/calibration
    demo/vendor/common/B91/exception.c
    demo/vendor/common/B91/flash_internal_private_func
    demo/vendor/common/common
    drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/TL_B91/drivers/include
)

target_link_options(${PROJECT_NAME} PRIVATE
    -O3
    -fmessage-length=0
    -fno-inline
    -Wall
    -g3
    -Xlinker --gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/chip/B91/link/flash_boot_ramcode.link
    -nostartfiles
    -mcpu=d25f
    -mext-dsp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    driver
    axon_driver_lib
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/B91/../../../tools/tl_gen_config_header_tool/tl_gen_config_header.sh   DHRYSTONE
    COMMENT "Executing pre-build steps"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/B91/../../../tools/tl_check_fw_tool/tl_check_fw.sh   DHRYSTONE   TL_PLATFORM_SDK_B91
    COMMENT "Executing post-build steps"
)
