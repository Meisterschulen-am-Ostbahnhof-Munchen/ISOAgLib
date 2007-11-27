/*! \mainpage Welcome to IsoAgLib.org!

INCLUDE_doc/txt/mainHeaderWithNavbar.txt_INCLUDE
<table class="infoboxLinks" border="0">
<tr><th>Quick-Links</th></tr>
<tr><td>
\ref XMLspec
</td></tr>
<tr><td>
\ref XMLProcSpec
</td></tr>
</table>

<table class="infobox" border="0">
<tr><th>Section Overview</th></tr>
<tr><td>The following parts are described in this section:
    <ul><li> \ref LatestNews </li>
        <li> \ref MainFeatures </li>
        <li> \ref StructuralOverview </li>
        <li> \ref UsingProjects "Known Applications" </li>
        <li> \ref Maintainers "Maintainers" </li>
        <li> \ref Acknowledgements </li>
        <li> \ref Authors
    </ul>
</td></tr>
</table>

\section Home Welcome to IsoAgLib.org!

IsoAglib provides a free portable C++ source library for development
of <a href="http://www.isobus.com/">ISO 11783 (ISOBUS)</a>
applications.

IsoAgLib is mainly developed on LINUX. The actual target device is
typically an embedded system. Some Win32 specific adaptations do
exist, too.

Follow the <a href="examples.html">tutorial examples</a> to learn how
to develop applications using IsoAglib.

\section LatestNews Latest News

\subsection NewsForBigRelease New for the 2.0 release
  - <b>December 2006:</b> Support of PEAK CAN cards for Linux development hosts
  - <b>October 2006:</b> Integrate enhanced scheduler, which plans exact reschedule times for all periodic subtasks of ISO<i><sub>AgLib</sub></i>, so that other program items
     exactly know when the timeEvent() function of ISO<i><sub>AgLib</sub></i> has to be called again.
  - <b>September 2006:</b> <b>ISO<i><sub>AgLib</sub></i></b> now is a real ISO library! Complete removal of all DIN-related or DIN-dependent code, classes and preprocessor directives.
    The following steps had to be done:
    + remove all unneeded DIN9684 and all its #ifdefs
    + merge ISONAME_c and DevKey_c to the new ISONAME_c as the only
      class to represent the device key
    + merge SystemMgmt_c and IsoMonitor_c to the new IsoMonitor_c class
      so that it hosts both all IdentItem_c (as list of pointers)
      and does also maintain list of all ECUs on the BUS
      --> all Network Management is done by this IsoMonitor_c class right
      now
    + optimize adress resolving for process data send, as some helper functions
      were only needed for the DIN way of life
    + optimize the TP send and receive classes, as several action flows were
      only needed for the parallel support of DIN, ISO and Fieldstar/LBS+ TP

\subsection NewsForNextIncrementalRelease Planned changes for 2.0.1 release
  - there are currently no planned changes on interface side for 2.0.1 release
  - CAN HAL will get its own central message FIFO implementation, to get better load balancing and message processing in order as received

\subsection CurrentAdditionNews News for the 1.2.1 release
  - <b>February 2006:</b> Enhance modularization of base information (Not-ProcessData parts from 11783-7) - this will be finished for release 1.2.2
  - <b>January 2006:</b> Periodic Source Adress (SA) request sending to get early cleanup of stale nodes in monitor list
  - <b>18.1.2006:</b> Virtual Terminal Client<br>
    - Now supporting correct scaling of the button-contents! (Each button has its own scaling-factor/centering-offset as the border is always 4 absolute 4 pixel, independent of how the object-pool will be scaled!)
      <b>ATTENTION: Please REBUILD vt2iso and RERUN it on your project's XML-definition, the initAllObjects function in the generated -functions.inc has changed!!<br>
    - Also new due to this slightly different scaling in the buttons: Don't use the same fontattributes-object in the datamask AND a button. Also (from a strict point of view) don't use the same fontattributes-object in different-sized buttons (as they all got a slightly different scaling-factor)!!!</b><br>
    - Please recheck all your fixed-bitmaps in your projects (if using any), because the scaling-routine has slightly changed, which may result in size-differences by 1 pixel - this may lead to NOT finding a matching fixed-bitmap anymore!

\subsection NewNews News for the 1.2.0 release
	- <b>during 2005:</b> Virtual Terminal Client<br>
		- <b>ISO<i><sub>AgLib</sub></i></b> has now its own internet domains: http://www.isoaglib.org , http://www.isoaglib.de and http://www.isoaglib.com
		- Added support for IsoLatin-9 fonts: Now for every font-attribute the "font_type" attribute has to be specified. (Please adapt your existing XML files.)<br>
		- Support for font-downscaling (in case a pool is designed for >200x200 pixel VTs)<br>
		- Cleanup on Tutorial3_0 example objectpool
		- Several more handler functions, for more information see "Changes" section below<br>
		- Vector-Informatik supports now the Open Sourve IDE <b>Dev-C++</b>, so that you don't need Microsoft compilers to get the tutorials to run with real CAN hardware on Win32.
		- New area of customer only extension modules. Specific extansions like complete level 2 and 3 Tractor ECU Client and Server support need several additional functions, that
			are at the moment only implemented by <a href="http://www.osb-ag.de/">OSB</a> and nobody else from the user community. Thus we don't want to give it for free to anonymous
			parties (including our competitors). Researchers and potential customer may ask <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> for granting personal access
			to the repository.
	- <b>2005-March:</b> MultiReceive classes implemented.

<b>
\ref OldNews
</b>

\section MainFeatures Main Features
The ISO<i><sub>AgLib</sub></i> is designed to provide the following main features
(read \ref InfGeneralInformation for more information):
- perform all tasks which can be automated in the background so that the application development is simplified
        and different interpretation of low level communication (like message formatting) is avoided
- provide <b>complete Open Source toolchain for ISO 11783 development - including virtual terminal</b> <br>
    Masks are defined with XML notation, which is then converted to ROM-enabled variable arrays which are then
    included in an object module of the project. ISO<i><sub>AgLib</sub></i> provides a simple function call
    to register the mask pool. ISO<i><sub>AgLib</sub></i> performs then some runtime adaptation to terminal properties
    ( choose usable colour depth of bitmaps, scaling, including handling of cases where
    layout properties like text alignment should be conserved even if no font of scaled size is available ) and finishes this process
    by an automatic upload of the adapted pool. Already active pools can be handled with simple access functions for control,
    and some easy handler functions for reaction on user input.<br>
    <b>Core target of ISO<i><sub>AgLib</sub></i> Virtual Terminal design:</b><br>
    - Enable creation of one single mask pool, that fits all terminal configurations
    - Provide attributes for project specific control of the runtime adaptation
    - Allow flexible addition of further attributes, if automatic layout control must be optimized for some terminal properties ( e.g. amount of softkeys )

    All these actions are demonstrated in the tutorial example \ref 3_0_VirtualTerminalIso.cpp .<br>
    &nbsp;<br>
    Brad Cox developed a tool for his company, that is able to convert the project files of the WTK mask generator to the XML definition.
    Please contact <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> if you are interested in this tool for your own use, as Brad
    Cox indicated already, that he might be willing to provide this undest some conditions. <br>
    But this tool would only help to start the virtual mask definition with XML, as this method provides some additional attributes to
    control the automatic runtime adaptation of the single core mask pool to the different terminal properties ( e.g. font size dependent spacing ).
- provide flexible and capable process data implementation as backbone for all documenting and control interaction
        (e.g. trigger dependent measurement value send, allow setpoint intervals with MIN / MAX,
        enable N --&gt; 1 relation between user and provider of process data)
- narrow hardware adaptation afford to small set of files with hardware adaptation layer
- greatest part of ISO<i><sub>AgLib</sub></i> source code can be used without any changes for different platforms
- enable project specific feature selection, source code maintenance and extension by strict modular design
- facilitate development of application for ISO 11783
- orientate design according to requirements of networks with more then two devices


\section StructuralOverview Structural Overview

\subsection StructureModularity Modular Design
The ISO<i><sub>AgLib</sub></i> is modularized according to the different functional
components. Only a small part of the modules is obligatory for all systems,
so that an individual project can adapt the capability and Flash ROM size
to the specific needs.<br>

\subsection StructureLayers Layered Design
The ISO<i><sub>AgLib</sub></i> is designed for easy adaptation to new hardware platforms. Thereby the greatest
part of the software can be used without any changes for all hardware types. The more this common software
part is used, the better the quality can get. <br>
The layered structure is described by the following diagram:
  \dot
  digraph LayeredStructure {
    fontname=ARIALN;
    /*ranksep=1.0;*/
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node [shape=record, fontname=ARIALN, fontsize=9, style=filled, fontcolor=black];
    edge [fontname=ARIALN, fontsize=9, dir="back"];
    ordering=out;
    pack=true;

    upperRuler [ label="<p1>a|<p21>b|<p22>b|<p23>b|<p3>c|<p4>d|<p5>e", style=invis ];

    subgraph cluster_Communication {
        style="filled";
        label="Communication";
        bottomlabel="Communication Services for ISO 11783";
        bgcolor=grey;
        color="grey";
        rank=same;
        multi_msg       [label="Data Stream\n(needed for\nISO Terminal)", color="cyan4", URL="\ref MultiMsgPage", width=1.25 ];
        iso_term        [label="ISO Virtual\n(complete optional)", color="white", URL="\ref XMLspec", width=1.5 ];
        process         [label="Process Data\n(optional)", color="white", URL="\ref ProcDataPage", width=1.25 ];
        system_mgmt     [label="System Management\n(partly obligatory)", color="cyan2", URL="\ref SystemMgmtPage", width=1.5 ];
        base            [label="Base Data\n(complete optional)", color="white", URL="\ref BaseDataPage", width=1.5 ];
        comm_doc        [label="Communication Services\nfor ISO 11783", URL="\ref CommOverPage", width=1.5 ];
    }

    subgraph cluster_DriverExtensions {
        style="filled";
        label="Driver Extensions";
        bgcolor=grey;
        color="grey";
        rank=same;
        rs232_drv    [label="RS232\n Supplementary\nDriver with\nIOSTREAMS Like\nRead/Write\nAccess", color="white", URL="\ref IsoAgLib::iRS232IO_c"];
        sensor_drv   [label="Sensor\nSupplementary\nDriver with\nScaling and\nRange-Check", color="white", URL="\ref IsoAgLib::iAnalogIRangeCheck_c"];
        actor_drv    [label="Actor\nSupplementary\nDriver with\nState Monitoring\nFunctions", color="white", URL="\ref IsoAgLib::iDigitalO_c"];
        can_drv      [label="CAN\nIndividual Mask/\nFilter Settings\nfor each Filterbox", color="yellow", URL="\ref IsoAgLib::iCanIo_c"];
        eeprom_drv   [label="EEPROM\nIO-STREAMS Like\nRead/Write Access", color="yellow", URL="\ref IsoAgLib::iEepromIo_c"];
        common_drv   [label="Common System\nAccess\n(Globally Used)", color="yellow", URL="\ref IsoAgLib::iSystem_c"];
        drv_doc      [label="Provide Driver Features\nwhich are not part of\nmost Platforms"];
    }

    subgraph cluster_HAL {
        style="filled";
        label="HAL";
        color="grey";
        bgcolor=grey;
        rank=same;
        rs232_hal    [label="RS232\nSupplementary\nDriver", color="white", URL="\ref hal/rs232.h"];
        sensor_hal   [label="Sensor\nSupplementary\nDriver", color="white", URL="\ref hal/sensor.h"];
        actor_hal    [label="Actor\nSupplementary\nDriver", color="white", URL="\ref hal/actor.h"];
        can_hal      [label="CAN", color="dodgerblue3", URL="\ref hal/can.h"];
        eeprom_hal   [label="EEPROM\n(Store Process Data,\nSelfConf SA, ...)", color="dodgerblue3", URL="\ref hal/eeprom.h"];
        common_hal   [label="Common\nSystem Access\n(Watchdog,Time, ...)", color="dodgerblue3", URL="\ref hal/system.h"];
        hal_doc      [label="Map Platform Specific\nAPI to Unique API\nfor IsoAgLib\n(Mapping during\ncompile-time -\nno runtime-overhead)",URL="\ref MainHalPage"];
    }

    subgraph cluster_Hardware {
        style="filled";
        label="Hardware";
        color="grey";
        bgcolor=grey;
        rank=same;
        bios      [ label="BIOS\nCollection of Low Level \nDriver Libraries", color="green2" ];
        place_hw  [ label="<p1>a|<p21>b|<p2>b|<p23>b|<p3>c|<p31>c1|<p32>c2|<p4>d|<p41>d1|<p42>d2|<p5>e|<p51>e1|<p52>e2|<p6>f|<p61>f1", style=invis ];
        rtos      [ label="RTOS\nRealtime Operating System", color="green2" ];
    }

    lowerRuler [ label="<p1>a|<p21>b|<p22>b|<p23>b|<p3>c|<p4>d|<p5>e|<p6>f", style=invis];

    upperRuler:p1 -> comm_doc [ style=invis, weight=6000 ];

    comm_doc      -> drv_doc [ style=invis, weight=6000 ];
    drv_doc       -> hal_doc [ style=invis, weight=6000 ];
    hal_doc       -> bios    [ style=invis, weight=6000 ];

    common_hal -> place_hw:p1 [ /*weight=4000*/ ];
    eeprom_hal -> place_hw:p2 [ /*weight=4000*/ ];
    can_hal    -> place_hw:p3 [ /*weight=1000*/ ];
    rs232_hal  -> place_hw:p4 [ /*weight=1000*/ ];
    sensor_hal -> place_hw:p5 [ /*weight=1000*/ ];
    actor_hal  -> place_hw:p6 [ /*weight=1000*/];

    bios -> lowerRuler:p1      [ style=invis/*, weight=3000*/ ];
    place_hw -> lowerRuler:p23 [ style=invis/*, weight=3000*/ ];
    rtos -> lowerRuler:p6      [ style=invis/*, weight=6000*/ ];

    system_mgmt -> common_drv [ style=invis/*, weight=2000 */ ];
    process     -> eeprom_drv [ style=invis/*, weight=2000 */ ];
    base        -> can_drv    [ style=invis/*, weight=2000 */ ];
    iso_term    -> rs232_drv  [ style=invis/*, weight=2000 */ ];
    multi_msg   -> actor_drv  [ style=invis/*, weight=2000 */];

    upperRuler:p21 -> system_mgmt [ style=invis/*, weight=2000 */ ];
    upperRuler:p22 -> process     [ style=invis/*, weight=2000 */ ];
    upperRuler:p23 -> base        [ style=invis/*, weight=2000 */ ];
    upperRuler:p3  -> iso_term    [ style=invis/*, weight=2000 */ ];
    upperRuler:p5  -> multi_msg   [ style=invis/*, weight=2000 */ ];

    process     -> eeprom_drv [ style="dotted"];
    system_mgmt -> can_drv    [ /*weight=1000, headport="nw", tailport="se"*/ ];
    process     -> can_drv    [ /*weight=1000*/ ];
    base        -> can_drv    [ /*weight=1000*/ ];
    iso_term    -> can_drv    [ /*weight=1000*/ ];
    multi_msg   -> can_drv    [ /*weight=1000*/ ];
    system_mgmt -> eeprom_drv [ style="dotted" ];
    common_drv  -> common_hal [ /*weight=3000*/ ];
    can_drv     -> can_hal    [ /*weight=3000*/];
    eeprom_drv  -> eeprom_hal [ /*weight=3000*/ ];
    rs232_drv   -> rs232_hal  [ /*weight=2000*/ ];
    sensor_drv  -> sensor_hal [ /*weight=2000*/ ];
    actor_drv   -> actor_hal  [ /*weight=2000*/ ];
}
\enddot

