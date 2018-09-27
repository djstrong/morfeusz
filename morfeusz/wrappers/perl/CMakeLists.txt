
if (NOT CMAKE_CROSSCOMPILING)
    find_package (SWIG REQUIRED)
    include (${SWIG_USE_FILE})
    set (MORFEUSZ_PERL_VERSION "0.1.0")

    find_package (PerlLibs REQUIRED)
    include_directories (${PERL_INCLUDE_PATH})
    include_directories (../..)

    set (CMAKE_SWIG_FLAGS "")

    set (SWIG_FILE ../morfeusz.i)
    set_source_files_properties (${SWIG_FILE} PROPERTIES CPLUSPLUS ON)
    #set_source_files_properties (${SWIG_FILE} PROPERTIES SWIG_FLAGS "-nodefaultctor")
    swig_add_module (morfeusz-perl perl ${SWIG_FILE})
    swig_link_libraries (morfeusz-perl ${PERL_LIBRARIES})
    
    if ( CMAKE_COMPILER_IS_GNUCC )
        set_property( TARGET morfeusz-perl APPEND_STRING PROPERTY COMPILE_FLAGS -w )
    endif ( CMAKE_COMPILER_IS_GNUCC )

    add_custom_target(package-perl
        COMMAND mkdir -p "${TARGET_DIR}" && ${CMAKE_COMMAND} -E tar "cfvz" "${TARGET_DIR}/morfeusz-perl-${MORFEUSZ_PERL_VERSION}-${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}.tar.gz" "${CMAKE_CURRENT_BINARY_DIR}/*.pm" "${CMAKE_CURRENT_BINARY_DIR}/*.so"
        DEPENDS morfeusz-perl libmorfeusz)
endif ()
