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
CMAKE_COMMAND = /afs/cern.ch/sw/lcg/contrib/CMake/2.8.12.2/Linux-i386/bin/cmake

# The command to remove a file.
RM = /afs/cern.ch/sw/lcg/contrib/CMake/2.8.12.2/Linux-i386/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /afs/cern.ch/sw/lcg/contrib/CMake/2.8.12.2/Linux-i386/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt

# Utility rule file for ExamplesComponentsList.

# Include the progress variables for this target.
include Examples/CMakeFiles/ExamplesComponentsList.dir/progress.make

Examples/CMakeFiles/ExamplesComponentsList: Examples/Examples.components

Examples/Examples.components: lib/libExamples.so
Examples/Examples.components: /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/bin/listcomponents.exe
	$(CMAKE_COMMAND) -E cmake_progress_report /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating Examples.components"
	cd /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Examples && /afs/cern.ch/sw/lcg/releases/LCG_68/Python/2.7.6/x86_64-slc6-gcc48-opt/bin/python /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/GAUDI/GAUDI_v25r2/cmake/env.py --xml /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/config/FCCSWBuildEnvironment.xml /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/bin/listcomponents.exe --output Examples.components libExamples.so

ExamplesComponentsList: Examples/CMakeFiles/ExamplesComponentsList
ExamplesComponentsList: Examples/Examples.components
ExamplesComponentsList: Examples/CMakeFiles/ExamplesComponentsList.dir/build.make
.PHONY : ExamplesComponentsList

# Rule to build all files generated by this target.
Examples/CMakeFiles/ExamplesComponentsList.dir/build: ExamplesComponentsList
.PHONY : Examples/CMakeFiles/ExamplesComponentsList.dir/build

Examples/CMakeFiles/ExamplesComponentsList.dir/clean:
	cd /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Examples && $(CMAKE_COMMAND) -P CMakeFiles/ExamplesComponentsList.dir/cmake_clean.cmake
.PHONY : Examples/CMakeFiles/ExamplesComponentsList.dir/clean

Examples/CMakeFiles/ExamplesComponentsList.dir/depend:
	cd /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/Examples /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Examples /afs/cern.ch/work/d/degrutto/private/FCC260315/FCC/myFCCSW/build.x86_64-slc6-gcc48-opt/Examples/CMakeFiles/ExamplesComponentsList.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Examples/CMakeFiles/ExamplesComponentsList.dir/depend

