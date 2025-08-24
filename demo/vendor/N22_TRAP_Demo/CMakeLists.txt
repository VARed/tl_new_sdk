# Auto-generated N22_TRAP_Demo demo CMake configuration
project(N22_TRAP_Demo)

set(SOURCES
    chip/tl322x/boot
    chip/tl322x/drivers
    chip/tl322x/link
    common/bt_debug
    common/sdk_version.c
    common/usb_dbg
    demo/vendor/N22_TRAP_Demo
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
    -T${CMAKE_CURRENT_SOURCE_DIR}/chip/tl322x/link/N22/ram_boot.link
    -nostartfiles
    -mcpu=n22
    -mabi=ilp32
    -fomit-frame-pointer
    -fno-strict-aliasing
    -std=c99
    -fno-jump-tables
    -fno-fat-lto-objects
    -fuse-ld=bfd
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    driver_n22
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/tl322x/../../../tools/tl_gen_config_header_tool/tl_gen_config_header.sh   N22_TRAP_Demo
    COMMENT "Executing pre-build steps"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/tl322x/../../../tools/tl_check_fw_tool/tl_check_fw.sh   N22_TRAP_Demo   TL_PLATFORM_SDK_322X
    COMMENT "Executing post-build steps"
)
