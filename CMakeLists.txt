cmake_minimum_required(VERSION 3.16)

# This project only targets Windows (via MinGW-w64 cross-compilation from Linux).
# The original code is 2003-era, Windows-9x/NT-era Win32 API usage throughout, so
# there is no meaningful non-Windows build target.
project(HLClient LANGUAGES CXX C)
enable_language(RC)

# The original Metrowerks project used ".cp" for C++ sources; CMake doesn't know that
# extension by default, so all *.cp files get silently treated as "no language" and
# fail to link. Teach CMake to treat them as C++.
list(APPEND CMAKE_CXX_SOURCE_FILE_EXTENSIONS cp)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	message(WARNING "HLClient targets Windows via MinGW-w64 cross-compilation. "
		"Configure with -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-i686.cmake")
endif()

option(HL_NO_QUICKTIME "Build without Apple QuickTime support (SDK not present in this repo for licensing reasons)" ON)

# gnu++98 (matching the code's original C++98-ish style) has one problem: pre-C++11,
# unnamed (anonymous) struct/class types can't be used as template arguments, and this
# codebase does that (e.g. HotlineNewsReadHistory.cp's PurgeOldest() calls swap() on an
# array of an anonymous local struct). C++11 relaxed that rule, so use gnu++11 instead
# -- it's a superset of what this code needs, not a rewrite of it.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS ON)

# Old Metrowerks-era code: lots of narrowing conversions, sign mismatches, unused
# params, etc. Don't try to make this warning-clean -- just get it building.
add_compile_options(-w -fpermissive -fno-strict-aliasing)

add_subdirectory(AppWarrior)
add_subdirectory(Apps/Client)
add_subdirectory(Apps/Server)
add_subdirectory("Apps/Tracker/Old Tracker")
add_subdirectory("Apps/Tracker/New Tracker")
