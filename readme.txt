/*! \mainpage 

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
    <ul><li> \ref Home "Welcome" </li>
        <li> \ref LatestNews </li>
        <li> \ref MainFeatures </li>
        <li> \ref StructuralOverview </li>
        <li> \ref UsingProjects "Known Applications" </li>
        <li> \ref Maintainers "Maintainers" </li>
        <li> \ref Authors
    </ul>
</td></tr>
</table>

<div style="clear:left"></div>

\section Home Welcome to IsoAgLib.org!
\htmlonly
<SCRIPT LANGUAGE="JavaScript">
<!-- Begin
  function popUp(URL) {
    window.open(URL, '" + id + "', 'toolbar=0,scrollbars=0,location=0,statusbar=0,menubar=0,resizable=1,width=300,height=470,left=100,top=100');
  }
// End -->
</script>
<span style="float:right"><a href="javascript:popUp('Images/isobus.png')"><img src="Images/isobus_thumb.png" alt="isobus.png" border="0"><br><small>ISOBUS overview</a></small></span>
\endhtmlonly
@isoaglib provides a free portable C++ source library for development
of <a href="http://www.isobus.com/">ISO 11783 (ISOBUS)</a>
applications.
The purpose of ISO 11783 is to provide an open interconnected system for on-board electronic systems. It is intended to enable electronic units to communicate with each other providing a standardized system.
ISO 11783 consists of 13 parts which specify a communications system for agricultural equipment based on the CAN 2.0 B [1] protocol.<br/>

\isoaglib is mainly developed on LINUX. The actual target device is typically an embedded system. There also exist some Win32 specific adaptations, as well.

\subsection GettingOverview More Information on IsoAgLib
To learn more about \isoaglib you can check the navigation menu for further topics or use one of the following links:
    - \ref MainFeatures
    - \ref StructuralOverview describes the structural design of \isoaglib
    - \ref InfGeneralInformation, \ref InfAgentNetwork, \ref InfServiceNetwork provide some insight on the design goals of the complete system
    - \ref 3_0_VirtualTerminalIso.cpp illustrates the ease of application development in an example for virtual terminal handling
    - the <a href="examples.html">tutorial examples page</a> provides other examples for studying purpose

\section LatestNews Latest News

<!--\subsection NewsForNextIncrementalRelease Planned changes for 2.1.1 release-->

\subsection NewsForBigRelease New for 2.1 release
  - CAN HAL has its own central message FIFO implementation, to get better load balancing, more efficient processing and easier reconfiguration after addition or deletion of FilterBox_c instances and message processing in order as received
  - PC can_server port vor Windows available (socket communication which can also be used under Linux)
  - fixed Working-Set-Master announce (TC client/server communication)
  - central mutex, so that a separate thread can safely access the \isoaglib, while no Scheduler_c::timeEvent() triggered internal activities of \isoaglib are running
  - reordering of directory structure, so that just a part of \isoaglib can be checked out, which contains only the library sources and managed BIOS files

<b>
\ref PageOldNews
</b>

\section MainFeatures Main Features
The \isoaglib is designed to provide the following main features
(read \ref InfGeneralInformation for more information):
- perform all tasks which can be automated in the background so that the application development is simplified
        and different interpretation of low level communication (like message formatting) is avoided
- provide <b>complete Open Source toolchain for ISO 11783 development - including virtual terminal</b> <br>
    Masks are defined with XML notation, which is then converted to ROM-enabled variable arrays which are then
    included in an object module of the project. \isoaglib provides a simple function call
    to register the mask pool. \isoaglib performs then some runtime adaptation to terminal properties
    (choose usable colour depth of bitmaps, scaling, including handling of cases where
    layout properties like text alignment should be conserved even if no font of scaled size is available) and finishes this process
    by an automatic upload of the adapted pool. Already active pools can be handled with simple access functions for control,
    and some easy handler functions for reaction on user input.<br>
    <b>Core target of \isoaglib Virtual Terminal design:</b><br>
    - Enable creation of one single mask pool, that fits all terminal configurations
    - Provide attributes for project specific control of the runtime adaptation
    - Allow flexible addition of further attributes, if automatic layout control must be optimized for some terminal properties (e.g. amount of softkeys )

    All these actions are demonstrated in the tutorial example \ref 3_0_VirtualTerminalIso.cpp .<br>
    &nbsp;<br>
    Brad Cox developed a tool for his company, that is able to convert the project files of the WTK mask generator to the XML definition.
    Please contact <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> if you are interested in this tool for your own use, as Brad
    Cox indicated already, that he might be willing to provide this undest some conditions. <br>
    But this tool would only help to start the virtual mask definition with XML, as this method provides some additional attributes to
    control the automatic runtime adaptation of the single core mask pool to the different terminal properties (e.g. font size dependent spacing ).
- provide flexible and capable process data implementation as backbone for all documenting and control interaction
        (e.g. trigger dependent measurement value send, allow setpoint intervals with MIN / MAX,
        enable N --&gt; 1 relation between user and provider of process data)
- narrow hardware adaptation afford to small set of files with hardware adaptation layer
- greatest part of \isoaglib source code can be used without any changes for different platforms
- enable project specific feature selection, source code maintenance and extension by strict modular design
- facilitate development of application for ISO 11783
- orientate design according to requirements of networks with more then two devices


\section StructuralOverview Structural Overview

\subsection StructureModularity Modular Design
The \isoaglib is modularized according to the different functional
components. Only a small part of the modules is obligatory for all systems,
so that an individual project can adapt the capability and Flash ROM size
to the specific needs.<br>

\subsection StructureLayers Layered Design
The \isoaglib is designed for easy adaptation to new hardware platforms. Thereby the greatest
part of the software can be used without any changes for all hardware types. The more this common software
part is used, the better the quality can get. <br>
The layered structure is described by the following diagram:

<!-- 
  THE LINKED PAGES NEED TO BE MODIFIED MANUALLY AS IT IS NOT (YET) POSSIBLE IN DOXYGEN 
  TO REFERENCE JUST THE LINK ADDRESS TO A PAGE WITHOUT ACTUALLY CREATING THE LINK!
  So it could be useful to use the dot creation tool first and then copy the proper urls 
  manually from the generated page.
  The graphic was exported from the OpenOffice Draw document admin/graphics/structure.odd
-->
\htmlonly
<img src="Images/structure.png" usemap="#structure" alt="Structural Overview" border="0">
<br/><small>additional information can be accessed by clicking on the respective area in the image</small>
<map name="structure">
    <area shape="rect" coords=" 20, 38,131, 94" href="CommOverPage.html" alt="Communication Services for ISO 11783" title="Communication Services for ISO 11783">
    <!--<area shape="rect" coords="183, 21,218,169" href="" alt="Scheduler" title="Scheduler">-->
    <area shape="rect" coords="228, 38,318, 94" href="MultiMsgPage.html" alt="Part 3: Data Link" title="Part 3: Data Link">
    <area shape="rect" coords="341, 38,433, 94" href="SystemMgmtPage.html" alt="Part 5: Network Management" title="Part 5: Network Management">    
    <area shape="rect" coords="455, 38,526, 94" href="XMLspec.html" alt="Part 6: ISO Virtual Terminal" title="Part 6: ISO Virtual Terminal">
    <area shape="rect" coords="548. 63,613, 94" href="BaseDataPage.html" alt="Part 7: Application Layer" title="Part 7: Application Layer">
    <area shape="rect" coords="613, 63,656, 94" href="ProcDataPage.html" alt="Part 7: Process Data" title="Part 7: Process Data">
    <!--<area shape="rect" coords="682, 38,755, 94" href="" alt="Part 10: Task Controller" title="Part 10: Task Controller">
        <area shape="rect" coords="776, 38,847, 94" href="" alt="Part 13: File Server" title="Part 13: File Server">
    -->

<!--<area shape="rect" coords=" 20,170,150,245" href="" alt="Provide Driver Features which are not part of most Platforms" title="Provide Driver Features which are not part of most Platforms">   -->
    <area shape="rect" coords="190,170,300,225" href="d3/db4/classIsoAgLib_1_1iSystem__c.html" alt="Common System Access (Globally Used)" title="Common System Access (Globally Used)">
    <area shape="rect" coords="323,170,416,227" href="d4/dca/classIsoAgLib_1_1iEepromIo__c.html" alt="EEPROM IO-STREAMS Like Read/Write Access" title="EEPROM IO-STREAMS Like Read/Write Access">
    <area shape="rect" coords="435,163,529,237" href="d9/dc6/classIsoAgLib_1_1iCanIo__c.html" alt="CAN individual mask/filter settings for each filterbox" title="CAN individual mask/filter settings for each filterbox">
    <area shape="rect" coords="550,152,641,244" href="d8/d6a/classIsoAgLib_1_1iRS232IO__c.html" alt="RS232 Supplementary Driver with IO STREAMS like read/write access" title="RS232 Supplementary Driver with IO STREAMS like read/write access">
    <area shape="rect" coords="663,154,754,244" href="dd/dc8/classIsoAgLib_1_1iAnalogIRangeCheck__c.html" alt="Sensor Supplementary Driver with Scaling and Range-Check" title="Sensor Supplementary Driver with Scaling and Range-Check">
    <area shape="rect" coords="776,153,867,242" href="da/d12/classIsoAgLib_1_1iDigitalO__c.html" alt="Actor Supplementary Driver with State Monitoring Functions" title="Actor Supplementary Driver with State Monitoring Functions">

    <area shape="rect" coords=" 19,321,170,397" href="MainHalPage.html" alt="Map Platform Specific API to Unique API for IsoAgLib (Mapping during compile-time -no runtime-overhead)" title="Map Platform Specific API to Unique API for IsoAgLib (Mapping during compile-time -no runtime-overhead)">
    <area shape="rect" coords="189,330,302,385" href="dc/db2/system_8h.html" alt="Common System Access (Watchdog,Time, ...)" title="Common System Access (Watchdog,Time, ...)">
    <area shape="rect" coords="323,330,415,385" href="d0/ded/eeprom_8h.html" alt="EEPROM (Store Process Data, SelfConf SA, ...)" title="EEPROM (Store Process Data, SelfConf SA, ...)">
    <area shape="rect" coords="437,330,532,385" href="d7/da8/can_8h.html" alt="CAN" title="CAN">
    <area shape="rect" coords="549,330,642,385" href="dd/d8b/rs232_8h.html" alt="RS232 Supplementary Driver" title="RS232 Supplementary Driver">    
    <area shape="rect" coords="663,330,755,385" href="d5/d2e/sensor_8h.html" alt="Sensor Supplementary Driver" title="Sensor Supplementary Driver">
    <area shape="rect" coords="776,330,866,385" href="df/d10/actor_8h.html" alt="Actor Supplementary Driver" title="Actor Supplementary Driver">

<!--<area shape="rect" coords="077,472,359,510" href="" alt="BIOS Collection of Low Level  Driver Libraries" title="BIOS Collection of Low Level  Driver Libraries">
    <area shape="rect" coords="529,473,811,510" href="" alt="RTOS Realtime Operating System" title="RTOS Realtime Operating System"> -->
</map>
\endhtmlonly
<!--

\dot
  digraph LayeredStructure {
    fontname=ARIALN;
    ranksep=1.0;
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
-->

\subsection StructObligatory Information on Obligatory Elements
The following elements are needed for all projects:
    - blue nodes in layer "HAL"
    - yellow nodes in layer "Driver Extensions"
    - cyan nodes in layer "Communication" (the subdirectory ISO11783 is only used for the respective protocol )

Therefore the minimum footprint of the \isoaglib in Flash ROM is quite low.

\subsection StructPartlyOptional Partly Optional

The supplementary drivers for RS232, Sensor Input and Actuator Control (mainly PWM) were developed for
the research project at TUM.


\section UsingProjects Known Applications
As the used license <i>GPL with exception</i> requires from each user to accompany any distributed Program
( or a work based on it - see GPL &sect;3) with some information on the used library, it would be quite kind to
inform additionally the authors of your project, so that each using project can be listed here.
This list can also serve as a prominent place for showing all contributing (by money and/or code) users.<br>

Last but not least, projects which use features like process data communication based on \isoaglib will
probably have less compatibility problems than in other combinations. This gets the more important, the more implements,
sensors, task controllers, core control units (expert system) and tractor builds more and more complex
interaction networks with all sorts of dependencies. Here \isoaglib can provide common patterns for the
management of such interactions. This list can then help to identify well supported device combinations.<br>

The <a href="http://www.isoaglib.com/">Commercial Overview Page</a> of \isoaglib has a
<a href="http://www.isoaglib.com/customer-agco.html">list of commercial products which use \isoaglib</a>.


The \isoaglib will be actively maintained by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> at <a href="http://www.osb-ag.de">OSB AG</a>
as long as the invested time is affordable in relation to corresponding projects and support contracts.<br>
Even in case the active maintenance by <a href="http://www.osb-ag.de">OSB AG</a> might be stopped some day, the GPL license and
the independent website at the
<a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology - Crop Production Engineering</a>
assure that any interested party can step in and continue the maintenance of the project. This guarantees, like in other Open Source projects, the
open access for every user. <br>
Also, in case the major part of the user community is unsatisfied with the maintenance by <a href="http://www.osb-ag.de">OSB AG</a>, it is
normal for Open Source projects like \isoaglib to fork the project as a last resort, if <a href="http://www.osb-ag.de">OSB AG</a>
can't or doesn't want to change the style of maintenance as requested. As far as possible, <a href="http://www.osb-ag.de">OSB AG</a>
will do everything to avoid such a fork.<br>
A comparable fork was performed by users and developers of the X11R6 server project for UNIX style operating systems - <b>XFree86</b>.
Caused by some licensing issues, a fork of the <a href="http://www.xfree86.org/">XFree86</a> was created by the <a href="http://X.Org">X.Org Foundation</a>,
which also appreciated by several graphic card manufacturers, as they can integrate their drivers with the new management method in a better way.
<br>
&nbsp;<br>
&nbsp;<br>
<b>
This way it is assured under all conditions, that the development time and money, which is invested in an application that uses the \isoaglib
can't be affected by the style and quality of future project maintenance.</b>


\section Maintainers Some information on the maintainers
The \isoaglib was initially created by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> who is now
working for the company <b><a href="http://www.osb-ag.de">OSB AG</a></b>. This company started business at the beginning of 2003, and has
already more than 160 engineers working in the five locations Munich, Stuttgart, Krefeld, Hamburg, Frankfurt, Köln and Berlin (all in Germany; <b>state November 2007</b> ).<br>
The main business focus is project support at the customer location in software, electronic and mechanical engineering.<br>
Some of the <b><a href="http://www.osb-ag.de">OSB AG</a></b> customers are:
    - <a href="http://www.agcocorp.com/">AGCO GmbH/<a href="http://www.fendt.com/">Fendt</a>
    - AL.systems GmbH
    - <a href="http://www.aoa-gauting.de/">Apparatebau Gauting GmbH</a>
    - <a href="http://www.machines.averydennison.com/">Avery Dennison Deutschland GmbH</a>
    - AZH GmbH
    - <a href="http://www.benqmobile.com">BenQ Mobile GmbH & Co OHG</a>
    - <a href="http://www.emag.de/">EMAG GmbH</a>
    - <a href="http://www.fiducia.de/">FIDUCIA IT AG</a>
    - <a href="http://www.fritzmeier.de/">FRITZMEIER Systems GmbH & Co.</a>
    - <a href="http://www.infineon.com/">Infineon Technologies AG</a>
    - <a href="http://www.mtu.de">MTU Aero Engines</a>
    - <a href="http://www.kvb.de/">Kassenärztliche Vereinigung Bayerns</a>
    - <a href="http://www.oce.de">Océ Printing Systems GmbH</a>
    - Panasonic Automotive Systems Europe GmbH
    - <a href="http://www.porsche.com/usa/">Dr. Ing. h.c.F. Porsche AG</a>
    - <a href="http://www.siemens.com">Siemens AG</a>
    - <a href="http://www.thyssenkrupp-technologies.de">ThyssenKrupp AutomotiveMechatronics GmbH</a>
    - <a href="http://www.valeo.de">Valeo Sicherheitssysteme GmbH</a>
    - <a href="http://www.valeo.de">Valeo Schalter und Sensoren GmbH</a>
    - <a href="http://bosch.com">Robert Bosch GmbH</a>
    - <a href="http://www.imis.com">Joh. Winklhofer & Söhne GmbH & Co. KG</a>
    - <a href="http://www.ivm-automotive.com/">IVM Automotive München Gmb</a>

More information on commercial backing of \isoaglib can be obtained <a href="http://www.isoaglib.com/">here</a>.


\subsection MaintainersExtensions Exclusive feature area for new customers and researchers
Even if \isoaglib provides already most of the features, a agricultural device
may need, there are still some functions, that are missing or that could ease application
development. But their implementation can only be done, when people of the user community
do at least part of the work, some sponsoring companies give OSB some money to do this,
or if OSB has the option to refinance the work with surrounding customer contracts (we want
at least to avoid that our competitors take the code for free and make their own business without
even asking us).

Interested parties don't have to pay much money, as we are mostly interested in information about
everybody who uses those additional features. So don't hesitate to contact <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
for registration to get access to the <b>EXT</b> part of \isoaglib.

The following areas are examples for such extensions:
    - tractor data level 2 + 3 server
        - <i>iTracLight_c</i> for lightning information and control
        - <i>iTracMoveSetpoint_c</i> for control of tractor moving parameters by implement
        - <i>iTracPtoSetpoint_c</i> for control of PTO settings by implement
        - <i>iTracAux_c</i> for management of axiliary valves of tractor (control and information )
    - additional features for virtual terminal access like multi language framework (simple definition of variable language depend parts of the mask pool)


\section Authors Authors and Contributors

	- Main Author: <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> designed
			the complete framework
	- Virtual Terminal Expert: <a href="mailto:M.Wodok@osb-ag:de">Martin Wodok</a> implemented
			the tool <b>vt2iso</b> and the virtual terminal handling
	- Pioneer Win32 User and <b>First External Contributor</b>: <a href="mailto:Brad.Cox@agcocorp:com">Brad Cox</a> who helped to find several optimization needs for documentation,
			tutorial and project generation support. The resulting documentation helped him finally to implement all missing VT Object types, which
			weren't needed for the <b>MiniVeg N</b> project. He is already using \isoaglib for some interesting test applications, and
			contributed all missing VT Objects, so that \isoaglib imeplements also VT Objects like Macro and AuxiliaryInput.
			Some other extensions from him are moveChildLocation and setOriginSKM functions for VT Objects, where appropriate.
			Last but not least, he started with us to integrate multi language support.

\subsection Acknowledgements Acknowledgements
\anchor InitialFundingDfg <b>Thanks to DFG funded research group IKB Duernast</b><br/>
The initial creation of the \isoaglib was enabled by the research project
<b>&quot;IKB D&uuml;rnast&quot;</b> which is leaded by <b>Prof. Auernhammer</b> at the <a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology
            - Crop Production Engineering</a> .

\anchor InitialSupport <b>Thanks to development by Sensortechnik Wiedemann (STW)</b><br/>
The development phase with some compiler issues was supported by the experts of the
<b>ESX</b> engineering team at <a href="http://www.sensor-technik.de">Sensortechnik Wiedemann (STW )</a> .
Some of the main design goals were derived during a project with <a href="http://www.sensor-technik.de">STW</a>
and <a href="http://www.fendt.com/">AGCO Fendt</a> .

\anchor IsoVtExtension <b>Thanks to Fritzmeier for contribution of ISO 11783 Virtual Terminal support</b><br/>
The development of the central control unit for the chlorophyll sensor
<a href="http://www.fritzmeier.de/engl/environment/environment_miniveg.htm">MiniVeg N</a> was heavily based on the prior work at \isoaglib,
so that Fritzmeier accepted to fund the development of ISO 11783 virtual terminal support in \isoaglib .

\anchor FirstWin32Users <b>Thanks to eager and patient initial developers with Win32</b><br/>
As the \isoaglib is mainly developed on LINUX, all Win32 specific adaptations
including driver mappings and project file generation for Win32 IDEs weren't optimal until
first users like <b>Brad Cox</b> started to try \isoaglib in combination with
<b>Microsoft Visual Studio C++</b> . As a result, \isoaglib:
-  provides now an automatic project file generation for Microsoft Visual Studio VC++ version 6.0 (DSP file ),
     with the requirement of installation of STLport, if version 7.0 and above isn't used
- Open Source Win32 IDE "Dev-C++" is used for Win32 compile of vt2iso, as there the several required utils can be installed a lot easier.


*/
