#!/usr/bin/env python3
import os
import glob
from pathlib import Path

class ToolchainGenerator:
    def __init__(self, root_dir):
        """Initialize toolchain generator with project root directory"""
        self.root_dir = Path(root_dir).absolute()
        self.cmake_dir = self.root_dir / "cmake"
        self.cmake_configs_dir = self.root_dir / "cmake_configs"
        
        # Create cmake directory if it doesn't exist
        os.makedirs(self.cmake_dir, exist_ok=True)
        
        # Get all unique chip names from config files
        self.chips = self._extract_chip_names()
    
    def _extract_chip_names(self):
        """Extract unique chip names from configuration files"""
        json_files = glob.glob(str(self.cmake_configs_dir / "*_cmake.json"))
        chips = set()
        
        for json_file in json_files:
            json_name = os.path.splitext(os.path.basename(json_file))[0]
            if "PLATFORM_SDK_" in json_name:
                chip = json_name.replace("PLATFORM_SDK_", "").replace("_cmake", "")
                chips.add(chip)
        
        return sorted(chips)
    
    def generate_shared_toolchain(self):
        """Generate main toolchain.cmake with shared configurations"""
        toolchain_path = self.cmake_dir / "toolchain.cmake"
        
        # Only generate if it doesn't exist
        if os.path.exists(toolchain_path):
            print(f"Shared toolchain config already exists, skipping: {toolchain_path}")
            return
        
        content = """# Shared toolchain configuration for all chips
# This file contains common settings shared across all chip platforms

# Common compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Common warning flags
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
    -Wno-unused-function
)

# Common optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g3)
else()
    add_compile_options(-O2 -g)
endif()

# Include chip-specific toolchain configuration
"""
        
        # Add chip-specific includes
        for chip in self.chips:
            chip_upper = chip.upper()
            content += f"if(CHIP_{chip_upper})\n"
            content += f"    include(toolchain_{chip.lower()}.cmake)\n"
            content += "endif()\n"
        
        with open(toolchain_path, 'w') as f:
            f.write(content)
        
        print(f"Generated shared toolchain config: {toolchain_path}")
    
    def generate_chip_toolchains(self):
        """Generate chip-specific toolchain configurations"""
        for chip in self.chips:
            self._generate_single_chip_toolchain(chip)
    
    def _generate_single_chip_toolchain(self, chip):
        """Generate toolchain file for a single chip"""
        chip_lower = chip.lower()
        toolchain_path = self.cmake_dir / f"toolchain_{chip_lower}.cmake"
        
        # Only generate if it doesn't exist
        if os.path.exists(toolchain_path):
            print(f"Chip toolchain for {chip} already exists, skipping: {toolchain_path}")
            return
        
        # Base content - adjust based on actual chip requirements
        content = f"# Toolchain configuration for {chip}\n\n"

        # Add chip-specific compiler settings
        if chip[:2] == "TC":
            # Example for TC32 architecture
            content += "# TC32 architecture specific settings\n"
            content += "set(ARCH tc32)\n"
            content += "set(CMAKE_C_COMPILER tc32-elf-gcc)\n"
            content += "set(CMAKE_ASM_COMPILER tc32-elf-gcc)\n"
            content += "set(CMAKE_LINKER tc32-elf-ld)\n"
            content += "set(CMAKE_OBJCOPY tc32-elf-objcopy)\n"
            
            # Chip-specific compiler flags
            content += "add_compile_options(\n"
            content += "    -mtc32\n"
            content += "    -mlittle-endian\n"
            content += ")\n\n"
            
            # Chip-specific linker flags
            content += "add_link_options(\n"
            content += "    -mtc32\n"
            content += "    -nostartfiles\n"
            content += ")\n"
            
        elif chip[:2] == "TL":
            # Example for RISC-V architecture
            content += "# RISC-V architecture specific settings\n"
            content += "set(ARCH riscv)\n"
            content += "set(CMAKE_C_COMPILER riscv32-elf-gcc)\n"
            content += "set(CMAKE_ASM_COMPILER riscv32-elf-gcc)\n"
            content += "set(CMAKE_LINKER riscv32-elf-ld)\n"
            content += "set(CMAKE_OBJCOPY riscv32-elf-objcopy)\n"
            
            # Chip-specific compiler flags
            content += "add_compile_options(\n"
            content += "    -march=rv32imc\n"
            content += "    -mabi=ilp32\n"
            content += "    -mcmodel=medany\n"
            content += ")\n\n"
            
            # Chip-specific linker flags
            content += "add_link_options(\n"
            content += "    -march=rv32imc\n"
            content += "    -mabi=ilp32\n"
            content += ")\n"
        
        with open(toolchain_path, 'w') as f:
            f.write(content)
        
        print(f"Generated chip toolchain for {chip}: {toolchain_path}")
    
    def generate_all(self):
        """Generate all toolchain configuration files"""
        self.generate_shared_toolchain()
        self.generate_chip_toolchains()
        print("Toolchain configuration generation completed")

if __name__ == "__main__":
    generator = ToolchainGenerator(os.getcwd())
    generator.generate_all()
