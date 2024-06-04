mkdir com\hb3344\ademco
"E:\local_program\swig\swigwin-4.1.1\swig.exe" -java -package com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_java_wrap.c libademco.swig
cl -c ../ademco.c ../hb_core.c ../hb_core_ademco.c ../hb_rf.c libademco_java_wrap.c -I"../" -I"%JAVA_HOME%include" -I"%JAVA_HOME%include/win32" /source-charset:utf-8 
cl -LD ademco.obj hb_core.obj hb_core_ademco.obj hb_rf.obj libademco_java_wrap.obj -o ademco_java.dll
javac -encoding utf-8 JavaDemo.java
rem java JavaDemo
