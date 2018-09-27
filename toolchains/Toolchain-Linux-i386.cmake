
set (CMAKE_SYSTEM_NAME Linux)
set (CMAKE_SYSTEM_VERSION 1)
set (CMAKE_C_COMPILER   gcc)
set (CMAKE_CXX_COMPILER g++)
set (CMAKE_C_FLAGS "-m32")
set (CMAKE_CXX_FLAGS "-m32")

set (CMAKE_SYSTEM_PROCESSOR "x86")

set (JAVA_ROOT ${CROSSMORFEUSZ_ROOT}/linux32/jdk1.7.0_45)
set (PYTHON_ROOT ${CROSSMORFEUSZ_ROOT}/linux32/python)

# here is the target environment located
set (CMAKE_FIND_ROOT_PATH  ${JAVA_ROOT} ${PYTHON_ROOT})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

set (JAVA_JVM_LIBRARY ${JAVA_ROOT}/bin)
set (JAVA_AWT_LIBRARY ${JAVA_ROOT}/bin)
set (JAVA_INCLUDE_PATH ${JAVA_ROOT}/include)
set (JAVA_INCLUDE_PATH2 ${JAVA_ROOT}/include/linux)