\subsection StructObligatory Information on Obligatory Elements
The following elements are needed for all projects:
    - blue nodes in layer "HAL"
    - yellow nodes in layer "Driver Extensions"
    - cyan nodes in layer "Communication" ( the subdirectory ISO11783 is only used for the respective protocol )

Therefore the minimum footprint of the ISO<i><sub>AgLib</sub></i> in Flash ROM is quite low.

\subsection StructPartlyOptional Partly Optional

The supplementary drivers for RS232, Sensor Input and Actuator Control ( mainly PWM ) were developed for
the research project at TUM.


\section UsingProjects Known Applications
As the used license <i>GPL with exception</i> requires from each user to accompany any distributed Program
( or a work based on it - see GPL &sect;3 ) with some information on the used library, it would be quite kind to
inform additionally the authors of your project, so that each using project can be listed here.
This list can also serve as a prominent place for showing all contributing ( by money and/or code ) users.<br>

Last but not least, projects which use features like process data communication based on ISO<i><sub>AgLib</sub></i> will
probably have less compatibility problems than in other combinations. This gets the more important, the more implements,
sensors, task controllers, core control units ( expert system ) and tractor builds more and more complex
interaction networks with all sorts of dependencies. Here ISO<i><sub>AgLib</sub></i> can provide common patterns for the
management of such interactions. This list can then help to identify well supported device combinations.<br>

