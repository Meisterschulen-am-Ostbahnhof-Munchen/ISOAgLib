/** @page LgplSrcOverview Overview on Structure of IsoAgLib
	 The core components of the ISO<i><sub>AgLib</sub></i> which are
	  licensed under the conditions of the LGPL license of the
	  <a href="http://www.fsf.org">Free Software Foundation</a> are placed in the directory
	  <i>\<xgpl_src\></i> . Please note that the license requires each user to provide
	  public access to each change of the files within this core directory.
	  As the <a href="mailto:Achim.Spangler@osb-ag:de">maintainers of this library</a>
	  will collect and incorporate all contributions. This process guarantees that each
	  contributor will get more than he personally provided. \n
	  The main contributors to this project are in order of the importance of their part:
	  -# <a href="http://www.dfg.de">Deutsche Forschungsgesellschaft ( DFG )</a> by funding the research project <a href="http://ikb.weihenstephan.de">IKB-D&uuml;rnast</a>
		-# <a href="http://www.tec.wzw.tum.de">Research Group of Professor Auernhammer</a> by running the research project <a href="http://ikb.weihenstephan.de">IKB-D&uuml;rnast</a>
		-# <a href="http://www.fritzmeier.de">Fritzmeier</a> by contribution of the development of the ISO 11783 Virutal Terminal framework
		   which was needed for the user interface of their product <a href="http://www.fritzmeier.com/engl/frameset_engl.htm?/engl/environment/environment_miniveg.htm">Chlorophyll Sensor <b>MiniVeg N</b></a>
		-# <a href="http://www.sensor-technik.de">Sensortechnik Wiedemann (STW)</a> by support for the research of the main author within his research for IKB-D&uuml;rnast

		\section LgplStructure Overview on Directory Structure of LGPL Licensed Parts of IsoAgLib
		The structure of the source files of the ISO<i><sub>AgLib</sub></i> is described in the following imagemap diagram.
		The informaiton "(partly) obligtory, optional" is normally true for all contained directories and files of the listed
		nodes, as long as no exception is listed in the diagram.
		\subsection LgplMainStructure Structure of Main Parts
  \dot
  digraph LgplMainStructure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [shape=record, fontname=ARIALN, fontsize=10, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=10];
		ordering="out";

	  root        [label="IsoAgLib\n/\npartly obligatory", color="green",width=15];
		xgpl_src    [label="LGPL Licensed\npartly obligatory", color="green",width=15];
		subgraph cluster_level1 {
			style="invis";
			rank=same;
			app_conf    [label="Application Configuration\nobligatory", color="green2", URL="\ref isoaglib_config.h",width=5];
			main_lib    [label="Main Library Parts\npartly obligatory", color="green",width=5];
			suppl       [label="Supplementary Driver Extensions\ncomplete optional", color="yellow", URL="\ref LgplSupplStructure",width=5];
		}
		subgraph cluster_level2 {
			style="invis";
			rank=same;
			m_comm      [label="Communication Protocol\npartly obligatory", color="green", URL="\ref CommOverPage",width=3];
			m_driver    [label="Enhanced Drivers for main library parts\npartly obligatory", color="green",width=3];
			m_hal       [label="Hardware Abstraction Layer for main library parts\npartly obligatory", color="green",width=3];
			m_util      [label="Utilities for main library parts\nobligatory", color="green2",width=3];
			m_type      [label="Platform Independent Typedefs\nobligatory", color="green2", shape="ellipse", URL="\ref IsoAgLib/typedef.h",width=3];
		}

		subgraph cluster_level311 {
			style="invis";
			rank=same;
			system_mgmt [label="System Management\npartly obligatory", color="green", URL="\ref SystemMgmtPage",width=2 ];
			base        [label="Base Data\ncomplete optional", color="yellow", URL="\ref BaseDataPage",width=2 ];
			multi_msg   [label="Data Stream\nneeded for ISO Terminal", color="greenyellow", URL="\ref MultiMsgPage",width=2 ];
			iso_term    [label="ISO Virtual\ncomplete optional", color="yellow", URL="\ref XMLspec",width=2 ];
			process     [label="Process Data\noptional", color="yellow", URL="\ref ProcDataPage",width=2 ];
			}
		subgraph cluster_level312 {
			style="invis";
			rank=same;
			can         [label="Extended CAN driver\nobligatory", color="green2", URL="\ref IsoAgLib::iCanIo_c"];
			eeprom      [label="Extended EEPROM driver\noptional; needed for features of process data and ISO 11783", color="greenyellow", URL="\ref IsoAgLib::iEepromIo_c"];
			system      [label="Extended Access to System Information\nobligatory", color="green2", URL="\ref IsoAgLib::iSystem_c"];
		}
		subgraph cluster_level32 {
		style="invis";
			rank=same;
			hal_config  [label="Central Header for Platform Configuration\nobligatory", color="green2", shape="ellipse", URL="\ref hal/config.h"];
			hal_system  [label="Central Header for Main Target Functions ( e.g. get_time() )\nobligatory", color="green2", shape="ellipse", URL="\ref hal/system.h"];
			hal_can     [label="Central Header for Platform specific CAN IO\nobligatory", color="green2", shape="ellipse", URL="\ref hal/can.h"];
			hal_eeprom  [label="Central Header for Platform specific EEPROM IO\noptional", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
		}
		subgraph cluster_level33 {
			style="invis";
			rank=same;
			hal_esx     [label="HAL for ESX of STW\nobligatory for this platform", color="green2", URL="\ref MainHalEsx"];
			hal_imi     [label="HAL for IMI of STW\nobligatory for this platform", color="green2", URL="\ref MainHalImi"];
			hal_pc      [label="Example HAL for PC\nobligatory for this platform", color="green2", URL="\ref MainHalPc"];
			hal_pm167   [label="HAL for PM167 of STW\nobligatory for this platform", color="green2", URL="\ref MainHalPm167"];
		}

		root     -> xgpl_src  [label="xgpl_src"];
		xgpl_src -> app_conf  [label="Application_Config"];
		xgpl_src -> main_lib  [label="IsoAgLib"];
		xgpl_src -> suppl  [label="supplementary_driver"];

		main_lib -> m_comm [label="comm"];
		main_lib -> m_driver [label="driver"];
		main_lib -> m_hal [label="hal"];
		main_lib -> m_util [label="util"];
		main_lib -> m_type [label="typedef.h"];

		m_comm   -> base [label="Base",tailport="nw"];
		m_comm   -> multi_msg [label="Multipacket",tailport="w"];
		m_comm   -> system_mgmt [label="SystemMgmt",tailport="sw"];
		m_comm   -> process [label="Process",tailport="s"];
		m_comm   -> iso_term [label="ISO_Terminal",tailport="se"];

		m_driver -> can [label="can"];
		m_driver -> eeprom [label="eeprom"];
		m_driver -> system [label="system"];

		m_hal    -> hal_config [label="config.h"];
		m_hal    -> hal_system [label="system.h"];
		m_hal    -> hal_can [label="can.h"];
		m_hal    -> hal_eeprom [label="eeprom.h"];
		m_hal    -> hal_esx [label="esx"];
		m_hal    -> hal_imi [label="imi"];
		m_hal    -> hal_pc [label="pc"];
		m_hal    -> hal_pm167 [label="pm167"];

		system     -> hal_system [label="implement depend",style="dotted"];
		can      -> hal_can [label="implement depend",style="dotted"];
		process    -> eeprom [label="feature dependency",style="dotted"];
		system_mgmt -> eeprom [label="feature dependency",style="dotted"];
		eeprom     -> hal_eeprom [label="implement depend",style="dotted"];
   }
	 \enddot
		\subsection LgplSupplStructure Structure of Supplementary Parts
  \dot
  digraph LgplSupplStructure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [shape=record, fontname=ARIALN, fontsize=10, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=10];

		suppl       [label="Supplementary Driver Extensions\nsupplementary_driver\ncomplete optional", color="yellow",width=15];

		subgraph cluster_LgplSupplStructure_level1 {
			style="invis";
			rank=same;
			sup_driver  [label="Supplementary Extended Drivers\noptional", color="yellow",width=5];
			sup_hal     [label="HAL for Supplementary Extended Drivers\noptional", color="yellow",width=10];
		}
		subgraph cluster_level21 {
			style="invis";
			rank=same;
			sup_actor   [label="Extended Digital/PWM Output\noptional", color="yellow", URL="\ref iDigitalO_c"];
			sup_rs232   [label="Extended RS232 IO\noptional", color="yellow", URL="\ref iRS232IO_c"];
			sup_sensor  [label="Extended Sensor Input\noptional", color="yellow", URL="\ref iAnalogIRangeCheck_c"];
		}
		subgraph cluster_level22 {
			style="invis";
			rank=same;
			sup_h_actor [label="Central HAL Header for Supplementary Actor Driver\noptional", color="yellow", shape="ellipse", URL="\ref hal/actor.h"];
			sup_h_rs232 [label="Central HAL Header for Supplementary RS232 Driver\noptional", color="yellow", shape="ellipse", URL="\ref hal/rs232.h"];
			sup_h_sens  [label="Central HAL Header for Supplementary Sensor Driver\noptional", color="yellow", shape="ellipse", URL="\ref hal/sensor.h"];
		}
		subgraph cluster_level3 {
			style="invis";
			rank=same;
			sup_hal_esx [label="HAL for Supplementary Drivers on ESX of STW\noptional", color="yellow", URL="\ref SupplHalEsx"];
			sup_hal_imi [label="HAL for Supplementary Drivers on IMI of STW\noptional", color="yellow", URL="\ref SupplHalImi"];
			sup_hal_pc  [label="HAL for Supplementary Drivers on PC\noptional", color="yellow", URL="\ref SupplHalPc"];
			sup_hal_pm167 [label="HAL for Supplementary Drivers on PM167 of STW\noptional", color="yellow", URL="\ref SupplHalPm167"];
		}
		suppl    -> sup_driver [label="driver"];
		suppl    -> sup_hal [label="hal"];
		sup_driver -> sup_actor [label="actor"];
		sup_driver -> sup_rs232 [label="rs232"];
		sup_driver -> sup_sensor [label="sensor"];

		sup_hal  -> sup_h_actor [label="actor.h"];
		sup_hal  -> sup_h_rs232 [label="rs232.h"];
		sup_hal  -> sup_h_sens [label="sensor.h"];
		sup_hal  -> sup_hal_esx [label="esx"];
		sup_hal  -> sup_hal_imi [label="imi"];
		sup_hal  -> sup_hal_pc [label="pc"];
		sup_hal  -> sup_hal_pm167 [label="pm167"];

		sup_actor   -> sup_h_actor [label="implement depend",style="dotted"];
		sup_rs232   -> sup_h_rs232 [label="implement depend",style="dotted"];
		sup_sensor   -> sup_h_sens [label="implement depend",style="dotted"];
		}
	\enddot



	 \section StructurePrinciple Principles of Structuring the IsoAgLib
	 \subsection StructurObligOpt Distribution of Core Components and Supplements
	 The ISO<i><sub>AgLib</sub></i> was developed for a research project, where an ECU should
	 detect automatically connected implementes and should then record the retrieveable information
	 on a PCMCIA memory card ( connected via RS232 ). Additionally some information were read from
	 digital and analog input sensors.

	 Thus the ISO<i><sub>AgLib</sub></i> had not only the target to implement the ISO 11783 protocol, but also
	 to provide a platform hardware independent API for capable hardware access.
	 A ruling target of ISO<i><sub>AgLib</sub></i> is to server <b>optionally</b> as a complete hardware extension layer, which
	 allows the application developer to use driver services which are not included in the standard platform libraries and which
	 introduce no platform dependency on the main project. This principle is used for the actively used research systems and for the
	 commercial development on the implement <a href="http://www.fritzmeier.com/engl/frameset_engl.htm?/engl/environment/environment_miniveg.htm">Chlorophyll Sensor <b>MiniVeg N</b></a>.

	 But as most projects will start with ISO<i><sub>AgLib</sub></i> as pure ISO 11783 implementation, all
	 supplementary parts can be simply excluded from project - by just not including in project file list ( and not calling methods
	 from the excluded files - ;-) ).
	 This results in the main directories <i>\<xgpl_src/IsoAgLib\></i> and <i>\<xgpl_src/supplementary_driver\></i>.

	 But not all files are obligatory for each ISO 11783 project - as described in the diagram above.
	 First of all, you can exclude complete protocol features by exclusion of their corresponding directories.
	 Secondly some main features like \link SystemMgmtPage System Management \endlink ( claim address for local identities, ISO monitor list of all network nodes ) or
	 \link ProcDataPage process data \endlink contains some options, which are not needed for all projects. This could be ruled by the decision for the
	 protocol ISO 11783, or by the feature depth of the used process data. Please have a look at the documentation of the communication parts, where
	 more information on optional parts is provided.

	 \subsection StructureInterface Grouping of Interface with corresponding Implementation
	 The ISO<i><sub>AgLib</sub></i> provides all application relevant classes, functions and other components in the namespace
	 <i>IsoAgLib</i>, and hides the implementation of these components in the namespace <i>__IsoAgLib</i>. The name of all interface
	 classes starts with a lower <b>'i'</b>, whereas the corresponding implementation class has the same name - without the <b>'i'</b> at the beginning.
	 The implementation classes and source files are also hidden within the subdirectory <b>/impl</b> of the respective directory -
	 %e.%g. <i>\<xgpl_src/IsoAgLib/util\></i> for interface and <i>\<xgpl_src/IsoAgLib/util/<b>impl</b>\></i> for implementation.
	 So the content for the several "/impl" subdirectories is really only meant for experts who want to know how the different functions
	 are implemented - %e.%g. for implementation of extensions and bug-fixes.

	 \subsection StructureHal Structure of the HAL
	 The platform dependent variants of the HAL are grouped by the name of the respective plateform. This name is defined in the central
	 configuration file isoaglib_config.h , where either the corresponding #define like SYSTEM_ESX can be constantly defined or can be
	 provided as runtime defines during the Make-Process ( as compiler option ). The conditional #ifdef rules in isoaglib_config.h
	 allow the headers of the ISO<i><sub>AgLib</sub></i> to fetch the corresponding headers from the central headers in the
	 directories <i>\<xgpl_src/IsoAgLib/hal\></i> and <i>\<xgpl_src/supplementary_driver/hal\></i>.
	 Each platform has its own subdirectory in the previous mentioned directories.
	 Like described in the diagram at the top of this page, each driver extension directory is mapped with a same named
	 directory within <i>\<xgpl_src/IsoAgLib/hal/PlatformXY\></i> or <i>\<xgpl_src/supplementary_driver/hal/PlatformXY\></i>.

	 The main idea of the HAL is to provide a unique API for hardware and system access where at least the used functions
	 have a unique name and functions ( described by their parameters ). This can be realized in most cases by simple
	 inline functions - which are a better C++ variant of the old style function-macros of C. The call of these inline functions is
	 replaced during compile time by their function body - thus the platform library functions are directly called during runtime.
	 If the ISO<i><sub>AgLib</sub></i> requires some features which can not be provided by simple inline functions, the needed additional
	 functionality can be implemented in the files named like system_target_extensions.h and system_target_extensions.cpp.

	 The name mapping is provided also with the help of namespaces, which cause no runtime overhead.
	 Therefore the needed platform specific headers are included with an <b>extern "C"</b> statement in the namespace
	 <b>__HAL</b>. The inline functions are defined as part of the namespace <b>HAL</b>, so that the names of the
	 platform specific headers and the HAL can be identically. The compiler can distinguish between them with the help of
	 the namespace.

	*/
