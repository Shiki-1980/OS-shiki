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

# Include any dependencies generated for this target.
include src/common/CMakeFiles/common.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/common/CMakeFiles/common.dir/compiler_depend.make

# Include the progress variables for this target.
include src/common/CMakeFiles/common.dir/progress.make

# Include the compile flags for this target's objects.
include src/common/CMakeFiles/common.dir/flags.make

src/common/CMakeFiles/common.dir/format.c.o: src/common/CMakeFiles/common.dir/flags.make
src/common/CMakeFiles/common.dir/format.c.o: src/common/format.c
src/common/CMakeFiles/common.dir/format.c.o: src/common/CMakeFiles/common.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/common/CMakeFiles/common.dir/format.c.o"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/common/CMakeFiles/common.dir/format.c.o -MF CMakeFiles/common.dir/format.c.o.d -o CMakeFiles/common.dir/format.c.o -c /root/data/OS-24FALL-FDU/src/common/format.c

src/common/CMakeFiles/common.dir/format.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/common.dir/format.c.i"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/common/format.c > CMakeFiles/common.dir/format.c.i

src/common/CMakeFiles/common.dir/format.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/common.dir/format.c.s"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/common/format.c -o CMakeFiles/common.dir/format.c.s

src/common/CMakeFiles/common.dir/list.c.o: src/common/CMakeFiles/common.dir/flags.make
src/common/CMakeFiles/common.dir/list.c.o: src/common/list.c
src/common/CMakeFiles/common.dir/list.c.o: src/common/CMakeFiles/common.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object src/common/CMakeFiles/common.dir/list.c.o"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/common/CMakeFiles/common.dir/list.c.o -MF CMakeFiles/common.dir/list.c.o.d -o CMakeFiles/common.dir/list.c.o -c /root/data/OS-24FALL-FDU/src/common/list.c

src/common/CMakeFiles/common.dir/list.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/common.dir/list.c.i"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/common/list.c > CMakeFiles/common.dir/list.c.i

src/common/CMakeFiles/common.dir/list.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/common.dir/list.c.s"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/common/list.c -o CMakeFiles/common.dir/list.c.s

src/common/CMakeFiles/common.dir/rbtree.c.o: src/common/CMakeFiles/common.dir/flags.make
src/common/CMakeFiles/common.dir/rbtree.c.o: src/common/rbtree.c
src/common/CMakeFiles/common.dir/rbtree.c.o: src/common/CMakeFiles/common.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object src/common/CMakeFiles/common.dir/rbtree.c.o"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/common/CMakeFiles/common.dir/rbtree.c.o -MF CMakeFiles/common.dir/rbtree.c.o.d -o CMakeFiles/common.dir/rbtree.c.o -c /root/data/OS-24FALL-FDU/src/common/rbtree.c

src/common/CMakeFiles/common.dir/rbtree.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/common.dir/rbtree.c.i"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/common/rbtree.c > CMakeFiles/common.dir/rbtree.c.i

src/common/CMakeFiles/common.dir/rbtree.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/common.dir/rbtree.c.s"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/common/rbtree.c -o CMakeFiles/common.dir/rbtree.c.s

src/common/CMakeFiles/common.dir/rc.c.o: src/common/CMakeFiles/common.dir/flags.make
src/common/CMakeFiles/common.dir/rc.c.o: src/common/rc.c
src/common/CMakeFiles/common.dir/rc.c.o: src/common/CMakeFiles/common.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object src/common/CMakeFiles/common.dir/rc.c.o"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/common/CMakeFiles/common.dir/rc.c.o -MF CMakeFiles/common.dir/rc.c.o.d -o CMakeFiles/common.dir/rc.c.o -c /root/data/OS-24FALL-FDU/src/common/rc.c

src/common/CMakeFiles/common.dir/rc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/common.dir/rc.c.i"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/common/rc.c > CMakeFiles/common.dir/rc.c.i

src/common/CMakeFiles/common.dir/rc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/common.dir/rc.c.s"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/common/rc.c -o CMakeFiles/common.dir/rc.c.s

src/common/CMakeFiles/common.dir/sem.c.o: src/common/CMakeFiles/common.dir/flags.make
src/common/CMakeFiles/common.dir/sem.c.o: src/common/sem.c
src/common/CMakeFiles/common.dir/sem.c.o: src/common/CMakeFiles/common.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object src/common/CMakeFiles/common.dir/sem.c.o"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/common/CMakeFiles/common.dir/sem.c.o -MF CMakeFiles/common.dir/sem.c.o.d -o CMakeFiles/common.dir/sem.c.o -c /root/data/OS-24FALL-FDU/src/common/sem.c