The <a href="http://www.isoaglib.com/">Commercial Overview Page</a> of ISO<i><sub>AgLib</sub></i> has a
<a href="http://www.isoaglib.com/customer-agco.html">list of commercial products which use ISO<i><sub>AgLib</sub></i></a>.


The ISO<i><sub>AgLib</sub></i> will be actively maintained by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> at <a href="http://www.osb-ag.de">OSB AG</a>
as long as the invested time is affordable in relation to corresponding projects and support contracts.<br>
Even in case the active maintenance by <a href="http://www.osb-ag.de">OSB AG</a> might be stopped some day, the GPL license and
the independent website at the
<a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology - Crop Production Engineering</a>
assure that any interested party can step in and continue the maintenance of the project. This guarantees, like in other Open Source projects, the
open access for every user. <br>
Also, in case the major part of the user community is unsatisfied with the maintenance by <a href="http://www.osb-ag.de">OSB AG</a>, it is
normal for Open Source projects like ISO<i><sub>AgLib</sub></i> to fork the project as a last resort, if <a href="http://www.osb-ag.de">OSB AG</a>
can't or doesn't want to change the style of maintenance as requested. As far as possible, <a href="http://www.osb-ag.de">OSB AG</a>
will do everything to avoid such a fork.<br>
A comparable fork was performed by users and developers of the X11R6 server project for UNIX style operating systems - <b>XFree86</b>.
Caused by some licensing issues, a fork of the <a href="http://www.xfree86.org/">XFree86</a> was created by the <a href="http://X.Org">X.Org Foundation</a>,
which also appreciated by several graphic card manufacturers, as they can integrate their drivers with the new management method in a better way.
<br>
&nbsp;<br>
&nbsp;<br>
<b>
This way it is assured under all conditions, that the development time and money, which is invested in an application that uses the ISO<i><sub>AgLib</sub></i>
can't be affected by the style and quality of future project maintenance.</b>


