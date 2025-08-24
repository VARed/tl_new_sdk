# Auto-generated EMI_BQB_Demo demo CMake configuration
project(EMI_BQB_Demo)

set(SOURCES
    chip/B91/boot
    chip/B91/drivers
    chip/B91/link
    common
    demo/vendor/EMI_BQB_Demo
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
    -O2
    -fmessage-length=0
    -ffunction-sections
    -fdata-sections
    -flto
    -Wall
    -Werror
    -Wextra
    -Wshadow
    -g3
    -Xlinker --gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/chip/B91/link/flash_boot.link
    -nostartfiles
    -fpack-struct
    -fshort-enums
    -g3
    -mcpu=d25f
    -ffunction-sections
    -fdata-sections
    -mext-dsp
    -mabi=ilp32f
    -fmessage-length=0
    -fomit-frame-pointer
    -fno-strict-aliasing
    -fuse-ld=bfd
    -Wno-gnu-zero-variadic-macro-arguments
    -O2
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    axon_driver_lib
    driver
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/B91/../../../tools/tl_gen_config_header_tool/tl_gen_config_header.sh   EMI_BQB_Demo
    COMMENT "Executing pre-build steps"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_riscv/B91/../../../tools/tl_check_fw_tool/tl_check_fw.sh   EMI_BQB_Demo   TL_PLATFORM_SDK_B91
    COMMENT "Executing post-build steps"
)
