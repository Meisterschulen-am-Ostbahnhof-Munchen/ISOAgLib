/** @page MainHalPage Structure of HAL for Main Parts of IsoAgLib
	The directory structure of the \isoaglib HAL
	is designed to ease quick adaptation to new platform types.

	\section MainHalAdaptationNew Adaptation to New Platform
	The following steps are enough for adaptation to a new hardware:
	-# create a new SYSTEM_XY define in the header isoaglib_config.h for your platform ( compare to SYSTEM_ESX )
	-# create a new #elif setting in isoaglib_config.h where the directory name for the HAL of the new platform is defined ( compare to SYSTEM_ESX )
	-# create a copy of the directory <i>\<xgpl_src/IsoAgLib/hal/esx\></i> with the directory name as specified in isoaglib_config.h
	-# do this also for <i>\<xgpl_src/supplementary_driver/hal/esx\></i> if you intend to use \isoaglib for actor, rs232 or sensor
	-# adapt the files in the new directories to your specific needs
		- adapt #include statements for platform specific library / BIOS / RTOS
		- adapt config settings in config.h ( compare to esx/config.h )
		- adapt typedefs settings in typedef.h ( compare to esx/typedef.h )
		- adapt the function mapping in the several files <i>\<target\>/\<device\>/\<device\>.h</i>
		- implement functionality which is not provided by platform library in file named like system<b>_target_extensions.h</b> and system<b>_target_extensions.cpp</b>
		- adapt CAN layer in hal_can_interface.h and hal_can_interface.cpp ( some CAN implementations use parameter structures which can't be mapped with inline functions )
	-# insert your new SYSTEM_XY setting in your Makefiles or project configuration
	-# send your adaptation to <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> , so that your adaptation can be used and possibly optimized
	   within the user community

	\section MainHalStructure Structure of HAL for Main Parts
		\subsection MainHalEsx Structure of HAL for ESX of STW
  \dot
  digraph MainHalEsxStructure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

		m_hal       [label="Hardware Abstraction Layer\nfor main library parts\npartly obligatory", color="green4", fontcolor="white"];

		hal_config  [label="Central Header for\nPlatform Configuration\nobligatory", color="green2", shape="ellipse", URL="\ref hal/config.h"];
		hal_system  [label="Central Header for\nMain Target Functions ( e.g. get_time() )\nobligatory", color="green2", shape="ellipse", URL="\ref hal/system.h"];
		hal_can     [label="Central Header for\nPlatform specific CAN IO\nobligatory", color="green2", shape="ellipse", URL="\ref hal/can.h"];
		hal_eeprom  [label="Central Header for\nPlatform specific EEPROM IO\noptional", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
		hal_esx     [label="HAL for ESX of STW\nobligatory for this platform", color="green2"];

		h_esx_system [label="Target Adaptation\nof Base System Calls\n(obligatory for this platform)", color="green2", URL="\ref esx/system/system.h"];
		h_esx_can   [label="Target Adaptation\nof CAN IO\n(obligatory for this platform)", color="green2", URL="\ref esx/can/can.h"];
		h_esx_eeprom [label="Target Adaptation\nof EEPROM\n(optional);needed from other features", color="greenyellow", URL="\ref esx/eeprom/eeprom.h"];
		h_esx_conf  [label="ESX specific\nConfiguration Settings\n(obligatory)", color="green2", shape="ellipse", URL="\ref esx/config.h"];
		h_esx_type  [label="ESX specific\nTypedefs\n(obligatory)", color="green2", shape="ellipse", URL="\ref esx/typedef.h"];
		h_esx_err   [label="ESX specific\nErrcode Mapping\n(obligatory)", color="green2", shape="ellipse", URL="\ref esx/errcodes.h"];

		m_hal    -> hal_config [label="config.h"];
		m_hal    -> hal_system [label="system.h"];
		m_hal    -> hal_can [label="can.h"];
		m_hal    -> hal_eeprom [label="eeprom.h"];
		m_hal    -> hal_esx [label="esx"];

		hal_esx  -> h_esx_system [label="system"];
		hal_esx  -> h_esx_can [label="can"];
		hal_esx  -> h_esx_eeprom [label="eeprom"];
		hal_esx  -> h_esx_conf [label="config.h"];
		hal_esx  -> h_esx_type [label="typedef.h"];
		hal_esx  -> h_esx_err [label="errcodes.h"];

		hal_system -> h_esx_system [label="implement depend",style="dotted"];
		hal_can  -> h_esx_can [label="implement depend",style="dotted"];
		hal_eeprom -> h_esx_eeprom [label="implement depend",style="dotted"];
		}
		\enddot
		\subsection MainHalImi Structure of HAL for IMI of STW
  \dot
  digraph MainHalImiStructure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

		m_hal       [label="Hardware Abstraction Layer\nfor main library parts\npartly obligatory", color="green4", fontcolor="white"];

		hal_config  [label="Central Header for\nPlatform Configuration\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/config.h"];
		hal_system  [label="Central Header for\nMain Target Functions (e.g. get_time())\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/system.h"];
		hal_can     [label="Central Header for\nPlatform specific CAN IO\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/can.h"];
		hal_eeprom  [label="Central Header for\nPlatform specific EEPROM IO\n(optional)", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
		hal_imi     [label="HAL for IMI of STW\n(obligatory for this platform)", color="green2"];

		h_imi_system [label="system", color="green2", URL="\ref imi/system/system.h"];
		h_imi_can   [label="can", color="green2", URL="\ref imi/can/can.h"];
		h_imi_eeprom [label="eeprom", color="greenyellow", URL="\ref imi/eeprom/eeprom.h"];
		h_imi_conf  [label="config.h", color="green2", shape="ellipse", URL="\ref imi/config.h"];
		h_imi_type  [label="typedef.h", color="green2", shape="ellipse", URL="\ref imi/typedef.h"];
		h_imi_err   [label="errcodes.h", color="green2", shape="ellipse", URL="\ref imi/errcodes.h"];

		m_hal    -> hal_config [label="config.h"];
		m_hal    -> hal_system [label="system.h"];
		m_hal    -> hal_can [label="can.h"];
		m_hal    -> hal_eeprom [label="eeprom.h"];
		m_hal    -> hal_imi [label="imi"];

		hal_imi  -> h_imi_system [label="system"];
		hal_imi  -> h_imi_can [label="can"];
		hal_imi  -> h_imi_eeprom [label="eeprom"];
		hal_imi  -> h_imi_conf [label="config.h"];
		hal_imi  -> h_imi_type [label="typedef.h"];
		hal_imi  -> h_imi_err [label="errcodes.h"];

		hal_system -> h_imi_system [label="implement depend",style="dotted"];
		hal_can  -> h_imi_can [label="implement depend",style="dotted"];
		hal_eeprom -> h_imi_eeprom [label="implement depend",style="dotted"];

	}
	\enddot
		\subsection MainHalPm167 Structure of HAL for PM167 of STW
  \dot
  digraph MainHalPm167Structure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

		m_hal       [label="Hardware Abstraction Layer\nfor main library parts\n(partly obligatory)", color="green4", fontcolor="white"];

		hal_config  [label="Central Header for\nPlatform Configuration\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/config.h"];
		hal_system  [label="Central Header for\nMain Target Functions (e.g. get_time())\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/system.h"];
		hal_can     [label="Central Header for\nPlatform specific CAN IO\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/can.h"];
		hal_eeprom  [label="Central Header for\nPlatform specific EEPROM IO\n(optional)", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
		hal_pm167   [label="HAL for PM167 of STW\n(obligatory for this platform)", color="green2"];

		h_pm167_system [label="system", color="green2", URL="\ref pm167/system/system.h"];
		h_pm167_can   [label="can", color="green2", URL="\ref pm167/can/can.h"];
		h_pm167_eeprom [label="eeprom", color="greenyellow", URL="\ref pm167/eeprom/eeprom.h"];
		h_pm167_conf  [label="config.h", color="green2", shape="ellipse", URL="\ref pm167/config.h"];
		h_pm167_type  [label="typedef.h", color="green2", shape="ellipse", URL="\ref pm167/typedef.h"];
		h_pm167_err   [label="errcodes.h", color="green2", shape="ellipse", URL="\ref pm167/errcodes.h"];

		m_hal    -> hal_config [label="config.h"];
		m_hal    -> hal_system [label="system.h"];
		m_hal    -> hal_can [label="can.h"];
		m_hal    -> hal_eeprom [label="eeprom.h"];
		m_hal    -> hal_pm167 [label="pm167"];

		hal_pm167 -> h_pm167_system [label="system"];
		hal_pm167 -> h_pm167_can [label="can"];
		hal_pm167 -> h_pm167_eeprom [label="eeprom"];
		hal_pm167 -> h_pm167_conf [label="config.h"];
		hal_pm167 -> h_pm167_type [label="typedef.h"];
		hal_pm167 -> h_pm167_err [label="errcodes.h"];

		hal_can  -> h_pm167_can [label="implement depend",style="dotted"];
		hal_system -> h_pm167_system [label="implement depend",style="dotted"];
		hal_eeprom -> h_pm167_eeprom [label="implement depend",style="dotted"];

	}
	\enddot
		\subsection MainHalPc Structure of HAL for PC
  \dot
  digraph MainHalPcStructure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

		m_hal       [label="Hardware Abstraction Layer\nfor main library parts\npartly obligatory", color="green4", fontcolor="white"];

		hal_config  [label="Central Header for\nPlatform Configuration\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/config.h"];
		hal_system  [label="Central Header for\nMain Target Functions (e.g. get_time())\nobligatory)", color="green2", shape="ellipse", URL="\ref hal/system.h"];
		hal_can     [label="Central Header for\nPlatform specific CAN IO\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/can.h"];
		hal_eeprom  [label="Central Header for\nPlatform specific EEPROM IO\n(optional)", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
		hal_pc      [label="Example HAL for PC\n(obligatory for this platform)", color="green2"];

		h_pc_system [label="system", color="green2", URL="\ref pc/system/system.h"];
		h_pc_can    [label="can", color="green2", URL="\ref pc/can/can.h"];
		h_pc_eeprom [label="eeprom", color="greenyellow", URL="\ref pc/eeprom/eeprom.h"];
		h_pc_conf   [label="config.h", color="green2", shape="ellipse", URL="\ref pc/config.h"];
		h_pc_type   [label="typedef.h", color="green2", shape="ellipse", URL="\ref pc/typedef.h"];
		h_pc_err    [label="errcodes.h", color="green2", shape="ellipse", URL="\ref pc/errcodes.h"];

		m_hal    -> hal_config [label="config.h"];
		m_hal    -> hal_system [label="system.h"];
		m_hal    -> hal_can [label="can.h"];
		m_hal    -> hal_eeprom [label="eeprom.h"];
		m_hal    -> hal_pc [label="pc"];

		hal_pc   -> h_pc_system [label="system"];
		hal_pc   -> h_pc_can [label="can"];
		hal_pc   -> h_pc_eeprom [label="eeprom"];
		hal_pc   -> h_pc_conf [label="config.h"];
		hal_pc   -> h_pc_type [label="typedef.h"];
		hal_pc   -> h_pc_err [label="errcodes.h"];

		hal_system -> h_pc_system [label="implement depend",style="dotted"];
		hal_can  -> h_pc_can [label="implement depend",style="dotted"];
		hal_eeprom -> h_pc_eeprom [label="implement depend",style="dotted"];

	}
	\enddot
	*/
