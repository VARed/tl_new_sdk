# Auto-generated Kconfig to CMake variable mapping

# Chip selection with direct chip names
if(CHIP_TC321X)
    set(CHIP_NAME "TC321X")
elseif(CHIP_TL321X)
    set(CHIP_NAME "TL321X")
elseif(CHIP_TL322X)
    set(CHIP_NAME "TL322X")
elseif(CHIP_TL721X)
    set(CHIP_NAME "TL721X")
elseif(CHIP_B91)
    set(CHIP_NAME "B91")
elseif(CHIP_B92)
    set(CHIP_NAME "B92")
endif()

# Optimization level
if(OPT_LEVEL_0)
    add_compile_options(-O0)
elseif(OPT_LEVEL_1)
    add_compile_options(-O1)
elseif(OPT_LEVEL_2)
    add_compile_options(-O2)
elseif(OPT_LEVEL_3)
    add_compile_options(-O3)
endif()

# Toolchain selection
if(TOOLCHAIN_RISCV)
    set(TOOLCHAIN_NAME "RISC-V Cross GCC")
elseif(TOOLCHAIN_TC32)
    set(TOOLCHAIN_NAME "TC32-GCC Toolchain")
endif()
