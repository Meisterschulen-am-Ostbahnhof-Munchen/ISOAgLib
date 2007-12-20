/** \page LgplSrcOverview License Parts Structural Overview
The core components of the \isoaglib which are licensed under the conditions of the LGPL license of the
 <a href="http://www.fsf.org">Free Software Foundation</a> are placed in the directory
 <i>\<xgpl_src\></i> . Please note that the license requires each user to provide
 public access to each change of the files within this core directory.
 As the <a href="mailto:Achim.Spangler@osb-ag:de">maintainers of this library</a>
 will collect and incorporate all contributions. This process guarantees that each
 contributor will get more than he personally provided. \n
 The main contributors to this project are in order of the importance of their part:
 -# <a href="http://www.dfg.de">Deutsche Forschungsgesellschaft (DFG)</a> by funding the research project <a href="http://ikb.weihenstephan.de">IKB-Dürnast</a>
 -# <a href="http://www.tec.wzw.tum.de">Research Group of Professor Auernhammer</a> by running the research project <a href="http://ikb.weihenstephan.de">IKB-Dürnast</a>
 -# <a href="http://www.fritzmeier.de">Fritzmeier</a> by contribution of the development of the ISO 11783 Virutal Terminal framework
    which was needed for the user interface of their product <a href="http://www.fritzmeier.com/engl/frameset_engl.htm?/engl/environment/environment_miniveg.htm">Chlorophyll Sensor <b>MiniVeg N</b></a>
 -# <a href="http://www.sensor-technik.de">Sensortechnik Wiedemann (STW)</a> by support for the research of the main author within his research for IKB-Dürnast

\section LgplStructure Overview on Directory Structure of LGPL Licensed Parts of IsoAgLib
The structure of the source files of the \isoaglib is described in the following imagemap diagram.
The informaiton "(partly) obligtory, optional" is normally true for all contained directories and files of the listed
nodes, as long as no exception is listed in the diagram.
\subsection LgplMainStructure Structure of Main Parts
  \dot
  digraph LgplMainStructure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];
    /*size="16,20";*/
    ordering="out";

 root        [label="IsoAgLib\n(partly obligatory)", color="green"/*width=15*/];
 xgpl_src    [label="LGPL Licensed\n(partly obligatory)", color="green"/*,width=15*/];
 subgraph cluster_level1 {
    style="invis";
    rank=same;
    app_conf    [label="Application Configuration\n(obligatory)", color="green2", URL="\ref isoaglib_config.h"/*,width=5*/];
    main_lib    [label="Main Library Parts\n(partly obligatory)", color="green"/*,width=5*/];
    suppl       [label="Supplementary Driver Extensions\n(complete optional)", color="yellow", URL="\ref LgplSupplStructure"/*,width=5*/];
 }
 subgraph cluster_level2 {
    style="invis";
    rank=same;
    m_comm      [label="Communication\nProtocol\n(partly obligatory)", color="green", URL="\ref CommOverPage"/*,width=3*/];
    m_driver    [label="Enhanced Drivers\nfor main library parts\n(partly obligatory)", color="green"/*,width=3*/];
    m_hal       [label="HAL for main\nlibrary parts\n(partly obligatory)", color="green"/*,width=3*/];
    m_util      [label="Utilities for\nmain library\n(partly obligatory)", color="green2"/*,width=3*/];
    m_type      [label="Platform\nindependent\ntypedefs\n(obligatory)", color="green2", shape="ellipse", URL="\ref IsoAgLib/typedef.h"/*,width=3*/];
 }

 subgraph cluster_level311 {
    style="invis";
    rank=same;
    network_mgmt [label="P5: Network\nManagement\n(partly obligatory)", color="green", URL="\ref NetworkMgmtPage"/*,width=2*/ ];
    multi_msg   [label="P3: DataLinkLayer\n(needed for VT)", color="greenyellow", URL="\ref DataLinkPage"/*,width=2*/ ];
    appLayer    [label="P7: AppLayer\n(complete optional)", color="yellow", URL="\ref AppLayerPage"/*,width=2*/ ];
    iso_term    [label="P6: Virtual Terminal\n(complete optional)", color="yellow", URL="\ref XMLspec"/*,width=2*/ ];
    process     [label="P2: Process Data\n(optional)", color="yellow", URL="\ref ProcDataPage"/*,width=2*/ ];
    taskCtrlr   [label="P10: Task\nController", color="yellow", URL="" ];
    fileSvr     [label="P13: File\nServer", color="yellow", URL="" ];
 }
 subgraph cluster_level312 {
    style="invis";
    rank=same;
    can         [label="Extended\nCAN driver\n(obligatory)", color="green2", URL="\ref IsoAgLib::iCanIo_c"];
    eeprom      [label="Extended\nEEPROM driver\n(optional)", color="greenyellow", URL="\ref IsoAgLib::iEepromIo_c"];
    system      [label="Extended\nAccess to\nSystem Information\n(obligatory)", color="green2", URL="\ref IsoAgLib::iSystem_c"];
 }
 subgraph cluster_level32 {
    style="invis";
    rank=same;
    hal_system  [label="Central Header\nfor Main Target\n Functions \n(e.g. get_time())\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/system.h"];
    hal_eeprom  [label="Central Header\nfor Platform\nspecific EEPROM IO\n(optional)", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
    hal_can     [label="Central Header\nfor Platform\nspecific CAN IO\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/can.h"];
    hal_config  [label="Central Header\nfor Platform\nConfiguration\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/config.h"];
 }
 subgraph cluster_level33 {
    style="invis";
    rank=same;
    hal_esx     [label="HAL for ESX of STW\n(obligatory for\nthis platform)", color="green2", URL="\ref MainHalEsx"];
    hal_imi     [label="HAL for IMI of STW\n(obligatory for\n this platform)", color="green2", URL="\ref MainHalImi"];
    hal_pc      [label="Example HAL for PC\n(obligatory for\n this platform)", color="green2", URL="\ref MainHalPc"];
    hal_pm167   [label="HAL for PM167 of STW\n(obligatory for\n this platform)", color="green2", URL="\ref MainHalPm167"];
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

 m_comm   -> multi_msg    [label="Multipacket"];
 m_comm   -> appLayer     [label="Applcation\nLayer"];
 m_comm   -> network_mgmt [label="Network\nMgmt"];
 m_comm   -> process      [label="Process"];
 m_comm   -> iso_term     [label="Virtual\nTerminal"];
 m_comm   -> fileSvr      [label="File\nServer"];
 m_comm   -> taskCtrlr    [label="Task\nController"];

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
 network_mgmt -> eeprom [label="feature dependency",style="dotted"];
 eeprom     -> hal_eeprom [label="implement depend",style="dotted"];
   }
\enddot

\subsection LgplSupplStructure Structure of Supplementary Parts
\dot
 digraph LgplSupplStructure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=8];
    suppl       [label="Supplementary Driver\nExtensions\nsupplementary_driver\n(complete optional)", color="yellow"/*,width=15*/];

 subgraph cluster_LgplSupplStructure_level1 {
    style="invis";
    rank=same;
    sup_driver  [label="Supplementary\nExtended Drivers\n(optional)", color="yellow"/*,width=5*/];
    sup_hal     [label="HAL for Supplementary\nExtended Drivers\n(optional)", color="yellow"/*,width=10*/];
 }
 subgraph cluster_level21 {
    style="invis";
    rank=same;
    sup_actor   [label="Extended Digital/PWM Output\n(optional)", color="yellow", URL="\ref IsoAgLib::iDigitalO_c"];
    sup_rs232   [label="Extended RS232 IO\n(optional)", color="yellow", URL="\ref IsoAgLib::iRS232IO_c"];
    sup_sensor  [label="Extended Sensor Input\n(optional)", color="yellow", URL="\ref IsoAgLib::iAnalogIRangeCheck_c"];
 }
 subgraph cluster_level22 {
    style="invis";
    rank=same;
    sup_h_actor [label="Central HAL Header\nfor Supplementary\nActor Driver\noptional", color="yellow", shape="ellipse", URL="\ref hal/actor.h"];
    sup_h_rs232 [label="Central HAL Header\nfor Supplementary\nRS232 Driver\noptional", color="yellow", shape="ellipse", URL="\ref hal/rs232.h"];
    sup_h_sens  [label="Central HAL Header\nfor Supplementary\nSensor Driver\noptional", color="yellow", shape="ellipse", URL="\ref hal/sensor.h"];
 }
 subgraph cluster_level3 {
    style="invis";
    rank=same;
    sup_hal_esx [label="HAL for\nSupplementary\nDrivers on\nESX of STW\n(optional)", color="yellow", URL="\ref SupplHalEsx"];
    sup_hal_imi [label="HAL for\nSupplementary\nDrivers on\nIMI of STW\n(optional)", color="yellow", URL="\ref SupplHalImi"];
    sup_hal_pc  [label="HAL for\nSupplementary\nDrivers on PC\n(optional)", color="yellow", URL="\ref SupplHalPc"];
    sup_hal_pm167 [label="HAL for\nSupplementary\nDrivers on\nPM167 of STW\n(optional)", color="yellow", URL="\ref SupplHalPm167"];
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



\section StructurePrinciple Structuring Principles of the IsoAgLib

\subsection StructurObligOpt Distribution of Core Components and Supplements
The \isoaglib was developed for a research project, where an ECU should
detect automatically connected implementes and should then record the retrieveable information
on a PCMCIA memory card (connected via RS232). Additionally some information were read from
digital and analog input sensors.

Thus the \isoaglib had not only the target to implement the ISO 11783 protocol, but also
to provide a platform hardware independent API for capable hardware access.
A ruling target of \isoaglib is to server <i>optionally</i> as a complete hardware extension layer, which
allows the application developer to use driver services which are not included in the standard platform libraries and which
introduce no platform dependency on the main project. This principle is used for the actively used research systems and for the
commercial development on the implement <a href="http://www.fritzmeier.com/engl/frameset_engl.htm?/engl/environment/environment_miniveg.htm">Chlorophyll Sensor <b>MiniVeg N</b></a>.

But as most projects will start with \isoaglib as pure ISO 11783 implementation, all
supplementary parts can be simply excluded from project - by just not including in project file list (and not calling methods
from the excluded files - ;-)).
This results in the main directories <tt>xgpl_src/IsoAgLib</tt> and <tt>xgpl_src/supplementary_driver</tt>.

