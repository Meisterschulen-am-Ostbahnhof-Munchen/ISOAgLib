@echo off
rem ---
rem runs proc2iso with the proc2iso.xsd file set as default XML schema
rem ---

rem --- retrieve path to proc2iso.exe and proc2iso.xsd
SET proc2isoBasePath=%0
SET proc2isoBasePath=%proc2isoBasePath:~0,-4%
SET p2iExe=%proc2isoBasePath%.exe
SET p2iXsd=%proc2isoBasePath%.xsd

rem --- run proc2iso
%p2iExe% -s=%p2iXsd% %1 %2 %3 %4
