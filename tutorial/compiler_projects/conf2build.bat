REM FILE: conf2build.bat

@Echo Off

if not exist "\cygwin\bin\sh.exe" goto error

\cygwin\bin\sh.exe  conf2build.sh   %1
goto end

:error
Echo 
Echo ERROR:
Echo     File NOT found \cygwin\bin\sh.exe
Echo 
Echo     Cygwin is required to run this MS-DOS Batch file.
Echo     Please download and install Cygwin from:
Echo         http://www.cygwin.com
goto end

:end
