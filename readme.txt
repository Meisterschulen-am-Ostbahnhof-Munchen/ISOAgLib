/*! \mainpage Overview on Documentation of ISO<i><sub>AgLib</sub></i>

<center>
<h1>Object Oriented Program Library ISO<i><sub>AgLib</sub></i></h1>
<h1>Development of ISO 11783 and DIN 9684 Applications</h1>
<h2>Copyright &copy; 1999 - 2004 Achim Spangler, Martin Wodok</h2>
<h2>Licensed with Exceptions under the General Public License (GPL) of the Free Software Foundation</h2>
</center>

<table width="100%" border="0" cellspacing="0" cellpadding="2">
<tr><td valign="Top" nowrap="true" >
\ref IndexWhereToStart<br>
\ref IndexNews<br>
\ref IndexChanges<br>
\ref IndexMainFeatures<br>
\ref IndexMemoryUsage<br>
\ref IndexStructuralOverview<br>
\ref IndexLicenseConditions<br>
\ref IndexAchnowledgements<br>
\ref IndexInitialSupport<br>
\ref IndexIsoVtExtension<br>
\ref IndexFirstWin32Users<br>
\ref IndexAuthors<br>
\ref IndexMaintainingCompany<br>
\ref IndexSupport<br>
\ref IndexTodo<br>
\ref IndexUsingProjects<br>
\ref HowtoDownload<br>
\ref HowtoAccessRepository<br>
\ref HowtoGetHelp<br>
\ref HowtoCreateDoc<br>
\ref HowtoLearnIsoAgLib<br>
\ref HowtoSetupCompilerTools<br>
\ref InstallIsoToolVt2Iso<br>
\ref HowtoCreateProjectFiles<br>
\ref InfGeneralInformation<br>
\ref InfAgentNetwork<br>
\ref InfServiceNetwork<br>
\ref InfRequirements<br>
\ref FurtherDevelopment<br>
\ref FurtherExamples<br>
\ref QuestionsAnswers<br>
\ref FurtherReading<br>
<br>
&nbsp;<br>
&nbsp;<br>
&nbsp;<br>
Last Update: <br>06 December 2004<br>
by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
<br></td>
<td valign="Top">
&nbsp;<br>
\section IndexWhereToStart Where to Start Learning IsoAgLib
\subsection IndexGettingOverview Where to Get an Overview
The links in the navigation column present you the following information:
	- Get short information on ISO<i><sub>AgLib</sub></i> and its structural design at \ref IndexMainFeatures and \ref IndexStructuralOverview
	- Get some insight on the design goals of the complete system at \ref InfGeneralInformation , \ref InfAgentNetwork and \ref InfServiceNetwork
	- Check ease of application development in an example for virtual terminal handling at 3_0_VirtualTerminalIso.cpp
			( other examples at <a href="examples.html">tutorial examples</a> page )

\subsection IndexStartToLearn Where to Start Learning the IsoAgLib
Please have a close look at all HOWTO chapters which are presented in the navigation column.
	- read at least some of the <a href="examples.html">tutorial examples</a> which are organized in groups ( first number ) and read some of the referenced further documentation:
		- Nr. 0 for simple applications which demonstrate management of ECU as device node at the network
		- Nr. 1 for simple applications which demonstrate usage of base data like tractor speed and distance
		- Nr. 2 for several levels of applications which demonstrate use of process data value exchange ( measurement data for monitoring and setpoint setting for control )
		- Nr. 3 for two examples of terminal handling: LBS+ style and virtual terminal style
	- check for modules which you should download to get the features you want at \ref HowtoDownload
	- check for pointers at documentation overview pages of the core features of ISO<i><sub>AgLib</sub></i> at \ref HowtoLearnIsoAgLib
	- check for installation instructions of vt2iso , a tool to convert XML mask definition to ROM-eable constant variable definition modules at \ref InstallIsoToolVt2Iso
	- check for instructions to create IDE project files for applications at \ref HowtoCreateProjectFiles ( the presented shell script does the very important and
		rather complicated job of selecting the needed source files for the wanted features of ISO<i><sub>AgLib</sub></i> )

\subsection IndexFurtherRead Further Documentation
Please read at least on the long run most of the pages which are direct referenced by the navigation column on this first page.
The overview pages referenced by \ref HowtoLearnIsoAgLib present several example snipptes for the classes which correspond to the described
feature group. The most important classes have also some detailed information on their features and usage.<br>
<b>Important:</b><br>
All elements with names without "i" at the beginning and which are grouped to the namespaces __IsoAgLib and __HAL are only interesting for experts who want to learn the internals of ISO<i><sub>AgLib</sub></i>.
But please be warned, that this stuff seems rather complicated if you are not familiar with object oriented design. You can be assured, that not only the authors of
the architecture accept the high quality of design. Some parts might look strange at the first sight - but then you will detect that such elements are very helpful
for reduction of code duplication and ease of code maintenance. One example is the Singleton Pattern, which is used for global, but well defined to core service classes
like ISOTerminal_c . The use of this Template guarantees that event the first access is always performed at a correct initialised instance. Even if the initialisation of a core
service class depends of the previous initialisation of one or more other classes, the correct order of initialisation is realized.<br>
Please contact the maintainers <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> and <a href="mailto:Martin.Wodok@osb-ag:de">Martin Wodok</a> if you don't understand
some parts, even after reading the presented documentation.

\subsection IndexGettingHelp Getting Help
Please send any questions you can really not solve based on the present documentation to the <a href="http://linux90.idvnet.de/pipermail/isoaglib/">email list of ISO<i><sub>AgLib</sub></i> </a> ,
as there not only the maintainers can try to help you out. Please <a href="http://linux90.idvnet.de/cgi-bin/mailman/listinfo/isoaglib">subscribe to the email list</a> before you start to
<a href="mailto:isoaglib@linux90.idvnet.de">send emails</a> .



\section IndexNews News

\subsection IndexNewNews News after the 1.0.0 release
	- <b>2004-11-15:</b> Create new email list for email exchange within user community so that users can help each other, and
		can discuss changes there, before somebody starts implementing the change<br>
		- subscribe at <a href="http://linux90.idvnet.de/cgi-bin/mailman/listinfo/isoaglib">http://linux90.idvnet.de/cgi-bin/mailman/listinfo/isoaglib</a>
		- access the archive of all emails at <a href="http://linux90.idvnet.de/pipermail/isoaglib/">http://linux90.idvnet.de/pipermail/isoaglib/</a>
	- <b>2004-11-18:</b> Add creation of <b>Microsoft Visual C++ V6.0</b> project files to update_makefile.sh , as <b>Dev-C++</b> can't build EXE files which are running with
		Vector-Informatik CAN cards
	- <b>2004-11-23:</b>update_makefile.sh runs in MSYS ( Win32 shell environment ) again, as intermediate use of perl commands is stopped
	- <b>2004-11-24:</b> Add driver for <b>Sontheim CAN cards</b> ( tested only with CANLPT, but in theory designed according general API documentation, as all card
		differences are handled by the driver )
	- <b>2004-12-03:</b> Finish heavy optimization of HEAP usage and optional exact documentation of HEAP usage during runtime via RS232, so that really needed HEAPSIZE can be derived
		( reduction of HEAPSIZE dependend on application can vary between 4 KByte and 10 KByte )


\subsection IndexOldNews News before 1.0.0 release
	- Complete Toolchain for ISO 11783 Virtual Terminal is integrated.
	- <a href="http://www.fritzmeier.com/engl/frameset_engl.htm?/engl/environment/environment_miniveg.htm">Chlorophyll Sensor <b>MiniVeg N</b></a> -
	    the first commercial application of <a href="www.fritzmeier.de">Fritzmeier</a>
	    is developed based on ISO<i><sub>AgLib</sub></i>
	- Company <a href="www.osb-ag.de">OSB AG</a> with the author of ISO<i><sub>AgLib</sub></i> is providing commercial
	    service for ISO<i><sub>AgLib</sub></i> applications, project development based on ISO<i><sub>AgLib</sub></i> and other
			embedded services
	- ISO<i><sub>AgLib</sub></i> is still being developed in cooperation with research group of <b>Prof. Auernhammer</b>
	    at <a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology - Crop Production Engineering</a>
	- ISO<i><sub>AgLib</sub></i> is now running also at ECU of <a href="http://www.mitron.fi">Mitron</a>
	- Directory structure and module structure of ISO<i><sub>AgLib</sub></i> is completly reorganized, so that
	    optional and obligatory parts can be detected better, and the corresponding files for
			internal implementation and interface are tighter grouped.
	- Heavy extended documentation with information on every important part of the library
	    with example code lines and some diagrams.
	- <b>Changed license to <i>GPL with extension</i> which derived from the <i>eCos</i> license. This license has an explicit definition of
		allowed usage of ISO<i><sub>AgLib</sub></i> which doesn't cause any license obligations on the using application.
		The original chosen LGPL license would require some inacceptable requirements to commercial applications.</b>
	- Shell script collects all needed source files based on a feature set definition in one small project spec file. Thus
		it is easy to avoid the inclusion of unneeded files. This shell script is also useable in Windows, as the
		Open Source tool <b>MSYS</b> provides a minimum but capable enough <b>POSIX</b> layer for Windows.
	- At least one very active and already contributing user from a well known manufacturer confirms, that ISO<i><sub>AgLib</sub></i>
		allows even the implementation of missing features after some work with it.
	- Each email adress in this project is intentionally corrupted, to make SPAMMING a little bit more complicated. Each human reader will easily detect how to
		"fix" the email adress - this "manipulation" might change if the maintainers detect that SPAM tools are able to derive automatically a
		working domain.



\section IndexChanges Changes
\subsection 010TO020 0.1.0 to 0.2.0

	- optional basic set of <b>ISO 11783</b> features
	- possibility of baud rate auto sensing on power up and on BUS OFF errors (with configurable wait time).
	- partitioning of sources in different packages. A base package contains system management, base tractor information
			and CAN driver. All optional modules like process data or EEPROM driver can now
			be downloaded as separate source archives.
	- ISO<i><sub>AgLib</sub></i> specific CAN driver module to ease hardware adoption
	- collection of all files which must be edited for hardware adoption to one directory and reduction of the amount
			of affected files

\subsection 020TO021 0.2.0 to 0.2.1

	- corrected some small errors of synchronisation between working 0.1.0 tree and the new 0.2.0
	- fixed bug in process data list management

\subsection 021TO100rc1 0.2.1 to 1.0.0rc1

	- reorganization of source files
	- ISO 11783 at larger extend:
	    - working set messages
			- virtual terminal
			- process data state of Spring 2003
			- some bugs of first ISO approach fixed
	- extended documentation with several examples and diagrams
	- global access to central service classes to avoid complicated access through several steps
	- reduced usage of heap memory
	- reorganized error handling
	- better integration in time schedule of application, as main function for periodic actions of ISO<i><sub>AgLib</sub></i> allows
	    now to define a final time of return, where ISO<i><sub>AgLib</sub></i> returns at latest to the application
	- smarter structure of namespaces
	- several variants of process data, to restrict resource usage to needed level by selection of process data
	    type, which provides exactly the needed features
	- simple integration of process data and local identity variables within the program scope -> better control of memory and activation

\subsection 100rc1TO100rc2 1.0.0rc1 to 1.0.0rc2

	- added several features to the project generation script to create project
		files for the Open Source Win32 IDE "Dev-C++" and for the Tasking EDE
	- enhanced documentation ( esp. Getting Started Guide, extended HTML links in examples
		to demonstrated features )
	- renamed all usages of "Announce" to the more standard conformant "Address Claim" and
		change "DPlus" to "CanEn" to make ISO<i><sub>AgLib</sub></i> terms easier to understand
	- enhanced overall integration into Win32 ( thanks to the help of Brad Cox )
	- integrated the virtual terminal mask handling tool <b>vt2iso</b> to Win32 build
		with Dev-C++ ( added alternative image manipulation library usage for <b>Free Image</b> )

\subsection 100rc2TO100rc3 1.0.0rc2 to 1.0.0rc3

	- added support for Vector Informatik CAN driver libraries <b>CANLIB</b> and <b>XL Driver Library</b>
		( integrate it in the project generation script, so that a Dev-C++ project can be simply created )
	- made management of Master Working Set state of a local IdentItem_c ( representing of local ISO 11783
		node in the library ) more robust against SA and Device Type conflicts
	- enhanced some documentation
	- fixed some small problems in interaction with CANoe virtual terminal ( thanks to Brad Cox )

\subsection 100rc3TO100rc4 1.0.0rc3 to 1.0.0rc4

	- reordered main documentation blocks
	- changed device type of virtual terminal example to tillage so that
		conflicts with the tractor instance of CANoe are avoided

\subsection 100rc4TO100rc5 1.0.0rc4 to 1.0.0rc5

	- added trigger request for claimed address as soon as local Ident is initialized as ISO 11783
		( avoid <DeviceType, DeviceTypeInst> clash in IsoMonitor_c )
	- implemented GPS decoding for ISO 11783 based on open accessible sources ( thanks to google )
		( please test and send changes, in case a reference system reveals wrong decoding;
			decoding of Longituge and Latitude are most probably correct, others?? )
			- CAN ID 0x00FFF3LU with GPS_STATE_PGN -> probably the GPS mode is in BYTE1???
			- CAN ID 0x00FEF3LU with GPS_LATITUDE_LONGITUDE_PGN -> a google'd example shows positioning and unit of latitude and longitude
			- CAN ID 0x00FEE8LU with GPS_SPEED_HEADING_ALTITUDE_PGN -> probably last 4 Bytes contain altitude
	- extended definition possibilities for application sources in project building script <b>update_makefile.sh</b>
	- fixed bug in vt2iso.cpp which prevented it creating bitmap arrays for the 480 resolution
	- fixed bug in FreeImage library handling, which places Y scanlines upsidedown in relation to
		"usual" graphic algorithms -> thus the bitmaps were flipped vertically

\subsection 100rc5TO100 1.0.0rc5 to 1.0.0

	- implemented all virtual terminal object types, which are defined in ISO 11783 ( thanks to Brad Cox )
	- implemented simple access function for easy change control of Vt-Object attributes like colour

\subsection 100TO101 1.0.0 to 1.0.1

	- now using specific LAST CAN MESSAGE OBJECT of CAN controllers to store CAN messages
		during CAN reconfiguration process for later process ( avoid loss of messages )<br>
		this is important to receive the very early triggered <b>ISO 11783</b> command <b>Request for Claimed Addresses</b>
	- as Vector-Informatik CAN CARD driver can't be used with EXE files, which are built with IDE "Dev-C++",
		the script update_makefile.sh was extended to create also Microsoft Visual Studio VC++ version 6.0 DSP files;
		This needs the installation of STLport if VC6 is used, see \ref Vc6StlPort for more information.
	- provided automatic CAN CARD HW selection for Vector-Informatik CAN CARDs so that:
		- either all CAN channels of real CAN cards can be counted
		- or just the CAN channels of the special virtual CAN device can be counted ( if no real CAN card is connected )
	- add driver for <b>Sontheim CAN cards</b> ( tested only with CANLPT, but in theory designed according general API documentation, as all card
		differences are handled by the driver )
	- adopted the check for new messages in CAN CARD to RTE style
	- added basic language handling and AUX functions handling into ISO Terminal
	- added <b>ISO 11783 Virtual Terminal</b> connection management, so that the pool is automatically
		reloaded on reconnection ( changed terminal properties are regarded )
	- done several cleanups and additions to <b>ISO 11783 Virtual Terminal</b>
	- enhance documentation for feature setup files for individual projects and reorganize the existing files, to make them easier to understand
	- enable HTTPS access to the subversion repository, as some proxies might hinder WebDAV access, which is needed by Subversion/Apache2<br>
		so please try HTTPS access in case you have problems with accessing the repository, before you contact <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
	- some cleanup of workarounds which are needed due to Tasking compiler faults - the STL of versions up to 7.56 have wrong includes
		( see file "include.cpp/algorithm" which includes "include.cpp/stl_algobase.h" which includes "string.h" into global namespace;
		  a C++ system header shall include C-Library functions into the "std::" namespace -> it should include <string> instead;
			other compilers do this correct )
	- introduce config setting for project config to select HEAP allocation strategy:
		- either allocate chunks of items ( for Tasking: 40 items per chunk ) to avoid HEAP defragmentation and to avoid memory allocation runtime as long as no new chunk is
			needed, which doesn't happen for most use cases, as the chunk size is pretty big in relation to the awaited numbers of mostly smaller than 10
		- or directly call malloc on each item creation, which causes greater overhead per item ( linkage of all malloced blocks in HEAP, whereas the chunks have a
			pretty efficient organization structure ) and which can cause longer malloc execution time on each item creation ( can be important, if large amount
			of monitor list entries, several measurement programs, setpoints occure, or if CAN filter setting must be reconfigured due to configuration change )
		- <b>PREFER CHUNK ALLOCATION AS LONG AS HEAP OVERHEAD IS ACCEPTIBLE, USE OTHER METHOD ONLY FOR RESTRICTED HEAP MEM CONDITIONS</b>
	- avoid allocation of HEAP for elements of temporal ManageMeasureProgLocal_c instances ( create default MeasureProgLocal_c instances on first real access )
	- optimize HEAP memory item size debug messages, so that real HEAP usage is reflected ( including overhead due to memory block linking in HEAP )<br>
			Thanks to the help of Tasking support ( information on simple integration of _malloc.c into EDE projects )
	- change update_makefile.sh shell script to avoid calls to <b>perl</b> functions, as these are only part of standard LINUX systems, and are <b>not</b>
		part of <b>MSYS</b> system ; additionally replace UNIX style directory seperation from slash to backslash for Win32 compilers and use Windows stly newline
	- change device types and CAN channels of tutorial examples, so that e.g. 1_0_ReadIso.cpp and 1_2_WriteIso.cpp can interact ( <b>a)</b> different device types
		for both ECUs and <b>b)</b> different CAN channels so that two channels of one PC CAN card can be connected )
	- unify number intervals of CAN HAL for BUS and MsgObj numbers to start with <b>0</b> for each type of HAL ( avoid HAL dependend offset constants ).



\section IndexMainFeatures Main Features
The ISO<i><sub>AgLib</sub></i> is designed to provide the following main features
(read <a href="information.html#general_info">here</a> for more information):<img width="60%" src="Diagramme/IsoAgLib-mixed-protocols.gif" align="Right">
- perform all tasks which can be automated in the background so that the application development is simplified
		and different interpretation of low level communication (like message formatting) is avoided
- provide <b>complete Open Source toolchain for ISO 11783 development - including virtual terminal</b> <br>
	Masks are defined with XML notation, which is then converted to ROM-eable variable arrays which are then
	included in an object module of the project. ISO<i><sub>AgLib</sub></i> provides a simple function call
	to register the mask pool. ISO<i><sub>AgLib</sub></i> performs then some runtime adoption to terminal properties
	( choose useable colour depth of bitmaps, scaling, including handling of cases where
	layout properties like text alignment should be conserved even if no font of scaled size is available ) and finishes this process
	by an automatic upload of the adopted pool. Already active pools can be handled with simple access functions for control,
	and some easy handler functions for reaction on user input.<br>
	All these actions are demonstrated in the tutorial example \ref 3_0_VirtualTerminalIso.cpp .
- provide flexible and capable process data implementation as backbone for all documenting and control interaction
		(e.g. trigger dependent measurement value send, allow setpoint intervals with MIN / MAX,
		enable N --&gt; 1 relation between user and provider of process data)
- narrow hardware adoption afford to small set of files with hardware adoption layer
- greatest part of ISO<i><sub>AgLib</sub></i> source code can be used without any changes for different platforms
- enable project specific feature selection, source code maintenance and extension by strict modular design
- facilitate development of application for both DIN 9684 and ISO 11783
- orientate design according to requirements of networks with more then two devices

\section IndexMemoryUsage Information on Memory Resource Requirements
The ISO<i><sub>AgLib</sub></i> was optimized in relation to the old LBS<i><sub>Lib</sub></i> to use as less HEAP
memory as possible. Thus the current ISO<i><sub>AgLib</sub></i> uses HEAP only for the following functions:
	- vector for internal scheduling of core functional components with approximate 10 pointers stored in a compact STL vector<T> ( internal data structure: array )
	- vector with <b>pointers</b> to all locally managed identities which are presented at the BUS as a node
		( <b> ISO<i><sub>AgLib</sub></i> enables the presentation of more than one node at the BUS</b> )
	- vector of <b>pointers</b> to forward CAN data processing to process data instances, which are located somewhere in the application scope
		with a STL vector<T> with as much pointers as variables are used ( you can reserve enough space at system start,
		to avoid reallocation of the internal array )<br>
		this HEAP usage takes only place, if process data are used
	- lists of measurement programs per local process data ( only for  ProcDataLocal_c or ProcDataLocalSimpleSetpoint_c instances )
	- lists for automatic CAN filter management
	- list of entries in monitor lists ( ISOItem_c, DINItem_c , DINServiceItem - each type only if the corresponding protocol is used )
	- vector for global lookup of sensor or actor I/O instances with one <b>pointer</b> per instance ( <b> only present if the corresponding I/O feaure is used </b> )
	- list for synchronisation data for LBS+ Terminal Mask Upload ( <b>only needed if DIN 9684 with LBS+ Terminal is used</b> )

\subsection IndexHeapDerive Derive Needed HEAPSIZE
The HEAP allocation strategy results in the need of <b>11 KByte</b> HEAPSIZE for the complete <b>MiniVeg N</b> system, which provides
57 process data for remote access ( e.g. Varioterminal ), handles several internal CAN nodes, connects to DIN 9684 and ISO 11783 BUSes with the corresponding
terminal types and is able to directly control fertilizer spreaders which are accessible by process data.<br>
<b>===>></b> Probability for need of more than 20 - 30 KByte is really low for other production systems<br>

ISO<i><sub>AgLib</sub></i> provides some debugging methods to collect information on needed HEAPSIZE for the monitored runtime conditions:
	- <b>DEBUG_HEAP_USEAGE</b> causes output of used HEAP amount for each change of HEAP-placed items ( see above for information on items which are placed at HEAP )
		on RS232 ( please activate RS232 output in project settings for your project )<br>
		-->> direct derive of needed HEAPSIZE, if appropriate worst case load conditions are tested
	- <b>DEBUG_CAN_BUFFER_FILLING</b> causes output of MAX filling and MIN free space of circular buffers ( output filter of individual MsgObj where new MAX filling or
		MIN free space was detected )<br>
		BIOS like the one of STW use special internal buffer memory to store CAN messages in circular buffers;
		each project must define size of this internal memory as a sum for all internal CAN and RS232 buffers<br>
		-->> direct derive of needed size of internal memory block to avoid CAN or RS232 buffer overflow <br>
		-->> ISO<i><sub>AgLib</sub></i> supports allocation of two different CAN buffer sizes, so that CAN filters with especially high load can
			get higher buffer size than other CAN filters with lower load ( see CONFIG_CAN_HIGH_LOAD_IDENT_LIST which is predefined in isoaglib_config.h and can
			be overridden in the project specific config file, that is created and updated by update_makefile.sh )

As some HEAP memory overhead can be caused by reallocation or some other small, not logged items, you should add approximate 1 KByte to the derived size.


\subsection IndexHeapStrategy Selection of HEAP Allocation Method
The <b>S</b>tandard <b>T</b>emplate <b>L</b>ibrary ( STL ) default allocation strategy, which allocates always large chunks of items ( Tasking: 40 ) has the following effects in relation to the malloc_allocation handler:
	- <b>POSITIVE:</b>
		- malloc, which can affect system runtime, is only called one time for a large block of Malloc-Memory ( and not for each single item )
		- cycles of creation-deletion-creation-.. <b>don't</b> cause HEAP fragmentation, as long as amplitude of change doesn't affect amount of needed chunks per datatype,
			because organization of items in one chunk is very efficient
		- low overhead in malloc managed memory block list ( typical malloc algorithm creates one block per malloc call, each block has pointers to PREV and NEXT -> 8 Byte for large memory modell )
	- <b>NEGATIVE:</b>
		- typical ISO<i><sub>AgLib</sub></i> applications will use less than 10 instances of the corresponding data types, so that each chunk will be used just for a low
			percentage ( STL uses one chunk per data type like monitor list item; several lists, which hold same data type as node can share space of one chunk )

Thus the ISO<i><sub>AgLib</sub></i> prints always the following size information:
	- HEAP memory size for malloc_alloc STL allocation handler ( set "OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED=1" in project feature setup file )
	- HEAP memory size for default chunk allocation strategy of STL ( <b>default</b> value "OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED=0" in project feature setup file )

With this information you can individually decide on the appropriate allocation strategy ( <b>OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED</b> in the
project specification file conf_foo ).

The following table present some exact numbers for the needed HEAP memory, so that the decision on the HEAP allocation strategy is eased:
( all numbers for 16 Bit CPU with large memory modell; unit Byte )
<table>
<tr><td>Item</td><td>Size without chunks</td><td>Size with chunks</td><td>Typical unused memory per chunk</td><td>Comment</td></tr>

<tr><td>Scheduler Entry</td><td>12 x scheduled items -> 56</td><td>160</td><td>104</td><td>Vector of pointers to 12 scheduled part tasks</td></tr>
<tr><td>Scheduled Times</td><td>12 x scheduled items -> 32</td><td>80</td><td>48</td><td>Vector of uint16_t execution times of scheduled part tasks</td></tr>

<tr><td>DIN 9684 Monitor List Entry ( DINItem_c )</td><td>10 x 42 DIN -> 420</td><td>1360</td><td>940</td><td>Calculated for 10 ECU nodes on monitored BUS</td></tr>
<tr><td>ISO 11783 Monitor List Entry ( ISOItem_c )</td><td>10 x 52 ISO -> 520</td><td>1768</td><td>1248</td><td>Calculated for 10 ECU nodes on monitored BUS</td></tr>

<tr><td>CAN Filter MsgObj_c</td><td>10 x 82 -> 820</td><td>2960</td><td>2140</td><td>Depends on used protocol parts -> CAN idents to receive</td></tr>
<tr><td>CAN Filter FilterBox_c</td><td>10 x 28 -> 280</td><td>800</td><td>520</td><td>Depends on used protocol parts -> CAN idents to receive</td></tr>

<tr><td>Local Process Data Entry</td><td>10 x pointers to local process data -> 48</td><td>168</td><td>120</td>
	<td>Only if 10 local process data instances are used</td></tr>
<tr><td>Remote Process Data Entry</td><td>10 x pointers to remote process data -> 48</td><td>168</td><td>120</td>
	<td>Only if 10 remote process data instances are used</td></tr>
<tr><td>MeasureProgLocal_c</td><td>10 x 76 -> 760</td><td>2720</td><td>1960</td><td>Only if 10 measure programs are active - one default instance per used ProcDataLocal_c or ProcDataLocalSimpleSetpoint_c instance</td></tr>

<tr><td>DIN LBS+ Mask Synchronisation data</td><td>13 x 66 -> 858</td><td>2320</td><td>1462</td><td>Only if a LBS+ mask project shall be used.</td></tr>
<tr><td>ISO Buffer of SendCommand_c</td><td> (only chunk allocation) </td><td>1284 ( chunk of 80 commands )</td><td>--</td><td>For buffer of SendCommand_c the malloc alloc is for most conditions not more efficient in HEAP use.</td></tr>
<tr><td>ISO Buffer of SendUpload_c</td><td>2 x 32 -> 64</td><td>888</td><td>824</td><td>For buffer of SendCommand_c the malloc alloc is for most conditions not more efficient in HEAP use.</td></tr>

<tr><td><b>SUM DIN 9684</b></td><td>3322</td><td>10736</td><td>7414</td><td>real numbers depend heavily on used features and targeted network configuration</td></tr>

<tr><td><b>SUM ISO 11783</b></td><td>3912</td><td>10996</td><td>7084</td><td>real numbers depend heavily on used features and targeted network configuration</td></tr>
</table>

<b>IMPORTANT:<br>
At least the Tasking compiler uses pages of each 16 KByte in HEAP, where the limits are not defined relative to the beginning of the heap, but are
strictly calculated by multitudes of 0x4000. As one chunk allocation must be fullfilled within one page, the needed HEAPSIZE can be approximate
2 KByte larger than needed, if just some few byte of a large chunk doesn't fit into one page. This worst case condition can be avoided for most
applications, if the HEAP is manually located at the beginning of a 0x4000 multitude in the linker/locator setting ( at least version 7.56 doesn't place
the HEAP automatically at an optimal address, so that manual control is needed ).
</b>


\subsection IndexBiosOsMemory Internal BIOS/OS Memory
ECUs like the several products of <a href="http://www.sensor-technik.de">STW</a> provide a BIOS which implements
core system activities like CAN and RS232 buffer handling, I/O functions like MEAN calculation and peak filtering.
They have an internal memory buffer that holds the CAN and RS232 buffers. Its size can be controled by the setting
<b>BUFFER_SIZE</b> in the file <b>Xos20go.asm</b>.
The amount of needed internal buffer memory for CAN communication is controlled by the following conditions:
	- amount of CAN channels
	- amount of CAN messages which shall be buffered per CAN controller Message Object<br>
		( this is defined by CONFIG_CAN_SEND_BUFFER_SIZE, CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN and CONFIG_CAN_HIGH_LOAD_REC_BUF_SIZE_MIN in the file isoaglib_config.h and can be overridden
			in the project specific config header which is created and updated by update_makefile.sh;
			default setting 20 items for send and 15 and 25 items for normal and high load receive;
			therefore 408 Bytes per buffer with 20 items )
This results in approximate <b>6 KByte</b> per CAN channel with the default values ( max 15 MsgObj with each 20 items in buffer ).<br>
The amount of needed internal buffer memory for RS232 handling depends on the used send and receive buffer. The
default size can be controlled by the settings CONFIG_RS232_DEFAULT_SND_PUF_SIZE and CONFIG_RS232_DEFAULT_REC_PUF_SIZE in isoaglib_config.h and can be overridden
in the project specific config header which is created and updated by update_makefile.sh;
These values can be overridden during runtime by interface functions of iRS232IO_c .
Here a 100 char RS232 buffer needs 108 Bytes of the internal STW memory pool.
<br>
Thus you should set the BIOS/OS internal buffer to approximate <b>7 KByte</b> for one CAN channel and 100 char RS232 send/receive buffer.

\subsection IndexLocalizedRAM Normal RAM
The greatest part of the ISO<i><sub>AgLib</sub></i> components is created as static variables in RAM,
so that the linker/locator can place everyting at fixed addresses, so that no memory lookup must be performed
during firmware execution.<br>



\section IndexStructuralOverview Structural Overview

\subsection StructureModularity Modular Design
The ISO<i><sub>AgLib</sub></i> is modularized according to the different functional
components. Only a small part of the modules is obligatory for all systems,
so that an individual project can adopt the capability and Flash ROM size
to the specific needs.<br>

\subsection StructureLayers Layered Design
The ISO<i><sub>AgLib</sub></i> is designed for easy adoption to new hardware platforms. Thereby the greatest
part of the software can be used without any changes for all hardware types. The more this common software
part is used, the better the quality can get. <br>
The layered structure is described by the following diagram:
  \dot
  digraph LayeredStructure {
		fontname=ARIALN;
		ranksep=1.5;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [shape=record, fontname=ARIALN, fontsize=10, style=filled, fontcolor=black];
    edge     [fontname=ARIALN, fontsize=10, dir="back"];
		ordering=out;
		pack=true;

		upperRuler [ label="<p1>a|<p21>b|<p22>b|<p23>b|<p3>c|<p4>d|<p5>e", width=13,style=invis ];

		subgraph cluster_Communication {
			style="filled";
			label="Communication";
			bottomlabel="Communication Services for ISO 11783\nand/or DIN 9684";
			bgcolor=grey;
			color="grey";
			rank=same;
			multi_msg		[label="Data Stream\nneeded for ISO or DIN Terminal", color="cyan4", URL="\ref MultiMsgPage",width=1.75 ];
			din_term		[label="LBS+ Terminal\ncomplete optional", color="white", URL="\ref DinTerminalPage",width=1.75 ];
			iso_term		[label="ISO Virtual\ncomplete optional", color="white", URL="\ref XMLspec",width=1.75 ];
			process			[label="Process Data\noptional", color="white", URL="\ref ProcDataPage",width=1.75 ];
			system_mgmt	[label="System Management\npartly obligatory", color="cyan", URL="\ref SystemMgmtPage",width=1.75 ];
			base				[label="Base Data\ncomplete optional", color="white", URL="\ref BaseDataPage",width=1.75 ];
			comm_doc		[label="Communication Services for ISO 11783\nand/or DIN 9684",width=3.5];
		}

		subgraph cluster_DriverExtensions {
			style="filled";
			label="Driver Extensions";
			bgcolor=grey;
			color="grey";
			rank=same;
			rs232_drv			[label="RS232\nSupplementary Driver\nwith IOSTREAMS Like\nRead/Write Access", color="white", URL="\ref iRS232IO_c"];
			sensor_drv		[label="Sensor\nSupplementary Driver\nwith Scaling and Range-Check", color="white", URL="\ref iAnalogIRangeCheck_c"];
			actor_drv			[label="Actor\nSupplementary Driver\nwith State Monitoring Functions", color="white", URL="\ref iDigitalO_c"];
			can_drv				[label="CAN\nIndividual Mask/Filter Settings\nfor Each Filterbox", color="yellow", URL="\ref IsoAgLib::iCANIO_c"];
			eeprom_drv		[label="EEPROM\nIOSTREAMS Like Read/Write Access", color="yellow", URL="\ref IsoAgLib::iEEPROMIO_c"];
			common_drv		[label="Common System Access\nGlobally Used", color="yellow", URL="\ref IsoAgLib::iSystem_c"];
			drv_doc				[label="Provide Driver Features wich are\nnot part of most Platforms",width=3.5];
		}
		subgraph cluster_HAL {
			style="filled";
			label="HAL";
			color="grey";
			bgcolor=grey;
			rank=same;
			rs232_hal			[label="RS232\nSupplementary Driver", color="white", URL="\ref hal/rs232.h",width=1.5];
			sensor_hal		[label="Sensor\nSupplementary Driver", color="white", URL="\ref hal/sensor.h",width=1.5];
			actor_hal			[label="Actor\nSupplementary Driver", color="white", URL="\ref hal/actor.h",width=1.5];
			can_hal				[label="CAN", color="blue", URL="\ref hal/can.h",width=1.5];
			eeprom_hal		[label="EEPROM\nStore Process Data, SelfConf SA, ...", color="blue", URL="\ref hal/eeprom.h",width=1.5];
			common_hal		[label="Common System Access\nWatchdog, Time, ...", color="blue", URL="\ref hal/system.h",width=1.5];
			hal_doc				[label="Map Platform Specific API\nto Unique API for IsoAgLib\n(Mapping during compile-time - no runtime-overhead)",width=3.5];
		}
		subgraph cluster_Hardware {
			style="filled";
			label="Hardware";
			color="grey";
			bgcolor=grey;
			rank=same;
			bios					[label="BIOS\nCollection of Low Level \nDriver Libraries", color="green",width=3.4];
			place_hw			[label="<p1>a|<p2>b|<p3>c|<p4>d|<p5>e|<p6>f", width=7,style=invis];
			rtos					[label="RTOS\nRealtime Operating System", color="green",width=3.4];
		}


		lowerRuler [ label="<p1>a|<p21>b|<p22>b|<p23>b|<p3>c|<p4>d|<p5>e|<p6>f", width=13,style=invis];


		upperRuler:p1 -> comm_doc [ style=invis,weight=6000,len=0.01,headport="w",tailport="w"] ;
		comm_doc -> drv_doc [ style=invis,weight=6000,len=0.01,headport="w",tailport="w"] ;
		drv_doc -> hal_doc [ style=invis,weight=6000,len=0.01,headport="w",tailport="w"] ;
		hal_doc -> bios [ style=invis,weight=6000,len=0.01,headport="w",tailport="w"] ;

		common_hal -> place_hw:p1 [ weight=4000,len=0.01] ;
		eeprom_hal -> place_hw:p2 [ weight=4000,len=0.01] ;
		can_hal -> place_hw:p3 [ weight=1000,len=0.01] ;
		rs232_hal -> place_hw:p4 [ weight=1000,len=0.01] ;
		sensor_hal -> place_hw:p5 [ weight=1000,len=0.01] ;
		actor_hal -> place_hw:p6 [ weight=1000,len=0.01,headport="e",tailport="e"] ;

		bios -> lowerRuler:p1 [ style=invis,weight=3000,len=0.01,headport="w",tailport="w"] ;
		place_hw -> lowerRuler:p23 [ style=invis,weight=3000,len=0.01] ;
		rtos -> lowerRuler:p6 [ style=invis,weight=6000,len=0.01,headport="e",tailport="e"] ;


		system_mgmt -> common_drv [ style=invis,weight=2000,len=0.01] ;
		process -> eeprom_drv [ style=invis,weight=2000,len=0.01] ;
		base -> can_drv [ style=invis,weight=2000,len=0.01] ;
		iso_term -> rs232_drv [ style=invis,weight=2000,len=0.01] ;
		din_term -> sensor_drv [ style=invis,weight=2000,len=0.01] ;
		multi_msg -> actor_drv [ style=invis,weight=2000,len=0.01,headport="e",tailport="e"] ;

		upperRuler:p21 -> system_mgmt [ style=invis,weight=2000,len=0.01] ;
		upperRuler:p22 -> process [ style=invis,weight=2000,len=0.01] ;
		upperRuler:p23 -> base [ style=invis,weight=2000,len=0.01] ;
		upperRuler:p3 -> iso_term [ style=invis,weight=2000,len=0.01] ;
		upperRuler:p4 -> din_term [ style=invis,weight=2000,len=0.01] ;
		upperRuler:p5 -> multi_msg [ style=invis,weight=2000,len=0.01,headport="e",tailport="e"] ;

		process			->		eeprom_drv [ style="dotted"];

		system_mgmt	->		can_drv [minlen=1.8,weight=1000];
		process			->		can_drv [minlen=1.8,weight=1000];
		base				->		can_drv [minlen=1.8,weight=1000];
		iso_term		->		can_drv [minlen=1.8,weight=1000];
		din_term		->		can_drv [minlen=1.8,weight=1000];
		multi_msg		->		can_drv [minlen=1.8,weight=1000];

		system_mgmt	->		eeprom_drv [ style="dotted"];

		common_drv	->		common_hal [len=1,weight=3000];

		can_drv			->		can_hal [len=1,weight=3000];

		eeprom_drv	->		eeprom_hal [len=1,weight=3000];

		rs232_drv		->		rs232_hal [len=1,weight=2000];

		sensor_drv	->		sensor_hal [len=1,weight=2000];

		actor_drv		->		actor_hal [len=1,weight=2000,headport="e",tailport="e"];
	 }
	 \enddot

\subsection StructObligatory Information on Obligatory Elements
The following elements are needed for all projects:
	- blue nodes in layer "HAL"
	- yellow nodes in layer "Driver Extensions"
	- cyan nodes in layer "Communication" ( the subdirectories ISO11783 and DIN9684 are only
			used for the respective protocol -> in case of single protocol use exclude the DIN or ISO
			protocol directory from the project. )

Therefore the minimum footprint of the ISO<i><sub>AgLib</sub></i> in Flash ROM is quite low.

\subsection StructPartlyOptional Partly Optional

The supplementary drivers for RS232, Sensor Input and Actuator Control ( mainly PWM ) were developed for
the research project at TUM.

\section IndexLicenseConditions License Conditions

This Open Source project is published under License Conditions which are taken from the <a href="http://ecos.sourceware.org/">eCos</a> project.
This license is defined by the FSF as GPL-compatible according to <a href="http://www.fsf.org/licenses/license-list.html">a list of Open Source licenses</a> .
Red Hat, the original creator of the eCos, and the current maintainers of this project aimed at the following targets, as cited from their
<a href="http://ecos.sourceware.org/license-overview.html">license overview page</a>:
<table width="100%" border="0" cellspacing="0" cellpadding="2" bgcolor="aqua">
<tr><td>
The goal of the license is to serve the eCos user community as a whole. It allows all eCos users to develop products without paying anybody anything, no matter how many developers are working on the product or how many units will be shipped. The license also guarantees that the eCos source code will always be freely available. This applies not only to the core eCos code itself but also to any changes that anybody makes to the core. In particular, it should prevent any company or individual contributing code to the system and then later claiming that all eCos users are now guilty of copyright or patent infringements and have to pay royalties. It should also prevent any company from making some small improvements, calling the result a completely new system, and releasing this under a new and less generous license.


The license does <i>not</i> require users to release the source code of any <i>applications</i> that are developed with eCos. However, if anybody makes any changes to code covered by the eCos license, or writes new files derived in any way from eCos code, then we believe that the entire user community should have the opportunity to benefit from this. The license stipulates that these changes must be made available in source code form to all recipients of binaries based on the modified code, either by including the sources along with the binaries you deliver (or with any device containing such binaries) or with a written offer to supply the source code to the general public for three years. It is perhaps most practical for eCos developers to make the source code available online and inform those who are receiving binaries containing eCos code, and probably also the eCos maintainers, about the location of the code. See the <a href="http://www.gnu.org/copyleft/GPL_with_exception.html">full text of the GPL</a> for the most authoritative definition of the obligations.


Although it is not strictly necessary to contribute the modified code back to the eCos open source project, we are always pleased to receive code contributions and hope that developers will also be keen to give back in return for what they received from the eCos project completely free of charge. The eCos maintainers are responsible for deciding whether such contributions should be applied to the public repository. In addition, a
<a href="http://ecos.sourceware.org/assign.html">copyright assignment</a> is required for any significant changes to the core eCos packages.


The result is a royalty-free system with minimal obligations on the part of application developers. This has resulted in the rapid uptake of eCos. At the same time, eCos is fully open source with all the benefits that implies in terms of quality and innovation. We believe that this is a winning combination.
</td></tr></table>


You can see the complete LICENSE terms at \ref GPL_with_exception where both the exception and
the GPL license text is presented.

You can identify the license protected parts by:<ul>
<li>Header at the top of each file
<li>Files which are placed within directories xgpl_src, build_utils and compiler_projects/projectGeneration ( all relative to root directory )
<li>README_LICENSED_PARTS.txt in each of the root directories where the license conditions of their contents is described
</ul>

<b>Important note on distinction between extensions of ISO<i><sub>AgLib</sub></i> ( which are covered by the license ) and independend
Application development:</b>
The ISO<i><sub>AgLib</sub></i> uses <b>C++</b> namespaces to group internal classes, functions, types, ... in units  <i>__IsoAgLib</i> and <i>__HAL</i>,
which can be clearly seperated from their interface counterparts <i>IsoAgLib</i> and <i>HAL</i>.
Each module which uses or calls <b>directly</b> elements of the internal namespaces <i>__IsoAgLib</i> and <i>__HAL</i> is regarded as extension
of the ISO<i><sub>AgLib</sub></i>.
So you must restrict your direct usage of ISO<i><sub>AgLib</sub></i> parts to the interface namespaces <i>IsoAgLib</i> and <i>HAL</i>.
This causes no runtime and firmware size overhead, as the mapping between the interface elements and their corresponding internal elements
is realized by inlining, which is replaced by the compiler with direct usage of internal parts.
Please contact the maintainer <a href="mailto:a.spangler@osb-ag:de">Achim Spangler</a>, in case you need for some reasons direct access to internal
parts. In case your planned function is really no extension to the ISO<i><sub>AgLib</sub></i>, the interface might be extended to provide the
needed access.


\section IndexAchnowledgements Acknowledgements
\subsection InitialFundingDfg Thanks to DFG funded research group "IKB Dürnast"
The initial creation of the ISO<i><sub>AgLib</sub></i> was enabled by the research project
<b>"IKB Dürnast"</b> which is leaded by <b>Prof. Auernhammer</b> at the <a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology
			- Crop Production Engineering</a> .

\subsection IndexInitialSupport Thanks to development by Sensortechnik Wiedemann ( STW )
The development phase with some compiler issues was supported by the experts of the
<b>ESX</b> engineering team at <a href="http://www.sensor-technik.de">Sensortechnik Wiedemann ( STW )</a> .
Some of the main design goals were derived during a project with <a href="http://www.sensor-technik.de">STW</a>
and <a href="http://www.fendt.com/">AGCO Fendt</a> .

\subsection IndexIsoVtExtension Thanks to Fritzmeier for contribution of ISO 11783 Virtual Terminal support
The development of the central control unit for the chlorophyll sensor
<a href="http://www.fritzmeier.de/engl/environment/environment_miniveg.htm">MiniVeg N</a> was heavily based on the prior work at ISO<i><sub>AgLib</sub></i>,
so that Fritzmeier accepted to fund the development of ISO 11783 virtual terminal support in ISO<i><sub>AgLib</sub></i> .

\subsection IndexFirstWin32Users Thanks to eager and patient initial developers with Win32
As the ISO<i><sub>AgLib</sub></i> is mainly developed on LINUX, all Win32 specific adoptions
including driver mappings and project file generation for Win32 IDEs weren't optimal until
first users like <b>Brad Cox</b> started to try ISO<i><sub>AgLib</sub></i> in combination with
<b>Microsoft Visual Studio C++</b> . As a result, ISO<i><sub>AgLib</sub></i>:
-  provides now an automatic project file generation for Microsoft Visual Studio VC++ version 6.0 ( DSP file ),
	 with the requirement of installation of STLport, if version 7.0 and above isn't used
- Open Source Win32 IDE "Dev-C++" is used for Win32 compile of vt2iso, as there the several required utils can be installed a lot easier.

\section IndexAuthors Authors and Contributors of the IsoAgLib

	- Main Author: <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> designed
			the complete framework
	- Virtual Terminal Expert: <a href="mailto:M.Wodok@osb-ag:de">Martin Wodok</a> implemented
			the tool <b>vt2iso</b> and the virtual terminal handling
	- Pioneer Win32 User and Contributor of missing VT Objects: <b>Brad Cox</b> who helped to find several optimization needs for documentation,
			tutorial and project generation support. The resulting documentation helped him finally to implement all missing VT Object types, which
			weren't needed for the MiniVeg project.


\section IndexMaintainingCompany Some information on the maintaining company OSB AG
The ISO<i><sub>AgLib</sub></i> was initially created by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> who is now
working for the company <b><a href="http://www.osb-ag.de">OSB AG</a></b>. This company started business at the beginning of 2003, and has
already about 60 engineers working in the three locations Munich, Stuttgart and Krefeld ( all in Germany; <b>state November 2004</b> ).<br>
The main business focus is project support at the customer location in software, electronic and mechanical engineering.<br>
Some of the <b><a href="http://www.osb-ag.de">OSB AG</a></b> customers are:
	- AOA
	- Avery Dennison
	- EMAG
	- FIDUCIA IT
	- Fritzmeier
	- Infineon
	- MTU
	- Porsche
	- Siemens
	- ThyssenKrupp
	- Valeo

The ISO<i><sub>AgLib</sub></i> will be actively maintained by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> at <a href="http://www.osb-ag.de">OSB AG</a>
as long as the invested time is affordable in relation to corresponding projects and support contracts.<br>
Even in case the active maintenance by <a href="http://www.osb-ag.de">OSB AG</a> might be stopped some day, the GPL license and
the independend website at the
<a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology - Crop Production Engineering</a>
assure that any interested party can step in and continue the maintenance of the project. This guarantees, like in other Open Source projects, the
open access for every user. <br>
Also, in case the major part of the user community is unsatisfied with the maintenance by <a href="http://www.osb-ag.de">OSB AG</a>, it is
normal for Open Source projects like ISO<i><sub>AgLib</sub></i> to fork the project as a last resort, if <a href="http://www.osb-ag.de">OSB AG</a>
can't or doesn't want to change the style of maintenance as requested. As far as possible, <a href="http://www.osb-ag.de">OSB AG</a>
will do everything to avoid such a fork.<br>
A comparable fork was performed by users and developers of the X11R6 server project for UNIX style operating systems - <b>XFree86</b>.
Caused by some licensing issues, a fork of the <a href="http://www.xfree86.org/">XFree86</a> was created by the <a href="http://freedesktop.org/XOrg">X.Org Foundation</a>.
<br>
&nbsp;<br>
&nbsp;<br>
<b>
This way it is assured under all conditions, that the development time and money which is invested in an application that uses the ISO<i><sub>AgLib</sub></i> can't be affected by
the style and quality of future project maintenance.
</b><br>

\section IndexSupport Support Possibilities
Even if the ISO<i><sub>AgLib</sub></i> provides very detaild documentation including several <b>HOWTO</b> chapters with
important starting information, the learning afford to get a running tutorial example is not completly neglectible ( reading the documentation
requires at least some time ). Additionally the <b>object oriented</b> development style might be new to most of the potential users.<br>

\subsection IndexWorkshop Tutorial Workshops for Development with IsoAgLib
The <a href="http://www.osb-ag.de">OSB AG</a> can offer workshops where all members can learn:
	- <b>XML</b> design and definition of <b>ISO 11783 Virtual Terminal</b> mask pools
	- possibilities to create flexible mask definitions which can be automatically adopted by the ISO<i><sub>AgLib</sub></i> to the properties of the connected terminal:
		- multi-language string handling
		- scaling to screen size
		- selection of available font size that is corresponding to the scaling factor of the complete layout
		- colour depth selection
		during runtime, so that a unique global mask pool can be defined.
	- integration of <b>IDE</b> of choice ( some project file format types are automatically supported )
	- integration of <b>ISO 11783</b> communication with existing application code ( ways to define interface modules )
	- integration of process data management to allow terminal independend control and monitoring access to an implement
	- adoption to individual target platform ( creation of corresponding <b>H</b>ardware <b>A</b>bstraction <b>L</b>ayer - HAL )
	- test of application with the <b>R</b>ealtime <b>T</b>est <b>E</b>nvironment ( RTE ), which is another also Open Source
		project of <a href="http://www.osb-ag.de">OSB AG</a> for test of embedded applications ( not only useable for ISO 11783 networks )
	- ...

The exact definition of a tutorial workshop can be defined corresponding to the request of
interested developers. These workshops can be performed at the
<a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology - Crop Production Engineering</a>
 location, or at any location of choice. <a href="http://www.osb-ag.de">OSB AG</a> can also offer closed workshops for the developers of one
company.

\subsection IndexHalAdoption Support for Adoption to Individual Target Platform
Even if the adoption of the ISO<i><sub>AgLib</sub></i> to an individual target platform ( OS or BIOS ) is eased
by the compact HAL definition, <a href="http://www.osb-ag.de">OSB AG</a> can offer support for this adoption or can perform the
complete adoption. Please contact <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> for details.

\subsection IndexSupportContract Support Contract
Like other Open Source projects, the maintainers can provide some free support, which is restricted by
the spare free time of the developers and other experts in the user community. For commercial users, who need a predefined
reaction time for each issue ( question or bug report ), might contact <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
for individual service contracts.

\subsection IndexFeaturesOnRequest Integration of Requested Features
As long as <a href="http://www.osb-ag.de">OSB AG</a> maintains the ISO<i><sub>AgLib</sub></i>, all received
patched will be reviewed and merged into the reference version of the software. Additionally all feature
extensions, which are needed for projects of <a href="http://www.osb-ag.de">OSB AG</a> will also be merged.<br>

But as the ISO 11783 standard definition contains a lot of functions, which are not needed for the own
projects of the ISO<i><sub>AgLib</sub></i> maintainers, <a href="http://www.osb-ag.de">OSB AG</a> can offer to
develop them on request. This is also true for several enhancements of ISO Virtual Terminal mask definitions, which
would enable a more capable and more flexible adoption to the features of the connected terminal. <br>
Some examples:
	- GUI tool to define the XML mask definition ( easier than edit of pure ASCII )
	- individual rules for selection of bitmap version in relation to size and color depth of connected
		terminal
	- extension of the font size relative positioning to map different font sizes for different
		language versions ( languages where all labels a longer than the corresponding strings in other
		languages might need a smaller font size to allow placement on the common layout;
		therefore font size dependend position alignment should be enhanced )
	- language specific bitmaps ( especially if some text is displayed in the bitmap )



\section IndexTodo Main TODO items
The following items are at the TODO list. You are encouraged to
help contributing for their implementation with source code or financial donation.

	- Write some tutorial examples for use of supplementary IO drivers<br>
		<b>Integrated as soon as there's some time</b>
	- Write tutorial example with second ISO<i><sub>AgLib</sub></i> CAN instance for proprietary CAN protocol at internal BUS<br>
		<b>Integrated as soon as there's some time</b>
	- Refinement of GPS decoding<br>
		<b>Integrated as soon as correct decoding is detected</b>
	- Calculate the RAM + ROM resource need of the ISO<i><sub>AgLib</sub></i> tutorial example programs
		for some embedded architectures with different feature lists<br>
		<b>Performed on request or in case somebody is volunteering</b>
	- Extending documentation on virtual terminal support and driver adoption architecture<br>
		<b>Ongoing process</b>
	- Extend tutorial examples<br>
		<b>Ongoing process when need for some more practical demonstration is detected<br>
			( Please have a close look on all <a href="examples.html">tutorial examples</a> before requesting further examples )</b>


<b>Important</b>
<table width="100%" border="0" cellspacing="0" cellpadding="2" bgcolor="aqua">
<tr><td>
Each email adress in this project is intentionally corrupted, to make SPAMMING a little bit more complicated. Each human reader will easily detect how to
"fix" the email adress - this "manupulation" might change if the maintainers detect that SPAM tools are able to derive automatically a
working domain.
</td></tr></table>


\section IndexUsingProjects List of Known Projects which use IsoAgLib
As the used license <i>GPL with exception</i> requires from each user to accompany any distributed Program
( or a work based on it - see GPL §3 ) with some information on the used library, it would be quite kind to
inform additionally the authors of your project, so that each using project can be listed here.
This list can also serve as a prominent place for showing all contributing ( by money and/or code ) users.<br>
Last but not least, projects which use features like process data communication based on ISO<i><sub>AgLib</sub></i> will
probably have less compatibility problems then in other combinations. This gets the more important, the more implements,
sensors, task controllers, core control units ( expert system ) and tractor builds more and more complex
interaction networks with all sorts of dependencies. Here ISO<i><sub>AgLib</sub></i> can provide common patterns for the
management of such interactions. This list can then help to identify well supported device combinations.<br>

<table width="100%" border="0" cellspacing="0" cellpadding="2" bgcolor="aqua">
<tr><td><b>Project</b></td><td><b>Organization</b></td><td><b>Contribution</b></td></td>
<tr>
	<td>Automated process data recording system "Task-Controller" which detects all connected implements and starts the recording of
		device type dependend process data ( e.g. application rate for spreaders ). It combines this information with some tractor information
		( e.g. fuel consumption, speed, hitch position ) and GPS positioning data. All data sets are stored at a PCMCIA memory card.
		The recorded data set can be configured by EEPROM settings, so that the firmware itself needn't be changed.
	</td>
	<td><a href="http://www.tec.wzw.tum.de/pflanztech/englisch/index.html">Department of Bio Resources and Land Use Technology - Crop Production Engineering</a> and
			funding organization DFG
	</td>
	<td>Initial development of ISO<i><sub>AgLib</sub></i> ( previously named LBS<i><sub>Lib</sub></i> )
	</td>
</tr>
<tr>
	<td><a href="http://www.fritzmeier.com/engl/frameset_engl.htm?/engl/environment/environment_miniveg.htm">Chlorophyll Sensor <b>MiniVeg N</b></a>
			which can be used in combination with AGCO Fendt Vario Tractors ( with Varioterminals ) or any tractor with an ISO 11783 virtual terminal.
			It can control by process data setting each standard conformant DIN 9684 or ISO 11783 fertilizer spreader which enables control by process data
			( several implements like spreader can be only controlled by the corresponding terminal ). An ISO<i><sub>AgLib</sub></i> powered PROXY is used
			to provide standard conformant process data control access for spreaders which can be at least controlled with some documented RS232 or CAN connection.
	</td>
	<td><a href="http://www.fritzmeier.com">Fritzmeier</a>
	</td>
	<td>Several optimization work for ISO<i><sub>AgLib</sub></i> was enabled by this project. This is especially important for the complete new part of
			virtual terminal ( ISO 11783 )
	</td>
</tr>
</table>

</td></tr>
  </table>
*/