But not all files are obligatory for each ISO 11783 project - as described in the diagram above.
First of all, you can exclude complete protocol features by exclusion of their corresponding directories.
Secondly some main features like \ref NetworkMgmtPage "Network Management (claim address for local identities, ISO monitor list of all network nodes) or
\link ProcDataPage process data \endlink contains some options, which are not needed for all projects. This could be ruled by the decision for the
protocol ISO 11783, or by the feature depth of the used process data. Please have a look at the documentation of the communication parts, where
more information on optional parts is provided.

\subsection StructureInterface Grouping of Interface with corresponding Implementation
The \isoaglib provides all application relevant classes, functions and other components in the namespace
<tt>IsoAgLib</tt>, and hides the implementation of these components in the namespace <tt>__IsoAgLib</tt>. The name of all interface
classes starts with a lower <b>'i'</b>, whereas the corresponding implementation class has the same name - without the <b>'i'</b> at the beginning.
The implementation classes and source files are also hidden within the subdirectory <tt>/impl</tt> of the respective directory -
%e.%g. <tt>xgpl_src/IsoAgLib/util</tt> for interface and <tt>xgpl_src/IsoAgLib/util/<b>impl</b>/</tt> for implementation.
So the content for the several "/impl" subdirectories is really only meant for experts who want to know how the different functions
are implemented - %e.%g. for implementation of extensions and bug-fixes.

