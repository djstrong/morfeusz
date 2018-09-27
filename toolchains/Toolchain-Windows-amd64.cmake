
set (CMAKE_SYSTEM_NAME Windows)
set (CMAKE_SYSTEM_VERSION 1)
set (CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set (CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set (CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set (CMAKE_SYSTEM_PROCESSOR "x86_64")

# here is the target environment located
set (WIN64_ROOT ${CROSSMORFEUSZ_ROOT}/windows64)
set (PYTHON_ROOT ${WIN64_ROOT}/Python27)
set (JAVA_ROOT ${WIN64_ROOT}/Java/jdk1.7.0_45)
set (CMAKE_FIND_ROOT_PATH  /usr/x86_64-w64-mingw32 ${JAVA_ROOT} ${PYTHON_ROOT})
set (CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PYTHON_ROOT}/libs)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic -I${JAVA_ROOT}/include/win32")

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