/*! \page HowtoDownloadPage Documentation and Download of IsoAgLib

<center>
<h1>Object Oriented Program Library ISO<i><sub>AgLib</sub></i></h1>
<h1>Development of ISO 11783 and DIN 9684 Applications</h1>
<h2>Copyright &copy; 1999 - 2004 Achim Spangler, Martin Wodok</h2>
<h2>Licensed with Exceptions under the General Public License (GPL) of the Free Software Foundation</h2>
</center>

<table width="100%" border="0" cellspacing="0" cellpadding="2">
<tr><td valign="Top" nowrap="true" >
\ref IndexWhereToStart<br>
\ref IndexNews<br>
\ref IndexChanges<br>
\ref IndexMainFeatures<br>
\ref IndexMemoryUsage<br>
\ref IndexStructuralOverview<br>
\ref IndexLicenseConditions<br>
\ref IndexAchnowledgements<br>
\ref IndexInitialSupport<br>
\ref IndexIsoVtExtension<br>
\ref IndexFirstWin32Users<br>
\ref IndexAuthors<br>
\ref IndexMaintainingCompany<br>
\ref IndexSupport<br>
\ref IndexTodo<br>
\ref IndexUsingProjects<br>
\ref HowtoDownload<br>
\ref HowtoAccessRepository<br>
\ref HowtoGetHelp<br>
\ref HowtoCreateDoc<br>
\ref HowtoLearnIsoAgLib<br>
\ref HowtoSetupCompilerTools<br>
\ref InstallIsoToolVt2Iso<br>
\ref HowtoCreateProjectFiles<br>
\ref InfGeneralInformation<br>
\ref InfAgentNetwork<br>
\ref InfServiceNetwork<br>
\ref InfRequirements<br>
\ref FurtherDevelopment<br>
\ref FurtherExamples<br>
\ref QuestionsAnswers<br>
\ref FurtherReading<br>
<br>
&nbsp;<br>
&nbsp;<br>
&nbsp;<br>
Last Update: <br>06 December 2004<br>
by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
<br></td>
<td valign="Top">

\section HowtoDownload HOWTO Identify and download needed archives

As the ISO<i><sub>AgLib</sub></i> provides several functions which are supplementary
or only relevant for specific conditions, the download is seperated in several
parts.

<table width="100%" border="0" cellspacing="0" cellpadding="2">
<tr><td valign="Top" nowrap="true" >Content Description</td>
	<td valign="Top" nowrap="true" >Archive</td>
	<td valign="Top" nowrap="true" >Size [KByte]</td>
</tr>
<tr><td>Core components which are needed for almost all use cases</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/coreArchiv-1.0.1.zip</td>
	<td>4152</td>
</tr>
<tr><td>Documentation ( Can also be recreated with Doxygen )</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/generatedHtmlDocuList-1.0.1.zip</td>
	<td>13125</td>
</tr>
<tr><td><b>Essential ISO 11783</b> specific parts including virtual terminal support</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/isoArchiv-1.0.1.zip</td>
	<td>408</td>
</tr>
<tr><td><b>Essential DIN 9684</b> specific parts including LBS+ terminal support/upload</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/dinArchiv-1.0.1.zip</td>
	<td>112</td>
</tr>
<tr><td>Data repository class with cached tractor information ( Base Data like tractor speed )</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/baseArchiv-1.0.1.zip</td>
	<td>28</td>
</tr>
<tr><td>Process Data support ( local and remote process data with several variants for different feature sets / resource requirements</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/procArchiv-1.0.1.zip</td>
	<td>276</td>
</tr>
<tr><td>Supplementary driver with object oriented modelling of RS232 ( access functions like C++ iostreams )</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/supplRs232Archiv-1.0.1.zip</td>
	<td>44</td>
</tr>
<tr><td>Supplementary driver with object oriented modelling of Actors ( effective diagnose based on properties for allowed current range )</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/supplActorArchiv-1.0.1.zip</td>
	<td>36</td>
</tr>
<tr><td>Supplementary driver with object oriented modelling of Sensors ( Digital and Analog Input )</td>
	<td>http://www.tec.wzw.tum.de/IsoAgLib/Download/supplSensorArchiv-1.0.1.zip</td>
	<td>80</td>
</tr>
</table>

The supplementary drivers were developed to control complete ECU IO in object oriented style
with ISO<i><sub>AgLib</sub></i> . This development patterns allows the combined control and diagnose
of IO channels with One-Channel-One-Instance relation. These driver extensions are complete optional.

All archives should be extracted from the same directory.

\section HowtoAccessRepository HOWTO Access the Version Management Repository
The sources of the ISO<i><sub>AgLib</sub></i> managed by the version management system
<a href="http://subversion.tigris.org">Subversion</a> which is developed as a replacement for CVS.
The ISO<i><sub>AgLib</sub></i> repository allows an anonymous read access with:
- subversion command line tools with URL
	<a href="http://linux90.idvnet.de:8092/svn/OSB/IsoAgLib/IsoAgLib">http://linux90.idvnet.de:8092/svn/OSB/IsoAgLib/IsoAgLib</a> for HTTP access to HEAD development<br>
	or<br>
	<a href="https://linux90.idvnet.de:8093/svn/OSB/IsoAgLib/IsoAgLib">https://linux90.idvnet.de:8093/svn/OSB/IsoAgLib/IsoAgLib</a> for HTTPS access ( safe against caching PROXY ) HEAD development<br>
	( you can also browse through the repository with a normal WWW Browser )
- subversion command line tools with URL
	<a href="http://linux90.idvnet.de:8092/svn/OSB/IsoAgLib/releases">http://linux90.idvnet.de:8092/svn/OSB/IsoAgLib/releases</a> for HTTP access to releases<br>
	or<br>
	<a href="https://linux90.idvnet.de:8093/svn/OSB/IsoAgLib/releases">https://linux90.idvnet.de:8093/svn/OSB/IsoAgLib/releases</a> for HTTPS access ( safe against caching PROXY ) to releases<br>
	( you can also browse through the repository with a normal WWW Browser )
- access different versions, including the possibility to trace changes with the URL
	<a href="http://linux90.idvnet.de:8092/websvn">http://linux90.idvnet.de:8092/websvn</a>
- use GUI tools like rapidsvn and TortoiseSVN ( integration in Windows File Manager for very easy
	version management access )
	which can be downloaded from <a href="http://subversion.tigris.org">Subversion Webiste</a>

Each user who wants to contribute enhancements should please do the following:
- Use Subversion or one of its GUI Tools to create a local working copy
- Update your repository ( especially important for creation of diffs ) with
	the command "svn update" ( or one of the corresponding commands in GUI tools )
- Collect all changes with the command "svn diff" ( or one of its corresponding commands in GUI tools )
	into one patch
- Send the patch to <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
	for inclusion ( evaluation of the change is then a lot easier )

\section HowtoGetHelp HOWTO get help
As this is an Open Source project, not only the maintainers of the project can provide sufficient help for all users <b>free of charge</b>.
So either you sign a support contract to get personal support.<br>
Or you direct your help requests to the community of ISO<i><sub>AgLib</sub></i> users and developers, who can be reached by the <a href="http://linux90.idvnet.de/pipermail/isoaglib/">email list</a>
( subscribe <a href="http://linux90.idvnet.de/cgi-bin/mailman/listinfo/isoaglib">here</a> and send mail <a href="mailto:isoaglib@linux90.idvnet.de">there</a> ). This way all users can help each other
without the dependency on the maintainers. New feature wishes, bug reports and other change suggestions can be discussed there, too.<br>
Please keep in mind, that you might be one time the helping party, while you could need help from another user the other time.<br>
Please keep this also in mind, if the company name in the email address of the asking user is not one of your best friends - maybe he can help you another time also.<br>
This works really fine in several other Open Source projects.


\section HowtoCreateDoc HOWTO Recreate Documentation with Doxygen

If you don't want to download the big archive with the generated documentation
( <b>13 MByte</b> ), you can simply recreate the documentation based on the downloaded
source and the contained informative text files.
<b>Steps:</b><ol>
<li>Download and install Doxygen from http://www.stack.nl/~dimitri/doxygen/ for your specific plattform
<li>Download and install graphviz with the tool "dot" from http://www.research.att.com/sw/tools/graphviz/download.html
<li>Let Doxygen create the documentation with the config file "Doxyfile" which is located in the project root directory
</ol>


</td></tr>
  </table>
*/



