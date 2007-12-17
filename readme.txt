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
        <li> \ref UsingProjects </li>
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
\isoaglib provides a free portable C++ source library for development
of <a id="ext" href="http://www.isobus.com/">ISO 11783 (ISOBUS)</a>
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

For previous changes see the \ref PageOldNews or view the complete \ref PageChanges.


\section MainFeatures Main Features
The \isoaglib provides the following features to facilitate the development of ISO 11783 applications (see \ref InfGeneralInformation for more information):
- A <strong>complete Open Source toolchain for ISO 11783 development</strong>
- <strong>Process Data Communication</strong>: Projects which use features like Process Data Communication based
  on \isoaglib will probably have fewer compatibility problems than other projects. 
  This gets more important the more implements, sensors, task controllers, core control units 
  (e.g. expert systems) and other devices are integrated.  The result is increasingly complex interaction networks
  with all sorts of dependencies. Here \isoaglib can provide common patterns for the management of such interactions.
  This list can help to identify well supported device combinations.
- <strong>A Virtual Terminal implementation</strong>: The \isoaglib Virtual Terminal design has the following goals:
  - Enable creation of one single mask pool, that fits all terminal configurations
  - Provide attributes for project specification of runtime adaptations
  - Allow for flexible addition of further attributes. This is useful where automatic layout control must be optimized for some terminal properties (e.g. the number of softkeys)
        All these actions are demonstrated in the tutorial example 3_0_VirtualTerminalIso.cpp on the <a href="$relpath$examples.html">tutorial examples page</a>.
- <strong>XML notation for mask definition</strong>:
  - XML mask files are converted to ROM-enabled variable arrays
    for inclusion in a project object module.
  - \isoaglib provides a simple function call to
    register the mask pool.
  - \isoaglib performs some runtime adaptations to terminal properties, e.g. choice of usable colour depth of bitmaps,
    scaling (including handling of cases where layout properties like text alignment should be conserved even if no font of scaled size
    is available)
  - process completion using an automatic upload of the adapted pool.
  - Active pools can be controled with simple access functions, and some simple handler functions are available for reaction on user input.
- <strong>Multitasking</strong>: Perform all tasks which can be automated in the background to simplify
  application development and avoid different interpretations of low level communication (like message formatting).
- A <strong>flexible and capable data implementation</strong> as the backbone for all documenting and control interactions (e.g. trigger dependent measurement value sends, MIN / MAX setpoint intervals, enable N --&gt; 1 relation between user and provider of process data)
- Use of a <strong>Hardware Adaption Layer</strong> to minimize target adaptation to a small set of files.
  The majority of \isoaglib source code can be used without any changes for different platforms 
- <strong>Project specific feature selection</strong>
- Source code maintenance and extension with enforced <strong>strict modular design</strong>
- Design orientated to the requirements of networks with more then two devices

<!--Brad Cox developed a tool for his company, that is able to convert the project files of the WTK mask generator to the XML definition. Please contact Achim Spangler if you are interested in this tool for your own use, as Brad Cox indicated already, that he might be willing to provide this undest some conditions.
But this tool would only help to start the virtual mask definition with XML, as this method provides some additional attributes to control the automatic runtime adaptation of the single core mask pool to the different terminal properties (e.g. font size dependent spacing).
\section wtk2xml XML Mask Generator
  <a href="mailto:Brad.Cox@agcocorp:com">Brad Cox</a> developed a tool
  for his company that is able to convert the project files of the WTK
  mask generator to the XML definition.  Please contact
  <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> if you
  are interested in this tool for your own use, as Brad Cox indicated
  that he might be willing to provide this under some conditions.

  Note that this tool helps to start the virtual mask definition
  with XML, as the method provides some additional attributes to
  control the automatic runtime adaptation of the single core mask
  pool to the different terminal properties
  (e.g. font size dependent spacing).

