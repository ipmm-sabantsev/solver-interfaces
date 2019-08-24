@set QTDIR=C:\Qt\Qt5.9.2\5.9.2\msvc2015
@set PATH=%QTDIR%\bin;%PATH%
@set QMAKESPEC=%QTDIR%\mkspecs\win32-msvc
call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" x86
qmake Interfaces.pro -o ..\projects\Interfaces.vcxproj