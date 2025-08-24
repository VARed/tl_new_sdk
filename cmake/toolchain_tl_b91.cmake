# Toolchain configuration for TL_B91

# RISC-V architecture specific settings
set(ARCH riscv)
set(CMAKE_C_COMPILER riscv32-elf-gcc)
set(CMAKE_ASM_COMPILER riscv32-elf-gcc)
set(CMAKE_LINKER riscv32-elf-ld)
set(CMAKE_OBJCOPY riscv32-elf-objcopy)
add_compile_options(
    -march=rv32imc
    -mabi=ilp32
    -mcmodel=medany
)

add_link_options(
    -march=rv32imc
    -mabi=ilp32
)
