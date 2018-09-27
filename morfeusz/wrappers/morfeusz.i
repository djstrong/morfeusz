
// XXX
// set module name with preprocessor
// because Mac OS X doesn't seem to recognize jniclassname option
#ifdef SWIGJAVA
%module(allprotected="1") MorfeuszWrapper
#else
%module(allprotected="1") morfeusz2
#endif

//%feature("autodoc", "2");
%{
#include "morfeusz2.h"
#include "MorfeuszImpl.hpp"
#include <vector>
#include <list>
%}

#ifdef SWIGJAVA
%include "morfeusz_java.i"
#endif

#ifdef SWIGPYTHON
%include "morfeusz_python.i"
#endif
        
#ifdef SWIGPERL
%include "morfeusz_perl.i"
#endif

%include "morfeusz_common.i"

%include "../morfeusz2.h"
