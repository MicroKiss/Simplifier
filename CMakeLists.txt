cmake_minimum_required(VERSION 3.10)

# Project name
project(Simplifier)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the compiler
#set(CMAKE_CXX_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe")
#set(CMAKE_C_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe")

set(CMAKE_CXX_COMPILER "C:/TDM-GCC-64/bin/g++.exe")
set(CMAKE_C_COMPILER "C:/TDM-GCC-64/bin/gcc.exe")


# Source files
set(SOURCES
    Src/Edge.cpp
    Src/Extras.cpp
    Src/Geometry.cpp
    Src/main.cpp
    Src/SimplifierApp.cpp
    Src/Simplify.cpp
    Src/STL.cpp
)

# Add executable
add_executable(Simplifier ${SOURCES})
target_include_directories(Simplifier PRIVATE ${CMAKE_SOURCE_DIR}/Src)
# Set compile options based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(Simplifier PRIVATE 
        -g        # Enable debugging information
        -Wall     # Enable all warnings
        -O0       # Disable optimizations
    )
    target_compile_definitions(Simplifier PRIVATE DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(Simplifier PRIVATE 
        -O2       # Optimize for speed
        -march=native # Enable optimizations for the host architecture
        -flto     # Enable link-time optimization
        -ffast-math # Enable fast math optimizations
        -Wall     # Enable all warnings
    )
endif()