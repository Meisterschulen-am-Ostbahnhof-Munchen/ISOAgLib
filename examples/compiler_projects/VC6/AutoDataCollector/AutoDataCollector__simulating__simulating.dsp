# Microsoft Developer Studio Project File - Name="AutoDataCollector" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=AutoDataCollector - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE
!MESSAGE NMAKE /f "AutoDataCollector.mak".
!MESSAGE
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "AutoDataCollector.mak" CFG="AutoDataCollector - Win32 Debug"
!MESSAGE
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "AutoDataCollector - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "AutoDataCollector - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AutoDataCollector - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2  /I "..\..\..\..\examples" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /D "SYSTEM_PC_VC" /D "PRJ_USE_AUTOGEN_CONFIG=config_AutoDataCollector.h" /D USE_CAN_EEPROM_EDITOR /YX /FD /TP /c
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "SYSTEM_PC_VC" /d "PRJ_USE_AUTOGEN_CONFIG=config_AutoDataCollector.h" /D USE_CAN_EEPROM_EDITOR
# ADD RSC /l 0x407 /d "NDEBUG" /d "SYSTEM_PC_VC" /d "PRJ_USE_AUTOGEN_CONFIG=config_AutoDataCollector.h" /D USE_CAN_EEPROM_EDITOR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:console /machine:I386  /libpath:"C:\STLport\lib"

!ELSEIF  "$(CFG)" == "AutoDataCollector - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od  /I "..\..\..\..\examples" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /D "SYSTEM_PC_VC" /D "PRJ_USE_AUTOGEN_CONFIG=config_AutoDataCollector.h" /D USE_CAN_EEPROM_EDITOR /YX /FD /TP /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "SYSTEM_PC_VC" /d "PRJ_USE_AUTOGEN_CONFIG=config_AutoDataCollector.h" /D USE_CAN_EEPROM_EDITOR
# ADD RSC /l 0x407 /d "_DEBUG" /d "SYSTEM_PC_VC" /d "PRJ_USE_AUTOGEN_CONFIG=config_AutoDataCollector.h" /D USE_CAN_EEPROM_EDITOR
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept  /libpath:"C:\STLport\lib"

!ENDIF

# Begin Target

