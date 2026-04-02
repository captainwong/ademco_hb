mkdir com\hb3344\ademco
"E:\local_program\swig\swigwin-4.1.1\swig.exe" -csharp -namespace com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_csharp_wrap.c libademco.swig
cl -c ../src/libademco/ademco.c ../src/libhb/hb_core.c ../src/libhb/hb_core_ademco.c ../src/libhbiot/rf/hb_rf.c libademco_csharp_wrap.c -I"../src" /source-charset:utf-8 
cl -LD ademco.obj hb_core.obj hb_core_ademco.obj hb_rf.obj libademco_csharp_wrap.obj -o libademco.dll
csc CSharpDemo.cs -recurse:com/hb3344/ademco/*.cs
rem CSharpDemo.exe
