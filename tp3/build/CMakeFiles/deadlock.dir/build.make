# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/build

# Include any dependencies generated for this target.
include CMakeFiles/deadlock.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/deadlock.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/deadlock.dir/flags.make

CMakeFiles/deadlock.dir/deadlock.c.o: CMakeFiles/deadlock.dir/flags.make
CMakeFiles/deadlock.dir/deadlock.c.o: ../deadlock.c
	$(CMAKE_COMMAND) -E cmake_progress_report /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/deadlock.dir/deadlock.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/deadlock.dir/deadlock.c.o   -c /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/deadlock.c

CMakeFiles/deadlock.dir/deadlock.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/deadlock.dir/deadlock.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/deadlock.c > CMakeFiles/deadlock.dir/deadlock.c.i

CMakeFiles/deadlock.dir/deadlock.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/deadlock.dir/deadlock.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/deadlock.c -o CMakeFiles/deadlock.dir/deadlock.c.s

CMakeFiles/deadlock.dir/deadlock.c.o.requires:
.PHONY : CMakeFiles/deadlock.dir/deadlock.c.o.requires

CMakeFiles/deadlock.dir/deadlock.c.o.provides: CMakeFiles/deadlock.dir/deadlock.c.o.requires
	$(MAKE) -f CMakeFiles/deadlock.dir/build.make CMakeFiles/deadlock.dir/deadlock.c.o.provides.build
.PHONY : CMakeFiles/deadlock.dir/deadlock.c.o.provides

CMakeFiles/deadlock.dir/deadlock.c.o.provides.build: CMakeFiles/deadlock.dir/deadlock.c.o

# Object files for target deadlock
deadlock_OBJECTS = \
"CMakeFiles/deadlock.dir/deadlock.c.o"

# External object files for target deadlock
deadlock_EXTERNAL_OBJECTS =

deadlock: CMakeFiles/deadlock.dir/deadlock.c.o
deadlock: CMakeFiles/deadlock.dir/build.make
deadlock: /usr/lib64/openmpi/lib/libmpi.so
deadlock: CMakeFiles/deadlock.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable deadlock"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/deadlock.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/deadlock.dir/build: deadlock
.PHONY : CMakeFiles/deadlock.dir/build

CMakeFiles/deadlock.dir/requires: CMakeFiles/deadlock.dir/deadlock.c.o.requires
.PHONY : CMakeFiles/deadlock.dir/requires

CMakeFiles/deadlock.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/deadlock.dir/cmake_clean.cmake
.PHONY : CMakeFiles/deadlock.dir/clean

CMakeFiles/deadlock.dir/depend:
	cd /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3 /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3 /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/build /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/build /user/5/.base/mazarsa/home/3a/calcul_parallèle/tp3/build/CMakeFiles/deadlock.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/deadlock.dir/depend
