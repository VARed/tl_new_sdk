# Auto-generated TRNG_Demo demo CMake configuration
project(TRNG_Demo)

set(SOURCES
    chip/B87/boot
    chip/B87/drivers
    common
    demo/vendor/TRNG_Demo
    demo/vendor/common/B87/calibration
    demo/vendor/common/B87/flash_internal_private_func
    demo/vendor/common/common
    drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/B87_Driver_Demo/drivers/include
)

target_link_options(${PROJECT_NAME} PRIVATE
    --gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_tc32/B87/boot.link
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    driver
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_tc32/B87/../../../tools/tl_check_fw_tool/tl_check_fw.sh   TRNG_Demo   B87_Driver_Demo
    COMMENT "Executing post-build steps"
)