\subsection StructureHal HAL Structure
The platform dependent variants of the HAL are grouped by the name of the respective plateform. This name is defined in the central
configuration file isoaglib_config.h , where either the corresponding #define like SYSTEM_ESX can be constantly defined or can be
provided as runtime defines during the Make-Process (as compiler option). The conditional #ifdef rules in isoaglib_config.h
allow the headers of the \isoaglib to fetch the corresponding headers from the central headers in the
directories <tt>xgpl_src/IsoAgLib/hal</tt> and <tt>xgpl_src/supplementary_driver/hal</tt>.
Each platform has its own subdirectory in the previous mentioned directories.
Like described in the diagram at the top of this page, each driver extension directory is mapped with a same named
directory within <tt>xgpl_src/IsoAgLib/hal/PlatformXY</tt> or <tt>xgpl_src/supplementary_driver/hal/PlatformXY</tt>.

The main idea of the HAL is to provide a unique API for hardware and system access where at least the used functions
have a unique name and functions (described by their parameters). This can be realized in most cases by simple
inline functions - which are a better C++ variant of the old style function-macros of C. The call of these inline functions is
replaced during compile time by their function body - thus the platform library functions are directly called during runtime.
If the \isoaglib requires some features which can not be provided by simple inline functions, the needed additional
functionality can be implemented in the files named like system_target_extensions.h and system_target_extensions.cpp.

The name mapping is provided also with the help of namespaces, which cause no runtime overhead.
Therefore the needed platform specific headers are included with an <tt>extern "C"</tt> statement in the namespace
<tt>__HAL</tt>. The inline functions are defined as part of the namespace <tt>HAL</tt>, so that the names of the
platform specific headers and the HAL can be identically. The compiler can distinguish between them with the help of
the namespace.

*/
