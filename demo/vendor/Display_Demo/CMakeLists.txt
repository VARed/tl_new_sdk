# Auto-generated Display_Demo demo CMake configuration
project(Display_Demo)

set(SOURCES
    chip/B80/boot
    chip/B80/drivers
    common
    demo/vendor/Display_Demo
    demo/vendor/common/B80/calibration
    demo/vendor/common/common
    drivers
    chip/B80B/drivers
    boards
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/chip/B80B_Driver_Demo/drivers/include
)

target_link_options(${PROJECT_NAME} PRIVATE
    --gc-sections
    -T${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_tc32/B80B/boot.link
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    driver_b80b
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/project/tlsr_tc32/B80B/../../../tools/tl_check_fw_tool/tl_check_fw.sh   Display_Demo   B80B_Driver_Demo
    COMMENT "Executing post-build steps"
)