\section brad_cox Brad Cox: Pioneer Win32 User and First External Contributor
  <a href="mailto:Brad.Cox@agcocorp:com">Brad Cox</a> helped to find
  several optimization needs for documentation, tutorial and project
  generation support.  The resulting documentation helped him to
  implement all missing VT Object types, even those which weren't needed
  for his <b>MiniVeg N</b> project.  Thanks to him, the
  ISO<i><sub>AgLib</sub></i> also implements VT Objects like Macro and
  AuxiliaryInput.

  As the ISO<i><sub>AgLib</sub></i> is mainly developed on Linux,
  all Win32 specific adaptations including driver mappings and project
  file generation for Win32 IDEs were not initially optimal.
  Brad Cox's efforts to try ISO<i><sub>AgLib</sub></i> in
  combination with <b>Microsoft Visual Studio C++</b> resulted in
  the following improvements to ISO<i><sub>AgLib</sub></i>:
  - it now provides automatic project file generation for Microsoft Visual
    Studio VC++ version 6.0 (DSP file), with the requirement of installation
    of STLport, if version 7.0 and above isn't used
  - the Open Source Win32 IDE "Dev-C++" is used for the Win32 compile
    of vt2iso, as there the several required utils can be installed a
    lot easier.

  Brad is already using ISO<i><sub>AgLib</sub></i> for some interesting
  test applications,

  Some other extensions that he has done are moveChildLocation and setOriginSKM
  functions for VT Objects.  He also got us started with the integration
  of multi-language support.
-->

\section StructuralOverview Structural Overview

\subsection StructureModularity Modular Design
The \isoaglib is modularized according to the different
ISO11783 functional components.  Only a small number of modules is
necessary for all systems, so that an individual project can adapt
capabilities and Flash ROM size to its specific needs.

\subsection StructureLayers Layered Design
The \isoaglib is designed to be easily adapted to new
hardware platforms.  The majority of the software can be used without any
changes on all platforms.  The more this common software part is used,
the better its quality will get.

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
    <area shape="rect" coords="228, 38,318, 94" href="DataLinkPage.html" alt="Part 3: Data Link" title="Part 3: Data Link">
    <area shape="rect" coords="341, 38,433, 94" href="NetworkMgmtPage.html" alt="Part 5: Network Management" title="Part 5: Network Management">    
    <area shape="rect" coords="455, 38,526, 94" href="XMLspec.html" alt="Part 6: ISO Virtual Terminal" title="Part 6: ISO Virtual Terminal">
    <area shape="rect" coords="548. 63,613, 94" href="AppLayerPage.html" alt="Part 7: Application Layer" title="Part 7: Application Layer">
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
        base            [label="Base Data\n(complete optional)", color="white", URL="\ref AppLayerPage", width=1.5 ];
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

\subsection StructObligatory Information on Mandatory Elements
The following elements are needed for all projects:
 - blue "HAL" layer nodes
 - yellow "Driver Extensions" layer nodes
 - cyan "Communication" layer nodes

The modularized principle ensures that the minimum footprint of the
\isoaglib in Flash ROM is quite low.

\subsection StructPartlyOptional Other Elements

The supplementary drivers for RS232, Sensor Input and Actuator Control
(mainly PWM) were developed for a research project at the Technical
University of Munich (TUM).


\section UsingProjects Customer Examples
As the used license <i>GPL with exception</i> requires from each user to accompany any distributed Program
( or a work based on it - see GPL &sect;3) with some information on the used library, it would be quite kind to
inform additionally the authors of your project, so that each using project can be listed here.
This list can also serve as a prominent place for showing all contributing (by money and/or code) users.<br>

Last but not least, projects which use features like process data communication based on \isoaglib will
probably have less compatibility problems than in other combinations. This gets the more important, the more implements,
sensors, task controllers, core control units (expert system) and tractor builds more and more complex
interaction networks with all sorts of dependencies. Here \isoaglib can provide common patterns for the
management of such interactions. This list can then help to identify well supported device combinations.<br>

