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
CMAKE_SOURCE_DIR = "/mnt/e/Programming Projects/C++/Chess Engine"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/mnt/e/Programming Projects/C++/Chess Engine/build"

# Include any dependencies generated for this target.
include CMakeFiles/OctoKnight.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/OctoKnight.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/OctoKnight.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/OctoKnight.dir/flags.make

CMakeFiles/OctoKnight.dir/src/main.cpp.o: CMakeFiles/OctoKnight.dir/flags.make
CMakeFiles/OctoKnight.dir/src/main.cpp.o: /mnt/e/Programming\ Projects/C++/Chess\ Engine/src/main.cpp
CMakeFiles/OctoKnight.dir/src/main.cpp.o: CMakeFiles/OctoKnight.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/mnt/e/Programming Projects/C++/Chess Engine/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/OctoKnight.dir/src/main.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/OctoKnight.dir/src/main.cpp.o -MF CMakeFiles/OctoKnight.dir/src/main.cpp.o.d -o CMakeFiles/OctoKnight.dir/src/main.cpp.o -c "/mnt/e/Programming Projects/C++/Chess Engine/src/main.cpp"

CMakeFiles/OctoKnight.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/OctoKnight.dir/src/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/mnt/e/Programming Projects/C++/Chess Engine/src/main.cpp" > CMakeFiles/OctoKnight.dir/src/main.cpp.i

CMakeFiles/OctoKnight.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/OctoKnight.dir/src/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/mnt/e/Programming Projects/C++/Chess Engine/src/main.cpp" -o CMakeFiles/OctoKnight.dir/src/main.cpp.s

# Object files for target OctoKnight
OctoKnight_OBJECTS = \
"CMakeFiles/OctoKnight.dir/src/main.cpp.o"

# External object files for target OctoKnight
OctoKnight_EXTERNAL_OBJECTS =

OctoKnight: CMakeFiles/OctoKnight.dir/src/main.cpp.o
OctoKnight: CMakeFiles/OctoKnight.dir/build.make
OctoKnight: CMakeFiles/OctoKnight.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/mnt/e/Programming Projects/C++/Chess Engine/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable OctoKnight"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OctoKnight.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/OctoKnight.dir/build: OctoKnight
.PHONY : CMakeFiles/OctoKnight.dir/build

CMakeFiles/OctoKnight.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/OctoKnight.dir/cmake_clean.cmake
.PHONY : CMakeFiles/OctoKnight.dir/clean

CMakeFiles/OctoKnight.dir/depend:
	cd "/mnt/e/Programming Projects/C++/Chess Engine/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/mnt/e/Programming Projects/C++/Chess Engine" "/mnt/e/Programming Projects/C++/Chess Engine" "/mnt/e/Programming Projects/C++/Chess Engine/build" "/mnt/e/Programming Projects/C++/Chess Engine/build" "/mnt/e/Programming Projects/C++/Chess Engine/build/CMakeFiles/OctoKnight.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/OctoKnight.dir/depend