/*! \page GettingInstalled Getting Started with IsoAgLib

<center>
<h1>Object Oriented Program Library ISO<i><sub>AgLib</sub></i></h1>
<h1>Development of ISO 11783 and DIN 9684 Applications</h1>
<h2>Copyright &copy; 1999 - 2004 Achim Spangler, Martin Wodok</h2>
<h2>Licensed with Exceptions under the General Public License (GPL) of the Free Software Foundation</h2>
</center>

<table width="100%" border="0" cellspacing="0" cellpadding="2">
<tr><td valign="Top" nowrap="true" >
\ref IndexWhereToStart<br>
\ref IndexNews<br>
\ref IndexChanges<br>
\ref IndexMainFeatures<br>
\ref IndexMemoryUsage<br>
\ref IndexStructuralOverview<br>
\ref IndexLicenseConditions<br>
\ref IndexAchnowledgements<br>
\ref IndexInitialSupport<br>
\ref IndexIsoVtExtension<br>
\ref IndexFirstWin32Users<br>
\ref IndexAuthors<br>
\ref IndexMaintainingCompany<br>
\ref IndexSupport<br>
\ref IndexTodo<br>
\ref IndexUsingProjects<br>
\ref HowtoDownload<br>
\ref HowtoAccessRepository<br>
\ref HowtoGetHelp<br>
\ref HowtoCreateDoc<br>
\ref HowtoLearnIsoAgLib<br>
\ref HowtoSetupCompilerTools<br>
\ref InstallIsoToolVt2Iso<br>
\ref HowtoCreateProjectFiles<br>
\ref InfGeneralInformation<br>
\ref InfAgentNetwork<br>
\ref InfServiceNetwork<br>
\ref InfRequirements<br>
\ref FurtherDevelopment<br>
\ref FurtherExamples<br>
\ref QuestionsAnswers<br>
\ref FurtherReading<br>
<br>
&nbsp;<br>
&nbsp;<br>
&nbsp;<br>
Last Update: <br>06 December 2004<br>
by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
<br></td>
<td valign="Top">


\section HowtoLearnIsoAgLib HOWTO Learn Usage of IsoAgLib

\subsection InformationInTutorials Information in Tutorial Examples

Please start your learning phase with the several tutorial examples, so that you can get
a guided tour through the several options and features of the library.
Each example has several links to informative HTML pages where you can more and deeper information
on the presented topics.

Look at <a href="examples.html">Example Page</a> for the available tutorial examples.

\subsection DDDocumentation Documentation Overview Pages

- \link LgplSrcOverview Overview on GPL Licensed Parts of IsoAgLib \endlink
	- \link CommOverPage Documentation on Communication Services of IsoAgLib \endlink
		- \link SchedulerPage Overview on Central Scheduler of IsoAgLib \endlink
		- \link SystemMgmtPage Overview on System Management of IsoAgLib ( %i.%e. monitor lists ) \endlink
		- \link BaseDataPage Overview on Base Data Handling in IsoAgLib \endlink
		- \link ProcDataPage Overview on Process Data Handling in IsoAgLib \endlink
		- \link DinTerminalPage Overview on Management of LBS+ Terminal Masks \endlink
		- \link XMLspec Overveiw on Interaction with ISO 11783 Virtual Terminal \endlink
	- \link MainHalPage Hardware Abstraction Layer (HAL) for main parts of IsoAgLib \endlink
	- \link SuppHalPage Hardware Abstraction Layer (HAL) for supplementary parts of IsoAgLib \endlink


\section HowtoSetupCompilerTools HOWTO Prepare Suitable Compiler and IDE

The ISO<i><sub>AgLib</sub></i> requires standard conformant C++ compilers with a
conformant implementation of the <b>S</b>tandard <b>T</b>emplate <b>L</b>ibrary ( STL ).
As at least the Microsoft Visual Studion of version 6 doesn't provide either, you have at least to
download the conformant STL version from http://www.stlport.org ( Important: Don't use the iostream version
of STLport with Visual Studio, as there are some problems ).<br>



\subsection CompKdevelop3 KDevelop3 for LINUX

The usage of ISO<i><sub>AgLib</sub></i> is quite easy with LINUX. As this platform is used by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>,
a first hand support is possible. The fine IDE named kdevelop ( see http://www.kdevelop.org ) provides
code completion, class browser, integrated debugger and several features more.
There are some basic kdevelop3 projects in compiler_projects/kdevelop_tmake/IsoAgLib . In case of problems,
<a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> can help with some more examples.<br>
The project files which can be created by a helper tool of the ISO<i><sub>AgLib</sub></i> can be very easily
compiled with standard GNUmake ( Important: This requires the existance of qmake of the Qt framework - this is available as
simple installable binary for all known LINUX distributions ).

\subsection CompDevVc6 Microsoft Visual Studio 6
The automatic project file generating script can write DSP project files, which can be directly used to create an EXEcuteable
program with Vector-Informatik CAN card support ( or other CAN cards ). As newer versions of VC++ can also import DSP project files,
no special support is provided.<br>

\subsubsection Vc6StlPort Installation of STLport - Open Source ANSI C++ compatible STL
<b>Important:</b>
The VC++ version 6 has no ANSI C++ compatible version of the <b>S</b>tandard <b>T</b>emplate <b>L</b>ibrary ( STL ) on board, so that
the STLport must be installed and used for ISO<i><sub>AgLib</sub></i>.<br>

The <a href="http://www.stlport.org/">STLport</a> is meanwhile a very well known and accepted implementation of the STL standard.
It is used by commercial IDEs like Borland C++ Builder 6 and Tasking EDE ( since version 8.0 ).

Installation Steps:
-# Download STLport from http://www.stlport.org/archive/STLport-4.6.2.tar.gz ( other versions at http://www.stlport.org/download.html )
-# extract to location like C:\ ( or use other directory - then change the following examples according to your individual path )
-# <b>rename directory C:\STLport-4.6.2 to C:\STLport , so that the scripts and automatically created paths needn't be adopted to new versions of STLport</b>
-# Configure and install STLport
	- with native IOSTREAMS of VC6 ( <b>recommended</b> ):<br>
		- simply uncomment the #define _STLP_NO_OWN_IOSTREAMS in the config header C:\STLport\stlport\stl_user_config.h
		- edit C:\Program Files\Microsoft Visual Studio\VC98\Bin\vcvars32.bat
			- FROM: set INCLUDE=%MSVCDir%\ATL\INCLUDE; %MSVCDir%\INCLUDE;%MSVCDir%\MFC\INCLUDE;%INCLUDE%
			- TO:   set INCLUDE=C:\STLport\stlport;%MSVCDir%\ATL\INCLUDE; %MSVCDir%\INCLUDE;%MSVCDir%\MFC\INCLUDE;\%INCLUDE%
		- add C:\STLport\stlport <b>in FRONT of the include path of your projects - this is already done for automatically created DSP files</b>
		- you are finshed! ( no compile or install of sources needed )
	- with IOSTREAMS version of STLport:<br>
		use installation overview from http://doc.ece.uci.edu/~mlai/notes/stlport_in_vc.html
		( the BAT vcvars32.bat is not located at C:\Program Files\Microsoft Visual Studio\VC98 but in C:\Program Files\Microsoft Visual Studio\VC98\Bin )
-# look at STLport own documentation from local installation file://C:/STLport/doc/install.html and file://C:/STLport/doc/select_io.html for further information
-# make shure that you adopt the feature setup files to reflect a differing path to your STLport installation ( C:\STLport is default setting )



\subsection CompDevCpp Dev-C++ for Win32

<b>Important:</b>
<i>
As the DLLs of the Vector-Informatik CAN cards can't be handled correct by Dev-C++ generated EXE programs, this IDE can't be used to
build Win32 applications with Vector-Informatik CAN card support.<br>
Dev-C++ is still a usefull and free IDE which can especially be used to build vt2iso, as the installation of the needed tools is here
a lot easier.
</i><br>



As <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> had several issues with MS Visual Studio, and the STL version of the standard <b>gcc</b> compiler
is quite good, <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> decided to maintain mainly the free IDE "Dev-C++" ( http://www.bloodshed.net/dev/devcpp.html )
for Win32 platforms.
This IDE has some important features:<ul>
<li>simple install
<li>automatic update within the extras menu
<li>download of extension modules within easy to handle download and install dialog window
	( <b>important</b>: the tools for <b>vt2iso</b> compilation: <i>xerces</i> and the image manipulation libraries <i>FreeImage</i> and <i>ImLib</i> can be
	  downloaded and installed this way )
<li>project file format is a clean ASCII, so that the project generation script can
    directly create a running project for Dev-C++
</ul>

\subsubsection InstallDevCpp Installation of Dev-C++
<ol>
<li>Please download at least the version 4.9.9.0 from http://www.bloodshed.net/dev/devcpp.html
<li>Check for updates with Dev-C++ menu entry TOOLS->CHECK FOR UPDATES
<li>Select the mirror "PlanetMirror.com" from the list ( at least the german version has a dead
	default mirror entry in the list ) in the dialog window "WebUpdate"
<li>Click on button "Check for updates" in the dialog window "WebUpdate"
<li>Select at least the following packages for download ( as they are needed for <b>vt2iso</b> )<ul>
	<li>FreeImage.DevPak
	<li>ImageLib.DevPak
	<li>xerces-c_2_5_0-mingw.DevPak ( see for special download instructions at \ref Win32Xerces )
	<li>( optionally further documentation or utility packages )
	</ul>
</ol>


\subsection CompTasking Tasking for Embedded Targets

The Tasking Compiler of version >= 7.56 is handling C++ in a sufficient manner and provides
a conformant STL ( important: the STL version of 8.0 is consuming tooo muuuch memory - contact
Tasking Support to get alternative like in version 7.56 where the resource requirements are fine; maybe some of the newer
Tasking Compiler versions than 8.0 manage the size overhead of STL better; the overhead is caused by the change from
and adopted SGI STL to the STLport system; while SGI STL uses templates only, the STLport defines several functions
as functions which can be compiled to a linkeable library; this approach is fine, as long as the target linker is able
to select just the needed functions from the STLport libraries; at least Tasking Compiler Version 8.0 included a lot too much
object code into the firmware, so that the overhead is not acceptible ).

ISO<i><sub>AgLib</sub></i> also provides some project files for the tutorials which can be used by the
Tasking compiler.

\subsubsection TaskingHeapsize Set Heapsize in Linker/Locator Specification ( ILO file )
The ISO<i><sub>AgLib</sub></i> stores some information in dynamic lists, so that you need some HEAP memory.
Please start with a great HEAPSIZE setting in the linker/locator specification ( see HEAPSIZE control in ILO file ), and
check the RS232 output, when you compile the project with the #define DEBUG_HEAP_USEAGE .

\subsubsection TaskingCanRs232Buffer Allocate enough buffer for CAN receive buffer queues and RS232 buffer
Some OS/BIOS which are designed like <a href="http://www.sensor-technik.de">STW BIOS</a> have a special setting to
allocate enough memory for the internal managed buffers for RS232 and CAN. The amount of items which are reserved by
ISO<i><sub>AgLib</sub></i> in isoaglib_config.h #define CONFIG_CAN_STD_LOAD_REC_BUF_SIZE_MIN and CONFIG_RS232_DEFAULT_SND_PUF_SIZE ( this can be overridden by
configuration call with iRS232IO_c::setSndPufferSize and iRS232IO_c::setRecPufferSize ).<br>
- Again - set the value ( in case of <a href="http://www.sensor-technik.de">STW BIOS</a> set variable BUFFER_SIZE in file "Xos20go.asm" ) to MAX or other great enough value for a start.
- Then compile your project with DEBUG_CAN_BUFFER_FILLING ( activate it in the feature setup file of your project )
- Check the RS232 output for fill grade of CAN buffers ( the MAX amount of buffered CAN messages per BUS is printed )
- Calculate the needed memory for RS232 ( corresponds to CONFIG_RS232_DEFAULT_SND_PUF_SIZE or the overridden value ) and CAN ( corresponds to the MAX output from step above )
- Adopt the buffer size value ( leave some additional space )

\section InstallIsoToolVt2Iso HOWTO Install vt2iso - Tool for Creation of ISO 11783 Virtual Terminal Masks

The ISO<i><sub>AgLib</sub></i> provides a complete Open Source Tool Chain for ISO 11783 virtual terminals.
A very important part is the tool <b>vt2iso</b> which parses the XML mask definition files and creates
ROMable constant declarations for all used virtual terminal objects, which can be easily uploaded to the terminal
( see at the corresponding tutorial for more information on this ).

\subsection InstallFreeImage Install Image Manipulation Library Free Image
The <b>vt2iso</b> can read almost all types of bitmaps to get the compileable variable array representation
of bitmaps for the ECU. This stuff is greatly supported by the Open Source libraries <b>FreeImage</b>
and <b>Paintlib</b> ( http://www.paintlib.de/paintlib/ ) ( alternative - choose the library that is easier to install for you ).<br>
Vt2iso uses these libraries to access the color values of all pixels ( and to convert from palettized / indexed formats
if needed ) so that the needed conversion to the chosen color depth can be performed.
Vt2iso uses an automatic calculation of a threshold to get the highest contrast in the picture ( optimum 50:50 relation between
black and white ).<br>
\subsubsection FreeImageLinux Installation of Free Image Library on LINUX
The FreeImage library can be best compiled from source for LINUX ( simple to compile with make based on
provided Makefile ).
Please make sure that the library and include files are installed to system directories ( need root user to install )
or adopt the Makefiles on your own ( no support by author for this config ).
The FreeImage library may also be available as a ready made binary distribution.

\subsubsection FreeImageWin32 Installation of Free Image Library on Win32
On Win32 Platform the Dev-C++ IDE provides a very simple download as extension package from within the program. Thus you have no
installation afford with any make or manual install ( see \ref InstallDevCpp ).
In case you still want to use Visual Studio, then please look at the manuals on the project webpage.

( Remark: As this program is only running on development PC, the compiler needn't be able to generate
  code for embedded platforms -> Tasking not needed for this on Win32 )

\subsection InstallXerces Install XML Parser Library Xerces of the Apache Foundation

The parsing of the XML files is performed by the Xerces library from apache, which can be downloaded from
http://xml.apache.org/xerces-c/index.html .

\subsubsection LinuxXerces Installation of Xerces-C on LINUX
Most of the LINUX distributions provide a binary package for Xerces-C which can be easily installed from within the
distribution specific package manager. Select at least a Xerces-C package which contains libxerces-c.so.2* and
the corresponding headers in /usr/include/xercesc/ ( or /usr/local/include/xercesc/ ).

\subsubsection Win32Xerces Installation of Xerces-C on Windows
<ol>
<li>Download the Dev-C++ prepared binary of Xerces-C from http://devpaks.org/show.php?devpak=41
<li>Call the packagemanager for Dev-C++ ( menu TOOLS->PACKAGE MANAGER ) and select
	the downloaded archive xerces-c_2_5_0-mingw.DevPak for install
</ol>

\subsection CompileVt2Iso Compile vt2iso

\subsubsection CompileVt2IsoLinux Compile VT2Iso on LINUX
<ol>
<li>cd build_utils/vt2iso
<li>call make
</ol>


\subsubsection CompileVt2IsoWin32 Compile VT2Iso on Win32 with Dev-C++
<ol>
<li>Install first Dev-C++ with the library packages FreeImage and ImageLib
<li>Install Xerces-C ( see above )
<li>Open the Dev-C++ project file build_utils/vt2iso/Vt2Iso.dev
<li>Start project compile
<li>Copy the dll's xerces-c_2_5_0.dll and FreeImage.dll to a place where Vt2Iso.exe
	can find them if you execute the EXE ( e.g. same directory as EXE or system directory )
	or extend the PATH. As <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> is very seldom using Win32 development tools,
	I ask you to check other resources on how to manage dll placement.
</ol>

\subsection UseVt2Iso Use Vt2Iso to create mask definition
You can find the description of the XML syntax which is used by vt2iso at \ref XMLspec .<br>
An example mask pool is defined for the example 3_0_VirtualTerminalIso.cpp . Some handler functions are also demonstrated there.<br>
The automatic generation itself is triggered by a call like described in the script build_utils/vt2iso-3-0_VirtualTerminalIso.sh ( LINUX )
or build_utils/vt2iso-3-0_VirtualTerminalIso.bat ( Win32 ).
This script/BAT results in suitable files with constant variable array definition for include into the main souce
file of a project ( in this example in IsoAgLib_Examples/tutorial/3_Terminal/3_0_VirtualTerminalIso/MaskDefinition/ ).



\section HowtoCreateProjectFiles HOWTO Create Project Files with Script update_makefile.sh

Caused by the huge amount of optional features in the ISO<i><sub>AgLib</sub></i> , the manual
selection of needed project files can get quite complicated.
The script compiler_projects/projectGeneration/update_makefile.sh can create project files and
project specific configuration headers for all projects. The feature setup is feed to this script
by configuration files with a very simple syntax. Please have a look at the comments inside the several
conf_* files in the directory compiler_projects/projectGeneration/ .
Call the script like this:<ul>
<li>cd compiler_projects/projectGeneration
<li>update_makefile.sh conf_0_0_AddressClaimIso
</ul>


\subsection UseMSYS Use MSYS to run script on Win32
As scripts can only natively executed in UNIX or LINUX, the tool MSYS ( http://www.mingw.org )
is needed as a minimal POSIX layer to call bash scripts within Win32.
This project is simply to install. You don't need the other tools on this side, as the
MinGW compiler is also integrated into the Dev-C++ tool.


\subsection ScriptIntegrateWin32Can Integrate Win32 CAN card drivers
The IsoAgLib provides at the moment an integration with the CAN drivers from Vector Informatik.
Both driver types <b>CANLIB</b> and <b>XL Driver Library</b> are supported by the source modules
<b>target_extension_can_w32_vector_canlib.cpp</b> and <b>target_extension_can_w32_vector_xl.cpp</b> .
Both modules support the usage of a thread based CAN receive, which can be activated by the
setting of the #define USE_THREAD ( if it's undef -> no threading is used ).
The integration of ISO<i><sub>AgLib</sub></i> and Vector CAN drivers runs as follows:<ol>
<li>Download either the current <b>CANLIB</b> or <b>XL Driver Library</b> driver
<li>Install the driver with the <i>SETUP.EXE</i> ( <b>don't rename the contained
		root directories <i>CANLIB</i> resp. <i>XL Driver Library</i> as the ISO<i><sub>AgLib</sub></i>
		uses them </b> )
<li>Set the project configuration file conf_* ( e.g. conf_0_0_AddressClaimIso ) as follows:<ol>
	<li>set the variables <i>USE_WIN32_HEADER_DIRECTORY</i> and <i>USE_WIN32_LIB_DIRECTORY</i> to your
			installation root ( <b>don't include the directory name part <i>CANLIB</i> resp. <i>XL Driver Library</i> </b> ) <br>
			Example: <ul>
			<li>You selected "D:/Development" during the execution of <i>SETUP.EXE</i>.
			<li><i>SETUP.EXE</i> placed everything in the directory "D:/Development/CANLIB"
					resp. "D:/Development/XL Driver Library"
			<li>Set both above mentioned variables to "D:/Development"
			</ul>
	<li>set the variable USE_CAN_DRIVER to either "vector_canlib" or "vector_xl"
	<li>select the CAN card type variable USE_WIN32_CAN_HW_TYPE to HWTYPE_VIRTUAL, HWTYPE_CANCARDX,
			HWTYPE_CANAC2, HWTYPE_CANAC2PCI, HWTYPE_CANCARDXL, HWTYPE_CANCARD2 ..... ( look at driver manual )
	<li>Set the target platform USE_TARGET_SYSTEM vaiable to pc_win32
	</ol>
<li>select in the driver library specific source file the wanted thread or not thread state
<li>let update_makefile.sh create a project file for Visual Studio VC6 and above
<li>build the project with Visual Studio VC6 and above
<li>install the .dll of the used CAN driver library ( file from Vector Informatik ) into the
		DLL-Search-Path of your system ( or into the directory of your resulting EXE )
</ol>

This strategy can be mapped to other Win32 or LINUX CAN cards as well. Please contact the
<a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> for help on adoption and please
send a running adoption back to the project.


\subsection ScriptOptions Available Command Line Options for update_makefile.sh
The script update_makefile.sh provides several command line options which enables the
individual overwrite of target specific project settings. Thus you can use a project config
file, which reflects for example a pc_linux platform, to get a project file for pc_win32
( Dev-C++ and Visual Studio VC6 DSP project file ).
The following parameters are available ( you can get this list by calling the script with
"./update_makefile.sh --help" ).
\code
Usage: ./update_makefile.sh [OPTION] project_config_file
Create filelist, Makefile and configuration settings for a IsoAgLib project.

  -h, --help                        print this message and exit.
  --doxygen-export-directory=DIR    write the filelist and configuration files with doxygen documentation
                                    blocks to the given directory instead of the default directory of all generated files.
  --IsoAgLib-root=DIR               use the given root directory instead of the entry in the selected configuration file.
  --target-system=TARGET            produce the project definition files for the selected TARGET instead of the
                                    target which is specified in the configuration file
                                    ( "pc_linux"|"pc_win32"|"esx"|"imi"|"pm167"|"mitron167" ).
  --pc-can-driver=CAN_DRIVER        produce the project definition files for the selected CAN_DRIVER if the project shall run on PC
                                    ( "simulating"|"sys"|"rte"|"vector_canlib"|"vector_xl" ).
  --pc-rs232-driver=RS232_DRIVER    produce the project definition files for the selected RS232_DRIVER if the project shall run on PC
                                    ( "simulating"|"sys"|"rte" ).

./update_makefile.sh parses the selected project configuration file and overwrites the default values for all contained settings.
It collects then the corresponding files which can then be imported to an individual IDE.
Additionally a project specific configuration header is created in the directory xgpl_src/Application_Config with the
name scheme ".config_<project_name>.h". If the #define PRJ_USE_AUTOGEN_CONFIG is set to ".config_<project_name>.h",
the central configuration header xgpl_src/Application_Config/isoaglib_config.h will include this header.
Please set additionally the SYSTEM_FOO for the wanted platform - $0 will output the correct define at the end of the
run.
Thus with these two DEFINE settings, the compiler can generate a clean running executable / HEX.

Report bugs to <Achim.Spangler@osb-ag:de>.

\endcode

The script parses the given project feature set selection and configuring file and<ol>
<li>selects all needed project file ( thus you don't have do manually select the neded files,
	a big part of the files can be excluded for simple projects )
<li>generate a project specific configuration header, which is placed in the directory
	"xgpl_src/Application_Config/" ( you can trigger the compiler to include it by simply
	setting the compile time DEFINE PRJ_USE_AUTOGEN_CONFIG to ".config_<project_name>.h"
<li>create suitable project files for the following build systems:<ul>
	<li>LINUX: make in the directory "compiler_projects/kdevelop_qmake/"
	<li>Win32: Dev-C++ in the directory "compiler_projects/Dev-C++/"
	<li>Win32: Visual Studio VC6 in the directory "compiler_projects/VC6/"
	<li>Win32 for Embedded: Tasking EDE in the directory "compiler_projects/EDE/"
	</ul>
<li>tells you which DEFINES must be set for compile, if none of the automatically created project files will be used.
</ol>

\subsection FeatureSetupFile Contents of Feature Setup File

The features and the corresponding source modules and configuration defines can be specified by configuration variables
in feature setup files. The variables can be grouped to several specification tasks.

The specification files for the tutorial examples list all configuration variables, even if just their
default value is used. The default state is OFF ( value 0 ) for most variables. You don't have to write
the default values in the specification of your own project.

\subsubsection ConfProjectIdent Project Identification

The project can be identified by:
	- <b>PROJECT</b> project name, which is used for the project file and project directory
	- <b>PROJECT_AUTHOR</b> author name, which is inserted into project file ( where this value is defined )
	- <b>PROJECT_AUTHOR_EMAIL</b> author email address, which is also inserted into project file ( where this value is defined )

\subsubsection ConfApplicationFiles Selection of Application Sources

The update_makefile.sh script can collect your individual source files into the project file of the wanted IDE.
This can be achived by the following settings.
	- <b>REL_APP_PATH</b> path in relation to IsoAgLib base directory to base directory of all application sources
			( use "../" to navigate to parent directory; use UNIX style directory separator "/"  )
	- selection of source files in two alternative ways:
		-# <b>APP_SRC_FILE</b> list single files seperated by whitespace
		-# Use search expression which can be defined with:
			- <b>APP_SEARCH_SRC_CONDITION</b> file extensions to select as source module ( default is "*.cc *.c *.cpp" )
			- <b>APP_SEARCH_HDR_CONDITION</b> file extensions to select as header ( default is "*.h *.hpp" )
			- <b>APP_PATH_EXLCUDE</b> paths to exclude from search - you can use joker "*" to simply define only part of the to be excluded path
			- <b>APP_SRC_EXCLUDE</b> files to exclude from search - you can use joker "*" to exclude all files which match to a specific pattern ( e.g. "test*.cpp" )

\subsubsection ConfBaseSetting Basic Project Settings
The following basic settings can be specified independend from hardware and feature setup:
	- <b>ISO_AG_LIB_PATH</b> relative path from directory where the feature setup file is located to the root directory of IsoAgLib ( "../.." for the tutorial examples )
	- <b>PRJ_DEFINES</b> individual #defines which are integrated into the project file - some examples for special debug purposes:
		- <i>DEBUG_CAN_BUFFER_FILLING</i> for RS232 output of MAX amount of buffered CAN messages
		                                  ( important to decide on amount of items in the BIOS/OS CAN receive buffer )
		- <i>DEBUG_HEAP_USEAGE</i> for RS232 output of amount of bytes which are needed of one
		                           item in the different types of dynamic lists: MsgObj_c , FilterBox_c , IsoItem_c , DinItem_c ,
		                           MeasureSubprog_c , SetpointRegister ;
		                           additionally print calculated amount of HEAP memory which is used by the several types of
		                           dynamic lists ( output on each list change )
	- <b>PRJ_SEND_DEBUG</b> activate several IsoAgLib internal debugging on RS232
	- <b>USE_FLOAT_DATA_TYPE</b> activate useage of float variable type ( avoid useage of float in subfunctions process data, RS232 output, GPS decode )

\subsubsection ConfBasicHwSetup Basic Hardware Setup
The basic hardware setup of your target ( including CAN hardware/driver ) can be defined in this group.
	- <b>USE_TARGET_SYSTEM</b> type of target system from list <i>{ "pc_linux" | "pc_win32" | "esx" | "imi" | "pm167" | "mitron167" }</i> or other coming supported HALs
	- <b>USE_CAN_DRIVER</b> CAN driver to use from list <i>{ "simulating"|"sys"|"rte"|"vector_canlib"|"vector_xl" }</i>,<br>
			- where type "simulating" uses file based CAN simulation on each PC system target,
			- target "pc_win32" allows additionally <i>{ "vector_canlib"|"vector_xl" }</i>,
			- and target "pc_linux" allows additionally <i>{ "rte" }</i>.
			- The other embedded targets allow only the system specific CAN driver - identified by "sys"
	- <b>USE_RS232_DRIVER</b> RS232 driver to use from list <i>{ "simulating"|"sys"|"rte" }</i>
			- where type "simulating" uses file based RS232 simulation on each PC system target,
			- "sys" uses the native RS232 of every target
			- and "rte" uses the RS232 interface of the RTE system
	- <b>CAN_BUS_CNT</b> count of available CAN BUSes at target - default 1
	- <b>CAN_INSTANCE_CNT</b> count of CAN channels which shall be supported by IsoAgLib ( the application can also use the extended CAN
	                          features of IsoAgLib so that e.g. channel 0 can be used for protocol and channel 1 for internal CAN communication )<br>
	                          Must be smaller or equal as CAN_BUS_CNT.
	- <b>PRT_INSTANCE_CNT</b> count of CAN channels where the communication protocol shall be supported - the IsoAgLib can be used as gateway to
	                          route traffic between the connected CAN interfaces ( or you could support DIN 9684 and ISO 11783 at two seperate
	                          connectors in parallel time - this is really working in practise )

\subsubsection ConfCoreComm Core Communication Setup
Setup of most important communication features:
	- <b>PRJ_ISO11783</b> selection of ISO 11783 protocol in the project
	- <b>PRJ_ISO_TERMINAL</b> selection of ISO 11783 Virtual Terminal support ( only available if PRJ_ISO11783 is activated )
	- <b>PRJ_DIN9684</b> selection of DIN 9684 protocol in the project
	                     ( both ISO and DIN can be used in a project in parallel - e.g. decide on system init on used protocol )
	- <b>PRJ_DIN_TERMINAL</b> selection of LBS+ Terminal support ( like Fendt Varioterminal ) ( only available if PRJ_DIN9684 is activated )
	- <b>PRJ_BASE</b> gather base data like tractor speed, distance, ISO GPS, PTO RPM from BUS and make them available independend of receive time

\subsubsection ConProcData Process Data Communication Setup
Setup of process data support:
	- <b>PRJ_PROCESS</b> overall activation of process data support
	- <b>PRJ_GPS</b> activation of DIN 9684 GPS data communication as used by DIN 9684 Fieldstar Terminals
	- <b>PROC_LOCAL</b> overall activation of local process data - local means that the corresponding ECU measures the data which can be requested by remote ECUs
		- <b>PROC_LOCAL_STD</b> select standard local process data iProcDataLocal_c with measure programs and enhanced management of received setpoints
		- <b>PROC_LOCAL_SIMPLE_MEASURE</b> select iProcDataLocalSimpleMeasure_c for local process data without measure programs to reduce
		     HEAP useage ( all measurement programs are stored on HEAP, as amount of active programs depend on remote ECUs )
		- <b>PROC_LOCAL_SIMPLE_SETPOINT</b> select iProcDataLocalSimpleSetpoint_c for local process data without selective reaction on received setpoints
		     ( simply store received setpoints and automatically ACK the receive of a setpoint )
		- <b>PROC_LOCAL_SIMPLE_MEASURE_SETPOINT</b> select iProcDataLocalSimpleSetpointSimpleMeasure_c for local process data with smallest resource needs - and most
		     restricted feature set
	- <b>PROC_REMOTE</b> overall activation of remote process data - remote means that the corresponding ECU requests measurement data from a remote ECU
		- <b>PROC_REMOTE_STD</b> select iProcDataRemote_c for remote process data which can use measure programs to request periodic measurement value update
		     and which provides setpoint management which can detect current master setpoint ( setpoint which is ACKed by commanded device ) and the self
		     requested value ( information if own setpoint is ACKed and if own requested setpoint interval is currently maintained by remote
		     device - this can happen if intersection between ACKed setpoint request and own setpoint request is _not_ empty, so that commanded implement can
		     satisfy both )
		- <b>PROC_REMOTE_SIMPLE_MEASURE</b> select iProcDataRemoteSimpleMeasure_c if capable setpoint management is needed, but simple one shot measurement value request is enough
		     ( reduce HEAP useage, as the potential parallel active increments are stored in dynamic lists at HEAP )
		- <b>PROC_REMOTE_SIMPLE_SETPOINT</b> select iProcDataRemoteSimpleSetpoint_c if measurement programs are needed, but no or only simple setpoint control is wanted
		     ( reduce memory useage, as no management data is needed for detection of active setpoint in relation to own setpoint wish )
		- <b>PROC_REMOTE_SIMPLE_MEASURE_SETPOINT</b> select iProcDataRemoteSimpleSetpointSimpleMeasure_c if only one shot measurement value requests and no setpoint handling is needed
		     ( in relation to iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c the single setpoint and the single measurement value can represent
		     different values )
		- <b>PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED</b> select iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c for a very simple approach on remote process data.
		     This is only useful if no distinguishing between setpoint and measurement value is needed

\subsubsection ConfOptDriver Optional Driver Selection
The ISO<i><sub>AgLib</sub></i> needs for basic communication service only access to core system functions like time ( optional
trigger watchdog ) and CAN. But as the author used it also for other I/O types during the initiating research project, and as
some other I/Os can deliver some important additional features to ISO<i><sub>AgLib</sub></i> core components, they are also
provided - and can be selected by the following configuration variables.
	- <b>PRJ_EEPROM</b> select if the EEPROM driver shall be used, to read/write EEPROM data from within the IsoAgLib. This is important for local
		   process data, as IsoAgLib can store intermediate values in EEPROM so that the value can be re-read on each system startup
		   ( important for cumulative values like distance, when the application has only to calculate the distance increments of the
		   current working session, and the IsoAgLib provides the cumulated value on BUS;<br>
		   it's also important to store the current SA on ISO 11783 address claim
		   --> try to reuse SA os last working session on next system start.<br>
		   the application has only to specify to EEPROM address to handle the SA;
		   IsoAgLib takes then care of storing and reloading the SA )
	- <b>PRJ_RS232</b> select iRS232IO_c for RS232 output which can be used like the standard C++ iostreams
	- <b>PRJ_SENSOR_DIGITAL</b> select iDigitalO_c ( one instance/variable of this class per channel ) for digital On/Off input
	     ( provides automatic range check which uses diagnose function of HAL - BIOS/OS - to check if current is in allowed interval );
	     use iActorO_c for global access to all active iDigitalO_c instances
	- <b>PRJ_SENSOR_ANALOG</b> select iAnalogI_c for analog ( current or voltage ) sensor input or iAnalogIRangeCheck_c with enhanced automatic range check
	- <b>PRJ_SENSOR_COUNTER</b> select iCounterI_c for counter input. the existing HAL implementations ( e.g. for ESX ) extend the BIOS/OS
	     function, as a IsoAgLib specific event counting for very low event repetition rates is provided - the BIOS version is only
	     designed for typical RPM values
	- <b>PRJ_ACTOR</b> select if the digital output driver shall be integrated into the project. you can then use the HAL of IsoAgLib to realize a mostly hardware independend
	     access on PWM output. This is completely independent from communication tasks, but was created during research and is actively used by OSB AG for its own
	     projects

\subsubsection ConfBuildLinux Build System Configuration for LINUX
In normal cases the update_makefile.sh script can find all needed helper programs on its own.
But in some distributions, the tool <b>qmake</b> of Trolltech might be installed at a very uncommon place.
The following places are search automatically:
	- call system program <b>which qmake</b> to find qmake, if it is located in the program search path of the system
	- search in directories with pattern "/usr/lib/qt*" or "/usr/share/*" ( there should be only one version of qmake in the matched paths )

\subsubsection ConfBuildEde Build System Configuration for Cross Compilers ( like Tasking EDE )
Configuration options typical for embedded project ( these are used for target systems like "esx", "imi", "pm167" )
where the system libraries aren't placed in standard directories ( UNIX/LINUX has standard directories /usr/include, /usr/local/include, /usr/lib
and /usr/local/lib -> IsoAgLib awaits the libraries in this directories for UNIX/LINUX ).<br>
These settings are at the moment only used for project generation of
	- Tasking EDE

The following config variables should match the need of most cross compilers:
	- <b>USE_EMBED_COMPILER_DIR</b> path to the embedded compiler
	- <b>USE_EMBED_LIB_DIRECTORY</b> path ( relative to ISO<i><sub>AgLib</sub></i> root or absolute )
	     to the libraries of the embedded BIOS / OS ( please seperate entries by whitespace and use only UNIX style directory seperator "/" )
	- <b>USE_EMBED_HEADER_DIRECTORY</b> path ( relative to ISO<i><sub>AgLib</sub></i> root or absolute )
	     to the headers of the embedded BIOS / OS ( please seperate entries by whitespace and use only UNIX style directory seperator "/" )
	- <b>USE_EMBED_LIBS</b> needed library files ( please seperate entries by whitespace )
	- <b>USE_EMBED_BIOS_SRC</b> special source files from the BIOS/OS which must be inserted into the sources list
	     ( e.g. startup files which could be delivered as asm file and must be compiled during runtime,
	     as the BIOS/OS allows there some individual adoption ) ( please seperate entries by whitespace )
	- <b>USE_EMBED_ILO</b> linker / locater control file which defines HEAPSIZE and other important project specific target settings for link time

\subsubsection ConfBuildWin32 Build System Configuration for Win32 ( like Visual C++ and Dev-C++ )
The update_makefile.sh script can create project files for Visual C++ ( DSP ) and Dev-C++.
As the Open Souce IDE Dev-C++ is really good, it is strong recommended to use to compile the vt2iso tool.
But the driver DLLs for Vector-Informatik CAN cards don't interact with Dev-C++ built EXE programs.
So update_makefile.sh was extended to write DSP project files, which are meant for VC6 and can be imported
into newer version of Microsoft Visual Studio VC++.<br>
As VC6 version of the <b>S</b>tandard <b>T</b>emplate <b>L</b>ibrary is really not standard conformant, you need
to install STLport as described in \ref Vc6StlPort. Please specify the path to STLport, if you didn't place it into C:\ and rename STLport-3.6.2 to STLport:
	- <b>USE_STLPORT_HEADER_DIRECTORY</b> directory to the <i>stlport</i> directory within the STLport package - the C++ headers are located there
	- <b>USE_STLPORT_LIBRARY_DIRECTORY</b> path to STLport libraries in case you selected the option to use the STLport version of IOSTREAMS ( and not the native version as advised by the doc at \ref Vc6StlPort )
	    let empty if native IOSTREAMS library is used

The next configuration variables are needed for Vector-Informatik CAN cards.
As Vector-Informatik provides two types of programming libraries which can be used for the same cards, the
ISO<i><sub>AgLib</sub></i> reflects this option. To ease directory management, ISO<i><sub>AgLib</sub></i>
awaits the potentially parallel installed versions rooted in the same parent directory.<br>
E.g when you selected "D:/Development" as base directory during the SETUP of both CAN program linbraries,
you will get
	- the CANLIB driver in "D:/Development/CANLIB"
	- the XL Driver Library in "D:/Development/XL Driver Library"
Now ISO<i><sub>AgLib</sub></i> can access both of them if you specify:
	- <b>USE_WIN32_LIB_DIRECTORY="D:/Development"</b>,
	- <b>USE_WIN32_HEADER_DIRECTORY="D:/Development"</b>
which is the default setting. The script <i>update_makefile.sh</i> will find both versions of the driver.

The ISO<i><sub>AgLib</sub></i> HAL adoption to the Vector-Informatik CAN driver can be configured by the configuration file
for a specific hardware type ( list <i>{ HWTYPE_VIRTUAL|HWTYPE_CANCARDX|HWTYPE_CANAC2|HWTYPE_CANAC2PCI }</i> ).
As a special option, ISO<i><sub>AgLib</sub></i> can use all real hardware channels it finds, so that your EXE can be flexibly
used for different CAN card types of Vector-Informatik. Your can use this option, if you select <b>HWTYPE_AUTO</b> as hardware
type. In case only the Vector-Informatik Virtual Channel is found, then these typically two channels are used instead.


The setup file can be derived from the following example
( see compiler_projects/projectGeneration/conf_0_0_AddressClaimIso ):

\code
# Filename: conf_0_0_AddressClaimIso
# Configuration setting for tutorial example 0_0_AddressClaimIso
# in the tutorial group 0_SystemMgmt

# ###############################################################
# Project Identification
# ###############################################################

# define project name - used for directory with makefile
PROJECT=0_0_AddressClaimIso

# define the project author
# - used for IDE project file types, where the author is stored in the project file
PROJECT_AUTHOR=MyName
PROJECT_AUTHOR_EMAIL=MyEmail@Location

# ###############################################################
# Selection of Application Sources
# ###############################################################

# define the path where the aplication can be found
# ( in relation to the root of the IsoAgLib
#    - ONLY RELATIVE PATH )
# seperate files by whitespace
REL_APP_PATH="IsoAgLib_Examples/tutorial/0_SystemMgmt/0_0_AddressClaimIso"
# define the name of the app
# ( if this variable is not set, then
#   all files of the selected directories are used )
# seperate files by whitespace
APP_SRC_FILE="0_0_AddressClaimIso.cpp"
# define the file extensions to include for the
# application source file search ( default: *.c *.cc *.cpp *.h *.hpp )
# APP_SEARCH_SRC_CONDITION="*.cc *.c *.cpp"
# APP_SEARCH_HDR_CONDITION="*.h *.hpp"
# define pathes ( jokers like "*" are allowed ) which shall be
# excluded from application file search
# ( example:
#    - exclude path Fii somewhere in deep dir: ".../Fii/..."
#    - exclude file Foo.cpp as part of directory Fii: ".../Fii/Foo.c"
#      ( includes occurences of Foo.cpp in other directories!! )
#  )
# APP_PATH_EXLCUDE=""
# define filenames which shall be excluded in all directories
# ( jokers like "*" are allowed - example "test*.cpp" )
# APP_SRC_EXCLUDE=""

# ###############################################################
# Selection of Application Sources
# ###############################################################

# define path to the main directory if the IsoAgLib
# - i.e. where both xgpl_src and IsoAgLibExamples are located
ISO_AG_LIB_PATH="../.."

# optionally set individual DEFINES - seperated by whitespace
# ( e.g. DEBUG to activate some debug messages )
# Some debugging defines:
# - DEBUG for different internal IsoAgLib information
# - DEBUG_CAN_BUFFER_FILLING for RS232 output of MAX amount of buffered CAN messages
#   ( important to decide on amount of items in the BIOS/OS CAN receive buffer )
# - DEBUG_HEAP_USEAGE for RS232 output of amount of bytes which are needed of one
#   item in the different types of dynamic lists: MsgObj_c , FilterBox_c , IsoItem_c , DinItem_c ,
#   MeasureSubprog_c , SetpointRegister ;
#   additionally print calculated amount of HEAP memory which is used by the several types of
#   dynamic lists ( output on each list change )
# PRJ_DEFINES=""

# select if Debug messages within the IsoAgLib shall be sent via RS232
# ( if this is activated, the PRJ_RS232 setting is activated -> integrate
#   the IsoAgLib RS232 driver extension to your project )
# PRJ_SEND_DEBUG=1

# select if the project shall use the float data type
# default is FALSE as far as possible
# ( this has an impact on process data, RS232 output,
#   GPS decode )
# USE_FLOAT_DATA_TYPE=0

# ###############################################################
# Basic Hardware Setup
# ###############################################################

# select watned type of target system: "pc_linux" | "pc_win32" | "esx" | "imi" | "pm167" | "mitron167"
USE_TARGET_SYSTEM="pc_linux"
# select wanted type of CAN driver connection: "simulating"|"sys"|"rte"|"vector_canlib"|"vector_xl"
# targets other than pc_linux or pc_win32 can only use "sys"
# only target "pc_win32" can use the driver "vector_canlib"|"vector_xl"
# only target "pc_linux" can us the driver "rte"
USE_CAN_DRIVER="simulating"
# select wanted type of RS232 driver connection: "simulating"|"sys"|"rte"
# targets other than pc_linux or pc_win32 can only use "sys"
# only target "pc_linux" can us the driver "rte"
USE_RS232_DRIVER="simulating"

# define the amount of available CAN BUSes at the ECU
# ( default 1 )
CAN_BUS_CNT=1

# define the amount of CAN instances to support
# ( all values lower than 1 are overwritten by default 1 )
CAN_INSTANCE_CNT=1

if [ $CAN_INSTANCE_CNT -gt 1 ] ; then
	# if more than one CAN BUS is defined, the selected
  # protocol(s) can be executed as independent instances
  # at more than one channel
  # ( if more than one CAN instance is used, but
  #   only one channel is used for protocol, than
  #   the index 0 - i.e. the first item is used for protocol )
  PRT_INSTANCE_CNT=1
else
	# default is to run only one protocol instance
  PRT_INSTANCE_CNT=1
fi

# ###############################################################
# Core Communication Setup
# ###############################################################

# set value of feature to 1 if wanted
# unset or set to 0 if not wanted
PRJ_ISO11783=1
# PRJ_ISO_TERMINAL=0
# PRJ_DIN9684=0
# PRJ_DIN_TERMINAL=0

# select if base data like tractor speed, distance, ISO GPS, PTO RPM
# shall be gathered from BUS ( either receive or send )
# PRJ_BASE=0

# ###############################################################
# Process Data Communication Setup
# ###############################################################

# activate globally the use of process data
# PRJ_PROCESS=0

# for DIN 9684 the process data system can be used
# to decode GPS information from Fieldstar or LBS+ extension
# ( if this and DIN9684 is activated, but PRJ_PROCESS is not
#   then the value of PRJ_PROCESS is overwritten with 1 to activate it )
# PRJ_GPS=0

# decide if local process data are wanted
# PROC_LOCAL=0

# select the feature set of the wanted local process data

# select standard local process data iProcDataLocal_c with
# measure programs and enhanced management of received setpoints
# PROC_LOCAL_STD=0

# select iProcDataLocalSimpleMeasure_c for
# local process data without measure programs to reduce
# HEAP useage ( all measurement programs are stored on HEAP, as
# amount of active programs depend on remote ECUs )
# PROC_LOCAL_SIMPLE_MEASURE=0

# select iProcDataLocalSimpleSetpoint_c for
# local process data without selective reaction on received setpoints
# ( simply store received setpoints and automatically ACK the receive of
#   a setpoint )
# PROC_LOCAL_SIMPLE_SETPOINT=0

# select iProcDataLocalSimpleSetpointSimpleMeasure_c for
# local process data with smallest resource needs - and most
# restricted feature set
# PROC_LOCAL_SIMPLE_MEASURE_SETPOINT=0

# decide if remote process data are wanted
# PROC_REMOTE=0

# select the feature set of the wanted remote process data

# select iProcDataRemote_c for remote process data which can
# use measure programs to request periodic measurement value update
# and which provides setpoint management which can detect current master
# setpoint ( setpoint which is ACKed by commanded device ) and the self
# requested value ( information if own setpoint is ACKed and if own
# requested setpoint interval is currently maintained by remote device
# - this can happen if intersection between ACKed setpoint request and
# own setpoint request is _not_ empty, so that commanded implement can
# satisfy both )
# PROC_REMOTE_STD=0

# select iProcDataRemoteSimpleMeasure_c if capable setpoint management
# is needed, but simple one shot measurement value request is enough
# ( reduce HEAP useage, as the potential parallel active increments are
#   stored in dynamic lists at HEAP )
# PROC_REMOTE_SIMPLE_MEASURE=0

# select iProcDataRemoteSimpleSetpoint_c if measurement programs are needed,
# but no or only simple setpoint control is wanted
# ( reduce memory useage, as no management data is needed for detection
#   of active setpoint in relation to own setpoint wish )
# PROC_REMOTE_SIMPLE_SETPOINT=0

# select iProcDataRemoteSimpleSetpointSimpleMeasure_c if only
# one shot measurement value requests and no setpoint handling is needed
# ( in relation to iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c
#   the single setpoint and the single measurement value can represent
#   different values )
# PROC_REMOTE_SIMPLE_MEASURE_SETPOINT=0

# select iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c for
# a very simple approach on remote process data.
# This is only useful if no distinguishing between setpoint and measurement
# value is needed
# PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED=0

# ###############################################################
# Optional Driver Selection
# ###############################################################

# select if the EEPROM driver shall be used, to read/write EEPROM
# data from within the IsoAgLib. This is important for local
# process data, as IsoAgLib can store intermediate values in EEPROM
# so that the value can be re-read on each system startup
# ( important for cumulative values like distance, when the
#   application has only to calculate the distance increments of the
#   current working session, and the IsoAgLib provides the cumulated
#   value on BUS;
#   it's also important to store the current SA on ISO 11783 address claim
#   --> try to reuse SA os last working session on next system start.
#       the application has only to specify to EEPROM address to handle the SA;
#       IsoAgLib takes then care of storing and reloading the SA )
# PRJ_EEPROM=0

# select iRS232IO_c for RS232 output which can be used like the
# standard C++ iostreams
# PRJ_RS232=0

# select iDigitalO_c ( one instance/variable of this class per channel )
# for digital On/Off input
# ( provides automatic range check which uses diagnose function of HAL
#  - BIOS/OS - to check if current is in allowed interval );
# use iActorO_c for global access to all active iDigitalO_c instances
# PRJ_SENSOR_DIGITAL=0

# select iAnalogI_c for analog ( current or voltage ) sensor input.
# or iAnalogIRangeCheck_c with enhanced automatic range check
# PRJ_SENSOR_ANALOG=0

# select iCounterI_c for counter input.
# the existing HAL implementations ( e.g. for ESX ) extend the BIOS/OS
# function, as a IsoAgLib specific event counting for very low
# event repetition rates is provided - the BIOS version is only
# designed for typical RPM values
# PRJ_SENSOR_COUNTER=0

# select if the digital output driver shall be integrated into the project.
# you can then use the HAL of IsoAgLib to realize a mostly hardware independend
# access on PWM output. This is completely independent from communication tasks,
# but was created during research and is actively used by OSB AG for its own
# projects
# PRJ_ACTOR=0


# ###############################################################
# Build System Configuration for LINUX
# ###############################################################

# optionally define path to qmake
# if it can be automatically found by
# + which qmake ( e.g. in search path for binaries )
# + can be found with sys-cmd find beneath /usr/lib/qt*
#   ( there should be only one version of qmake in this path
#      -> if there are more qt installations, please manually define
#          it by uncommenting and adopting the following line )
# QMAKE_PATH=/usr/lib/qt3/bin/qmake

# ###############################################################
# Build System Configuration for Cross Compilers ( like Tasking EDE )
# ###############################################################

# #############################################
# Configuration options typical for embedded project
# ( these are used for target systems like "esx",
#   "imi", "pm167" )
# where the system libraries aren't placed in standard
# directories ( UNIX/LINUX has standard directories
# /usr/include, /usr/local/include, /usr/lib
# and /usr/local/lib -> IsoAgLib awaits the libraries
# in this directories for UNIX/LINUX )
# These settings are at the moment only used for project
# generation of
# + Tasking EDE
#
# But this could be extended to other compilers.
# These config settings allow to create project files
# which can be used to generate runnable HEX out of the
# box from automatic generation
# #############################################

# SETTINGS FOR EMBEDDED PROJECTS WITHIN CROSS COMPILER
# specify the path to the embedded compiler
# USE_EMBED_COMPILER_DIR="c:/programme/tasking/7.56/c166"

# specify the path ( relative to IsoAgLib root or absolute )
# to the libraries of the embedded BIOS / OS
# ( please seperate entries by whitespace )
# USE_EMBED_LIB_DIRECTORY="commercial_BIOS/bios_esx"

# specify the path ( relative to IsoAgLib root or absolute )
# to the headers of the embedded BIOS / OS
# ( please seperate entries by whitespace )
# USE_EMBED_HEADER_DIRECTORY="commercial_BIOS/bios_esx"

# specify the needed library files
# ( please seperate entries by whitespace )
# USE_EMBED_LIBS="C756/Xos20l.lib Module/Xma20l.lib"

# specify special source files from the BIOS/OS
# which must be inserted into the sources list
# ( e.g. startup files which could be delivered as
#   asm file and must be compiled during runtime,
#   as the BIOS/OS allows there some individual adoption )
# ( please seperate entries by whitespace )
# USE_EMBED_BIOS_SRC="Xos20go.asm Xos20err.c xos20esx.h XOS20EEC.H XOS20EEC.OBJ"

# specify the needed linker / locater control file
# USE_EMBED_ILO="Xos20lcs.ilo"

# ###############################################################
# Build System Configuration for Win32 ( like Visual C++ and Dev-C++ )
# ###############################################################

# specify path to headers of STLport - default corresponds to \ref Vc6StlPort
# USE_STLPORT_HEADER_DIRECTORY="C:/STLport/stlport"

# specify path to STLport libraries in case you selected the option to use the STLport
# version of IOSTREAMS ( and not the native version as advised by the doc at \ref Vc6StlPort )
# let empty if native IOSTREAMS library is used
# USE_STLPORT_LIBRARY_DIRECTORY=""

# SETTINGS FOR WIN32 PROJECTS WITH WIN32 CAN CARD
# ( esp. Vector Informatik )
# + Dev-C++ (esp. needed for Win32 CAN driver integration
#             which is at the moment provided for
#             Vector Informatik canlib43 and XL Driver Library )
# specify the path ( relative to IsoAgLib root or absolute )
# to the WIN32 CAN libraries
# IMPORTANT: Set the path to the directory where you installed
# the CAN driver library during its SETUP.EXE process
# As the individual library file sets are at fixed places
# within a CAN driver installation, the IsoAgLib will
# select the library files automatically
# ==>> this path can be used without changes, if several
#      driver version like "CANLIB" and "XL Driver Library"
#      are installed in parallel
#      ( all IsoAgLib parts will access the individual
#        headers and libraries with the addition of the
#        default directory name from the SETUP.EXE
#        e.g. "<USE_WIN32_LIB_DIRECTORY>/CANLIB"
#          or "<USE_WIN32_LIB_DIRECTORY>/XL Driver Library" )
# USE_WIN32_LIB_DIRECTORY="D:/Development"

# specify the path ( relative to IsoAgLib root or absolute )
# to the WIN32 CAN headers
# IMPORTANT: Set the path to the directory where you installed
# the CAN driver library during its SETUP.EXE process
# ==>> this path can be used without changes, if several
#      driver version like "CANLIB" and "XL Driver Library"
#      are installed in parallel
#      ( all IsoAgLib parts will access the individual
#        headers and libraries with the addition of the
#        default directory name from the SETUP.EXE
#        e.g. "<USE_WIN32_LIB_DIRECTORY>/CANLIB"
#          or "<USE_WIN32_LIB_DIRECTORY>/XL Driver Library" )
# ( please seperate entries by whitespace )
# USE_WIN32_HEADER_DIRECTORY="D:/Development"

# specify the wanted Hardware Type of the CAN card
# ( esp. Vector Informatik CAN drivers can be used for several
#   CAN cards of their product line )
# Examples: HWTYPE_AUTO|HWTYPE_VIRTUAL|HWTYPE_CANCARDX|HWTYPE_CANAC2|HWTYPE_CANAC2PCI
#           HWTYPE_CANCARDXL|HWTYPE_CANCARD2
# ( refer their documentaion to find more types )
# ( IsoAgLib places automatically "XL_" before the card define,
#   if the XL driver is selected --> DONT'T set the XL_ here again
#   ==> this allows simpler switch between different CAN driver libs )
# USE_WIN32_CAN_HW_TYPE="HWTYPE_AUTO"

\endcode





</td></tr>
  </table>
*/


