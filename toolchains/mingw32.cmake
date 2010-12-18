
# Tested: Arch Linux (mingw32-gcc 4.5).

set (CMAKE_SYSTEM_NAME Windows)

set (CMAKE_C_COMPILER   i486-mingw32-gcc)
set (CMAKE_CXX_COMPILER i486-mingw32-g++)

#add_definitions (-static-libgcc -static-libstdc++)

set (ENV{PKG_CONFIG_PATH} /opt/mingw32/lib/pkgconfig)

set (CMAKE_FIND_ROOT_PATH /opt/mingw32)

set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


