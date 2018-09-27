

########## generate default dictionary data #################

if (NOT EMBEDDED_DEFAULT_DICT)
    set (ANALYZER_DICTIONARY_FILE "${DEFAULT_DICT_DIR}/${DEFAULT_DICT_NAME}-a.dict")
    set (GENERATOR_DICTIONARY_FILE "${DEFAULT_DICT_DIR}/${DEFAULT_DICT_NAME}-s.dict")
endif ()

if (SKIP_DICTIONARY_BUILDING)
    message ("SKIPPING dictionary building")
else ()
    if (EMBEDDED_DEFAULT_DICT)
        if (DEFAULT_DICT_DIR)
            set (ADDITIONAL_CMD --dict-dir="${DEFAULT_DICT_DIR}" --dict="${DEFAULT_DICT_NAME}")
        endif ()
        add_custom_command (
            OUTPUT "${ANALYZER_DICTIONARY_CPP}" "${GENERATOR_DICTIONARY_CPP}"
            COMMAND python ${PROJECT_SOURCE_DIR}/fsabuilder/morfeusz_builder --input-files="${INPUT_DICTIONARIES}" --dict="${DEFAULT_DICT_NAME}" --analyzer-cpp="${ANALYZER_DICTIONARY_CPP}" --generator-cpp="${GENERATOR_DICTIONARY_CPP}" --tagset-file="${INPUT_TAGSET}" --segments-file="${SEGMENT_RULES_FILE}" ${ADDITIONAL_CMD}
            DEPENDS "${INPUT_DICTIONARY}"
            COMMENT "Building default dictionary C++ files"
        )
    else ()
        add_custom_command (
            OUTPUT "${ANALYZER_DICTIONARY_FILE}" "${GENERATOR_DICTIONARY_FILE}" 
            COMMAND python ${PROJECT_SOURCE_DIR}/fsabuilder/morfeusz_builder --input-files="${INPUT_DICTIONARIES}" --dict="${DEFAULT_DICT_NAME}" --dict-dir="${DEFAULT_DICT_DIR}"  --tagset-file="${INPUT_TAGSET}" --segments-file="${SEGMENT_RULES_FILE}"
            DEPENDS "${INPUT_DICTIONARY}"
            COMMENT "Building default dictionary files"
        )
    endif ()
endif()

if (EMBEDDED_DEFAULT_DICT)
    add_custom_target ( analyzer-dictionary DEPENDS "${ANALYZER_DICTIONARY_CPP}")
    add_custom_target ( generator-dictionary DEPENDS "${GENERATOR_DICTIONARY_CPP}")
else ()
    add_custom_target ( analyzer-dictionary DEPENDS "${ANALYZER_DICTIONARY_FILE}")
    add_custom_target ( generator-dictionary DEPENDS "${GENERATOR_DICTIONARY_FILE}")
endif ()
add_custom_target ( dictionary DEPENDS analyzer-dictionary generator-dictionary)
add_custom_target ( dictionary-deb
    COMMAND "${PROJECT_SOURCE_DIR}/createDictionaryDeb.sh" "${DEFAULT_DICT_DIR}" "${DEFAULT_DICT_NAME}" "${DICT_VERSION}" "${TARGET_DIR}"
        DEPENDS dictionary)
add_custom_target ( bin-deb
    COMMAND "${PROJECT_SOURCE_DIR}/createBinDeb.sh" "${CMAKE_CURRENT_BINARY_DIR}" "${Morfeusz_LIB_VERSION}" "${ARCHITECTURE}" "${TARGET_DIR}"
        DEPENDS morfeusz_analyzer morfeusz_generator)
add_custom_target ( lib-deb
    COMMAND "${PROJECT_SOURCE_DIR}/createLibraryDeb.sh" "${CMAKE_CURRENT_BINARY_DIR}" "${Morfeusz_LIB_VERSION}" "${ARCHITECTURE}" ${DEFAULT_DICT_NAME} "${TARGET_DIR}"
        DEPENDS libmorfeusz)
add_custom_target ( dev-deb
    COMMAND "${PROJECT_SOURCE_DIR}/createDevDeb.sh" "${PROJECT_SOURCE_DIR}/morfeusz" "${Morfeusz_LIB_VERSION}" "${TARGET_DIR}"
    DEPENDS "${PROJECT_SOURCE_DIR}/morfeusz/morfeusz2.h" "${PROJECT_SOURCE_DIR}/morfeusz/morfeusz2_c.h")


include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

#### build #####

