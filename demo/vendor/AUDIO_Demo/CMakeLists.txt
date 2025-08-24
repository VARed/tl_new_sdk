# Auto-generated Audio_Demo demo CMake configuration
project(Audio_Demo)

set(SOURCES
    chip/B85/boot
    chip/B85/drivers
    common
    demo/vendor/AUDIO_Demo/AUDIO_V1.0/AUDIO_Demo
    demo/vendor/common/B85/calibration
    demo/vendor/common/B85/flash_internal_private_func
    demo/vendor/common/common
    drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/B85_Driver_Demo/drivers/include
)

target_link_options(${PROJECT_NAME} PRIVATE
    --gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_tc32/B85/boot.link
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    driver
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_tc32/B85/../../../tools/tl_check_fw_tool/tl_check_fw.sh   Audio_Demo   B85_Driver_Demo
    COMMENT "Executing post-build steps"
)
