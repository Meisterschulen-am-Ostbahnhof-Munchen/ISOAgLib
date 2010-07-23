#############################################################################
# Makefile for building: ecuDisplay
# Project:               ecuDisplay
#############################################################################

####### Project specific variables
TARGET = ecuDisplay
ISOAGLIB_PATH = ../../../..
INSTALL_PATH = /usr/local
APP_INC = -I../../../../tutorial/src/ecu_display -I../../../../tutorial/src/common -I../../../../tutorial/src/common
LIBPATH = 
EXTERNAL_LIBS = 

####### Include a version definition file into the Makefile context - when this file exists
-include versions.mk


PROJ_DEFINES = $(VERSION_DEFINES) -DPRJ_USE_AUTOGEN_CONFIG=config_ecuDisplay.h -D'NDEBUG' -DSYSTEM_WITH_ENHANCED_CAN_HAL -DCAN_DRIVER_MESSAGE_QUEUE

####### Definition of compiler binary prefix corresponding to selected target
COMPILER_BINARY_PRE = 


first: all

####### Files
SOURCES_LIBRARY = ../../../../library/xgpl_src/supplementary_driver/driver/datastreams/volatilememory_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/util/iliberr_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/util/impl/util_funcs.cpp \
		../../../../library/xgpl_src/IsoAgLib/util/iassert.cpp \
		../../../../library/xgpl_src/IsoAgLib/scheduler/ischedulertask_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/scheduler/impl/scheduler_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/scheduler/impl/schedulertask_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/scheduler/impl/schedulerentry_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/streamchunk_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/multisendpkg_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/chunk_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/stream_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/multisend_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/istate_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isofilterbox_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isosystempkg_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/baseitem_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/isoterminalpkg_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectnumbervariable_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectellipse_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectmeter_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectauxiliaryfunction_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectlinearbargraph_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectrectangle_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectlineattributes_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectstring_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectworkingset_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobject_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectarchedbargraph_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputattributes_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectdatamask_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtserverinstance_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectkey_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectoutputlist_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputnumber_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectobjectpointer_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectalarmmask_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectoutputnumber_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectfillattributes_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectcontainer_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectstringvariable_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectauxiliaryinput_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectfontattributes_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/isoterminal_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputlist_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectsoftkeymask_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectmacro_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectbutton_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectpicturegraphic_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectline_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectoutputstring_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputstring_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtclientservercommunication_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputboolean_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectpolygon_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/iisoterminalobjectpool_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobject_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/tracgeneral_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/tracpto_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/basecommon_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/timeposgps_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/comm/Part12_DiagnosticsServices/impl/diagnosticpgnhandler_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/driver/eeprom/impl/eepromio_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/driver/system/impl/system_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/canio_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/canpkg_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/cancustomer_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/filterbox_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/ident_c.cpp \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/eeprom/eeprom_target_extensions.cpp \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/eeprom/pc_eeditor_can.cpp \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/eeprom/pc_eeditor_rs232.cpp \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/system/system_target_extensions.cpp \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/can/target_extension_can_client_msq.cpp \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/can/msq_helper.cpp \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/can/hal_can_interface.cpp

SOURCES_APP = ../../../../tutorial/src/ecu_display/component_tecu.cpp \
		../../../../tutorial/src/ecu_display/component_vtclient.cpp \
		../../../../tutorial/src/ecu_display/ecu_display.cpp \
		../../../../tutorial/src/ecu_display/component_timepos.cpp \
		../../../../tutorial/src/common/ecu_main.cpp

HEADERS_APP =