\section Maintainers Some information on the maintainers
The ISO<i><sub>AgLib</sub></i> was initially created by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> who is now
working for the company <b><a href="http://www.osb-ag.de">OSB AG</a></b>. This company started business at the beginning of 2003, and has
already more than 160 engineers working in the five locations Munich, Stuttgart, Krefeld, Hamburg and Berlin ( all in Germany; <b>state February 2007</b> ).<br>
The main business focus is project support at the customer location in software, electronic and mechanical engineering.<br>
Some of the <b><a href="http://www.osb-ag.de">OSB AG</a></b> customers are:
    - AGCO GmbH / Fendt
    - Apparatebau Gauting GmbH
    - Avery Dennison Deutschland GmbH
    - AZH GmbH
  - BMW
    - EMAG
    - FIDUCIA IT
    - Fritzmeier
    - Infineon Technologies AG
    - MTU Aero Engines
    - Porsche
    - Robert Bosch GmbH
    - Siemens AG
    - ThyssenKrupp AutomotiveMechatronics GmbH
    - Valeo

More information on commercial backing of ISO<i><sub>AgLib</sub></i> can be obtained <a href="http://www.isoaglib.com/">here</a>.


\subsection MaintainersExtensions Exclusive feature area for new customers and researchers
Even if ISO<i><sub>AgLib</sub></i> provides already most of the features, a agricultural device
may need, there are still some functions, that are missing or that could ease application
development. But their implementation can only be done, when people of the user community
do at least part of the work, some sponsoring companies give OSB some money to do this,
or if OSB has the option to refinance the work with surrounding customer contracts (we want
at least to avoid that our competitors take the code for free and make their own business without
even asking us).

