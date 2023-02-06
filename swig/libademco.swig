%module libademco

%include "constraints.i"
%include "typemaps.i"
%include "cpointer.i"
%include "stdint.i"
#ifdef SWIGJAVA
%include "arrays_java.i";
%include "enums.swg" /*NB: requires JDK-1.5+
                       See: http://www.swig.org/Doc1.3/Java.html#enumerations */
#elif defined SWIGCSHARP
%include "arrays_csharp.i";

#endif

%{
/* Includes the header in the wrapper code */
#include "../hb_com.h"
%}

#ifdef SWIGJAVA
// from arrays_java.i (signed char)
JAVA_ARRAYS_DECL(uint8_t, jbyte, Byte, Uint8)
JAVA_ARRAYS_IMPL(uint8_t, jbyte, Byte, Uint8)
JAVA_ARRAYS_TYPEMAPS(uint8_t, byte, jbyte, Uint8, "[B")
%apply uint8_t[] { uint8_t* }

#elif defined SWIGCSHARP
%apply uint8_t INPUT[] {const uint8_t*}
%apply uint8_t OUTPUT[] {uint8_t*}
#endif

%pointer_functions(int, intp);
%pointer_functions(uint16_t, uint16p);
%pointer_functions(uint32_t, uint32p);
%pointer_functions(size_t, size_tp);

/* Parse the header file to generate wrappers */
%include "../ademco.h"
%include "../hb_com.h"