INSTALL_FILES_LIBRARY = ../../../../library/xgpl_src/supplementary_driver/driver/datastreams/streaminput_c.h \
		../../../../library/xgpl_src/supplementary_driver/driver/datastreams/volatilememory_c.h \
		../../../../library/xgpl_src/IsoAgLib/util/iliberr_c.h \
		../../../../library/xgpl_src/IsoAgLib/util/compiler_adaptation.h \
		../../../../library/xgpl_src/IsoAgLib/util/config.h \
		../../../../library/xgpl_src/IsoAgLib/util/compiler_adaptation_unicode.h \
		../../../../library/xgpl_src/IsoAgLib/util/iutil_funcs.h \
		../../../../library/xgpl_src/IsoAgLib/util/iassert.h \
		../../../../library/xgpl_src/IsoAgLib/util/convert.h \
		../../../../library/xgpl_src/IsoAgLib/hal/hal_system.h \
		../../../../library/xgpl_src/IsoAgLib/hal/hal_config.h \
		../../../../library/xgpl_src/IsoAgLib/hal/hal_typedef.h \
		../../../../library/xgpl_src/IsoAgLib/hal/hal_can.h \
		../../../../library/xgpl_src/IsoAgLib/hal/hal_eeprom.h \
		../../../../library/xgpl_src/IsoAgLib/scheduler/ischedulertask_c.h \
		../../../../library/xgpl_src/IsoAgLib/scheduler/ischeduler_c.h \
		../../../../library/xgpl_src/IsoAgLib/isoaglib_config.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/icanpkgext_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/imultisendstreamer_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/imultisend_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/istate_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/iisofilter_s.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/iisoitem_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/iisofiltermanager_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobject_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectellipse_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectinputboolean_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectinputnumber_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectpolygon_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectstring_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/iisoterminal_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectfillattributes_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectstringvariable_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectobjectpointer_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectmeter_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectnumbervariable_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectfontattributes_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivttypes.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectinputattributes_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectinputstring_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectoutputstring_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectalarmmask_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtserverinstance_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectmacro_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectarchedbargraph_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectcontainer_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectrectangle_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectline_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/iisoterminalobjectpool_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectlineattributes_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtincludes.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectworkingset_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectauxiliaryinput_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectinputlist_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectpicturegraphic_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectoutputnumber_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectoutputlist_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientservercommunication_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectsoftkeymask_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectmask_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectkey_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectauxiliaryfunction_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectbutton_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectdatamask_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtobjectlinearbargraph_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/itracgeneral_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/itracpto_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/itracmove_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/ibasetypes.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/itimeposgps_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/eeprom/ieepromio_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/system/isystem_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/icanio_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/iident_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/icancustomer_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/icanpkg_c.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/eeprom/eeprom_target_extensions.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/eeprom/pc_eeditor_rs232.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/eeprom/pc_eeditor_can.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/eeprom/eeprom.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/system/system.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/system/system_target_extensions.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/config.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/can/can_server.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/can/hal_can_interface.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/can/can_typedef.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/can/can_target_extensions.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/can/can.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/typedef.h \
		../../../../library/xgpl_src/IsoAgLib/hal/pc/errcodes.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgn_c.h \
		../../../../library/xgpl_src/IsoAgLib/util/impl/singleton.h \
		../../../../library/xgpl_src/IsoAgLib/util/impl/util_funcs.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/stream_c.h \
		../../../../library/xgpl_src/IsoAgLib/scheduler/impl/schedulertask_c.h \
		../../../../library/xgpl_src/IsoAgLib/scheduler/impl/scheduler_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isofiltermanager_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/tracgeneral_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/tracmove_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/tracpto_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/timeposgps_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/isoterminal_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/istate_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectellipse_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputboolean_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputnumber_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectpolygon_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectstring_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectfillattributes_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectstringvariable_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectobjectpointer_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectmeter_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectnumbervariable_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectfontattributes_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputattributes_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputstring_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectoutputstring_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectalarmmask_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtserverinstance_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectmacro_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectarchedbargraph_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectcontainer_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectrectangle_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectline_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectlineattributes_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectworkingset_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectauxiliaryinput_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectinputlist_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectpicturegraphic_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectoutputnumber_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectoutputlist_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtclientservercommunication_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectsoftkeymask_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobject_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectkey_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectauxiliaryfunction_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectbutton_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectdatamask_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vtobjectlinearbargraph_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/eeprom/impl/eepromio_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/system/impl/system_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/canio_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/ident_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/cancustomer_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/canpkg_c.h \
		../../../../library/xgpl_src/IsoAgLib/util/impl/singleton_container.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/saclaimhandler_c.h \
		../../../../library/xgpl_src/IsoAgLib/scheduler/impl/schedulerentry_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isosystempkg_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isofilterbox_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl/basecommon_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/streamchunk_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/multisendpkg_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/baseitem_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part12_DiagnosticsServices/impl/diagnosticpgnhandler_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/vttypes.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl/isoterminalpkg_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isofilterbox_c.h \
		../../../../library/xgpl_src/IsoAgLib/driver/can/impl/filterbox_c.h \
		../../../../library/xgpl_src/IsoAgLib/util/impl/singleton_container_decl.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/chunk_c.h \
		../../../../library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl/streamchunk_c.h \
		../../../../library/xgpl_src/IsoAgLib/util/impl/bitfieldwrapper_c.h

