@echo off
rem usage: build_examples.bat x86|x64
if "%1"=="x64" goto x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
devenv /Build "Release|x86" ..\examples\ademco_hb_examples.sln
goto end
:x64
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
devenv /Build "Release|x64" ..\examples\ademco_hb_examples.sln
:end
