/*! \mainpage Overview on Documentation of IsoAgLib

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
<tr><th>Overview</th></tr>
<tr><td>The following parts are described in this section:
    <ul><li> \ref Home
    </li><li>\ref LatestNews
    </li><li>\ref Acknowledgements
    </li><li>\ref Authors
    </li></ul>
</td></tr>
</table>
<br/>
<table class="infobox" border="0">
<tr><th>See also</th></tr>
<tr><td>
    <ul><li> \ref Home
    </li><li>\ref LatestNews
    </li><li>\ref Acknowledgements
    </li><li>\ref Authors
    </li></ul>
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

\section Authors Authors and Contributors of the IsoAgLib

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