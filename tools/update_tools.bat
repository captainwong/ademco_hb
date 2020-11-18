xcopy /i /s /y /exclude:exclude.txt ..\examples\Win32\Release .\Win32
del .\Win32\ademco_java.dll
del .\Win32\gen_event_md.exe
xcopy /i /s /y /exclude:exclude.txt ..\examples\x64\Release .\x64
del .\x64\gen_event_md.exe
PAUSE
