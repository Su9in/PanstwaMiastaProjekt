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
CMAKE_SOURCE_DIR = /home/maatapeja/Desktop/projekt2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/maatapeja/Desktop/projekt2/build

# Include any dependencies generated for this target.
include CMakeFiles/serwer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/serwer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/serwer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/serwer.dir/flags.make

CMakeFiles/serwer.dir/serwer.cpp.o: CMakeFiles/serwer.dir/flags.make
CMakeFiles/serwer.dir/serwer.cpp.o: ../serwer.cpp
CMakeFiles/serwer.dir/serwer.cpp.o: CMakeFiles/serwer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/maatapeja/Desktop/projekt2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/serwer.dir/serwer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/serwer.dir/serwer.cpp.o -MF CMakeFiles/serwer.dir/serwer.cpp.o.d -o CMakeFiles/serwer.dir/serwer.cpp.o -c /home/maatapeja/Desktop/projekt2/serwer.cpp

CMakeFiles/serwer.dir/serwer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/serwer.dir/serwer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/maatapeja/Desktop/projekt2/serwer.cpp > CMakeFiles/serwer.dir/serwer.cpp.i

CMakeFiles/serwer.dir/serwer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/serwer.dir/serwer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/maatapeja/Desktop/projekt2/serwer.cpp -o CMakeFiles/serwer.dir/serwer.cpp.s

# Object files for target serwer
serwer_OBJECTS = \
"CMakeFiles/serwer.dir/serwer.cpp.o"

# External object files for target serwer
serwer_EXTERNAL_OBJECTS =

serwer: CMakeFiles/serwer.dir/serwer.cpp.o
serwer: CMakeFiles/serwer.dir/build.make
serwer: CMakeFiles/serwer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/maatapeja/Desktop/projekt2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable serwer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/serwer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/serwer.dir/build: serwer
.PHONY : CMakeFiles/serwer.dir/build

CMakeFiles/serwer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/serwer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/serwer.dir/clean

CMakeFiles/serwer.dir/depend:
	cd /home/maatapeja/Desktop/projekt2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/maatapeja/Desktop/projekt2 /home/maatapeja/Desktop/projekt2 /home/maatapeja/Desktop/projekt2/build /home/maatapeja/Desktop/projekt2/build /home/maatapeja/Desktop/projekt2/build/CMakeFiles/serwer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/serwer.dir/depend