The following table contains a few commercial products which use \isoaglib:
<table class="download" border=0 cellspacing=0>
<tr><th>Company</th>
    <th>Used parts</th>
    <th>Description</th></tr>
<tr><td valign="top" align="center" width="80"><a id="ext" href="http://www.agocorp.com"><i>Agco Corp.</i></a></td>
    <td valign="top"><i><ul><li>Virtual Terminal (11783-6)</li>
            <li>TC (11783-10)</li>
            <li>GPS converter</li></ul></i></td>
    <td><i>Based on \isoaglib the development team of <a id="ext" href="http://www.osb-ag.de">OSB AG</a> implemented the ISO VT Server (11783-6) on the Terminal Opus A1 from Wachendorff.
        The operating system is a Linux platform and the OSB AG team managed to develop a running prototype within three months.
        At the Agritechnica 2005 a running ISO Terminal was presented.<br>
        After the Agritechnica 2005, the Task Controller Server (11783-10) and GPS Cinverter were integrated into the system. Which made operating data logging and transferring GPS information from RS232 to the ISOBUS in NMEA 2000 format possible.
        This is the next step of operating data logging; now the farmer has the possibility to synchronize his field recorder with relevant GPS data.</i></td></tr>
<tr><td valign="top" align="center"><a id="ext" href="http://www.fritzmeier.de"><i>Fritzmeier Umwelttechnik GmbH</i></a></td>
    <td valign="top"><i><ul><li>Control system of the MiniVegN</li></ul></i></td>
    <td><i>Within this project the \isoaglib was the base for the development of a control system between MiniVegN, tractor and fertilize spreader. 
        The laser of the MiniVegN tests the nitrogen content of the plant and sends the information to the spreader. The spreader has to spread out the right amount of fertilizer. 
        With the \isoaglib the MiniVegN is compatible to the standard ISO 11783.</i></td></tr>
<tr><td valign="top" align="center"><a id="ext" href="http://www.zunhammer.de"><i>Zunhammer GmbH</i></a></td>
    <td valign="top"><i><ul><li>User Interface</li><li>ISO control system</li></ul></i></td>
    <td><i>"The challenge was to bring Zunhammer liquid manure spreader to ISO compatibility within four weeks.
        The control of the spreader had to be done by ISO terminal.
        The open source library \isoaglib was the right instrument and together with the Zunhammer development team the we managed to build a running prototype punctual to the Agritechnica 2005.
        The time frame of four weeks was achived."</i>
</td></tr>
</table>


The \isoaglib will be actively maintained by <a id="ext" href="mailto:Achim.Spangler_at_osb-ag:de">Achim Spangler</a> at <a id="ext" href="http://www.osb-ag.de">OSB AG</a>
as long as the invested time is affordable in relation to corresponding projects and support contracts.<br>
Even in case the active maintenance by <a id="ext" href="http://www.osb-ag.de">OSB AG</a> might be stopped some day, the GPL license and
the independent website at the
<a id="ext" href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology - Crop Production Engineering</a>
assure that any interested party can step in and continue the maintenance of the project. This guarantees, like in other Open Source projects, the
open access for every user. <br>
Also, in case the major part of the user community is unsatisfied with the maintenance by <a id="ext" href="http://www.osb-ag.de">OSB AG</a>, it is
normal for Open Source projects like \isoaglib to fork the project as a last resort, if <a id="ext" href="http://www.osb-ag.de">OSB AG</a>
can't or doesn't want to change the style of maintenance as requested. As far as possible, <a id="ext" href="http://www.osb-ag.de">OSB AG</a>
will do everything to avoid such a fork.<br>
A comparable fork was performed by users and developers of the X11R6 server project for UNIX style operating systems - <b>XFree86</b>.
Caused by some licensing issues, a fork of the <a id="ext" href="http://www.xfree86.org/">XFree86</a> was created by the <a id="ext" href="http://X.Org">X.Org Foundation</a>,
which also appreciated by several graphic card manufacturers, as they can integrate their drivers with the new management method in a better way.
<br>
&nbsp;<br>
&nbsp;<br>
<b>
This way it is assured under all conditions, that the development time and money, which is invested in an application that uses the \isoaglib
can't be affected by the style and quality of future project maintenance.</b>


