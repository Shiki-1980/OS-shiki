# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /root/data/OS-24FALL-FDU/CMakeFiles /root/data/OS-24FALL-FDU//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /root/data/OS-24FALL-FDU/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -P /root/data/OS-24FALL-FDU/CMakeFiles/VerifyGlobs.cmake
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named qemu

# Build rule for target.
qemu: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 qemu
.PHONY : qemu

# fast build rule for target.
qemu/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/qemu.dir/build.make CMakeFiles/qemu.dir/build
.PHONY : qemu/fast

#=============================================================================
# Target rules for targets named qemu-debug

# Build rule for target.
qemu-debug: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 qemu-debug
.PHONY : qemu-debug

# fast build rule for target.
qemu-debug/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/qemu-debug.dir/build.make CMakeFiles/qemu-debug.dir/build
.PHONY : qemu-debug/fast

#=============================================================================
# Target rules for targets named debug

# Build rule for target.
debug: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 debug
.PHONY : debug

# fast build rule for target.
debug/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/debug.dir/build.make CMakeFiles/debug.dir/build
.PHONY : debug/fast

#=============================================================================
# Target rules for targets named init_libc

# Build rule for target.
init_libc: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 init_libc
.PHONY : init_libc

# fast build rule for target.
init_libc/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/init_libc.dir/build.make CMakeFiles/init_libc.dir/build
.PHONY : init_libc/fast

#=============================================================================
# Target rules for targets named libc

# Build rule for target.
libc: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 libc
.PHONY : libc

# fast build rule for target.
libc/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/libc.dir/build.make CMakeFiles/libc.dir/build
.PHONY : libc/fast

#=============================================================================
# Target rules for targets named kernel8.elf

# Build rule for target.
kernel8.elf: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 kernel8.elf
.PHONY : kernel8.elf

# fast build rule for target.
kernel8.elf/fast:
	$(MAKE) $(MAKESILENT) -f src/CMakeFiles/kernel8.elf.dir/build.make src/CMakeFiles/kernel8.elf.dir/build
.PHONY : kernel8.elf/fast

#=============================================================================
# Target rules for targets named kernel

# Build rule for target.
kernel: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 kernel
.PHONY : kernel

# fast build rule for target.
kernel/fast:
	$(MAKE) $(MAKESILENT) -f src/CMakeFiles/kernel.dir/build.make src/CMakeFiles/kernel.dir/build
.PHONY : kernel/fast

#=============================================================================
# Target rules for targets named aarch64

# Build rule for target.
aarch64: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 aarch64
.PHONY : aarch64

# fast build rule for target.
aarch64/fast:
	$(MAKE) $(MAKESILENT) -f src/aarch64/CMakeFiles/aarch64.dir/build.make src/aarch64/CMakeFiles/aarch64.dir/build
.PHONY : aarch64/fast

#=============================================================================
# Target rules for targets named common

# Build rule for target.
common: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 common
.PHONY : common

# fast build rule for target.
common/fast:
	$(MAKE) $(MAKESILENT) -f src/common/CMakeFiles/common.dir/build.make src/common/CMakeFiles/common.dir/build
.PHONY : common/fast

#=============================================================================
# Target rules for targets named kernelx

# Build rule for target.
kernelx: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 kernelx
.PHONY : kernelx

# fast build rule for target.
kernelx/fast:
	$(MAKE) $(MAKESILENT) -f src/kernel/CMakeFiles/kernelx.dir/build.make src/kernel/CMakeFiles/kernelx.dir/build
.PHONY : kernelx/fast

#=============================================================================
# Target rules for targets named driver

# Build rule for target.
driver: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 driver
.PHONY : driver

# fast build rule for target.
driver/fast:
	$(MAKE) $(MAKESILENT) -f src/driver/CMakeFiles/driver.dir/build.make src/driver/CMakeFiles/driver.dir/build
.PHONY : driver/fast

#=============================================================================
# Target rules for targets named user

# Build rule for target.
user: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 user
.PHONY : user

# fast build rule for target.
user/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/user.dir/build.make src/user/CMakeFiles/user.dir/build
.PHONY : user/fast

#=============================================================================
# Target rules for targets named user_bin

# Build rule for target.
user_bin: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 user_bin
.PHONY : user_bin

# fast build rule for target.
user_bin/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/user_bin.dir/build.make src/user/CMakeFiles/user_bin.dir/build
.PHONY : user_bin/fast