set(SRC_FILES 
    const.cpp
    Dictionary.cpp
    DictionariesRepository.cpp
    Environment.cpp
    IdResolverImpl.cpp
    fsa/const.cpp
    MorphInterpretation.cpp 
    Morfeusz.cpp
    MorfeuszImpl.cpp
    ResultsIteratorImpl.cpp
    InflexionGraph.cpp
    charset/TextReader.cpp
    charset/CharsetConverter.cpp
    case/CaseConverter.cpp
    case/caseconv.cpp
    charset/conversion_tables.cpp
    cli/cli.cpp
    segrules/segrules.cpp
    segrules/SegrulesFSA.cpp
    case/CasePatternHelper.cpp
    deserialization/morphInterps/InterpretedChunksDecoder.cpp
    deserialization/morphInterps/InterpretedChunksDecoder4Analyzer.cpp
    deserialization/morphInterps/InterpretedChunksDecoder4Generator.cpp
    deserialization/InterpsGroupsReader.cpp
    deserialization/MorphDeserializer.cpp
    morfeusz2_c.cpp
    c_api/ResultsManager.cpp
    morfeusz_analyzer_old.cpp
)

if (EMBEDDED_DEFAULT_DICT)
    list (APPEND SRC_FILES "${ANALYZER_DICTIONARY_CPP}" "${GENERATOR_DICTIONARY_CPP}")
    set_source_files_properties ( SOURCE "${ANALYZER_DICTIONARY_CPP}" PROPERTIES GENERATED TRUE)
    set_source_files_properties ( SOURCE "${GENERATOR_DICTIONARY_CPP}" PROPERTIES GENERATED TRUE)
endif ()

file (GLOB TEST_SRC_FILES tests/*.cpp)

add_library (libmorfeusz SHARED ${SRC_FILES})
set_target_properties (libmorfeusz PROPERTIES OUTPUT_NAME "morfeusz2")

add_executable (morfeusz_analyzer morfeusz_analyzer.cpp)
add_executable (morfeusz_analyzer_old morfeusz_analyzer_old.cpp)
add_executable (morfeusz_generator morfeusz_generator.cpp)

target_link_libraries (morfeusz_analyzer libmorfeusz)
target_link_libraries (morfeusz_analyzer_old libmorfeusz)
target_link_libraries (morfeusz_generator libmorfeusz)

add_subdirectory (wrappers)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_custom_target (morfeusz-repair-library
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/gui-libs
        COMMAND ${INSTALL_NAME_TOOL} -change /usr/x86_64-apple-darwin9/x86_64-apple-darwin9/lib/libstdc++.6.dylib /usr/lib/libstdc++.6.dylib ${CMAKE_CURRENT_BINARY_DIR}/libmorfeusz2.dylib
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/libmorfeusz2.dylib ${CMAKE_CURRENT_BINARY_DIR}/gui-libs
        COMMAND ${INSTALL_NAME_TOOL} -id libmorfeusz2.dylib ${CMAKE_CURRENT_BINARY_DIR}/libmorfeusz2.dylib
        COMMAND ${INSTALL_NAME_TOOL} -id @executable_path/libmorfeusz2.dylib ${CMAKE_CURRENT_BINARY_DIR}/gui-libs/libmorfeusz2.dylib
        DEPENDS libmorfeusz)
    add_dependencies (morfeusz_analyzer morfeusz-repair-library)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries (libmorfeusz ws2_32)
    set (TARGET_LIB_DIR bin)
else ()
    set (TARGET_LIB_DIR lib)
endif ()

# INSTALL

#if (NOT EMBEDDED_DEFAULT_DICT)
#    add_dependencies (libmorfeusz dictionary)
#    install (FILES "${ANALYZER_DICTIONARY_FILE}" "${GENERATOR_DICTIONARY_FILE}" DESTINATION "${DEFAULT_DICT_INSTALL_DIR}")
#endif ()

install (FILES morfeusz2.h morfeusz2_c.h DESTINATION include)
install (TARGETS libmorfeusz DESTINATION ${TARGET_LIB_DIR})
install (TARGETS morfeusz_analyzer morfeusz_generator DESTINATION bin)

# TESTING

if (NOT CMAKE_CROSSCOMPILING AND NOT SKIP_TESTING AND NOT ONLY_DICTIONARY)
    find_library(CPPUNIT_LIB cppunit)
    if(${CPPUNIT_LIB} STREQUAL "CPPUNIT_LIB-NOTFOUND")
        message(FATAL_ERROR "Couldn't find cppunit library. Please install cppunit or add -D SKIP_TESTING=1 option.")
    endif()
    add_executable (test_runner test_runner.cpp ${TEST_SRC_FILES})
    target_link_libraries (test_runner ${CPPUNIT_LIB} libmorfeusz)
    add_custom_target (build-tests DEPENDS test_runner)
    add_test(CPPUnitTests test_runner)
endif ()
