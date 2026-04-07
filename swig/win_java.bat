mkdir com\hb3344\ademco
"E:\local_program\swig\swigwin-4.1.1\swig.exe" -java -package com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_java_wrap.c libademco.swig
cl -c ../src/libademco/ademco.c ../src/libhb/hb_core.c ../src/libhb/hb_core_ademco.c ../src/libhbiot/rf/hb_rf.c -I"../src" /source-charset:utf-8 
ping 127.0.0.1 -n 1 > nul
cl -c libademco_java_wrap.c -I"../src" -I"%JAVA_HOME%include" -I"%JAVA_HOME%include/win32" /source-charset:utf-8 
ping 127.0.0.1 -n 1 > nul
cl -LD ademco.obj hb_core.obj hb_core_ademco.obj hb_rf.obj libademco_java_wrap.obj -o ademco_java.dll
ping 127.0.0.1 -n 1 > nul
javac -encoding utf-8 JavaDemo.java
rem java JavaDemo
