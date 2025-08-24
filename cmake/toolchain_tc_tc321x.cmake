# Toolchain configuration for TC_TC321X

# TC32 architecture specific settings
set(ARCH tc32)
set(CMAKE_C_COMPILER tc32-elf-gcc)
set(CMAKE_ASM_COMPILER tc32-elf-gcc)
set(CMAKE_LINKER tc32-elf-ld)
set(CMAKE_OBJCOPY tc32-elf-objcopy)
add_compile_options(
    -mtc32
    -mlittle-endian
)

add_link_options(
    -mtc32
    -nostartfiles
)
