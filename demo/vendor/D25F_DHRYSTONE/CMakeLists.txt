# Auto-generated D25F_DHRYSTONE demo CMake configuration
project(D25F_DHRYSTONE)

set(SOURCES
    chip/tl322x/boot
    chip/tl322x/drivers
    chip/tl322x/link
    common/bt_debug
    common/sdk_version.c
    common/usb_dbg
    demo/vendor/Dhrystone_Demo
    demo/vendor/common/common
    demo/vendor/common/tl322x/calibration
    demo/vendor/common/tl322x/exception.c
    demo/vendor/common/tl322x/exception_n22.c
    drivers
    chip/TL322X/drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/TL_TL322X/drivers/include
)

target_link_options(${PROJECT_NAME} PRIVATE
    -O3
    -fmessage-length=0
    -flto
    -fno-inline
    -g3
    -Xlinker --gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/chip/tl322x/link/D25F/flash_boot_ramcode.link
    -nostartfiles
    -mext-dsp
    -mabi=ilp32f
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    driver_d25f
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/tl322x/../../../tools/tl_gen_config_header_tool/tl_gen_config_header.sh   D25F_DHRYSTONE
    COMMENT "Executing pre-build steps"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/tl322x/../../../tools/tl_check_fw_tool/tl_check_fw.sh   D25F_DHRYSTONE   TL_PLATFORM_SDK_322X
    COMMENT "Executing post-build steps"
)
