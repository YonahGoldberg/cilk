# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.29.0/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.29.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/yonah/Repos/cilk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/yonah/Repos/cilk/build

# Include any dependencies generated for this target.
include CMakeFiles/cilk.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/cilk.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/cilk.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cilk.dir/flags.make

CMakeFiles/cilk.dir/src/benchmark.cpp.o: CMakeFiles/cilk.dir/flags.make
CMakeFiles/cilk.dir/src/benchmark.cpp.o: /Users/yonah/Repos/cilk/src/benchmark.cpp
CMakeFiles/cilk.dir/src/benchmark.cpp.o: CMakeFiles/cilk.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/yonah/Repos/cilk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cilk.dir/src/benchmark.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/cilk.dir/src/benchmark.cpp.o -MF CMakeFiles/cilk.dir/src/benchmark.cpp.o.d -o CMakeFiles/cilk.dir/src/benchmark.cpp.o -c /Users/yonah/Repos/cilk/src/benchmark.cpp

CMakeFiles/cilk.dir/src/benchmark.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/cilk.dir/src/benchmark.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yonah/Repos/cilk/src/benchmark.cpp > CMakeFiles/cilk.dir/src/benchmark.cpp.i

CMakeFiles/cilk.dir/src/benchmark.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/cilk.dir/src/benchmark.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yonah/Repos/cilk/src/benchmark.cpp -o CMakeFiles/cilk.dir/src/benchmark.cpp.s

CMakeFiles/cilk.dir/src/tests/fib.cpp.o: CMakeFiles/cilk.dir/flags.make
CMakeFiles/cilk.dir/src/tests/fib.cpp.o: /Users/yonah/Repos/cilk/src/tests/fib.cpp
CMakeFiles/cilk.dir/src/tests/fib.cpp.o: CMakeFiles/cilk.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/yonah/Repos/cilk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/cilk.dir/src/tests/fib.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/cilk.dir/src/tests/fib.cpp.o -MF CMakeFiles/cilk.dir/src/tests/fib.cpp.o.d -o CMakeFiles/cilk.dir/src/tests/fib.cpp.o -c /Users/yonah/Repos/cilk/src/tests/fib.cpp

CMakeFiles/cilk.dir/src/tests/fib.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/cilk.dir/src/tests/fib.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yonah/Repos/cilk/src/tests/fib.cpp > CMakeFiles/cilk.dir/src/tests/fib.cpp.i

CMakeFiles/cilk.dir/src/tests/fib.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/cilk.dir/src/tests/fib.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yonah/Repos/cilk/src/tests/fib.cpp -o CMakeFiles/cilk.dir/src/tests/fib.cpp.s

CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o: CMakeFiles/cilk.dir/flags.make
CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o: /Users/yonah/Repos/cilk/src/tests/quicksort.cpp
CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o: CMakeFiles/cilk.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/yonah/Repos/cilk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o -MF CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o.d -o CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o -c /Users/yonah/Repos/cilk/src/tests/quicksort.cpp

CMakeFiles/cilk.dir/src/tests/quicksort.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/cilk.dir/src/tests/quicksort.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yonah/Repos/cilk/src/tests/quicksort.cpp > CMakeFiles/cilk.dir/src/tests/quicksort.cpp.i

CMakeFiles/cilk.dir/src/tests/quicksort.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/cilk.dir/src/tests/quicksort.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yonah/Repos/cilk/src/tests/quicksort.cpp -o CMakeFiles/cilk.dir/src/tests/quicksort.cpp.s

CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o: CMakeFiles/cilk.dir/flags.make
CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o: /Users/yonah/Repos/cilk/src/tests/rectmul.cpp
CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o: CMakeFiles/cilk.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/yonah/Repos/cilk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o -MF CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o.d -o CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o -c /Users/yonah/Repos/cilk/src/tests/rectmul.cpp

CMakeFiles/cilk.dir/src/tests/rectmul.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/cilk.dir/src/tests/rectmul.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yonah/Repos/cilk/src/tests/rectmul.cpp > CMakeFiles/cilk.dir/src/tests/rectmul.cpp.i

CMakeFiles/cilk.dir/src/tests/rectmul.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/cilk.dir/src/tests/rectmul.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yonah/Repos/cilk/src/tests/rectmul.cpp -o CMakeFiles/cilk.dir/src/tests/rectmul.cpp.s

CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o: CMakeFiles/cilk.dir/flags.make
CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o: /Users/yonah/Repos/cilk/src/tests/nqueens.cpp
CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o: CMakeFiles/cilk.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/yonah/Repos/cilk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o -MF CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o.d -o CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o -c /Users/yonah/Repos/cilk/src/tests/nqueens.cpp

CMakeFiles/cilk.dir/src/tests/nqueens.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/cilk.dir/src/tests/nqueens.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yonah/Repos/cilk/src/tests/nqueens.cpp > CMakeFiles/cilk.dir/src/tests/nqueens.cpp.i

CMakeFiles/cilk.dir/src/tests/nqueens.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/cilk.dir/src/tests/nqueens.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yonah/Repos/cilk/src/tests/nqueens.cpp -o CMakeFiles/cilk.dir/src/tests/nqueens.cpp.s

CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o: CMakeFiles/cilk.dir/flags.make
CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o: /Users/yonah/Repos/cilk/src/scheduler_instance.cpp
CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o: CMakeFiles/cilk.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/yonah/Repos/cilk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o -MF CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o.d -o CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o -c /Users/yonah/Repos/cilk/src/scheduler_instance.cpp

CMakeFiles/cilk.dir/src/scheduler_instance.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/cilk.dir/src/scheduler_instance.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/yonah/Repos/cilk/src/scheduler_instance.cpp > CMakeFiles/cilk.dir/src/scheduler_instance.cpp.i

CMakeFiles/cilk.dir/src/scheduler_instance.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/cilk.dir/src/scheduler_instance.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/yonah/Repos/cilk/src/scheduler_instance.cpp -o CMakeFiles/cilk.dir/src/scheduler_instance.cpp.s

# Object files for target cilk
cilk_OBJECTS = \
"CMakeFiles/cilk.dir/src/benchmark.cpp.o" \
"CMakeFiles/cilk.dir/src/tests/fib.cpp.o" \
"CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o" \
"CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o" \
"CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o" \
"CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o"

# External object files for target cilk
cilk_EXTERNAL_OBJECTS =

cilk: CMakeFiles/cilk.dir/src/benchmark.cpp.o
cilk: CMakeFiles/cilk.dir/src/tests/fib.cpp.o
cilk: CMakeFiles/cilk.dir/src/tests/quicksort.cpp.o
cilk: CMakeFiles/cilk.dir/src/tests/rectmul.cpp.o
cilk: CMakeFiles/cilk.dir/src/tests/nqueens.cpp.o
cilk: CMakeFiles/cilk.dir/src/scheduler_instance.cpp.o
cilk: CMakeFiles/cilk.dir/build.make
cilk: /usr/local/lib/libbenchmark.a
cilk: CMakeFiles/cilk.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/yonah/Repos/cilk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable cilk"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cilk.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cilk.dir/build: cilk
.PHONY : CMakeFiles/cilk.dir/build

CMakeFiles/cilk.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cilk.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cilk.dir/clean

CMakeFiles/cilk.dir/depend:
	cd /Users/yonah/Repos/cilk/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/yonah/Repos/cilk /Users/yonah/Repos/cilk /Users/yonah/Repos/cilk/build /Users/yonah/Repos/cilk/build /Users/yonah/Repos/cilk/build/CMakeFiles/cilk.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/cilk.dir/depend

