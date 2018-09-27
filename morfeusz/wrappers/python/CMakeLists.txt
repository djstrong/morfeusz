
# SWIG
#set(CMAKE_SWIG_OUTDIR swig)
#~ FIND_PACKAGE (SWIG REQUIRED)
FIND_PACKAGE (PythonLibs)
#~ INCLUDE (${SWIG_USE_FILE})

set (PYMORFEUSZ_VERSION "0.4.0")

# SWIG Java
INCLUDE_DIRECTORIES (${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES (../..)


set (SWIG_PYTHON_OUTFILE_CXX "${CMAKE_CURRENT_BINARY_DIR}/swigPYTHON.cpp")
set (SWIG_PYTHON_OUTFILE_PY "${CMAKE_CURRENT_BINARY_DIR}/morfeusz2.py")
add_custom_command ( 
    OUTPUT "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}"
    COMMAND swig -python -c++ -o "${SWIG_PYTHON_OUTFILE_CXX}" "${CMAKE_SOURCE_DIR}/morfeusz/wrappers/morfeusz.i"
    DEPENDS libmorfeusz
)
add_custom_target (generate_python_wrapper ALL
        DEPENDS "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}")

set (SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set (SETUP_PY_TMP    "${CMAKE_CURRENT_BINARY_DIR}/setupTmp.py")
set (SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set (DEPS        "${SWIG_PYTHON_OUTFILE_CXX}" "${SWIG_PYTHON_OUTFILE_PY}")
set (OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build")

configure_file (${SETUP_PY_IN} ${SETUP_PY})

add_custom_target (generate_python_setup_file
        DEPENDS ${SETUP_PY})

add_custom_command (OUTPUT ${OUTPUT}
                       COMMAND python
                       ARGS ${SETUP_PY} build
                       DEPENDS generate_python_wrapper generate_python_setup_file libmorfeusz)

add_custom_target (build_python_wrapper
        DEPENDS ${OUTPUT})

add_custom_target (pymorfeusz
        DEPENDS generate_python_wrapper generate_python_setup_file libmorfeusz build_python_wrapper)

if (UNIX)
    add_custom_target (install-python
        COMMAND python ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX}
        DEPENDS pymorfeusz
    )
    add_custom_target (package-python-bin
        COMMAND python ${SETUP_PY} bdist_egg -d "${TARGET_DIR}" --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS pymorfeusz
    )
    set (PACKAGE_DEPENDS "package-python-bin")
    
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        add_custom_target (package-python-deb-sdist
            COMMAND python ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --depends "${CPACK_DEBIAN_PACKAGE_NAME}" --copyright-file "${PROJECT_SOURCE_DIR}/License.txt"
            DEPENDS pymorfeusz
        )
        add_custom_target (package-python-deb-build
            COMMAND debuild -us -uc
            WORKING_DIRECTORY deb_dist/morfeusz2-${PYMORFEUSZ_VERSION}
            DEPENDS package-python-deb-sdist
        )
        add_custom_target(package-python-deb
            COMMAND mkdir -p "${TARGET_DIR}" && cp ${CMAKE_CURRENT_BINARY_DIR}/deb_dist/python-morfeusz2*.deb "${TARGET_DIR}"
            DEPENDS package-python-deb-build
        )
        list (APPEND PACKAGE_DEPENDS package-python-deb)
    endif ()
    add_custom_target (package-python
        DEPENDS pymorfeusz ${PACKAGE_DEPENDS})
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT CMAKE_CROSSCOMPILING)
    add_custom_target (package-python-win-installer
        COMMAND python ${SETUP_PY} bdist_wininst -d ${TARGET_DIR} --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        DEPENDS pymorfeusz
    )
    list (APPEND PACKAGE_DEPENDS package-python-win-installer)
    add_custom_target (package-python
        DEPENDS pymorfeusz ${PACKAGE_DEPENDS})
endif ()
