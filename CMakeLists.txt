# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES
	isoaglib_project_config
	IsoAgLib-hal
	Versuche
	)

set(COMPONENT_SRCDIRS
  library/xgpl_src/IsoAgLib/comm 
  library/xgpl_src/IsoAgLib/comm/impl
  library/xgpl_src/IsoAgLib/comm/Part12_DiagnosticsServices/impl
  library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl
  library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl
  library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client
  library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl
  library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl
  library/xgpl_src/IsoAgLib/driver/can/impl
  library/xgpl_src/IsoAgLib/driver/system/impl
  library/xgpl_src/IsoAgLib/hal/generic_utils/can
  library/xgpl_src/IsoAgLib/hal/generic_utils/system
  library/xgpl_src/IsoAgLib/scheduler/impl
  library/xgpl_src/IsoAgLib/util
  library/xgpl_src/IsoAgLib/util/impl
  library/xgpl_src/supplementary_driver/driver/datastreams
  library/xgpl_src/supplementary_driver/driver/eeprom/impl
)
  
set(COMPONENT_ADD_INCLUDEDIRS 
  .
  library/xgpl_src
  library/xgpl_src/IsoAgLib/comm 
  library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client
  library/xgpl_src/IsoAgLib/hal/generic_utils/can
  library/xgpl_src/IsoAgLib/hal/generic_utils/system
  library/xgpl_src/IsoAgLib/util
  library/xgpl_src/supplementary_driver/driver/datastreams
  
)

set(COMPONENT_PRIV_INCLUDEDIRS 
  library/xgpl_src/IsoAgLib/comm/impl
  library/xgpl_src/IsoAgLib/comm/Part12_DiagnosticsServices/impl
  library/xgpl_src/IsoAgLib/comm/Part3_DataLink/impl
  library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl
  library/xgpl_src/IsoAgLib/comm/Part6_VirtualTerminal_Client/impl
  library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/impl
  library/xgpl_src/IsoAgLib/driver/can/impl
  library/xgpl_src/IsoAgLib/driver/system/impl
  library/xgpl_src/IsoAgLib/scheduler/impl
  library/xgpl_src/IsoAgLib/util/impl
  library/xgpl_src/supplementary_driver/driver/eeprom/impl
)
  
register_component()
