# Auto-generated Camera_Demo demo CMake configuration
project(Camera_Demo)

set(SOURCES
    chip/TL721X/boot
    chip/TL721X/drivers
    chip/TL721X/link
    common
    demo/aiot_dk1/Camera_Demo
    demo/vendor/common/TL721X/calibration
    demo/vendor/common/TL721X/exception.c
    demo/vendor/common/common
    tools/secure_boot_tool/config/TL721X/secure_boot_tool_cfg.ini
    drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/TL_TL721X/drivers/include
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
    -T${CMAKE_CURRENT_SOURCE_DIR}/chip/TL721X/link/flash_boot.link
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
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/TL721X/../../../tools/tl_gen_config_header_tool/tl_gen_config_header.sh   Camera_Demo
    COMMENT "Executing pre-build steps"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/TL721X/../../../tools/tl_check_fw_tool/tl_check_fw.sh   Camera_Demo   TL_PLATFORM_SDK_721X
    COMMENT "Executing post-build steps"
)
