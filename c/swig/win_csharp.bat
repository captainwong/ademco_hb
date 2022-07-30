mkdir com\hb3344\ademco
"C:\Users\Jack\Downloads\swigwin-3.0.12\swig.exe" -DDEBUG=1 -csharp -namespace com.hb3344.ademco -outdir com/hb3344/ademco -o libademco_csharp_wrap.c libademco.swig
cl -c -DDEBUG=1 ../ademco.c libademco_csharp_wrap.c -I"../"
cl -LD ademco.obj libademco_csharp_wrap.obj -o libademco.dll
csc CSharpDemo.cs -recurse:com/hb3344/ademco/*.cs
CSharpDemo.exe