/*! \page information Development Targets and Usage Preconditions of IsoAgLib

<center>
<h1>Object Oriented Program Library ISO<i><sub>AgLib</sub></i></h1>
<h1>Development of ISO 11783 and DIN 9684 Applications</h1>
<h2>Copyright &copy; 1999 - 2004 Achim Spangler, Martin Wodok</h2>
<h2>Licensed with Exceptions under the General Public License (GPL) of the Free Software Foundation</h2>
</center>

<table width="100%" border="0" cellspacing="0" cellpadding="2">
<tr><td valign="Top" nowrap="true" >
\ref IndexWhereToStart<br>
\ref IndexNews<br>
\ref IndexChanges<br>
\ref IndexMainFeatures<br>
\ref IndexMemoryUsage<br>
\ref IndexStructuralOverview<br>
\ref IndexLicenseConditions<br>
\ref IndexAchnowledgements<br>
\ref IndexInitialSupport<br>
\ref IndexIsoVtExtension<br>
\ref IndexFirstWin32Users<br>
\ref IndexAuthors<br>
\ref IndexMaintainingCompany<br>
\ref IndexSupport<br>
\ref IndexTodo<br>
\ref IndexUsingProjects<br>
\ref HowtoDownload<br>
\ref HowtoAccessRepository<br>
\ref HowtoGetHelp<br>
\ref HowtoCreateDoc<br>
\ref HowtoLearnIsoAgLib<br>
\ref HowtoSetupCompilerTools<br>
\ref InstallIsoToolVt2Iso<br>
\ref HowtoCreateProjectFiles<br>
\ref InfGeneralInformation<br>
\ref InfAgentNetwork<br>
\ref InfServiceNetwork<br>
\ref InfRequirements<br>
\ref FurtherDevelopment<br>
\ref FurtherExamples<br>
\ref QuestionsAnswers<br>
\ref FurtherReading<br>
<br>
&nbsp;<br>
&nbsp;<br>
&nbsp;<br>
Last Update: <br>06 December 2004<br>
by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
<br></td>
<td valign="Top">

\section InfGeneralInformation General Information
The ISO<i><sub>AgLib</sub></i> was primarily developed for massive automatic process data recording within the research group
<a href="http://www.tec.wzw.tum.de/pflanztech/englisch/forschung/forschung.html">IKB-D&uuml;rnast</a>
. Since publication of this project it is used for:
	- Implement Indicator (IMI) which detects working state dependent on configuration and tractor base
    data information (LBS-Base) and records process data:
		- working state
		- working width
		- working time
		- working distance
		- working area
		- whole time
		- whole distance
	- IMI for transport work, where no working width and area are point of interest
	- simulated tractor which sets LBS-Base data according to settings of analog sensor inputs
    and sends them on BUS, and which manages measuring programs for IMIs dependent on digital inputs (switch)
- Task Controller which retrieves GPS information from a Fieldstar (TM) terminal, combines them
    with dynamic detected process data of pre configured IMI types and polls the information
		via RS232 to a PC or PCMCIA Flashdisk in simple table format. Last but not least it measures the hitch force
		(with sensors of EHR) and the fuel consumption and polls them, too. The Task Controller
		detects dependent on GPS, if the actual position is in one of the configured (EEPROM)
		areas of type mounting, transport or field. With this it can gather the information
		for each area type in its own subset.


&nbsp;<br>

The main basic design principles of the ISO<i><sub>AgLib</sub></i> are:
	- use Hardware-Adaption-Layer to restrict hardware dependent parts, to
    ease adoption to different ECU types
	- use hardware independent Communication-Layer, which implements all interaction via ISO11783 or DIN9684/CAN
	- very capable and flexible design -&gt; e.g. it can handle received setpoints from a group of
    different senders, so that the main application can decide, which sender to accept
	- modularized structure to ease source code administration and to enable de/activation of
    specific function groups (e.g. RS232, sensor, actor)


&nbsp;<br>


The ISO<i><sub>AgLib</sub></i> version <b>1.0.0rc1</b> is running stable, as it's already used for the
<a href="http://www.fritzmeier.com/engl/frameset_engl.htm?/engl/environment/environment_miniveg.htm">Chlorophyll Sensor MiniVeg N</a>
of the manufacturer <a href="www.fritzmeier.de">Fritzmeier</a> . As it's verified with automated tests, that the final device
is fitting to all defined requirements, the ISO<i><sub>AgLib</sub></i> will at least provide a robust correct function at least for this application.

In case the ISO<i><sub>AgLib</sub></i> would suffer some problems for other applications, you can be sure that the author
<a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a> maintains this project actively.

\section InfAgentNetwork Modelling as Network of Autonomous Agents

The targeted flexible network, where devices of different manufacturers,
models and built years are providing and using general services can be seen
as a network of partial autonomous agents. K&uuml;hnel establishes the following
criteria for the use of agents in the view of the whole system (book: "Agentenbasierte
Softwareentwicklung", A.: Ralf K&uuml;hnel, V.: Addison-Wesley):
- inherent&nbsp;distribution of data, competence and leadership
- existence&nbsp;of specific software for sub tasks which should be integrated
- complexity of algorithms which could be subdivided in distributed parts
- tough or impossible design of global control
Criteria for the modelling as agents are in the view of the single devices:- need for adoption to frequently changing environments
- partial and possibly erroneous information about environment
- restricted prediction of environment
- possibility of environment&nbsp;changes which are only partially induced  by own activities
This matches the following properties of a LBS (DIN 9684 and ISO 11783) network:
- single devices (resp. their ECU) must have the competence and leadership for security relevant tasks
- controller software for single devices exists in many cases
- manufacturer of a device knows best, how construction independent information and work services can be realized by a specific machine
- global control would need a steadily increasing configuration management database to allow any desired combination of devices
- frequently change of the (network) environment
- a single implement or the tractor can't predict or control the work of the whole work combination
Therefore the ISO<i><sub>AgLib</sub></i> provides the following services:<br>

<table width="100%" border="0" cellspacing="0" cellpadding="2">
<tr><td valign="Top"><b>Knowledge Management System</b></td>
<td valign="Top">

-# application can simply store and update local process data information
-# optional&nbsp;periodic store of actual value in EEPROM<br>
-# periodic or one time information requests from remote devices are handled in the background
-# capable support for one time or periodic information request (including request of MIN or MAX on specific trigger events)
-# several remote devices can independently reset the stored
   value, if they have registered a measuring program (time or distance proportional
   or value dependent triggered sending of values)<br>
-# process data can be flexible created or deleted during runtime to adopt to actual information needs
-# all information types are handled with the same API
-# each information type (e.g. application rate) is addressed by an unique identifier
   which contains the device type (e.g. spreader), mounting position and location
   in the device specific dictionary so that dynamic source address or protocol
   type (DIN 9684 or ISO 11783) is irrelevant for the application design<br></td></tr>

<tr><td valign="Top"><b>Control Interaction</b><br></td>
<td valign="Top">

-# API of each process data has subgroups for measurement information and work control
-# setpoints can be defined as exact values, MIN or MAX limits or intervals<br>
-# information if sent setpoint was answered with accept or reject (differentiated by reason)
-# information if previously rejected setpoint is still consistent with the actual measurement value<br>
-# parallel received setpoints for the same process data are distinguished by sender
   ident, so that selective accept and reject is possible
-# accepted setpoint sender is marked as MASTER, so that
   succeeding setpoints of the MASTER can be distinguished from setpoints of
   other devices
-# optional automatic information of setpoint MASTER, if actual measurement value exceeds the configurable admissible tolerance

</td></tr></table>

\section InfServiceNetwork Service Network
The interactions between the devices of a DIN 9684 or ISO 11783 network can be modelled as
the providing and using of construction  independent services. This is illustrated
with the following diagrams.<br>
\image html Diagramme/service_network_connection.gif
&nbsp;<br>
&nbsp;<br>
&nbsp;<br>

The devices of this network are using and providing services as process data. The correct function of the elements is only
possible, if no device is dependent on a service of another not existent device.
<br clear="All">
<hr>
\image html Diagramme/service_network_conflict.gif
&nbsp;<br>
&nbsp;<br>
&nbsp;<br>Resource conflicts can occur, if several devices want to control the driving speed and RPM of PTO of the tractor.
Therefore each implement should avoid the sending of exact setpoints in favour of intervals.

<br clear="All">
<hr>
\image html Diagramme/service_network_flexibility.gif
&nbsp;<br>
&nbsp;<br>
&nbsp;Several implements which want to control the work of the tractor are no problem, if they define intervals instead of
exact values. Additionally they should use process data which are as abstract as possible. <br>
The rotary harrow could
define setpoints for driving speed and PTO of the tractor, so that the tractor would be fixed very tight to the needs
of this one implement. This wouldn't allow the fertilizer to control the driving speed.<br>
<br>
If the rotary harrow defines alternatively the relation between driving speed and PTO, the tractor can realize this
setting for different speeds (dependent on the transmission of the tractor). The tractor can
regard the setpoint from the fertilizer in this case.

<br clear="All">
\section InfRequirements Requirements
\subsection InfReqPlatform Platform
The ISO<i><sub>AgLib</sub></i> is running on the following systems (state 05 April 2004):
- ESX (based on Infineon C167) of <a href="http://www.sensor-technik.de">STW</a>
- Processor Module PM167 based on Infineon C167) of <a href="http://www.sensor-technik.de">STW</a>
- Implement Indicator (IMI) based on Infineon C167) of <a href="http://www.sensor-technik.de">STW</a>
- PC running with LINUX with real RS232 and CAN IO based on the Realtime Test Environment (RTE)
- PC running with LINUX with pure simulating ECU BIOS (CAN communication provided by file I/O)
- PC running with WINDOWS with pure simulating ECU BIOS (CAN communication provided by file I/O or Vector CANcardX)

\subsection InfReqCompiler Compiler
The object oriented design of the ISO<i><sub>AgLib</sub></i> is implemented
with C++ and uses the features namespaces and templates. The "Standard Template
Library" (STL) is used as proven and high quality implementation of dynamic
lists for monitor lists, process data, etc. .Therefore a modern (old versions
often implement some parts wrong or produce bad code) C++ compiler is needed.
Actual optimizing technologies allow program size and execution speed which
is equivalent or better than comparable C implementations.<br>
Suitable compilers exists for processors with at least 16 Bit (e.g. the C16x
series). For quick process data management (which are standardized as 32
Bit values) and more flexible heap memory access, a 32 Bit processor is appropriate.
But as all research work is performed with the ECU <b>ESX</b> of <a href="http://www.sensor-technik.de">STW</a>
 which uses the Infineon 16 Bit processor C167, a 32 Bit processor is not obligatory.<br>
From spring 2002 on all three well known compiler manufacturers (<a href="http://www.tasking.com/products/C166-ST10/home.html">Tasking</a>
, <a href="http://www.hightec-rt.com/">HighTec</a>
 and <a href="http://www.keil.com">KEIL</a>
) provide a suitable C++ compiler for C16x processors. The company <a href="http://www.comeaucomputing.com/">Comeau Computing</a>
 creates a C++ extension for a C compiler on demand. Quick high quality integration
of C++ into C compilers is possible with the help of the state of the art
C++ front-end of <a href="http://www.edg.com">EDG</a>
 which have a customer reference list of some <a href="http://www.edg.com/customers.html">well known brands</a>
.<br>
As soon as a project is compiled by a C++ aware compiler, a project can integrate
parts written in C and C++ without any problems.<br><h3>Memory</h3>
Dependent on the intensity of process data use, the ISO<i><sub>AgLib</sub></i>
 needs in most cases less than 16 Kbytes of RAM. More detailed documentation
has to be created with the new process data management, which doesn't occupy
heap memory for unneeded functions like setpoint management, when only measurement
values are communicated (new feature in 0.2.0).<br>
The ISO<i><sub>AgLib</sub></i> with the minimum module configuration needs
about 66 Kbytes Flash ROM. A configuration with full featured with process
data needs about 106 Kbytes Flash ROM.<br>
The RAM and Flash ROM size depends heavily on the configuration of the ISO<i><sub>AgLib</sub></i> and on the optimizing capabilities of the compiler.<br>
<h3>Programming Skills</h3>
As long as the ISO<i><sub>AgLib</sub></i> should only be used as it is, without
analysing or optimizing it, the needed additional C++ skills (in relation
to C) are small. The <b>24</b> current <a href="examplex.html">tutorial examples</a> and the three real
research world examples provide a sufficient introduction in the usage of
C++ and the ISO<i><sub>AgLib</sub></i>.<br>
As soon as the internals of the ISO<i><sub>AgLib</sub></i> should be understood, the needed level of C++ skills is increased dependent on the configuration of the ISO<i><sub>AgLib</sub></i>. Companies who are interested in a reliable guarantee that needed bugfixes
or extensions are realized by external developers (if local developers
are not familiar with C++ and the ISO<i><sub>AgLib</sub></i>) should contact <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
 to establish commercial support contracts including training, project attendance and ISO<i><sub>AgLib</sub></i> ISO 900x certification.<br>
<br><b>
Everybody who wants to get familiar with the ISO<i><sub>AgLib</sub></i> should start with the <a href="examplex.html">tutorial examples</a>.</b><br>
</td></tr>
  </table>
*/

