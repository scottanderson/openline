# CMake toolchain file for cross-compiling HLClient to Windows (32-bit) using the
# MinGW-w64 cross-compiler (i686-w64-mingw32-*). Install with:
#   apt-get install g++-mingw-w64-i686
#
# Usage:
#   cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-i686.cmake
#   cmake --build build

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(TOOLCHAIN_PREFIX i686-w64-mingw32)

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER  ${TOOLCHAIN_PREFIX}-windres)
set(CMAKE_AR           ${TOOLCHAIN_PREFIX}-ar)
set(CMAKE_RANLIB       ${TOOLCHAIN_PREFIX}-ranlib)

# Only look in the target environment for libraries/headers, but allow finding
# programs (cmake, make, etc.) on the host.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
