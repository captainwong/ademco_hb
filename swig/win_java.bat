mkdir com\hb3344\ademco
"C:\Users\Jack\Downloads\swigwin-4.0.2\swig.exe" -java -package com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_java_wrap.c libademco.swig
cl -c ../ademco.c ../hb_com.c libademco_java_wrap.c -I"../" -I"%JAVA_HOME%include" -I"%JAVA_HOME%include/win32"
cl -LD ademco.obj hb_com.obj libademco_java_wrap.obj -o ademco_java.dll
javac -encoding utf-8 JavaDemo.java
rem java JavaDemo
