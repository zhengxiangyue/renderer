# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/Cancel/Develop/c++/graphic

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/Cancel/Develop/c++/graphic/cmake-build-debug

# Include any dependencies generated for this target.
include lib/CMakeFiles/m.dir/depend.make

# Include the progress variables for this target.
include lib/CMakeFiles/m.dir/progress.make

# Include the compile flags for this target's objects.
include lib/CMakeFiles/m.dir/flags.make

lib/CMakeFiles/m.dir/myGraphics.cpp.o: lib/CMakeFiles/m.dir/flags.make
lib/CMakeFiles/m.dir/myGraphics.cpp.o: ../lib/myGraphics.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Cancel/Develop/c++/graphic/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/CMakeFiles/m.dir/myGraphics.cpp.o"
	cd /Users/Cancel/Develop/c++/graphic/cmake-build-debug/lib && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/m.dir/myGraphics.cpp.o -c /Users/Cancel/Develop/c++/graphic/lib/myGraphics.cpp

lib/CMakeFiles/m.dir/myGraphics.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/m.dir/myGraphics.cpp.i"
	cd /Users/Cancel/Develop/c++/graphic/cmake-build-debug/lib && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/Cancel/Develop/c++/graphic/lib/myGraphics.cpp > CMakeFiles/m.dir/myGraphics.cpp.i

lib/CMakeFiles/m.dir/myGraphics.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/m.dir/myGraphics.cpp.s"
	cd /Users/Cancel/Develop/c++/graphic/cmake-build-debug/lib && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/Cancel/Develop/c++/graphic/lib/myGraphics.cpp -o CMakeFiles/m.dir/myGraphics.cpp.s

lib/CMakeFiles/m.dir/myGraphics.cpp.o.requires:

.PHONY : lib/CMakeFiles/m.dir/myGraphics.cpp.o.requires

lib/CMakeFiles/m.dir/myGraphics.cpp.o.provides: lib/CMakeFiles/m.dir/myGraphics.cpp.o.requires
	$(MAKE) -f lib/CMakeFiles/m.dir/build.make lib/CMakeFiles/m.dir/myGraphics.cpp.o.provides.build
.PHONY : lib/CMakeFiles/m.dir/myGraphics.cpp.o.provides

lib/CMakeFiles/m.dir/myGraphics.cpp.o.provides.build: lib/CMakeFiles/m.dir/myGraphics.cpp.o


# Object files for target m
m_OBJECTS = \
"CMakeFiles/m.dir/myGraphics.cpp.o"

# External object files for target m
m_EXTERNAL_OBJECTS =

lib/libm.a: lib/CMakeFiles/m.dir/myGraphics.cpp.o
lib/libm.a: lib/CMakeFiles/m.dir/build.make
lib/libm.a: lib/CMakeFiles/m.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/Cancel/Develop/c++/graphic/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libm.a"
	cd /Users/Cancel/Develop/c++/graphic/cmake-build-debug/lib && $(CMAKE_COMMAND) -P CMakeFiles/m.dir/cmake_clean_target.cmake
	cd /Users/Cancel/Develop/c++/graphic/cmake-build-debug/lib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/m.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/CMakeFiles/m.dir/build: lib/libm.a

.PHONY : lib/CMakeFiles/m.dir/build

lib/CMakeFiles/m.dir/requires: lib/CMakeFiles/m.dir/myGraphics.cpp.o.requires

.PHONY : lib/CMakeFiles/m.dir/requires

lib/CMakeFiles/m.dir/clean:
	cd /Users/Cancel/Develop/c++/graphic/cmake-build-debug/lib && $(CMAKE_COMMAND) -P CMakeFiles/m.dir/cmake_clean.cmake
.PHONY : lib/CMakeFiles/m.dir/clean

lib/CMakeFiles/m.dir/depend:
	cd /Users/Cancel/Develop/c++/graphic/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/Cancel/Develop/c++/graphic /Users/Cancel/Develop/c++/graphic/lib /Users/Cancel/Develop/c++/graphic/cmake-build-debug /Users/Cancel/Develop/c++/graphic/cmake-build-debug/lib /Users/Cancel/Develop/c++/graphic/cmake-build-debug/lib/CMakeFiles/m.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/CMakeFiles/m.dir/depend

