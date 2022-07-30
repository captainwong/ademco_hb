mkdir com\hb3344\ademco
"C:\Users\Jack\Downloads\swigwin-3.0.12\swig.exe" -java -package com.hb3344.ademco -outdir com/hb3344/ademco -o ademco_java_wrap.c libademco.swig
cl -c ../ademco.c ademco_java_wrap.c -I"../" -I"%JAVA_HOME%include" -I"%JAVA_HOME%include/win32"
cl -LD ademco.obj ademco_java_wrap.obj -o ademco.dll