\section Maintainers Maintainers
The \isoaglib was initially created by <a id="ext" href="mailto:Achim.Spangler_at_osb-ag:de">Achim Spangler</a> who is now
working for the company <b><a id="ext" href="http://www.osb-ag.de">OSB AG</a></b>. This company started business at the beginning of 2003, and has
already more than 160 engineers working in the five locations Munich, Stuttgart, Krefeld, Hamburg, Frankfurt, Köln and Berlin (all in Germany; <b>state November 2007</b>).<br>
The main business focus is project support at the customer location in software, electronic and mechanical engineering.<br>
Some of the <b><a id="ext" href="http://www.osb-ag.de">OSB AG</a></b> customers are: 
<a id="ext" href="http://www.agcocorp.com/">AGCO GmbH/<a id="ext" href="http://www.fendt.com/">Fendt</a>,<!-->AL.systems GmbH,-->
<a id="ext" href="http://www.aoa-gauting.de/">Apparatebau Gauting GmbH</a>,<!--<a id="ext" href="http://www.machines.averydennison.com/">Avery Dennison Deutschland GmbH</a>,
AZH GmbH,-->
<a id="ext" href="http://www.benqmobile.com">BenQ Mobile GmbH & Co OHG</a>,
<a id="ext" href="http://www.emag.de/">EMAG GmbH</a>,
<a id="ext" href="http://www.fiducia.de/">FIDUCIA IT AG</a>,
<a id="ext" href="http://www.fritzmeier.de/">FRITZMEIER Systems GmbH & Co.</a>,
<a id="ext" href="http://www.infineon.com/">Infineon Technologies AG</a>,
<a id="ext" href="http://www.mtu.de">MTU Aero Engines</a>,
<a id="ext" href="http://www.kvb.de/">Kassenärztliche Vereinigung Bayerns</a>,<!--<a id="ext" href="http://www.oce.de">Océ Printing Systems GmbH</a>, Panasonic Automotive Systems Europe GmbH,-->
<a id="ext" href="http://www.porsche.com/usa/">Dr. Ing. h.c.F. Porsche AG</a>,
<a id="ext" href="http://www.siemens.com">Siemens AG</a>,
<a id="ext" href="http://www.thyssenkrupp-technologies.de">ThyssenKrupp AutomotiveMechatronics GmbH</a>,
<a id="ext" href="http://www.valeo.de">Valeo GmbH</a>,
<a id="ext" href="http://bosch.com">Robert Bosch GmbH</a>,<!--<a id="ext" href="http://www.imis.com">Joh. Winklhofer & Söhne GmbH & Co. KG</a>,-->
<a id="ext" href="http://www.ivm-automotive.com/">IVM Automotive München Gmb</a>

More information on commercial backing of \isoaglib can be obtained in the \ref GeneralSupportInformation and the \ref PageContactAndSupport section.


\subsection MaintainersExtensions Exclusive feature area for new customers and researchers
Even if \isoaglib provides already most of the features, a agricultural device
may need, there are still some functions, that are missing or that could ease application
development. But their implementation can only be done, when people of the user community
do at least part of the work, some sponsoring companies give OSB some money to do this,
or if OSB has the option to refinance the work with surrounding customer contracts (we want
at least to avoid that our competitors take the code for free and make their own business without
even asking us).

Interested parties don't have to pay much money, as we are mostly interested in information about
everybody who uses those additional features. So don't hesitate to contact <a id="ext" href="mailto:Achim.Spangler_at_osb-ag:de">Achim Spangler</a>
for registration to get access to the <b>EXT</b> part of \isoaglib.

