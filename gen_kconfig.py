#!/usr/bin/env python3
import os
import json
import glob
from pathlib import Path

class KconfigGenerator:
    def __init__(self, root_dir):
        """Initialize the Kconfig generator with the project root directory"""
        self.root_dir = Path(root_dir).absolute()
        self.cmake_configs_dir = self.root_dir / "cmake_configs"
        self.json_files = glob.glob(str(self.cmake_configs_dir / "*_cmake.json"))
        self.kconfig_root = self.root_dir / "Kconfig"
        
        # Ensure output directories exist
        os.makedirs(self.root_dir, exist_ok=True)
    
    def parse_json(self, json_path):
        """Parse a single JSON configuration file"""
        with open(json_path, 'r') as f:
            try:
                return json.load(f)
            except json.JSONDecodeError as e:
                print(f"Error parsing {json_path}: {e}")
                return None
    
    def generate_root_kconfig(self):
        """Generate the root Kconfig file"""
        content = """# Auto-generated SDK root Kconfig
mainmenu "Telink SDK Configuration"

# Include chip configurations
source "chip/Kconfig"

# Include demo configurations
source "demo/Kconfig"

# Include toolchain configurations
source "tools/Kconfig"
"""
        with open(self.kconfig_root, 'w') as f:
            f.write(content)
        print(f"Generated root Kconfig: {self.kconfig_root}")
    
    def generate_chip_kconfig(self):
        """Generate Kconfig configurations for chips using direct names"""
        chip_kconfig_dir = self.root_dir / "chip"
        os.makedirs(chip_kconfig_dir, exist_ok=True)
        chip_kconfig = chip_kconfig_dir / "Kconfig"
        
        content = "# Auto-generated chip configurations\n\n"
        content += "menu \"Chip Selection\"\n\n"
        
        # Extract all chip names - using direct names like TC321X, B92, TL321X
        chips = []
        for json_file in self.json_files:
            json_name = os.path.splitext(os.path.basename(json_file))[0]
            if "PLATFORM_SDK_" in json_name:
                # Remove prefixes to get direct chip names
                chip = json_name.replace("PLATFORM_SDK_", "").replace("_cmake", "")
                chips.append(chip)
        
        # Add chip selection configurations with direct names
        for i, chip in enumerate(chips):
            content += f"config CHIP_{chip.upper()}\n"  # e.g., CHIP_TC321X, CHIP_B92
            content += f"    bool \"{chip}\"\n"  # Display name as direct chip name
            if i == 0:
                content += "    default y\n"
            content += f"    help\n"
            content += f"        Select {chip} as the target platform\n\n"
        
        content += "endmenu\n"
        
        with open(chip_kconfig, 'w') as f:
            f.write(content)
        print(f"Generated chip Kconfig: {chip_kconfig}")
    
    def generate_demo_kconfig(self):
        """Generate Kconfig configurations for demo programs"""
        demo_kconfig_dir = self.root_dir / "demo"
        os.makedirs(demo_kconfig_dir, exist_ok=True)
        demo_kconfig = demo_kconfig_dir / "Kconfig"
        
        content = "# Auto-generated demo program configurations\n\n"
        content += "menu \"Demo Program Selection\"\n\n"
        
        # Collect all demos
        demos = set()
        for json_file in self.json_files:
            json_data = self.parse_json(json_file)
            if json_data and "targets" in json_data:
                for target in json_data["targets"]:
                    if "name" in target:
                        demos.add(target["name"])
        
        # Add demo selection configurations
        for demo in sorted(demos):
            content += f"config DEMO_{demo.upper()}\n"
            content += f"    bool \"{demo}\"\n"
            content += f"    help\n"
            content += f"        Enable {demo} sample program\n\n"
        
        content += "endmenu\n"
        
        with open(demo_kconfig, 'w') as f:
            f.write(content)
        print(f"Generated demo Kconfig: {demo_kconfig}")
    
    def generate_toolchain_kconfig(self):
        """Generate Kconfig configurations for toolchains"""
        tool_kconfig_dir = self.root_dir / "tools"
        os.makedirs(tool_kconfig_dir, exist_ok=True)
        tool_kconfig = tool_kconfig_dir / "Kconfig"
        
        content = "# Auto-generated toolchain configurations\n\n"
        
        # Toolchain selection
        content += "menu \"Toolchain Selection\"\n\n"
        content += "config TOOLCHAIN_RISCV\n"
        content += "    bool \"RISC-V Cross GCC\"\n"
        content += "    default y\n"
        content += "    help\n"
        content += "        Use RISC-V cross-compilation toolchain\n\n"
        
        content += "config TOOLCHAIN_TC32\n"
        content += "    bool \"TC32-GCC Toolchain\"\n"
        content += "    help\n"
        content += "        Use TC32 compilation toolchain\n\n"
        content += "endmenu\n\n"
        
        # Optimization level configuration
        content += "menu \"Compilation Optimization Level\"\n\n"
        content += "config OPT_LEVEL_0\n"
        content += "    bool \"-O0 (No optimization)\"\n"
        
        content += "config OPT_LEVEL_1\n"
        content += "    bool \"-O1 (Basic optimization)\"\n"
        
        content += "config OPT_LEVEL_2\n"
        content += "    bool \"-O2 (More optimization)\"\n"
        content += "    default y\n"
        
        content += "config OPT_LEVEL_3\n"
        content += "    bool \"-O3 (Maximum optimization)\"\n"
        content += "endmenu\n"
        
        with open(tool_kconfig, 'w') as f:
            f.write(content)
        print(f"Generated toolchain Kconfig: {tool_kconfig}")
    
    def generate_kconfig_cmake(self):
        """Generate file mapping Kconfig configurations to CMake variables with direct chip names"""
        cmake_dir = self.root_dir / "cmake"
        os.makedirs(cmake_dir, exist_ok=True)
        kconfig_cmake = cmake_dir / "kconfig.cmake"
        
        content = "# Auto-generated Kconfig to CMake variable mapping\n\n"
        content += "# Chip selection with direct chip names\n"
        # Using direct chip names without extra prefixes
        content += "if(CHIP_TC321X)\n"
        content += "    set(CHIP_NAME \"TC321X\")\n"
        content += "elseif(CHIP_TL321X)\n"
        content += "    set(CHIP_NAME \"TL321X\")\n"
        content += "elseif(CHIP_TL322X)\n"
        content += "    set(CHIP_NAME \"TL322X\")\n"
        content += "elseif(CHIP_TL721X)\n"
        content += "    set(CHIP_NAME \"TL721X\")\n"
        content += "elseif(CHIP_B91)\n"
        content += "    set(CHIP_NAME \"B91\")\n"
        content += "elseif(CHIP_B92)\n"
        content += "    set(CHIP_NAME \"B92\")\n"
        content += "endif()\n\n"
        
        # Optimization level
        content += "# Optimization level\n"
        content += "if(OPT_LEVEL_0)\n"
        content += "    add_compile_options(-O0)\n"
        content += "elseif(OPT_LEVEL_1)\n"
        content += "    add_compile_options(-O1)\n"
        content += "elseif(OPT_LEVEL_2)\n"
        content += "    add_compile_options(-O2)\n"
        content += "elseif(OPT_LEVEL_3)\n"
        content += "    add_compile_options(-O3)\n"
        content += "endif()\n\n"
        
        # Toolchain selection
        content += "# Toolchain selection\n"
        content += "if(TOOLCHAIN_RISCV)\n"
        content += "    set(TOOLCHAIN_NAME \"RISC-V Cross GCC\")\n"
        content += "elseif(TOOLCHAIN_TC32)\n"
        content += "    set(TOOLCHAIN_NAME \"TC32-GCC Toolchain\")\n"
        content += "endif()\n"
        
        with open(kconfig_cmake, 'w') as f:
            f.write(content)
        print(f"Generated kconfig.cmake: {kconfig_cmake}")
    
    def generate_all(self):
        """Generate all Kconfig files"""
        self.generate_root_kconfig()
        self.generate_chip_kconfig()
        self.generate_demo_kconfig()
        self.generate_toolchain_kconfig()
        self.generate_kconfig_cmake()

if __name__ == "__main__":
    generator = KconfigGenerator(os.getcwd())
    generator.generate_all()
    print("Kconfig file generation completed")