/*! \page outlook Further Development and Applications of IsoAgLib

<center>
<h1>Object Oriented Program Library ISO<i><sub>AgLib</sub></i></h1>
<h1>Development of ISO 11783 and DIN 9684 Applications</h1>
<h2>Copyright &copy; 1999 - 2004 Achim Spangler, Martin Wodok</h2>
<h2>Licensed with Exceptions under the General Public License (GPL) of the Free Software Foundation</h2>
</center>


<table width="100%" border="0" cellspacing="0" cellpadding="2">
<tr><td valign="Top" nowrap="true" >
\ref IndexWhereToStart<br>
\ref IndexNews<br>
\ref IndexChanges<br>
\ref IndexMainFeatures<br>
\ref IndexMemoryUsage<br>
\ref IndexStructuralOverview<br>
\ref IndexLicenseConditions<br>
\ref IndexAchnowledgements<br>
\ref IndexInitialSupport<br>
\ref IndexIsoVtExtension<br>
\ref IndexFirstWin32Users<br>
\ref IndexAuthors<br>
\ref IndexMaintainingCompany<br>
\ref IndexSupport<br>
\ref IndexTodo<br>
\ref IndexUsingProjects<br>
\ref HowtoDownload<br>
\ref HowtoAccessRepository<br>
\ref HowtoGetHelp<br>
\ref HowtoCreateDoc<br>
\ref HowtoLearnIsoAgLib<br>
\ref HowtoSetupCompilerTools<br>
\ref InstallIsoToolVt2Iso<br>
\ref HowtoCreateProjectFiles<br>
\ref InfGeneralInformation<br>
\ref InfAgentNetwork<br>
\ref InfServiceNetwork<br>
\ref InfRequirements<br>
\ref FurtherDevelopment<br>
\ref FurtherExamples<br>
\ref QuestionsAnswers<br>
\ref FurtherReading<br>
<br>
&nbsp;<br>
&nbsp;<br>
&nbsp;<br>
Last Update: <br>06 December 2004<br>
by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
<br></td>
<td valign="Top">

\section FurtherDevelopment Further Development
The ISO<i><sub>AgLib</sub></i> is published as Open Source according to the GPL license ( with exceptions ). Therefore everybody has the right
to use, to analyse and to change the source code, as long as the changes to the ISO<i><sub>AgLib</sub></i> itself are provided
to the public with the same license conditions. Thus the ISO<i><sub>AgLib</sub></i> could be used by a lot of manufacturers
without the fear of monopolistic control. This would allow the compatible use of all sophisticated features of
DIN 9684 and ISO 11783 for their machines. The author <a href="mailto:Achim.Spangler@gmx:de">Achim Spangler</a>
can arrange service contracts with a software company to provide extensions and other service for the ISO<i><sub>AgLib</sub></i>.
\image html Diagramme/IsoAgLib-further_develop.gif

\section FurtherExamples Examples
\image html Diagramme/IsoAgLib-imi.gif
The ISO<i><sub>AgLib</sub></i> is used at the experimental station D&uuml;rnast to document the work of all tractor dependent work
including simple soil cultivation implements. Small computing devices, which are called "Implement Indicator" (IMI) were
developed for this purpose based on the ISO<i><sub>AgLib</sub></i>. A ISO<i><sub>AgLib</sub></i>
 based Task-Controller records all available process data information from
the connected implements, combines them with GPS time and position data,
groups the data according to the areas farm, transport and field and stores
everything on PCMCIA memory cards. <br clear="All">
&nbsp;<br>
&nbsp;<br>
The work result with applicating implements like seeder can be optimized, if the ECU of the seeder can command the tractor
\image html Diagramme/IsoAgLib-seed_power_harrow_interaction.gif
 to drive not faster than the machine construction allows dependent on the
wanted seed density and depth and environment conditions. A front mounted
power harrow could indirectly control the loosening of soil compaction (work
intensity), if it can define a relation between driving speed and RPM of
the Power Take off Output Shaft (PTO). Damage could be avoided, if the tractor
would additionally enable the definition of a maximum limit for the hubload
on the PTO axle. If the tractor finally enables the control of the front
EHR position, the power harrow can control the work depth. This can be used
by the seeder to guarantee suitable soil conditions for the needed seeding
depth.<br>
The ISO<i><sub>AgLib</sub></i> supports this interactions with its capable process data implementation. Limits can be
defined as MIN or MAX setpoint. Parallel setpoints for the driving speed can by handled by the tractor, if it is based on the
ISO<i><sub>AgLib</sub></i>.
<br clear="All">
&nbsp;<br>
&nbsp;<br>
A modular extendable network for the fertilizer dosing can be also realized based on the process data features of the
ISO<i><sub>AgLib</sub></i>. Most of the actual sold systems with LBS (DIN 9684) aware fertilizers consist only of functions
\image html Diagramme/fertiliser_control_collaboration.gif
to control the application rate with a standard terminal or with application maps. But the latter is derived from
static information like yield maps, so that environmental conditions of the actual year are not integrated.<br>
Some systems utilize a spectroscopic sensor to detect the amount of chlorophyll in the plants. As long as the shortage
of chlorophyll is mainly caused by a shortage of nitrogen in the soil, the application rate dosing based on a standalone
sensor is correct. But chlorophyll shortage can also be caused by water stress and other conditions.<br>
Therefore additional moisture sensors and data bases have to be developed in future to avoid a false nitrogen dosing.
All these information sources should be evaluated by a mobile expert system, which could send an ultimate
nitrogen application rate setpoint to the fertilizer.<br>
Most of these devices have to be developed in future. Therefore a farmer should have the possibility to simply
integrate new devices in an existing network, without changing the existing elements.<br>
This is possible, if some basic strategies are used by all devices:
- use setpoint intervals in favour of exact values
- provide sensored values as process data (e.g. a chlorophyll sensor which only sends application rate setpoints can't be
		used by a mobile expert system as information source)
- interprete setpoints which are sent by other devices
- fertilizer&nbsp;should ask the driver for a priority ranking of different conflicting setpoint sender (this ranking should
		be stored for future use)

</td></tr>
  </table>
*/