#=============================================================================
# Target rules for targets named cat

# Build rule for target.
cat: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 cat
.PHONY : cat

# fast build rule for target.
cat/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/cat.dir/build.make src/user/CMakeFiles/cat.dir/build
.PHONY : cat/fast

#=============================================================================
# Target rules for targets named echo

# Build rule for target.
echo: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 echo
.PHONY : echo

# fast build rule for target.
echo/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/echo.dir/build.make src/user/CMakeFiles/echo.dir/build
.PHONY : echo/fast

#=============================================================================
# Target rules for targets named init

# Build rule for target.
init: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 init
.PHONY : init

# fast build rule for target.
init/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/init.dir/build.make src/user/CMakeFiles/init.dir/build
.PHONY : init/fast

#=============================================================================
# Target rules for targets named ls

# Build rule for target.
ls: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 ls
.PHONY : ls

# fast build rule for target.
ls/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/ls.dir/build.make src/user/CMakeFiles/ls.dir/build
.PHONY : ls/fast

#=============================================================================
# Target rules for targets named sh

# Build rule for target.
sh: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 sh
.PHONY : sh

# fast build rule for target.
sh/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/sh.dir/build.make src/user/CMakeFiles/sh.dir/build
.PHONY : sh/fast

#=============================================================================
# Target rules for targets named mkdir

# Build rule for target.
mkdir: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 mkdir
.PHONY : mkdir

# fast build rule for target.
mkdir/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/mkdir.dir/build.make src/user/CMakeFiles/mkdir.dir/build
.PHONY : mkdir/fast

#=============================================================================
# Target rules for targets named usertests

# Build rule for target.
usertests: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 usertests
.PHONY : usertests

# fast build rule for target.
usertests/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/usertests.dir/build.make src/user/CMakeFiles/usertests.dir/build
.PHONY : usertests/fast

#=============================================================================
# Target rules for targets named mkfs

# Build rule for target.
mkfs: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 mkfs
.PHONY : mkfs

# fast build rule for target.
mkfs/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/mkfs.dir/build.make src/user/CMakeFiles/mkfs.dir/build
.PHONY : mkfs/fast

#=============================================================================
# Target rules for targets named mmaptest

# Build rule for target.
mmaptest: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 mmaptest
.PHONY : mmaptest

# fast build rule for target.
mmaptest/fast:
	$(MAKE) $(MAKESILENT) -f src/user/CMakeFiles/mmaptest.dir/build.make src/user/CMakeFiles/mmaptest.dir/build
.PHONY : mmaptest/fast

#=============================================================================
# Target rules for targets named test

# Build rule for target.
test: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 test
.PHONY : test

# fast build rule for target.
test/fast:
	$(MAKE) $(MAKESILENT) -f src/test/CMakeFiles/test.dir/build.make src/test/CMakeFiles/test.dir/build
.PHONY : test/fast

#=============================================================================
# Target rules for targets named fs

# Build rule for target.
fs: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 fs
.PHONY : fs

# fast build rule for target.
fs/fast:
	$(MAKE) $(MAKESILENT) -f src/fs/CMakeFiles/fs.dir/build.make src/fs/CMakeFiles/fs.dir/build
.PHONY : fs/fast

#=============================================================================
# Target rules for targets named image

# Build rule for target.
image: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 image
.PHONY : image

# fast build rule for target.
image/fast:
	$(MAKE) $(MAKESILENT) -f boot/CMakeFiles/image.dir/build.make boot/CMakeFiles/image.dir/build
.PHONY : image/fast

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... debug"
	@echo "... image"
	@echo "... init_libc"
	@echo "... kernel"
	@echo "... libc"
	@echo "... qemu"
	@echo "... qemu-debug"
	@echo "... user_bin"
	@echo "... aarch64"
	@echo "... cat"
	@echo "... common"
	@echo "... driver"
	@echo "... echo"
	@echo "... fs"
	@echo "... init"
	@echo "... kernel8.elf"
	@echo "... kernelx"
	@echo "... ls"
	@echo "... mkdir"
	@echo "... mkfs"
	@echo "... mmaptest"
	@echo "... sh"
	@echo "... test"
	@echo "... user"
	@echo "... usertests"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -P /root/data/OS-24FALL-FDU/CMakeFiles/VerifyGlobs.cmake
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