Interested parties don't have to pay much money, as we are mostly interested in information about
everybody who uses those additional features. So don't hesitate to contact <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
for registration to get access to the <b>EXT</b> part of ISO<i><sub>AgLib</sub></i>.

The following areas are examples for such extensions:
    - tractor data level 2 + 3 server
        - <i>iTracLight_c</i> for lightning information and control
        - <i>iTracMoveSetpoint_c</i> for control of tractor moving parameters by implement
        - <i>iTracPtoSetpoint_c</i> for control of PTO settings by implement
        - <i>iTracAux_c</i> for management of axiliary valves of tractor ( control and information )
    - additional features for virtual terminal access like multi language framework (simple definition of variable language depend parts of the mask pool)


\section Acknowledgements Acknowledgements
\subsection InitialFundingDfg Thanks to DFG funded research group IKB Duernast
The initial creation of the ISO<i><sub>AgLib</sub></i> was enabled by the research project
<b>&quot;IKB D&uuml;rnast&quot;</b> which is leaded by <b>Prof. Auernhammer</b> at the <a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology
			- Crop Production Engineering</a> .

\subsection InitialSupport Thanks to development by Sensortechnik Wiedemann (STW)
The development phase with some compiler issues was supported by the experts of the
<b>ESX</b> engineering team at <a href="http://www.sensor-technik.de">Sensortechnik Wiedemann ( STW )</a> .
Some of the main design goals were derived during a project with <a href="http://www.sensor-technik.de">STW</a>
and <a href="http://www.fendt.com/">AGCO Fendt</a> .