/*! \page disc_further Contact and Background Information on IsoAgLib

<center>
<h1>Object Oriented Program Library ISO<i><sub>AgLib</sub></i></h1>
<h1>Development of ISO 11783 and DIN 9684 Applications</h1>
<h2>Copyright &copy; 1999 - 2004 Achim Spangler, Martin Wodok</h2>
<h2>Licensed with Exceptions under the General Public License (GPL) of the Free Software Foundation</h2>
</center>

<table width="100%" border="0" cellspacing="0" cellpadding="2">
<tr><td valign="Top" nowrap="true" >
\ref IndexWhereToStart<br>
\ref IndexNews<br>
\ref IndexChanges<br>
\ref IndexMainFeatures<br>
\ref IndexMemoryUsage<br>
\ref IndexStructuralOverview<br>
\ref IndexLicenseConditions<br>
\ref IndexAchnowledgements<br>
\ref IndexInitialSupport<br>
\ref IndexIsoVtExtension<br>
\ref IndexFirstWin32Users<br>
\ref IndexAuthors<br>
\ref IndexMaintainingCompany<br>
\ref IndexSupport<br>
\ref IndexTodo<br>
\ref IndexUsingProjects<br>
\ref HowtoDownload<br>
\ref HowtoAccessRepository<br>
\ref HowtoGetHelp<br>
\ref HowtoCreateDoc<br>
\ref HowtoLearnIsoAgLib<br>
\ref HowtoSetupCompilerTools<br>
\ref InstallIsoToolVt2Iso<br>
\ref HowtoCreateProjectFiles<br>
\ref InfGeneralInformation<br>
\ref InfAgentNetwork<br>
\ref InfServiceNetwork<br>
\ref InfRequirements<br>
\ref FurtherDevelopment<br>
\ref FurtherExamples<br>
\ref QuestionsAnswers<br>
\ref FurtherReading<br>
<br>
&nbsp;<br>
&nbsp;<br>
&nbsp;<br>
Last Update: <br>06 December 2004<br>
by <a href="mailto:Achim.Spangler@osb-ag:de">Achim Spangler</a>
<br></td>
<td valign="Top">

\section QuestionsAnswers Questions, Answers and Discussion
An Open Source project needs active users, who post their questions, hints and <b>answers to other questions</b>
to a central open communication platform. All users are strongly encouraged to help answering questions and to
search for reasons of posted ISO<i><sub>AgLib</sub></i> bugs.
Please use the <a href="http://ikb.weihenstephan.de/HyperNews/get/tp_2.html">Bulletin Board of partial project 2</a>
 for all communication.
The ISO<i><sub>AgLib</sub></i> was developed with the help of the following Open Source projects:
- <a href="http://www.kdevelop.org">KDevelop</a> an IDE for development of object oriented programs running Linux
- <a href="http://www.umbrello.org">Umbrello</a> for modelling of the project with the Unified Modelling Language (UML)
- KDoc , a source code parser to generate API documentation from the sources
- <a href="http://www.doxygen.org">doxygen</a> , a comprehensive source code parser, which can be highly configured, and which is used
    to generate the published API documentation
- sgmltools, <a href="http://www.lyx.org">lyx</a>, for editing of the handbook
- Source Navigator, a comprehensive source editor with a capable source parser (e.g. object dependencies,
    function calls)
- LINUX and all its standard development tools
- ...

Thanks to the DFG, which finances the partial project 2 of the research group "IKB-D&uuml;rnast".
Thanks to the research group of Prof. H. Auernhammer and to Martin Wodok (m.wodok@osb-ag:de), who
developed LBS_Terminal.

\section FurtherReading Further Reading on Open Source

- archived newsgroups related to C++ <a href="http://www.deja.com/bg.xp?level=comp.lang.c%2b%2b">http://www.deja.com/bg.xp?level=comp.lang.c%2b%2b</a>
with good search function <br>
&agrave;
  interesting discussions to C++ and efficiency, with some important people participating in comp.lang.c++.moderated
- Fear of Forking, How the GPL Keeps Linux Unified and Strong <a href="http://www.linuxcare.com/viewpoints/article/11-17-99.epl">http://www.linuxcare.com/viewpoints/article/11-17-99.epl</a>
- C++ in the Real World: Advice from the Trenches
   <a href="http://www.cantrip.org/realworld.html">http://www.cantrip.org/realworld.html</a>
	 (C++ and Performance)
- Giving something back" to the Linux and/or Open Source community <a href="http://linux.com/jobs/newsitem.phtml?sid=74&amp;aid=7302">http://linux.com/jobs/newsitem.phtml?sid=74&amp;aid=7302</a>
- Will C++ be faster than Fortran?
<a href="http://www.acl.lanl.gov/iscope97/papers/veldhuizen.ps"> http://www.acl.lanl.gov/iscope97/papers/veldhuizen.ps</a>
- IN THE TRENCHES - Twelve Rules For A Better Open Source Project <a href="http://www.linux-mag.com/2000-02/trench_01.html">http://www.linux-mag.com/2000-02/trench_01.html</a>
- Information&nbsp;about the used GPL-compatible derivatied license, as defined by the <a href="http://ecos.sourceware.org/license-overview.html">eCos project</a>
- Information&nbsp;about IKB-D&uuml;rnast and about partial project 2, which develops the ISO<i><sub>AgLib</sub></i>:<br>
<a href="http://ikb.weihenstephan.de/en/research/tp_2/actions.html">http://ikb.weihenstephan.de/en/research/tp_2/actions.html</a><br>
<a href="http://ikb.weihenstephan.de/en/research/tp_2/overview.html">http://ikb.weihenstephan.de/en/research/tp_2/overview.html</a><br></td></tr>
</table>
*/

