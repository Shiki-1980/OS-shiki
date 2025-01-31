# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/data/OS-24FALL-FDU

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/data/OS-24FALL-FDU

# Utility rule file for qemu-debug.

# Include any custom commands dependencies for this target.
include CMakeFiles/qemu-debug.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/qemu-debug.dir/progress.make

CMakeFiles/qemu-debug:
	qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a72 -smp 4 -m 4096 -nographic -monitor none -serial mon:stdio -global virtio-mmio.force-legacy=false -drive file=/root/data/OS-24FALL-FDU/boot/sd.img,if=none,format=raw,id=d0 -device virtio-blk-device,drive=d0,bus=virtio-mmio-bus.0 -kernel /root/data/OS-24FALL-FDU/src/kernel8.elf -gdb tcp::1234 -S

qemu-debug: CMakeFiles/qemu-debug
qemu-debug: CMakeFiles/qemu-debug.dir/build.make
.PHONY : qemu-debug

# Rule to build all files generated by this target.
CMakeFiles/qemu-debug.dir/build: qemu-debug
.PHONY : CMakeFiles/qemu-debug.dir/build

CMakeFiles/qemu-debug.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/qemu-debug.dir/cmake_clean.cmake
.PHONY : CMakeFiles/qemu-debug.dir/clean

CMakeFiles/qemu-debug.dir/depend:
	cd /root/data/OS-24FALL-FDU && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/data/OS-24FALL-FDU /root/data/OS-24FALL-FDU /root/data/OS-24FALL-FDU /root/data/OS-24FALL-FDU /root/data/OS-24FALL-FDU/CMakeFiles/qemu-debug.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/qemu-debug.dir/depend

