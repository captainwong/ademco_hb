@echo off
rem usage: build_win.bat java|csharp|node
rem 通过 swig 生成绑定并用 VS 编译，实际构建逻辑在 ..\swig\<同名>.bat
if "%1"=="java" goto java
if "%1"=="csharp" goto csharp
if "%1"=="node" goto node
echo usage: %0 java^|csharp^|node
exit /b 1

:java
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
set JAVA_HOME=C:\Program Files\Java\latest\jdk-26\
cd ..\swig
call win_java.bat
goto end

:csharp
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
cd ..\swig
call win_csharp.bat
goto end

:node
cd ..\swig
call win_node.bat
goto end

:end
