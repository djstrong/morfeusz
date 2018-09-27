#!/bin/bash

set -e -o pipefail

if [ "$#" -ne 4 ]; then
    echo "Must provide exactly 4 arguments: <CROSSMORFEUSZ_ROOT> <INPUT_DICTIONARIES> <DEFAULT_DICT_NAME> <DICT_VERSION>"
    exit 1
fi

export CROSSMORFEUSZ_ROOT="$1"
export INPUT_DICTIONARIES="$2"
export DEFAULT_DICT_NAME="$3"
export DICT_VERSION="$4"
export ANALYZER_DICTIONARY_CPP=`mktemp`.cpp
export GENERATOR_DICTIONARY_CPP=`mktemp`.cpp
export DICT_DIR=`mktemp -d`

function buildDictionaries {
    
    INPUT_TAGSET=input/morfeusz-sgjp.tagset
    SEGMENT_RULES_FILE=input/segmenty.dat
    
    python fsabuilder/morfeusz_builder \
        --input-files="$INPUT_DICTIONARIES" \
        --tagset-file="$INPUT_TAGSET" \
        --segments-file="$SEGMENT_RULES_FILE" \
        --analyzer-cpp="$ANALYZER_DICTIONARY_CPP" \
        --generator-cpp="$GENERATOR_DICTIONARY_CPP" \
        --dict="$DEFAULT_DICT_NAME" \
        --dict-dir="$DICT_DIR"
    
    echo "DONE building dictionaries" >&2
}

function build {
    set -e -o pipefail
    os=$1
    arch=$2
    embedded=$3
    shift
    shift
    shift
    targets=$@
    
    srcDir=`pwd`
    buildDir=buildall/$os-$arch-$embedded
    targetDir=$srcDir/target
    toolchain=$srcDir/toolchains/Toolchain-$os-$arch.cmake
    
    echo "Will use $toolchain toolchain"

    rm -rf $buildDir
    #~ rm -rf $targetDir
    mkdir -p $buildDir
    mkdir -p $targetDir
    cd $buildDir
    
    if [ "$embedded" = true ]
    then
        if [ "$os" == "Linux" ]
        then
            CPACK_GENERATOR=TGZ
        fi
        CMAKE_ARGS="-D CROSSMORFEUSZ_ROOT=$CROSSMORFEUSZ_ROOT \
            -D CMAKE_TOOLCHAIN_FILE=$toolchain \
            -D TARGET_DIR=$targetDir \
            -D ANALYZER_DICTIONARY_CPP=$ANALYZER_DICTIONARY_CPP \
            -D GENERATOR_DICTIONARY_CPP=$GENERATOR_DICTIONARY_CPP \
            -D DEFAULT_DICT_NAME=$DEFAULT_DICT_NAME \
            -D SKIP_DICTIONARY_BUILDING=1 \
            -D EMBEDDED_DEFAULT_DICT=1 \
            -D DICT_VERSION=$DICT_VERSION"
        if [ "$CPACK_GENERATOR" != "" ]
        then
            CMAKE_ARGS="$CMAKE_ARGS -D CPACK_GENERATOR=$CPACK_GENERATOR"
        fi
    else
        echo "setting default ACL to prevent control-file-has-bad-permissions lintian error"
        setfacl -R -d -m o::rx -m g::rx -m u::rwx .
        
        CMAKE_ARGS="-D CROSSMORFEUSZ_ROOT=$CROSSMORFEUSZ_ROOT \
            -D CMAKE_TOOLCHAIN_FILE=$toolchain \
            -D TARGET_DIR=$targetDir \
            -D DEFAULT_DICT_DIR=$DICT_DIR \
            -D DEFAULT_DICT_NAME=$DEFAULT_DICT_NAME \
            -D SKIP_DICTIONARY_BUILDING=1 \
            -D CPACK_GENERATOR=DEB \
            -D DICT_VERSION=$DICT_VERSION"
    fi
    cmake $CMAKE_ARGS $srcDir 2>&1
    echo "building $toolchain" >&2
    make
    make $targets

    for f in `find "$targetDir" -name "*-Linux-*.deb"`
    do 
        mv "$f" "`echo $f | sed -r 's/Linux-amd64.deb$/amd64.deb/' | sed -r 's/Linux-i386.deb/i386.deb/'`"
    done

    cd "$srcDir"
}

function log {
    os=$1
    arch=$2
    stdbuf -oL sed -e $"s/^/$os-$arch:\t/" | tee "log/$os-$arch.out"
}

export -f build
export -f log

rm -rf log target
mkdir -p log

buildDictionaries 2>&1 | log All all

{
    #echo "build Linux amd64 true package package-java package-python package-builder 2>&1 | log Linux-tgz amd64"
    #echo "build Linux amd64 false package 2>&1 | log Linux-deb amd64"
    #echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 true package package-java 2>&1 | log Linux-tgz i386"
    #echo "LDFLAGS=-m32;CFLAGS=-m32;CXXFLAGS=-m32 build Linux i386 false package 2>&1 | log Linux-deb i386"
    #echo "build Windows amd64 true package package-java 2>&1 | log Windows amd64"
    #echo "build Windows i386 true package package-java 2>&1 | log Windows i386"
    echo "build Darwin amd64 true package package-java 2>&1 | log Darwin amd64"
} | xargs -n1 -P2 -d$'\n' bash -c