####### Some Standard Rules
SUBLIBS  = -lrt
CC       = $(COMPILER_BINARY_PRE)gcc
CXX      = $(COMPILER_BINARY_PRE)g++
STRIP    = $(COMPILER_BINARY_PRE)strip
LEX      = flex
YACC     = yacc

GCC_MAJOR    := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f1)
ifneq ($(GCC_MAJOR),2)
	EXTRA_CFLAGS =  -Wextra -Winit-self -Wmissing-include-dirs
else
endif

CFLAGS   = -pipe -O -Wall -g $(EXTRA_CFLAGS) -fno-builtin -fno-exceptions -Wshadow -Wcast-qual -Wcast-align -Woverloaded-virtual -Wpointer-arith $(PROJ_DEFINES)
CXXFLAGS = -pipe -O -Wall -g $(EXTRA_CFLAGS) -fno-builtin -fno-exceptions -Wshadow -Wcast-qual -Wcast-align -Woverloaded-virtual -Wpointer-arith $(PROJ_DEFINES)

LEXFLAGS =
YACCFLAGS= -d
INCPATH  = -I. -I$(ISOAGLIB_PATH)/library -I$(ISOAGLIB_PATH)/library/xgpl_src $(APP_INC) $(BIOS_INC)
LINK     = $(COMPILER_BINARY_PRE)g++
LFLAGS   = -pthread $(LIBPATH)
LIBS     = $(BIOS_LIB) $(SUBLIBS) $(EXTERNAL_LIBS)
AR       = ar cqs
RANLIB   =
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
INSTALL_FILE= $(COPY_FILE)
INSTALL_DIR = $(COPY_DIR)
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJ_DIR_LIBRARY  = objects_library
OBJ_DIR_APP  = objects_app

####### Build Rules for Library Part

OBJECTS_LIBRARY = $(patsubst %.cpp,$(OBJ_DIR_LIBRARY)/%.o,$(notdir $(SOURCES_LIBRARY)))
DEPS_LIBRARY := $(patsubst %.o,%.d,$(OBJECTS_LIBRARY))

# $(OBJ_DIR_LIBRARY)/%.d :
# 	$(CXX) -c $(CXXFLAGS) $(INCPATH) -MM -MT $(patsubst %.d,%.o,$@)  $(filter %/$(patsubst $(OBJ_DIR)/%.d,%,$@).cpp,$(SOURCES)) > $@

$(OBJ_DIR_LIBRARY)/%.d :
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -MM $(filter %/$(patsubst $(OBJ_DIR_LIBRARY)/%.d,%,$@).cpp,$(SOURCES_LIBRARY)) 2>/dev/null | \
	perl -p -e 's!^([^:]+:)!$(OBJ_DIR_LIBRARY)/$$1!' > $@

include $(DEPS_LIBRARY)

####### Build Rules for Application Part

OBJECTS_APP = $(patsubst %.cpp,$(OBJ_DIR_APP)/%.o,$(notdir $(SOURCES_APP)))
DEPS_APP := $(patsubst %.o,%.d,$(OBJECTS_APP))

# $(OBJ_DIR_APP)/%.d :
# 	$(CXX) -c $(CXXFLAGS) $(INCPATH) -MM -MT $(patsubst %.d,%.o,$@)  $(filter %/$(patsubst $(OBJ_DIR)/%.d,%,$@).cpp,$(SOURCES)) > $@

$(OBJ_DIR_APP)/%.d :
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -MM $(filter %/$(patsubst $(OBJ_DIR_APP)/%.d,%,$@).cpp,$(SOURCES_APP)) 2>/dev/null | \
	perl -p -e 's!^([^:]+:)!$(OBJ_DIR_APP)/$$1!' > $@

include $(DEPS_APP)

####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

$(OBJ_DIR_LIBRARY)/%.o : $(filter %/$(patsubst $(OBJ_DIR_LIBRARY)/%.d,%,$@).cpp,$(SOURCES_LIBRARY)) $(OBJ_DIR_LIBRARY)/%.d
	$(CXX) $(CXXFLAGS) $(INCPATH)  -o $@ -c $(filter %/$(patsubst $(OBJ_DIR_LIBRARY)/%.o,%,$@).cpp,$(SOURCES_LIBRARY))

$(OBJ_DIR_APP)/%.o : $(filter %/$(patsubst $(OBJ_DIR_APP)/%.d,%,$@).cpp,$(SOURCES_APP)) $(OBJ_DIR_APP)/%.d
	$(CXX) $(CXXFLAGS) $(INCPATH)  -o $@ -c $(filter %/$(patsubst $(OBJ_DIR_APP)/%.o,%,$@).cpp,$(SOURCES_APP))

####### Build rules
deps : $(DEPS_LIBRARY) $(DEPS_APP)

