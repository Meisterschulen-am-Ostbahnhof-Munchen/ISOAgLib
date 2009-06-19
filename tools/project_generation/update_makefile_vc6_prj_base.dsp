# Microsoft Developer Studio Project File - Name="${INSERT_PROJECT}" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=${INSERT_PROJECT} - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE
!MESSAGE NMAKE /f "${INSERT_PROJECT}.mak".
!MESSAGE
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "${INSERT_PROJECT}.mak" CFG="${INSERT_PROJECT} - Win32 Debug"
!MESSAGE
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "${INSERT_PROJECT} - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "${INSERT_PROJECT} - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "\$(CFG)" == "${INSERT_PROJECT} - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC ${INSERT_RELEASE_USE_MFC}
# PROP Use_Debug_Libraries ${INSERT_RELEASE_USE_DEBUG_LIBRARIES}
# PROP Output_Dir "${INSERT_RELEASE_OUTPUT_DIR}"
# PROP Intermediate_Dir "${INSERT_RELEASE_INTERMEDIATE_DIR}"
$([ 0 -eq "${INSERT_RELEASE_IGNORE_EXPORT_LIB}" ] || printf '# PROP Ignore_Export_Lib %s' "${INSERT_RELEASE_IGNORE_EXPORT_LIB}")
# PROP Target_Dir "${INSERT_RELEASE_TARGET_DIR}"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP ${INSERT_RELEASE_CPP_PARAMETERS}
# ADD BASE RSC /l 0x407 /d "NDEBUG" ${INSERT_d_DEFINES}
# ADD RSC ${INSERT_RELEASE_RSC_PARAMETERS}
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ${INSERT_RELEASE_LINKER_PARAMETERS}

!ELSEIF  "\$(CFG)" == "${INSERT_PROJECT} - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC ${INSERT_DEBUG_USE_MFC}
# PROP Use_Debug_Libraries ${INSERT_DEBUG_USE_DEBUG_LIBRARIES}
# PROP Output_Dir "${INSERT_DEBUG_OUTPUT_DIR}"
# PROP Intermediate_Dir "${INSERT_DEBUG_INTERMEDIATE_DIR}"
# PROP Ignore_Export_Lib ${INSERT_DEBUG_IGNORE_EXPORT_LIB}
# PROP Target_Dir "${INSERT_DEBUG_TARGET_DIR}"
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP ${INSERT_DEBUG_CPP_PARAMETERS}
# ADD BASE RSC /l 0x407 /d "_DEBUG" ${INSERT_d_DEFINES}
# ADD RSC ${INSERT_DEBUG_RSC_PARAMETERS}
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ${INSERT_DEBUG_LINKER_PARAMETERS}

!ENDIF

# Begin Target

# Name "${INSERT_PROJECT} - Win32 Release"
# Name "${INSERT_PROJECT} - Win32 Debug"
