echo on

SET project_dir="%cd%"

echo Set up environment...
set PATH=%QT%\bin\;C:\Qt\Tools\QtCreator\bin\;C:\Qt\QtIFW2.0.1\bin\;%PATH%
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %PLATFORM%

echo Building YourApp...
qmake CONFIG+=x86_64 CONFIG-=debug CONFIG+=release
nmake

echo Packaging...
cd %project_dir%\release\
windeployqt --qmldir ..\ui\ re-play.exe
xcopy "%project_dir%\avlibs\bin\*.dll" "%project_dir%\release\" /sy

echo Packaging portable archive...
7z a %project_dir%\Re-play.windows.x64.portable.zip %project_dir%\release\
