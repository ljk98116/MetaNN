# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/ljk98/MetaNN

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ljk98/MetaNN/build

# Include any dependencies generated for this target.
include test/CMakeFiles/varTypeDictTest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include test/CMakeFiles/varTypeDictTest.dir/compiler_depend.make

# Include the progress variables for this target.
include test/CMakeFiles/varTypeDictTest.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/varTypeDictTest.dir/flags.make

test/CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o: test/CMakeFiles/varTypeDictTest.dir/flags.make
test/CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o: ../test/VarTypeDictTest.cpp
test/CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o: test/CMakeFiles/varTypeDictTest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ljk98/MetaNN/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o"
	cd /home/ljk98/MetaNN/build/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT test/CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o -MF CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o.d -o CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o -c /home/ljk98/MetaNN/test/VarTypeDictTest.cpp

test/CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.i"
	cd /home/ljk98/MetaNN/build/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ljk98/MetaNN/test/VarTypeDictTest.cpp > CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.i

test/CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.s"
	cd /home/ljk98/MetaNN/build/test && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ljk98/MetaNN/test/VarTypeDictTest.cpp -o CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.s

# Object files for target varTypeDictTest
varTypeDictTest_OBJECTS = \
"CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o"

# External object files for target varTypeDictTest
varTypeDictTest_EXTERNAL_OBJECTS =

test/varTypeDictTest: test/CMakeFiles/varTypeDictTest.dir/VarTypeDictTest.cpp.o
test/varTypeDictTest: test/CMakeFiles/varTypeDictTest.dir/build.make
test/varTypeDictTest: /usr/local/lib/libCppUTest.a
test/varTypeDictTest: /usr/local/lib/libCppUTestExt.a
test/varTypeDictTest: test/CMakeFiles/varTypeDictTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ljk98/MetaNN/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable varTypeDictTest"
	cd /home/ljk98/MetaNN/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/varTypeDictTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/varTypeDictTest.dir/build: test/varTypeDictTest
.PHONY : test/CMakeFiles/varTypeDictTest.dir/build

test/CMakeFiles/varTypeDictTest.dir/clean:
	cd /home/ljk98/MetaNN/build/test && $(CMAKE_COMMAND) -P CMakeFiles/varTypeDictTest.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/varTypeDictTest.dir/clean

test/CMakeFiles/varTypeDictTest.dir/depend:
	cd /home/ljk98/MetaNN/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ljk98/MetaNN /home/ljk98/MetaNN/test /home/ljk98/MetaNN/build /home/ljk98/MetaNN/build/test /home/ljk98/MetaNN/build/test/CMakeFiles/varTypeDictTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/varTypeDictTest.dir/depend