\subsection IsoVtExtension Thanks to Fritzmeier for contribution of ISO 11783 Virtual Terminal support
The development of the central control unit for the chlorophyll sensor
<a href="http://www.fritzmeier.de/engl/environment/environment_miniveg.htm">MiniVeg N</a> was heavily based on the prior work at ISO<i><sub>AgLib</sub></i>,
so that Fritzmeier accepted to fund the development of ISO 11783 virtual terminal support in ISO<i><sub>AgLib</sub></i> .

\subsection FirstWin32Users Thanks to eager and patient initial developers with Win32
As the ISO<i><sub>AgLib</sub></i> is mainly developed on LINUX, all Win32 specific adaptations
including driver mappings and project file generation for Win32 IDEs weren't optimal until
first users like <b>Brad Cox</b> started to try ISO<i><sub>AgLib</sub></i> in combination with
<b>Microsoft Visual Studio C++</b> . As a result, ISO<i><sub>AgLib</sub></i>:
-  provides now an automatic project file generation for Microsoft Visual Studio VC++ version 6.0 ( DSP file ),
	 with the requirement of installation of STLport, if version 7.0 and above isn't used
- Open Source Win32 IDE "Dev-C++" is used for Win32 compile of vt2iso, as there the several required utils can be installed a lot easier.

\section Authors Authors and Contributors

	- Main Author: <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> designed
			the complete framework
	- Virtual Terminal Expert: <a href="mailto:M.Wodok@osb-ag:de">Martin Wodok</a> implemented
			the tool <b>vt2iso</b> and the virtual terminal handling
	- Pioneer Win32 User and <b>First External Contributor</b>: <a href="mailto:Brad.Cox@agcocorp:com">Brad Cox</a> who helped to find several optimization needs for documentation,
			tutorial and project generation support. The resulting documentation helped him finally to implement all missing VT Object types, which
			weren't needed for the <b>MiniVeg N</b> project. He is already using ISO<i><sub>AgLib</sub></i> for some interesting test applications, and
			contributed all missing VT Objects, so that ISO<i><sub>AgLib</sub></i> imeplements also VT Objects like Macro and AuxiliaryInput.
			Some other extensions from him are moveChildLocation and setOriginSKM functions for VT Objects, where appropriate.
			Last but not least, he started with us to integrate multi language support.
*/