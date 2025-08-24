# Auto-generated HASH_Demo demo CMake configuration
project(HASH_Demo)

set(SOURCES
    chip/TL321X/boot
    chip/TL321X/drivers
    chip/TL321X/link
    common
    demo/vendor/HASH_Demo/HASH_Demo_v1.1.9/HASH_Demo
    demo/vendor/common/TL321X/calibration
    demo/vendor/common/TL321X/exception.c
    demo/vendor/common/common
    tools/secure_boot_tool/config/TL321X/secure_boot_tool_cfg.ini
    drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/TL_TL321X/drivers/include
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
    -T${CMAKE_CURRENT_SOURCE_DIR}/chip/TL321X/link/flash_boot.link
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
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/TL321X/../../../tools/tl_gen_config_header_tool/tl_gen_config_header.sh   HASH_Demo
    COMMENT "Executing pre-build steps"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/TL321X/../../../tools/tl_check_fw_tool/tl_check_fw.sh   HASH_Demo   TL_PLATFORM_SDK_321X
    COMMENT "Executing post-build steps"
)
