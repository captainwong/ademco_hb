xcopy /i /s /y /exclude:exclude.txt ..\examples\Win32\Release .\Win32
del .\Win32\ademco_java.dll
del .\Win32\gen_event_md.exe
xcopy /i /s /y /exclude:exclude.txt ..\examples\x64\Release .\x64
del .\x64\gen_event_md.exe

set the_7z="C:\Program Files\7-Zip\7z.exe"
%the_7z% a Win32.7z Win32
%the_7z% a x64.7z x64

PAUSE