src/common/CMakeFiles/common.dir/sem.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/common.dir/sem.c.i"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/common/sem.c > CMakeFiles/common.dir/sem.c.i

src/common/CMakeFiles/common.dir/sem.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/common.dir/sem.c.s"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/common/sem.c -o CMakeFiles/common.dir/sem.c.s

src/common/CMakeFiles/common.dir/spinlock.c.o: src/common/CMakeFiles/common.dir/flags.make
src/common/CMakeFiles/common.dir/spinlock.c.o: src/common/spinlock.c
src/common/CMakeFiles/common.dir/spinlock.c.o: src/common/CMakeFiles/common.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object src/common/CMakeFiles/common.dir/spinlock.c.o"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/common/CMakeFiles/common.dir/spinlock.c.o -MF CMakeFiles/common.dir/spinlock.c.o.d -o CMakeFiles/common.dir/spinlock.c.o -c /root/data/OS-24FALL-FDU/src/common/spinlock.c

src/common/CMakeFiles/common.dir/spinlock.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/common.dir/spinlock.c.i"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/common/spinlock.c > CMakeFiles/common.dir/spinlock.c.i

src/common/CMakeFiles/common.dir/spinlock.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/common.dir/spinlock.c.s"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/common/spinlock.c -o CMakeFiles/common.dir/spinlock.c.s

src/common/CMakeFiles/common.dir/string.c.o: src/common/CMakeFiles/common.dir/flags.make
src/common/CMakeFiles/common.dir/string.c.o: src/common/string.c
src/common/CMakeFiles/common.dir/string.c.o: src/common/CMakeFiles/common.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object src/common/CMakeFiles/common.dir/string.c.o"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/common/CMakeFiles/common.dir/string.c.o -MF CMakeFiles/common.dir/string.c.o.d -o CMakeFiles/common.dir/string.c.o -c /root/data/OS-24FALL-FDU/src/common/string.c

src/common/CMakeFiles/common.dir/string.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/common.dir/string.c.i"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/common/string.c > CMakeFiles/common.dir/string.c.i

src/common/CMakeFiles/common.dir/string.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/common.dir/string.c.s"
	cd /root/data/OS-24FALL-FDU/src/common && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/common/string.c -o CMakeFiles/common.dir/string.c.s

# Object files for target common
common_OBJECTS = \
"CMakeFiles/common.dir/format.c.o" \
"CMakeFiles/common.dir/list.c.o" \
"CMakeFiles/common.dir/rbtree.c.o" \
"CMakeFiles/common.dir/rc.c.o" \
"CMakeFiles/common.dir/sem.c.o" \
"CMakeFiles/common.dir/spinlock.c.o" \
"CMakeFiles/common.dir/string.c.o"

# External object files for target common
common_EXTERNAL_OBJECTS =

src/common/libcommon.a: src/common/CMakeFiles/common.dir/format.c.o
src/common/libcommon.a: src/common/CMakeFiles/common.dir/list.c.o
src/common/libcommon.a: src/common/CMakeFiles/common.dir/rbtree.c.o
src/common/libcommon.a: src/common/CMakeFiles/common.dir/rc.c.o
src/common/libcommon.a: src/common/CMakeFiles/common.dir/sem.c.o
src/common/libcommon.a: src/common/CMakeFiles/common.dir/spinlock.c.o
src/common/libcommon.a: src/common/CMakeFiles/common.dir/string.c.o
src/common/libcommon.a: src/common/CMakeFiles/common.dir/build.make
src/common/libcommon.a: src/common/CMakeFiles/common.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking C static library libcommon.a"
	cd /root/data/OS-24FALL-FDU/src/common && $(CMAKE_COMMAND) -P CMakeFiles/common.dir/cmake_clean_target.cmake
	cd /root/data/OS-24FALL-FDU/src/common && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/common.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/common/CMakeFiles/common.dir/build: src/common/libcommon.a
.PHONY : src/common/CMakeFiles/common.dir/build

src/common/CMakeFiles/common.dir/clean:
	cd /root/data/OS-24FALL-FDU/src/common && $(CMAKE_COMMAND) -P CMakeFiles/common.dir/cmake_clean.cmake
.PHONY : src/common/CMakeFiles/common.dir/clean

src/common/CMakeFiles/common.dir/depend:
	cd /root/data/OS-24FALL-FDU && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/data/OS-24FALL-FDU /root/data/OS-24FALL-FDU/src/common /root/data/OS-24FALL-FDU /root/data/OS-24FALL-FDU/src/common /root/data/OS-24FALL-FDU/src/common/CMakeFiles/common.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/common/CMakeFiles/common.dir/depend

