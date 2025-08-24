# Auto-generated ALG_REG_Demo demo CMake configuration
project(ALG_REG_Demo)

set(SOURCES
    chip/TC321X/boot
    chip/TC321X/drivers
    common
    demo/vendor/ALG_REG_Demo
    demo/vendor/common/TC321X/calibration
    demo/vendor/common/common
    drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/TC_TC321X/drivers/include
)

target_link_options(${PROJECT_NAME} PRIVATE
    --gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_tc32/TC321X/boot.link
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    driver_tc321x
    soft-fp
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_tc32/TC321X/../../../tools/tl_check_fw_tool/tl_check_fw.sh   ALG_REG_Demo   TC_PLATFORM_SDK_321X
    COMMENT "Executing post-build steps"
)