The following areas are examples for such extensions:
    - tractor data level 2 + 3 server
        - <i>iTracLight_c</i> for lightning information and control
        - <i>iTracMoveSetpoint_c</i> for control of tractor moving parameters by implement
        - <i>iTracPtoSetpoint_c</i> for control of PTO settings by implement
        - <i>iTracAux_c</i> for management of axiliary valves of tractor (control and information)
    - additional features for virtual terminal access like multi language framework (simple definition of variable language depend parts of the mask pool)


\section Authors Authors and Contributors
- Main Author: <a id="ext" href="mailto:Achim.Spangler_at_osb-ag:de">Achim Spangler</a> designed the complete framework
- Virtual Terminal Expert: <a id="ext" href="mailto:M.Wodok_at_osb-ag:de">Martin Wodok</a> implemented the tool <b>vt2iso</b> and the virtual terminal handling
- Pioneer Win32 User and <b>First External Contributor</b>: <a id="ext" href="mailto:Brad.Cox_at_agcocorp:com">Brad Cox</a> who helped to find several optimization needs for documentation, tutorial and project generation support. The resulting documentation helped him finally to implement all missing VT Object types, which weren't needed for the <b>MiniVeg N</b> project. He is already using \isoaglib for some interesting test applications, and contributed all missing VT Objects, so that \isoaglib imeplements also VT Objects like Macro and AuxiliaryInput.
  Some other extensions from him are moveChildLocation and setOriginSKM functions for VT Objects, where appropriate.
  Last but not least, he started with us to integrate multi language support.

\subsection Acknowledgements Acknowledgements
- Thanks to the <strong>DFG</strong>, which finances the partial project 2 of the research group &quot;IKB-D&uuml;rnast&quot;.
- Thanks to the <strong>research group of Prof. H. Auernhammer</strong> and to <a id="ext" href="mailto:m.wodok_at_osb-ag:de">Martin Wodok</a>, who developed LBS_Terminal.
- \anchor InitialFundingDfg <strong>Thanks to DFG funded research group IKB Duernast</strong><br/>
  The initial creation of the \isoaglib was enabled by the research project
  <b>&quot;IKB D&uuml;rnast&quot;</b> which is leaded by <b>Prof. Auernhammer</b> at the <a id="ext" href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and 
  Land Use Technology - Crop Production Engineering</a>.
- \anchor InitialSupport <strong>Thanks to development by Sensortechnik Wiedemann (STW)</strong><br/>
  The development phase with some compiler issues was supported by the experts of the <b>ESX</b> engineering team at <a id="ext" href="http://www.sensor-technik.de">Sensortechnik Wiedemann (STW)</a>.
  Some of the main design goals were derived during a project with <a id="ext" href="http://www.sensor-technik.de">STW</a>
  and <a id="ext" href="http://www.fendt.com/">AGCO Fendt</a> .
- \anchor IsoVtExtension <strong>Thanks to Fritzmeier for contribution of ISO 11783 Virtual Terminal support</strong><br/>
  The development of the central control unit for the chlorophyll sensor
  <a id="ext" href="http://www.fritzmeier.de/engl/environment/environment_miniveg.htm">MiniVeg N</a> was heavily based on the prior work at \isoaglib,
  so that Fritzmeier accepted to fund the development of ISO 11783 virtual terminal support in \isoaglib .
- \anchor FirstWin32Users <strong>Thanks to eager and patient initial developers with Win32</strong><br/>
  As the \isoaglib is mainly developed on LINUX, all Win32 specific adaptations
  including driver mappings and project file generation for Win32 IDEs weren't optimal until
  first users like <b>Brad Cox</b> started to try \isoaglib in combination with <i>Microsoft Visual Studio C++</i> . 
  As a result, \isoaglib:
  - provides now an automatic project file generation for Microsoft Visual Studio VC++ version 6.0 (DSP file),
    with the requirement of installation of STLport, if version 7.0 and above isn't used
  - Open Source Win32 IDE "Dev-C++" is used for Win32 compile of vt2iso, as there the several required utils can be installed a lot easier.

*/
