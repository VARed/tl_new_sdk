# Auto-generated Secure_Boot_Demo demo CMake configuration
project(Secure_Boot_Demo)

set(SOURCES
    chip/B92/boot
    chip/B92/drivers/adc.c
    chip/B92/drivers/aes.c
    chip/B92/drivers/audio.c
    chip/B92/drivers/clock.c
    chip/B92/drivers/compatibility_pack
    chip/B92/drivers/core.c
    chip/B92/drivers/error_handler
    chip/B92/drivers/flash
    chip/B92/drivers/flash.c
    chip/B92/drivers/gpio.c
    chip/B92/drivers/i2c.c
    chip/B92/drivers/lib
    chip/B92/drivers/lpc.c
    chip/B92/drivers/mspi.c
    chip/B92/drivers/plmt.c
    chip/B92/drivers/pwm.c
    chip/B92/drivers/qdec.c
    chip/B92/drivers/reg_include
    chip/B92/drivers/s7816.c
    chip/B92/drivers/spi.c
    chip/B92/drivers/stimer.c
    chip/B92/drivers/timer.c
    chip/B92/drivers/uart.c
    chip/B92/drivers/usbhw.c
    chip/B92/drivers/watchdog.c
    chip/B92/link
    common
    demo/vendor/Debug_Demo
    demo/vendor/common/B92/calibration
    demo/vendor/common/B92/exception.c
    demo/vendor/common/common
    tools/secure_boot_tool/config/B92/secure_boot_tool_cfg.ini
    drivers
    chip/B92/drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/TL_B92/drivers/include
)

target_link_options(${PROJECT_NAME} PRIVATE
    -O2
    -fmessage-length=0
    -ffunction-sections
    -fdata-sections
    -flto
    -Werror
    -Wall
    -Wextra
    -Wshadow
    -Wimplicit-fallthrough
    -Wpointer-arith
    -Wredundant-decls
    -Wcast-qual
    -Wsign-compare
    -Wunused-parameter
    -Wunused-variable
    -Wswitch
    -Wstrict-prototypes
    -Wmissing-field-initializers
    -Wdeprecated-declarations
    -Wenum-conversion
    -Wpacked-not-aligned
    -Waddress-of-packed-member
    -Wundef
    -g3
    -Xlinker --gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/chip/B92/link/flash_boot.link
    -nostartfiles
    -mcpu=d25f
    -mext-dsp
    -fomit-frame-pointer
    -fno-strict-aliasing
    -std=c99
    -fno-jump-tables
    -fno-fat-lto-objects
    -fuse-ld=bfd
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    driver
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/B92/../../../tools/tl_gen_config_header_tool/tl_gen_config_header.sh   Secure_Boot_Demo
    COMMENT "Executing pre-build steps"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/B92/../../../tools/tl_check_fw_tool/tl_check_fw.sh   Secure_Boot_Demo   TL_PLATFORM_SDK_B92;../../../../tools/secure_boot_tool/secure_boot_tool.sh ../../../../tools/secure_boot_tool ../../../../tools/secure_boot_tool/config/B92/secure_boot_tool_cfg.ini ./${BuildArtifactFileBaseName}.bin ./${BuildArtifactFileBaseName}_run_desc.bin
    COMMENT "Executing post-build steps"
)