static_library:  $(OBJECTS_LIBRARY)
	-$(DEL_FILE) $(OBJ_DIR_LIBRARY)/libisoaglib.a
	ar rcs $(OBJ_DIR_LIBRARY)/libisoaglib.a $^

dynamic_library:  $(OBJECTS_LIBRARY)
	-$(DEL_FILE) $(OBJ_DIR_LIBRARY)/libisoaglib.so
	$(CXX) -shared -o $(OBJ_DIR_LIBRARY)/libisoaglib.so $^


all: Makefile static_library dynamic_library  $(TARGET)
$(TARGET):  $(OBJECTS_APP) static_library dynamic_library
	$(LINK) $(LFLAGS) -o $(TARGET)_static $(OBJECTS_APP) $(OBJ_DIR_LIBRARY)/libisoaglib.a $(LIBS)
	$(LINK) $(LFLAGS) -o $(TARGET)_dynamic $(OBJECTS_APP) $(OBJ_DIR_LIBRARY)/libisoaglib.so $(LIBS)
	-$(DEL_FILE) $(TARGET)
	ln -s $(TARGET)_static $(TARGET)

yaccclean:
lexclean:
clean:
	-$(DEL_FILE) $(OBJECTS_LIBRARY) $(DEPS_LIBRARY) $(OBJECTS_APP) $(DEPS_APP)
	-$(DEL_FILE) $(OBJ_DIR_LIBRARY)/libisoaglib.a $(OBJ_DIR_LIBRARY)/libisoaglib.so
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET)_static $(TARGET)_dynamic $(TARGET)


FORCE:

####### Compile


####### Install
install_library: $(INSTALL_FILES_LIBRARY)
	mkdir -p $(INSTALL_PATH)/lib;
	@for INSTITEM in $(INSTALL_FILES_LIBRARY); do \
	SEARCH=$(ISOAGLIB_PATH)/library/xgpl_src; \
	REPLACE=$(INSTALL_PATH)/include; \
	#echo "Search $$SEARCH -> replace $$REPLACE"; \
	VAR_TEST=`echo $$INSTITEM | sed -e "s#$$SEARCH#$$REPLACE#g"`; \
	#echo "INSTITEM $$INSTITEM , ISOAGLIB_PATH $(ISOAGLIB_PATH) , INSTALL_PATH $(INSTALL_PATH) -> VAR_TEST $$VAR_TEST"; \
	DEST_PATH=`dirname $$VAR_TEST`; \
	#echo "DestPath $$DEST_PATH" ; \
	mkdir -p $$DEST_PATH; \
	install -m 0644 $$INSTITEM $$DEST_PATH; \
	done && \
	install -m 0644 ../../../../tutorial/src/ecu_display/config_ecuDisplay.h $(INSTALL_PATH)/include; \
	install -m 0644 $(OBJ_DIR_LIBRARY)/libisoaglib.a $(INSTALL_PATH)/lib/; \
	install -m 0755 $(OBJ_DIR_LIBRARY)/libisoaglib.so $(INSTALL_PATH)/lib/; \
	$(STRIP) $(INSTALL_PATH)/lib/libisoaglib.a; \
	$(STRIP) $(INSTALL_PATH)/lib/libisoaglib.so; \
	echo Finished installing includes/libraries to directory \"$(INSTALL_PATH)\"

####### Uninstall
uninstall_library: $(INSTALL_FILES_LIBRARY)
	@for INSTITEM in $(INSTALL_FILES_LIBRARY); do \
	SEARCH=$(ISOAGLIB_PATH)/library/xgpl_src; \
	REPLACE=$(INSTALL_PATH)/include; \
	#echo "Search $$SEARCH -> replace $$REPLACE"; \
	VAR_TEST=`echo $$INSTITEM | sed -e "s#$$SEARCH#$$REPLACE#g"`; \
	#echo "INSTITEM $$INSTITEM , ISOAGLIB_PATH $(ISOAGLIB_PATH) , INSTALL_PATH $(INSTALL_PATH) -> VAR_TEST $$VAR_TEST"; \
	#echo "VAR_TEST $$VAR_TEST" ; \
	rm -f $$VAR_TEST; \
	done && \
	CONFIG=`basename ../../../../tutorial/src/ecu_display/config_ecuDisplay.h`; \
	rm $(INSTALL_PATH)/include/$$CONFIG; \
	rm $(INSTALL_PATH)/lib/libisoaglib.a; \
	rm $(INSTALL_PATH)/lib/libisoaglib.so; \
	echo Finished uninstalling includes/libraries from directory \"$(INSTALL_PATH)\"

uninstall:

