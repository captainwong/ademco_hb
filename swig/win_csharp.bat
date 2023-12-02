mkdir com\hb3344\ademco
"E:\local_program\swig\swigwin-4.1.1\swig.exe" -csharp -namespace com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_csharp_wrap.c libademco.swig
cl -c ../ademco.c ../hb_com.c libademco_csharp_wrap.c -I"../"
cl -LD ademco.obj hb_com.obj libademco_csharp_wrap.obj -o libademco.dll
csc CSharpDemo.cs -recurse:com/hb3344/ademco/*.cs
rem CSharpDemo.exe
