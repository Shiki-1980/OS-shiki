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
include src/test/CMakeFiles/test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/test/CMakeFiles/test.dir/compiler_depend.make

# Include the progress variables for this target.
include src/test/CMakeFiles/test.dir/progress.make

# Include the compile flags for this target's objects.
include src/test/CMakeFiles/test.dir/flags.make

src/test/CMakeFiles/test.dir/io_test.c.o: src/test/CMakeFiles/test.dir/flags.make
src/test/CMakeFiles/test.dir/io_test.c.o: src/test/io_test.c
src/test/CMakeFiles/test.dir/io_test.c.o: src/test/CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/test/CMakeFiles/test.dir/io_test.c.o"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/test/CMakeFiles/test.dir/io_test.c.o -MF CMakeFiles/test.dir/io_test.c.o.d -o CMakeFiles/test.dir/io_test.c.o -c /root/data/OS-24FALL-FDU/src/test/io_test.c

src/test/CMakeFiles/test.dir/io_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/test.dir/io_test.c.i"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/test/io_test.c > CMakeFiles/test.dir/io_test.c.i

src/test/CMakeFiles/test.dir/io_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/test.dir/io_test.c.s"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/test/io_test.c -o CMakeFiles/test.dir/io_test.c.s

src/test/CMakeFiles/test.dir/kalloc_test.c.o: src/test/CMakeFiles/test.dir/flags.make
src/test/CMakeFiles/test.dir/kalloc_test.c.o: src/test/kalloc_test.c
src/test/CMakeFiles/test.dir/kalloc_test.c.o: src/test/CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object src/test/CMakeFiles/test.dir/kalloc_test.c.o"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/test/CMakeFiles/test.dir/kalloc_test.c.o -MF CMakeFiles/test.dir/kalloc_test.c.o.d -o CMakeFiles/test.dir/kalloc_test.c.o -c /root/data/OS-24FALL-FDU/src/test/kalloc_test.c

src/test/CMakeFiles/test.dir/kalloc_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/test.dir/kalloc_test.c.i"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/test/kalloc_test.c > CMakeFiles/test.dir/kalloc_test.c.i

src/test/CMakeFiles/test.dir/kalloc_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/test.dir/kalloc_test.c.s"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/test/kalloc_test.c -o CMakeFiles/test.dir/kalloc_test.c.s

src/test/CMakeFiles/test.dir/proc_test.c.o: src/test/CMakeFiles/test.dir/flags.make
src/test/CMakeFiles/test.dir/proc_test.c.o: src/test/proc_test.c
src/test/CMakeFiles/test.dir/proc_test.c.o: src/test/CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object src/test/CMakeFiles/test.dir/proc_test.c.o"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/test/CMakeFiles/test.dir/proc_test.c.o -MF CMakeFiles/test.dir/proc_test.c.o.d -o CMakeFiles/test.dir/proc_test.c.o -c /root/data/OS-24FALL-FDU/src/test/proc_test.c

src/test/CMakeFiles/test.dir/proc_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/test.dir/proc_test.c.i"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/test/proc_test.c > CMakeFiles/test.dir/proc_test.c.i

src/test/CMakeFiles/test.dir/proc_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/test.dir/proc_test.c.s"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/test/proc_test.c -o CMakeFiles/test.dir/proc_test.c.s

src/test/CMakeFiles/test.dir/rand.c.o: src/test/CMakeFiles/test.dir/flags.make
src/test/CMakeFiles/test.dir/rand.c.o: src/test/rand.c
src/test/CMakeFiles/test.dir/rand.c.o: src/test/CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object src/test/CMakeFiles/test.dir/rand.c.o"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/test/CMakeFiles/test.dir/rand.c.o -MF CMakeFiles/test.dir/rand.c.o.d -o CMakeFiles/test.dir/rand.c.o -c /root/data/OS-24FALL-FDU/src/test/rand.c

src/test/CMakeFiles/test.dir/rand.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/test.dir/rand.c.i"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/test/rand.c > CMakeFiles/test.dir/rand.c.i

src/test/CMakeFiles/test.dir/rand.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/test.dir/rand.c.s"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/test/rand.c -o CMakeFiles/test.dir/rand.c.s