# Name "AutoDataCollector - Win32 Release"
# Name "AutoDataCollector - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cc;cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\filterbox_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\cancustomer_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\msgobj_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\ident_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\canio_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\canpkg_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\system\impl\system_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\eeprom\impl\eepromio_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\generic_utils\can\canfifo_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\generic_utils\can\write_central_fifo.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\can\target_extension_can_simulating.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\can\hal_can_interface.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\system\system_target_extensions.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\eeprom\pc_eeditor_can.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\eeprom\pc_eeditor_rs232.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\eeprom\eeprom_target_extensions.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\impl\util_funcs.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\iliberr_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part3_DataLink\impl\canpkgext_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdSetpointElements\impl\setpointbase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdSetpointElements\impl\setpointregister_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\procident_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\processelementbase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\processpkg_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\process_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\processcmd_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\procdatabase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\StdSetpointElements\impl\setpointremote_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\impl\procdataremotebase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\Std\impl\procdataremote_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\StdMeasureElements\impl\measureprogremote_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\processdatachangehandler_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdMeasureElements\impl\measureprogbase_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdMeasureElements\impl\measuresubprog_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Scheduler\impl\scheduler_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Scheduler\impl\schedulertask_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Scheduler\impl\schedulerentry_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\istate_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isoname_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isoitem_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isomonitor_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isofiltermanager_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isosystempkg_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isofilterbox_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\baseitem_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\identitem_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isorequestpgn_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\supplementary_driver\driver\rs232\impl\rs232io_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\supplementary_driver\hal\pc\rs232\target_extension_rs232_simulating.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\autodatacollector.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\getyflexmanager_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\specificrecordconfig_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\defaultrecordconfig_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\procdatamanager_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\gpsmanager_c.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tractor\TractorConsole\TractorConsole.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tractor\TractorInternalCan\TractorInternalCan.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_1_Sensor\4_1_Sensor.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_2_Pwm\4_2_Pwm.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_0_Eeprom\4_0_Eeprom.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_4_WriteFlexibleIso\1_4_WriteFlexibleIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_5_ReadGps\1_5_ReadGps.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_0_ReadIso\1_0_ReadIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_2_WriteIso\1_2_WriteIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\6_ParallelMutex\6_0_VtThreads\6_0_VtThreads.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\5_CanFeatures\5_0_InternalCan\5_0_InternalCan.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\5_CanFeatures\5_1_ProprietaryPgn\5_1_ProprietaryPgn.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_3_VirtualTerminalPhase3\3_3_VirtualTerminalPhase3.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\3_2_VirtualTerminalMultiIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_0_VirtualTerminalIso\3_0_VirtualTerminalIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\0_NetworkManagement\0_3_LookupIso\0_3_LookupIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\0_NetworkManagement\0_0_AddressClaimIso\0_0_AddressClaimIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_3_LocalReadDataSimpleMeasureSetpointIso\2_3_LocalReadDataSimpleMeasureSetpointIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_9_RemoteWriteSetpointStdIso\2_9_RemoteWriteSetpointStdIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_7_RemoteReadDataStdIso\2_7_RemoteReadDataStdIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_2_LocalReadDataSimpleSetpointIso\2_2_LocalReadDataSimpleSetpointIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_10_RemoteWriteSetpointSimpleSetpointIso\2_10_RemoteWriteSetpointSimpleSetpointIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_1_LocalReadDataSimpleMeasureIso\2_1_LocalReadDataSimpleMeasureIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_0_LocalReadDataStdIso\2_0_LocalReadDataStdIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_4_LocalWriteSetpointStdIso\2_4_LocalWriteSetpointStdIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_5_LocalWriteSetpointSimpleMeasureIso\2_5_LocalWriteSetpointSimpleMeasureIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_6_LocalWriteSetpointSimpleSetpointIso\2_6_LocalWriteSetpointSimpleSetpointIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_8_RemoteReadDataSimpleMeasureIso\2_8_RemoteReadDataSimpleMeasureIso.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\GeneralTest\PcCompilation\PcCompilation.cpp
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\CanServerMessenger\CanServerMessenger.cpp
# End Source File

# End Group

# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\icanpkg_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\iident_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\icanio_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\msgobj_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\canio_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\ident_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\cancustomer_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\canpkg_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\impl\filterbox_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\can\icancustomer_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\system\isystem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\system\impl\system_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\eeprom\impl\eepromio_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\driver\eeprom\ieepromio_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\typedef.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\isoaglib_config.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\config.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\generic_utils\can\write_central_fifo.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\generic_utils\can\canfifo_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\generic_utils\can\icanfifo.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\config.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\can\hal_can_interface.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\can\can_typedef.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\can\can_target_extensions.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\can\can.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\can\can_server.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\system\system_target_extensions.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\system\system.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\typedef.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\eeprom\eeprom_target_extensions.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\eeprom\pc_eeditor_rs232.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\eeprom\eeprom.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\eeprom\pc_eeditor_can.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\pc\errcodes.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\system.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\can.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\hal\eeprom.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\config.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\iutil_funcs.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\impl\singleton.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\impl\singleton_container_decl.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\impl\singleton_container.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\impl\util_funcs.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\compiler_adaptation.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\convert.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\util\iliberr_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part3_DataLink\impl\canpkgext_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\proc_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\processdatachangehandler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdSetpointElements\isetpointregister_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdSetpointElements\impl\setpointregister_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdSetpointElements\impl\setpointbase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\iprocess_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\elementddi_s.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\proc_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\procdatabase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\procident_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\processwsmtaskmsghandler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\process_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\processpkg_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\processcmd_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\impl\processelementbase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Local\impl\procdatalocalbase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\StdSetpointElements\impl\setpointremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\StdSetpointElements\isetpointremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\impl\procdataremotebase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\Std\impl\procdataremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\Std\iprocdataremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\StdMeasureElements\impl\measureprogremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\Remote\StdMeasureElements\imeasureprogremote_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdMeasureElements\impl\measuresubprog_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part7_ProcessData\StdMeasureElements\impl\measureprogbase_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Scheduler\ischeduler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Scheduler\impl\schedulerentry_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Scheduler\impl\schedulertask_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Scheduler\impl\scheduler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part10_TaskController_Client\idevpropertyhandler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\iisofilter_s.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isomonitor_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\identitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isofilterbox_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\saclaimhandler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isoitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isorequestpgnhandler_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isosystempkg_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\istate_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isorequestpgn_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\baseitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isoname_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\impl\isofiltermanager_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\iisoname_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\iisoitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\iidentitem_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\istate_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\iisomonitor_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\IsoAgLib\comm\Part5_NetworkManagement\iisofiltermanager_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\supplementary_driver\driver\rs232\irs232io_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\supplementary_driver\driver\rs232\impl\rs232io_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\supplementary_driver\hal\rs232.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\supplementary_driver\hal\pc\rs232\rs232.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\library\xgpl_src\supplementary_driver\hal\pc\rs232\rs232_target_extensions.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\config_AutoDataCollector.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\getyflexmanager_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\procdatamanager_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\defaultrecordconfig_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\gpsmanager_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\specificrecordconfig_c.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\AutoDataCollector\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tractor\TractorConsole\config_tractorconsole.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tractor\TractorConsole\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tractor\config_tractor.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tractor\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tractor\TractorInternalCan\config_tractorinternalcan.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tractor\TractorInternalCan\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_1_Sensor\config_4_1_Sensor.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_1_Sensor\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_2_Pwm\config_4_2_Pwm.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_2_Pwm\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_0_Eeprom\config_4_0_Eeprom.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\4_SupplementaryDriver\4_0_Eeprom\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_4_WriteFlexibleIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_4_WriteFlexibleIso\config_1_4_WriteFlexibleIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_5_ReadGps\config_1_5_ReadGps.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_5_ReadGps\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_0_ReadIso\config_1_0_ReadIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_0_ReadIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_2_WriteIso\config_1_2_WriteIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\1_ApplicationLayer\1_2_WriteIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\6_ParallelMutex\6_0_VtThreads\MaskDefinition\simpleVTIsoPool_derived-cpp.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\6_ParallelMutex\6_0_VtThreads\MaskDefinition\simpleVTIsoPool_derived-h.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\6_ParallelMutex\6_0_VtThreads\MaskDefinition\simpleVTIsoPool_direct.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\6_ParallelMutex\6_0_VtThreads\config_6_0_VtThreads.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\6_ParallelMutex\6_0_VtThreads\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\6_ParallelMutex\6_0_VtThreads\config_6_0_VtThreadsEsxu.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\5_CanFeatures\5_0_InternalCan\config_5_0_InternalCan.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\5_CanFeatures\5_0_InternalCan\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\5_CanFeatures\5_1_ProprietaryPgn\config_5_1_ProprietaryPgn.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\5_CanFeatures\5_1_ProprietaryPgn\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\5_CanFeatures\5_1_ProprietaryPgn\5_1_ProprietaryPgn.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_3_VirtualTerminalPhase3\MaskDefinition\simpleVTIsoPool_derived-cpp.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_3_VirtualTerminalPhase3\MaskDefinition\simpleVTIsoPool_derived-h.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_3_VirtualTerminalPhase3\MaskDefinition\simpleVTIsoPool_direct.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_3_VirtualTerminalPhase3\config_3_3_VirtualTerminalPhase3.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_3_VirtualTerminalPhase3\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\MaskDefinition2\simpleVTIsoPool2_derived-cpp.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\MaskDefinition2\simpleVTIsoPool2_direct.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\MaskDefinition2\simpleVTIsoPool2_derived-h.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\config_3_2_VirtualTerminalMultiIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\MaskDefinition1\simpleVTIsoPool_derived-cpp.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\MaskDefinition1\simpleVTIsoPool_derived-h.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_2_VirtualTerminalMultiIso\MaskDefinition1\simpleVTIsoPool_direct.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_0_VirtualTerminalIso\MaskDefinition\simpleVTIsoPool_derived-cpp.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_0_VirtualTerminalIso\MaskDefinition\simpleVTIsoPool_derived-h.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_0_VirtualTerminalIso\MaskDefinition\simpleVTIsoPool_direct.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_0_VirtualTerminalIso\config_3_0_VirtualTerminalIsoEsxu.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_0_VirtualTerminalIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\3_VirtualTerminal_Client\3_0_VirtualTerminalIso\config_3_0_VirtualTerminalIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\0_NetworkManagement\0_3_LookupIso\config_0_3_LookupIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\0_NetworkManagement\0_3_LookupIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\0_NetworkManagement\0_0_AddressClaimIso\config_0_0_AddressClaimIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\0_NetworkManagement\0_0_AddressClaimIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_3_LocalReadDataSimpleMeasureSetpointIso\devicedescription\DeviceDescription.xml-func.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_3_LocalReadDataSimpleMeasureSetpointIso\config_2_3_LocalReadDataSimpleMeasureSetpointIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_3_LocalReadDataSimpleMeasureSetpointIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_9_RemoteWriteSetpointStdIso\config_2_9_RemoteWriteSetpointStdIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_9_RemoteWriteSetpointStdIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_7_RemoteReadDataStdIso\config_2_7_RemoteReadDataStdIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_7_RemoteReadDataStdIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_2_LocalReadDataSimpleSetpointIso\config_2_2_LocalReadDataSimpleSetpointIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_2_LocalReadDataSimpleSetpointIso\devicedescription\DeviceDescription.xml-func.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_2_LocalReadDataSimpleSetpointIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_10_RemoteWriteSetpointSimpleSetpointIso\config_2_10_RemoteWriteSetpointSimpleSetpointIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_10_RemoteWriteSetpointSimpleSetpointIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_1_LocalReadDataSimpleMeasureIso\devicedescription\DeviceDescription.xml-func.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_1_LocalReadDataSimpleMeasureIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_1_LocalReadDataSimpleMeasureIso\config_2_1_LocalReadDataSimpleMeasureIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_0_LocalReadDataStdIso\config_2_0_LocalReadDataStdIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_0_LocalReadDataStdIso\devicedescription\DeviceDescription.xml-func.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_0_LocalReadDataStdIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_4_LocalWriteSetpointStdIso\config_2_4_LocalWriteSetpointStdIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_4_LocalWriteSetpointStdIso\devicedescription\DeviceDescription.xml-func.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_4_LocalWriteSetpointStdIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_5_LocalWriteSetpointSimpleMeasureIso\devicedescription\DeviceDescription.xml-func.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_5_LocalWriteSetpointSimpleMeasureIso\config_2_5_LocalWriteSetpointSimpleMeasureIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_5_LocalWriteSetpointSimpleMeasureIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_6_LocalWriteSetpointSimpleSetpointIso\config_2_6_LocalWriteSetpointSimpleSetpointIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_6_LocalWriteSetpointSimpleSetpointIso\devicedescription\DeviceDescription.xml-func.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_6_LocalWriteSetpointSimpleSetpointIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_8_RemoteReadDataSimpleMeasureIso\config_2_8_RemoteReadDataSimpleMeasureIso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Tutorials\2_ProcessData\2_8_RemoteReadDataSimpleMeasureIso\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\GeneralTest\PcCompilation\config_PcCompilation.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\GeneralTest\PcCompilation\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\GeneralTest\PcCompilation\config_PcCompilationSysNotEnHal.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Imi\eeprom_adr.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Imi\config_imi_iso.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\Imi\version.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\CanServerMessenger\config_CanServerMessenger.h
# End Source File

# Begin Source File
SOURCE=..\..\..\..\examples\src\CanServerMessenger\version.h
# End Source File

# End Group
# End Target
# End Project
