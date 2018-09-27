
set (MORFEUSZBUILDER "morfeusz-builder")
set (MORFEUSZBUILDER_VERSION "0.1.0")
set (DIST_PATH "${CMAKE_CURRENT_BINARY_DIR}")

set (SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set (SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set (DEPS        "")

configure_file (${SETUP_PY_IN} ${SETUP_PY})

add_custom_command (OUTPUT ${SETUP_PY}
                       COMMAND python
                       ARGS setup.py build
                       DEPENDS ${DEPS})

add_custom_target (builder-setup DEPENDS ${SETUP_PY})

add_custom_target (install-builder
    COMMAND python ${SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX}
    DEPENDS builder-setup
)
#~ add_custom_target (package-builder-tgz
    #~ COMMAND python ${SETUP_PY} bdist -d "${TARGET_DIR}" --plat-name='all'
    #~ DEPENDS builder-setup
#~ )
add_custom_target (package-builder-egg
    COMMAND python ${SETUP_PY} bdist_egg -d "${TARGET_DIR}"
    DEPENDS builder-setup
)
add_custom_target (package-builder-sdist
    COMMAND python ${SETUP_PY} sdist -d "${TARGET_DIR}"
    DEPENDS builder-setup
)

set (PACKAGE_DEPENDS "package-builder-egg" "package-builder-sdist")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_custom_target (package-builder-deb-sdist
        COMMAND python ${SETUP_PY} --command-packages=stdeb.command sdist_dsc --package "${MORFEUSZBUILDER}" --depends "${CPACK_DEBIAN_PACKAGE_NAME}" --depends python-pyparsing
        DEPENDS builder-setup
    )
    add_custom_target (package-builder-deb-build
        COMMAND debuild -us -uc
        WORKING_DIRECTORY deb_dist/${MORFEUSZBUILDER}-${MORFEUSZBUILDER_VERSION}
        DEPENDS package-builder-deb-sdist
    )
    add_custom_target(package-builder-deb
        COMMAND mkdir -p "${TARGET_DIR}" && cp ${CMAKE_CURRENT_BINARY_DIR}/deb_dist/${MORFEUSZBUILDER}*.deb "${TARGET_DIR}"
        DEPENDS package-builder-deb-build
    )
    list (APPEND PACKAGE_DEPENDS package-builder-deb)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    #~ add_custom_target (package-python-win-installer
        #~ COMMAND python ${SETUP_PY} bdist_wininst -d ${TARGET_DIR} --plat-name "${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}"
        #~ DEPENDS builder-setup
    #~ )
    #~ list (APPEND PACKAGE_DEPENDS package-python-win-installer)
    
    #~ add_custom_target (buildfsa-exec ALL
        #~ COMMAND pyinstaller --noconfirm --onefile --console --strip --distpath="${DIST_PATH}" --clean fsa/morfeusz_builder
    #~ )
#~ 
#~ add_executable (morfeusz_builder IMPORTED)
#~ add_dependencies (morfeusz_builder buildfsa-exec)
#~ set_property (TARGET morfeusz_builder PROPERTY IMPORTED_LOCATION "${DIST_PATH}")
#~ install (PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/morfeusz_builder" DESTINATION bin)
endif ()

add_custom_target(package-builder DEPENDS ${PACKAGE_DEPENDS})