src/test/CMakeFiles/test.dir/rbtree_test.c.o: src/test/CMakeFiles/test.dir/flags.make
src/test/CMakeFiles/test.dir/rbtree_test.c.o: src/test/rbtree_test.c
src/test/CMakeFiles/test.dir/rbtree_test.c.o: src/test/CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object src/test/CMakeFiles/test.dir/rbtree_test.c.o"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/test/CMakeFiles/test.dir/rbtree_test.c.o -MF CMakeFiles/test.dir/rbtree_test.c.o.d -o CMakeFiles/test.dir/rbtree_test.c.o -c /root/data/OS-24FALL-FDU/src/test/rbtree_test.c

src/test/CMakeFiles/test.dir/rbtree_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/test.dir/rbtree_test.c.i"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/test/rbtree_test.c > CMakeFiles/test.dir/rbtree_test.c.i

src/test/CMakeFiles/test.dir/rbtree_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/test.dir/rbtree_test.c.s"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/test/rbtree_test.c -o CMakeFiles/test.dir/rbtree_test.c.s

src/test/CMakeFiles/test.dir/user_proc_test.c.o: src/test/CMakeFiles/test.dir/flags.make
src/test/CMakeFiles/test.dir/user_proc_test.c.o: src/test/user_proc_test.c
src/test/CMakeFiles/test.dir/user_proc_test.c.o: src/test/CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object src/test/CMakeFiles/test.dir/user_proc_test.c.o"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT src/test/CMakeFiles/test.dir/user_proc_test.c.o -MF CMakeFiles/test.dir/user_proc_test.c.o.d -o CMakeFiles/test.dir/user_proc_test.c.o -c /root/data/OS-24FALL-FDU/src/test/user_proc_test.c

src/test/CMakeFiles/test.dir/user_proc_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/test.dir/user_proc_test.c.i"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/data/OS-24FALL-FDU/src/test/user_proc_test.c > CMakeFiles/test.dir/user_proc_test.c.i

src/test/CMakeFiles/test.dir/user_proc_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/test.dir/user_proc_test.c.s"
	cd /root/data/OS-24FALL-FDU/src/test && gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/data/OS-24FALL-FDU/src/test/user_proc_test.c -o CMakeFiles/test.dir/user_proc_test.c.s

# Object files for target test
test_OBJECTS = \
"CMakeFiles/test.dir/io_test.c.o" \
"CMakeFiles/test.dir/kalloc_test.c.o" \
"CMakeFiles/test.dir/proc_test.c.o" \
"CMakeFiles/test.dir/rand.c.o" \
"CMakeFiles/test.dir/rbtree_test.c.o" \
"CMakeFiles/test.dir/user_proc_test.c.o"

# External object files for target test
test_EXTERNAL_OBJECTS =

src/test/libtest.a: src/test/CMakeFiles/test.dir/io_test.c.o
src/test/libtest.a: src/test/CMakeFiles/test.dir/kalloc_test.c.o
src/test/libtest.a: src/test/CMakeFiles/test.dir/proc_test.c.o
src/test/libtest.a: src/test/CMakeFiles/test.dir/rand.c.o
src/test/libtest.a: src/test/CMakeFiles/test.dir/rbtree_test.c.o
src/test/libtest.a: src/test/CMakeFiles/test.dir/user_proc_test.c.o
src/test/libtest.a: src/test/CMakeFiles/test.dir/build.make
src/test/libtest.a: src/test/CMakeFiles/test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/root/data/OS-24FALL-FDU/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking C static library libtest.a"
	cd /root/data/OS-24FALL-FDU/src/test && $(CMAKE_COMMAND) -P CMakeFiles/test.dir/cmake_clean_target.cmake
	cd /root/data/OS-24FALL-FDU/src/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/test/CMakeFiles/test.dir/build: src/test/libtest.a
.PHONY : src/test/CMakeFiles/test.dir/build

src/test/CMakeFiles/test.dir/clean:
	cd /root/data/OS-24FALL-FDU/src/test && $(CMAKE_COMMAND) -P CMakeFiles/test.dir/cmake_clean.cmake
.PHONY : src/test/CMakeFiles/test.dir/clean

src/test/CMakeFiles/test.dir/depend:
	cd /root/data/OS-24FALL-FDU && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/data/OS-24FALL-FDU /root/data/OS-24FALL-FDU/src/test /root/data/OS-24FALL-FDU /root/data/OS-24FALL-FDU/src/test /root/data/OS-24FALL-FDU/src/test/CMakeFiles/test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : src/test/CMakeFiles/test.dir/depend

