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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubuntu/900_opencv/frisbeelocating

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/900_opencv/frisbeelocating

# Include any dependencies generated for this target.
include CMakeFiles/frisbeelocating.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/frisbeelocating.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/frisbeelocating.dir/flags.make

CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o: CMakeFiles/frisbeelocating.dir/flags.make
CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o: frisbeelocating.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ubuntu/900_opencv/frisbeelocating/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o -c /home/ubuntu/900_opencv/frisbeelocating/frisbeelocating.cpp

CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ubuntu/900_opencv/frisbeelocating/frisbeelocating.cpp > CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.i

CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ubuntu/900_opencv/frisbeelocating/frisbeelocating.cpp -o CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.s

CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o.requires:
.PHONY : CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o.requires

CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o.provides: CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o.requires
	$(MAKE) -f CMakeFiles/frisbeelocating.dir/build.make CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o.provides.build
.PHONY : CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o.provides

CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o.provides.build: CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o

# Object files for target frisbeelocating
frisbeelocating_OBJECTS = \
"CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o"

# External object files for target frisbeelocating
frisbeelocating_EXTERNAL_OBJECTS =

frisbeelocating: CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o
frisbeelocating: CMakeFiles/frisbeelocating.dir/build.make
frisbeelocating: /usr/lib/libopencv_vstab.so.2.4.8
frisbeelocating: /usr/lib/libopencv_tegra.so.2.4.8
frisbeelocating: /usr/lib/libopencv_imuvstab.so.2.4.8
frisbeelocating: /usr/lib/libopencv_facedetect.so.2.4.8
frisbeelocating: /usr/lib/libopencv_videostab.so.2.4.8
frisbeelocating: /usr/lib/libopencv_video.so.2.4.8
frisbeelocating: /usr/lib/libopencv_ts.a
frisbeelocating: /usr/lib/libopencv_superres.so.2.4.8
frisbeelocating: /usr/lib/libopencv_stitching.so.2.4.8
frisbeelocating: /usr/lib/libopencv_softcascade.so.2.4.8
frisbeelocating: /usr/lib/libopencv_photo.so.2.4.8
frisbeelocating: /usr/lib/libopencv_objdetect.so.2.4.8
frisbeelocating: /usr/lib/libopencv_ml.so.2.4.8
frisbeelocating: /usr/lib/libopencv_legacy.so.2.4.8
frisbeelocating: /usr/lib/libopencv_imgproc.so.2.4.8
frisbeelocating: /usr/lib/libopencv_highgui.so.2.4.8
frisbeelocating: /usr/lib/libopencv_gpu.so.2.4.8
frisbeelocating: /usr/lib/libopencv_flann.so.2.4.8
frisbeelocating: /usr/lib/libopencv_features2d.so.2.4.8
frisbeelocating: /usr/lib/libopencv_core.so.2.4.8
frisbeelocating: /usr/lib/libopencv_contrib.so.2.4.8
frisbeelocating: /usr/lib/libopencv_calib3d.so.2.4.8
frisbeelocating: /usr/local/cuda-6.0/lib/libcudart.so
frisbeelocating: /usr/local/cuda-6.0/lib/libnppc.so
frisbeelocating: /usr/local/cuda-6.0/lib/libnppi.so
frisbeelocating: /usr/local/cuda-6.0/lib/libnpps.so
frisbeelocating: /usr/local/cuda-6.0/lib/libcufft.so
frisbeelocating: /usr/lib/libopencv_photo.so.2.4.8
frisbeelocating: /usr/lib/libopencv_legacy.so.2.4.8
frisbeelocating: /usr/lib/libopencv_video.so.2.4.8
frisbeelocating: /usr/lib/libopencv_objdetect.so.2.4.8
frisbeelocating: /usr/lib/libopencv_ml.so.2.4.8
frisbeelocating: /usr/lib/libopencv_calib3d.so.2.4.8
frisbeelocating: /usr/lib/libopencv_features2d.so.2.4.8
frisbeelocating: /usr/lib/libopencv_highgui.so.2.4.8
frisbeelocating: /usr/lib/libopencv_imgproc.so.2.4.8
frisbeelocating: /usr/lib/libopencv_flann.so.2.4.8
frisbeelocating: /usr/lib/libopencv_core.so.2.4.8
frisbeelocating: CMakeFiles/frisbeelocating.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable frisbeelocating"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/frisbeelocating.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/frisbeelocating.dir/build: frisbeelocating
.PHONY : CMakeFiles/frisbeelocating.dir/build

CMakeFiles/frisbeelocating.dir/requires: CMakeFiles/frisbeelocating.dir/frisbeelocating.cpp.o.requires
.PHONY : CMakeFiles/frisbeelocating.dir/requires

CMakeFiles/frisbeelocating.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/frisbeelocating.dir/cmake_clean.cmake
.PHONY : CMakeFiles/frisbeelocating.dir/clean

CMakeFiles/frisbeelocating.dir/depend:
	cd /home/ubuntu/900_opencv/frisbeelocating && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/900_opencv/frisbeelocating /home/ubuntu/900_opencv/frisbeelocating /home/ubuntu/900_opencv/frisbeelocating /home/ubuntu/900_opencv/frisbeelocating /home/ubuntu/900_opencv/frisbeelocating/CMakeFiles/frisbeelocating.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/frisbeelocating.dir/depend
