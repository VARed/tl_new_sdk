#!/usr/bin/env python3
import os
import json
import glob
import shutil
from pathlib import Path

class CMakeGenerator:
    def __init__(self, root_dir):
        """Initialize the CMake generator with the root directory of the project"""
        self.root_dir = Path(root_dir).absolute()
        self.cmake_configs_dir = self.root_dir / "cmake_configs"
        self.json_files = glob.glob(str(self.cmake_configs_dir / "*_cmake.json"))
        
        # Ensure output directories exist
        os.makedirs(self.root_dir, exist_ok=True)
        # Create a directory to store chip-specific build configurations
        self.chip_build_dir = self.root_dir / "chip_builds"
        os.makedirs(self.chip_build_dir, exist_ok=True)
        
        # Flag to track if root CMakeLists has been generated
        self.root_cmake_generated = False
        
    def parse_json(self, json_path):
        """Parse a single JSON configuration file"""
        with open(json_path, 'r') as f:
            try:
                return json.load(f)
            except json.JSONDecodeError as e:
                print(f"Error parsing {json_path}: {e}")
                return None
    
    def generate_root_cmakelists(self):
        """Generate the root directory CMakeLists.txt only once"""
        # Only generate root CMakeLists once to prevent overwriting
        if self.root_cmake_generated:
            return
            
        root_cmake = self.root_dir / "CMakeLists.txt"
        
        content = """cmake_minimum_required(VERSION 3.19)
project(tl_new_sdk)

# Include all submodules
include(cmake/toolchain.cmake)
include(cmake/kconfig.cmake)

# Enable chip-specific configurations
option(CHIP_TC321X "Enable TC321X chip support" OFF)
option(CHIP_TL321X "Enable TL321X chip support" OFF)
option(CHIP_B92 "Enable B92 chip support" OFF)

# Recursively add all subdirectories
file(GLOB_RECURSE SUB_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach(DIR ${SUB_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/CMakeLists.txt)
        add_subdirectory(${DIR})
    endif()
endforeach()

# Include Kconfig configuration
include(${CMAKE_BINARY_DIR}/kconfig.cmake)
"""
        
        with open(root_cmake, 'w') as f:
            f.write(content)
        print(f"Generated root CMakeLists.txt: {root_cmake}")
        self.root_cmake_generated = True  # Mark as generated
    
    def generate_chip_specific_cmake(self, chip_name):
        """Generate chip-specific CMake configuration that won't be overwritten"""
        # Create unique directory for each chip's build configuration
        chip_specific_dir = self.chip_build_dir / chip_name
        os.makedirs(chip_specific_dir, exist_ok=True)
        
        # Generate chip-specific CMakeLists
        chip_cmake = chip_specific_dir / "CMakeLists.txt"
        with open(chip_cmake, 'w') as f:
            f.write(f"# Chip-specific configuration for {chip_name}\n")
            f.write(f"set(CHIP_NAME {chip_name})\n\n")
            f.write(f"# Include chip-specific sources\n")
            f.write(f"add_subdirectory(${{CMAKE_SOURCE_DIR}}/chip/{chip_name}/boot)\n")
            f.write(f"add_subdirectory(${{CMAKE_SOURCE_DIR}}/chip/{chip_name}/drivers)\n")
            f.write(f"add_subdirectory(${{CMAKE_SOURCE_DIR}}/chip/{chip_name}/link)\n\n")
            f.write(f"# Set chip-specific compiler definitions\n")
            f.write(f"add_definitions(-DCHIP_{chip_name.upper()})\n")
        
        # Create a symlink to the chip-specific configuration for easy access
        if os.name != 'nt':  # Skip on Windows which has limited symlink support
            symlink_path = self.root_dir / f"CMakeLists_{chip_name}.txt"
            if not os.path.exists(symlink_path):
                os.symlink(chip_cmake, symlink_path)
        
        print(f"Generated chip-specific CMake for {chip_name}: {chip_cmake}")
        return chip_specific_dir
    
    def generate_subdir_cmakelists(self, json_data, json_name):
        """Generate CMakeLists.txt for each subdirectory without overwriting"""
        if not json_data or "targets" not in json_data:
            return
            
        # Get direct chip name
        chip_name = json_name.replace("PLATFORM_SDK_", "").replace("_cmake", "")
        chip_dir = self.root_dir / "chip" / chip_name
        os.makedirs(chip_dir, exist_ok=True)
        
        # Generate chip directory CMakeLists (unique per chip, no overwrite)
        chip_cmake = chip_dir / "CMakeLists.txt"
        
        # Only generate if it doesn't exist or force regeneration is needed
        if not os.path.exists(chip_cmake):
            with open(chip_cmake, 'w') as f:
                f.write(f"# Auto-generated {chip_name} chip CMake configuration\n")
                f.write(f"set(CHIP_NAME {chip_name})\n\n")
                f.write("add_subdirectory(boot)\n")
                f.write("add_subdirectory(drivers)\n")
                f.write("add_subdirectory(link)\n")
            print(f"Generated chip directory CMakeLists: {chip_cmake}")
        else:
            print(f"Chip CMakeLists already exists, skipping: {chip_cmake}")
        
        # Generate chip-specific build configuration
        self.generate_chip_specific_cmake(chip_name)
        
        # Process each target
        for target in json_data["targets"]:
            target_name = target.get("name")
            if not target_name:
                continue
                
            # Create demo directory
            demo_dir = self.root_dir / "demo" / "vendor" / target_name
            os.makedirs(demo_dir, exist_ok=True)
            
            # Generate demo directory CMakeLists
            demo_cmake = demo_dir / "CMakeLists.txt"
            
            # Only generate if it doesn't exist
            if not os.path.exists(demo_cmake):
                with open(demo_cmake, 'w') as f:
                    f.write(f"# Auto-generated {target_name} demo CMake configuration\n")
                    f.write(f"project({target_name})\n\n")
                    
                    # Add source files
                    if "directories" in target:
                        f.write("set(SOURCES\n")
                        for dir_path in target["directories"]:
                            f.write(f"    {dir_path}\n")
                        f.write(")\n\n")
                    
                    # Add compilation target
                    f.write("add_executable(${PROJECT_NAME} ${SOURCES})\n\n")
                    
                    # Add chip-specific include path
                    f.write(f"target_include_directories(${{PROJECT_NAME}} PRIVATE\n")
                    f.write(f"    ${{CMAKE_SOURCE_DIR}}/chip/{chip_name}/drivers/include\n")
                    f.write(")\n\n")
                    
                    # Add linker options
                    if "linker_options" in target:
                        f.write("target_link_options(${PROJECT_NAME} PRIVATE\n")
                        for opt in target["linker_options"]:
                            f.write(f"    {opt}\n")
                        f.write(")\n\n")
                    
                    # Link libraries
                    if "linker_libraries" in target:
                        f.write("target_link_libraries(${PROJECT_NAME} PRIVATE\n")
                        for lib in target["linker_libraries"]:
                            f.write(f"    {lib}\n")
                        f.write(")\n\n")
                    
                    # Pre-build and post-build steps
                    if "pre_build" in target and len(target["pre_build"]) > 0:
                        f.write("add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD\n")
                        f.write(f"    COMMAND {target['pre_build'][0]}\n")
                        f.write("    COMMENT \"Executing pre-build steps\"\n")
                        f.write(")\n\n")
                    
                    if "post_build" in target:
                        f.write("add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD\n")
                        f.write(f"    COMMAND {target['post_build'][0]}\n")
                        f.write("    COMMENT \"Executing post-build steps\"\n")
                        f.write(")\n")
                
                print(f"Generated demo CMakeLists.txt: {demo_cmake}")
            else:
                print(f"Demo CMakeLists already exists, skipping: {demo_cmake}")
    
    def generate_toolchain_config(self):
        """Generate toolchain configuration that supports multiple chips"""
        toolchain_dir = self.root_dir / "cmake"
        os.makedirs(toolchain_dir, exist_ok=True)
        toolchain_file = toolchain_dir / "toolchain.cmake"
        
        # Only generate if it doesn't exist
        if not os.path.exists(toolchain_file):
            content = """# Multi-chip toolchain configuration
if(CHIP_TC321X)
    include(${CMAKE_SOURCE_DIR}/cmake/toolchain_tc321x.cmake)
elseif(CHIP_TL321X)
    include(${CMAKE_SOURCE_DIR}/cmake/toolchain_tl321x.cmake)
elseif(CHIP_B92)
    include(${CMAKE_SOURCE_DIR}/cmake/toolchain_b92.cmake)
else()
    message(FATAL_ERROR "No chip selected! Please enable one of the CHIP_* options.")
endif()
"""
            with open(toolchain_file, 'w') as f:
                f.write(content)
            print(f"Generated multi-chip toolchain config: {toolchain_file}")
        else:
            print(f"Toolchain config already exists, skipping: {toolchain_file}")
    
    def generate_all(self):
        """Generate all CMakeLists.txt files without overwriting existing ones"""
        # Generate root CMakeLists once
        self.generate_root_cmakelists()
        
        # Generate shared toolchain configuration
        self.generate_toolchain_config()
        
        # Create necessary auxiliary directories
        os.makedirs(self.root_dir / "cmake", exist_ok=True)
        
        for json_file in self.json_files:
            json_name = os.path.splitext(os.path.basename(json_file))[0]
            print(f"Processing configuration file: {json_name}")
            
            json_data = self.parse_json(json_file)
            if json_data:
                self.generate_subdir_cmakelists(json_data, json_name)

if __name__ == "__main__":
    # Assume the script runs in the SDK root directory
    generator = CMakeGenerator(os.getcwd())
    generator.generate_all()
    print("CMakeLists.txt generation completed without overwrites")
