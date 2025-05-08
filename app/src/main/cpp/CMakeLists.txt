# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)
project("hacker")
add_library(hacker SHARED hacker.cpp hook.cpp)
find_package(bytehook REQUIRED CONFIG)
target_link_libraries(hacker bytehook::bytehook log)