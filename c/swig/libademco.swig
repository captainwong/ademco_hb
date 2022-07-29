 %module libademco

%include "constraints.i"
%include "typemaps.i"
%include "cpointer.i"
%include "stdint.i"
%include "arrays_java.i";
%include "enums.swg" /*NB: requires JDK-1.5+
                       See: http://www.swig.org/Doc1.3/Java.html#enumerations */


 %{
 /* Includes the header in the wrapper code */
 #include "../ademco.h"
 %}

// from arrays_java.i (signed char)
JAVA_ARRAYS_DECL(uint8_t, jbyte, Byte, Uint8)
JAVA_ARRAYS_IMPL(uint8_t, jbyte, Byte, Uint8)
JAVA_ARRAYS_TYPEMAPS(uint8_t, byte, jbyte, Uint8, "[B")
%apply uint8_t[] { uint8_t* }


%pointer_functions(int, intp);
%pointer_functions(uint16_t, uint16p);
%pointer_functions(uint32_t, uint32p);
 
 /* Parse the header file to generate wrappers */
 %include "../ademco.h"
 