/** \page GPL_with_exception IsoAgLib Library License - GNU General Public License with exceptions - GNU Project - Free Software Foundation (FSF)
\section GplException Definition of GPL Exception according to eCos project
The following paragraph defines for each source file the allowed
integration into applications:<br>
<b>
As a special exception, if other files instantiate templates or use
macros or inline functions from this file, or you compile this file and
link it with other works to produce a work based on this file, this file
does not by itself cause the resulting work to be covered by the GNU
General Public License. However the source code for this file must still
be made available in accordance with section (3) of the
GNU General Public License.

This exception does not invalidate any other reasons why a work based on
this file might be covered by the GNU General Public License.

Alternative licenses for IsoAgLib may be arranged by contacting
the main author Achim Spangler by a.spangler@osb-ag:de
</b>
<HR>

\section GplLicenseContent Table of Contents
<UL>

  <LI> \ref GplSec1
<UL>
<LI> \ref GplSec2
<LI> \ref GplSec3

</UL>
</UL>
<P>

<HR>

<P>



\section GplSec1 GNU GENERAL PUBLIC LICENSE
<P>
Version 2, June 1991

</P>

<PRE>
Copyright (C) 1989, 1991 Free Software Foundation, Inc.
59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

Everyone is permitted to copy and distribute verbatim copies
of this license document, but changing it is not allowed.
</PRE>



\subsection GplSec2 Preamble

<P>
  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.  This
General Public License applies to most of the Free Software
Foundation's software and to any other program whose authors commit to
using it.  (Some other Free Software Foundation software is covered by
the GNU Library General Public License instead.)  You can apply it to
your programs, too.

</P>
<P>
  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
this service if you wish), that you receive source code or can get it
if you want it, that you can change the software or use pieces of it
in new free programs; and that you know you can do these things.

</P>
<P>
  To protect your rights, we need to make restrictions that forbid
anyone to deny you these rights or to ask you to surrender the rights.
These restrictions translate to certain responsibilities for you if you
distribute copies of the software, or if you modify it.

</P>
<P>
  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must give the recipients all the rights that
you have.  You must make sure that they, too, receive or can get the
source code.  And you must show them these terms so they know their
rights.

</P>
<P>
  We protect your rights with two steps: (1) copyright the software, and
(2) offer you this license which gives you legal permission to copy,
distribute and/or modify the software.

</P>
<P>
  Also, for each author's protection and ours, we want to make certain
that everyone understands that there is no warranty for this free
software.  If the software is modified by someone else and passed on, we
want its recipients to know that what they have is not the original, so
that any problems introduced by others will not reflect on the original
authors' reputations.

</P>
<P>
  Finally, any free program is threatened constantly by software
patents.  We wish to avoid the danger that redistributors of a free
program will individually obtain patent licenses, in effect making the
program proprietary.  To prevent this, we have made it clear that any
patent must be licensed for everyone's free use or not licensed at all.

</P>
<P>
  The precise terms and conditions for copying, distribution and
modification follow.

</P>


\subsection GplSec3 TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION


<P>

<STRONG>0.</STRONG>
 This License applies to any program or other work which contains
a notice placed by the copyright holder saying it may be distributed
under the terms of this General Public License.  The "Program", below,
refers to any such program or work, and a "work based on the Program"
means either the Program or any derivative work under copyright law:
that is to say, a work containing the Program or a portion of it,
either verbatim or with modifications and/or translated into another
language.  (Hereinafter, translation is included without limitation in
the term "modification".)  Each licensee is addressed as "you".
<P>

Activities other than copying, distribution and modification are not
covered by this License; they are outside its scope.  The act of
running the Program is not restricted, and the output from the Program
is covered only if its contents constitute a work based on the
Program (independent of having been made by running the Program).
Whether that is true depends on what the Program does.

<P>

<STRONG>1.</STRONG>
 You may copy and distribute verbatim copies of the Program's
source code as you receive it, in any medium, provided that you
conspicuously and appropriately publish on each copy an appropriate
copyright notice and disclaimer of warranty; keep intact all the
notices that refer to this License and to the absence of any warranty;
and give any other recipients of the Program a copy of this License
along with the Program.
<P>

You may charge a fee for the physical act of transferring a copy, and
you may at your option offer warranty protection in exchange for a fee.
<P>

<STRONG>2.</STRONG>
 You may modify your copy or copies of the Program or any portion
of it, thus forming a work based on the Program, and copy and
distribute such modifications or work under the terms of Section 1
above, provided that you also meet all of these conditions:
<P>

<UL>

<LI><STRONG>a)</STRONG>
     You must cause the modified files to carry prominent notices
     stating that you changed the files and the date of any change.

<P>
<LI><STRONG>b)</STRONG>
     You must cause any work that you distribute or publish, that in
     whole or in part contains or is derived from the Program or any
     part thereof, to be licensed as a whole at no charge to all third
     parties under the terms of this License.

<P>
<LI><STRONG>c)</STRONG>
     If the modified program normally reads commands interactively
     when run, you must cause it, when started running for such
     interactive use in the most ordinary way, to print or display an
     announcement including an appropriate copyright notice and a
     notice that there is no warranty (or else, saying that you provide
     a warranty) and that users may redistribute the program under
     these conditions, and telling the user how to view a copy of this
     License.  (Exception: if the Program itself is interactive but
     does not normally print such an announcement, your work based on
     the Program is not required to print an announcement.)
</UL>

These requirements apply to the modified work as a whole.  If
identifiable sections of that work are not derived from the Program,
and can be reasonably considered independent and separate works in
themselves, then this License, and its terms, do not apply to those
sections when you distribute them as separate works.  But when you
distribute the same sections as part of a whole which is a work based
on the Program, the distribution of the whole must be on the terms of
this License, whose permissions for other licensees extend to the
entire whole, and thus to each and every part regardless of who wrote it.
<P>

Thus, it is not the intent of this section to claim rights or contest
your rights to work written entirely by you; rather, the intent is to
exercise the right to control the distribution of derivative or
collective works based on the Program.
<P>

In addition, mere aggregation of another work not based on the Program
with the Program (or with a work based on the Program) on a volume of
a storage or distribution medium does not bring the other work under
the scope of this License.

<P>

<STRONG>3.</STRONG>
 You may copy and distribute the Program (or a work based on it,
under Section 2) in object code or executable form under the terms of
Sections 1 and 2 above provided that you also do one of the following:


<!-- we use this doubled UL to get the sub-sections indented, -->
<!-- while making the bullets as unobvious as possible. -->
<UL>

<LI><STRONG>a)</STRONG>
     Accompany it with the complete corresponding machine-readable
     source code, which must be distributed under the terms of Sections
     1 and 2 above on a medium customarily used for software interchange; or,

<P>
<LI><STRONG>b)</STRONG>
     Accompany it with a written offer, valid for at least three
     years, to give any third party, for a charge no more than your
     cost of physically performing source distribution, a complete
     machine-readable copy of the corresponding source code, to be
     distributed under the terms of Sections 1 and 2 above on a medium
     customarily used for software interchange; or,

<P>
<LI><STRONG>c)</STRONG>
     Accompany it with the information you received as to the offer
     to distribute corresponding source code.  (This alternative is
     allowed only for noncommercial distribution and only if you
     received the program in object code or executable form with such
     an offer, in accord with Subsection b above.)
</UL>

The source code for a work means the preferred form of the work for
making modifications to it.  For an executable work, complete source
code means all the source code for all modules it contains, plus any
associated interface definition files, plus the scripts used to
control compilation and installation of the executable.  However, as a
special exception, the source code distributed need not include
anything that is normally distributed (in either source or binary
form) with the major components (compiler, kernel, and so on) of the
operating system on which the executable runs, unless that component
itself accompanies the executable.
<P>

If distribution of executable or object code is made by offering
access to copy from a designated place, then offering equivalent
access to copy the source code from the same place counts as
distribution of the source code, even though third parties are not
compelled to copy the source along with the object code.
<P>

<STRONG>4.</STRONG>
 You may not copy, modify, sublicense, or distribute the Program
except as expressly provided under this License.  Any attempt
otherwise to copy, modify, sublicense or distribute the Program is
void, and will automatically terminate your rights under this License.
However, parties who have received copies, or rights, from you under
this License will not have their licenses terminated so long as such
parties remain in full compliance.

<P>

<STRONG>5.</STRONG>
 You are not required to accept this License, since you have not
signed it.  However, nothing else grants you permission to modify or
distribute the Program or its derivative works.  These actions are
prohibited by law if you do not accept this License.  Therefore, by
modifying or distributing the Program (or any work based on the
Program), you indicate your acceptance of this License to do so, and
all its terms and conditions for copying, distributing or modifying
the Program or works based on it.

<P>

<STRONG>6.</STRONG>
 Each time you redistribute the Program (or any work based on the
Program), the recipient automatically receives a license from the
original licensor to copy, distribute or modify the Program subject to
these terms and conditions.  You may not impose any further
restrictions on the recipients' exercise of the rights granted herein.
You are not responsible for enforcing compliance by third parties to
this License.

<P>

<STRONG>7.</STRONG>
 If, as a consequence of a court judgment or allegation of patent
infringement or for any other reason (not limited to patent issues),
conditions are imposed on you (whether by court order, agreement or
otherwise) that contradict the conditions of this License, they do not
excuse you from the conditions of this License.  If you cannot
distribute so as to satisfy simultaneously your obligations under this
License and any other pertinent obligations, then as a consequence you
may not distribute the Program at all.  For example, if a patent
license would not permit royalty-free redistribution of the Program by
all those who receive copies directly or indirectly through you, then
the only way you could satisfy both it and this License would be to
refrain entirely from distribution of the Program.
<P>

If any portion of this section is held invalid or unenforceable under
any particular circumstance, the balance of the section is intended to
apply and the section as a whole is intended to apply in other
circumstances.
<P>

It is not the purpose of this section to induce you to infringe any
patents or other property right claims or to contest validity of any
such claims; this section has the sole purpose of protecting the
integrity of the free software distribution system, which is
implemented by public license practices.  Many people have made
generous contributions to the wide range of software distributed
through that system in reliance on consistent application of that
system; it is up to the author/donor to decide if he or she is willing
to distribute software through any other system and a licensee cannot
impose that choice.
<P>

This section is intended to make thoroughly clear what is believed to
be a consequence of the rest of this License.

<P>

<STRONG>8.</STRONG>
 If the distribution and/or use of the Program is restricted in
certain countries either by patents or by copyrighted interfaces, the
original copyright holder who places the Program under this License
may add an explicit geographical distribution limitation excluding
those countries, so that distribution is permitted only in or among
countries not thus excluded.  In such case, this License incorporates
the limitation as if written in the body of this License.

<P>

<STRONG>9.</STRONG>
 The Free Software Foundation may publish revised and/or new versions
of the General Public License from time to time.  Such new versions will
be similar in spirit to the present version, but may differ in detail to
address new problems or concerns.
<P>

Each version is given a distinguishing version number.  If the Program
specifies a version number of this License which applies to it and "any
later version", you have the option of following the terms and conditions
either of that version or of any later version published by the Free
Software Foundation.  If the Program does not specify a version number of
this License, you may choose any version ever published by the Free Software
Foundation.

<P>


<STRONG>10.</STRONG>
 If you wish to incorporate parts of the Program into other free
programs whose distribution conditions are different, write to the author
to ask for permission.  For software which is copyrighted by the Free
Software Foundation, write to the Free Software Foundation; we sometimes
make exceptions for this.  Our decision will be guided by the two goals
of preserving the free status of all derivatives of our free software and
of promoting the sharing and reuse of software generally.



<P><STRONG>NO WARRANTY</STRONG></P>

<P>

<STRONG>11.</STRONG>
 BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.

<P>

<STRONG>12.</STRONG>
 IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

<P>


<H2>END OF TERMS AND CONDITIONS</H2>

*/
