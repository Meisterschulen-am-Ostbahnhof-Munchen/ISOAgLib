/** @page CommOverPage Overview on Communication Protocol Services of IsoAgLib for DIN 9684 and ISO 11783
	* The implementation of all standardized protocol services is located in the directory
	* <i>\<xgpl_src/IsoAgLib/comm/\></i> and its subdirectories.
	*
	* @section ServiceDependency Dependencies of Protocol Services
	* Most of the protocol features can be selected independent from other functions for
	* a specific project. The depdendencies are described in the following overview diagram, which also
	* shows the distribution of services on the different subdirectories.
  \dot
  digraph CommServDepend {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [shape=record, fontname=ARIALN, fontsize=10, style=filled, fontcolor=blue, color=greenyellow];
    edge     [fontname=ARIALN, fontsize=10, dir="back"];

		can        [ label="CAN Communication" ];
		eeprom     [ label="EEPROM" ];
		root       [ label="Communication Services\nDIN 9684 and ISO 11783" ];
		scheduler        [ label="Scheduler\nFunction: Coordination of all services\nLocation-Directory: Scheduler", URL="\ref SchedulerPage" ];
		system_mgmt [ label="System Management\nFunction: Monitor lists of all nodes on network\nLocation-Directory: SystemMgmt", URL="\ref SystemMgmtPage" ];
		iso_system [ label="ISO Networkmanagement\nFunction: Monitor lists of ISO 11783 nodes\nLocation-Directory: System/ISO11783", URL="\ref SystemMgmtAnnIso"  ];
		din_system [ label="DIN Networkmanagement\nFunction: Monitor lists of DIN 9684 nodes\nLocation-Directory: System/DIN9684", URL="\ref SystemMgmtAnnDin" ];
		base   [ label="Base Data\nFunction: Periodically sent base information ( mostly tractor )\nLocation-Directory: Base_c", URL="\ref BaseDataPage" ];
		iso_base   [ label="ISO 11783 Data\nFunction: Manage periodically sent tractor and calendar PGN information\nLocation-Directory: Base_c", URL="\ref BaseDataIsoType" ];
		din_base   [ label="DIN 9684 Data\nFunction: Manage Base Data Type 1,2 and Calendar\nLocation-Directory: Base", URL="\ref BaseDataPage" ];
		multi_msg  [ label="Data Stream\nFunction: Send stream of more than 8-Byte\nLocation-Directory: Multipacket", URL="\ref MultiMsgPage" ];
		din_term   [ label="LBS+ Terminal\nFunction: LBS+ terminal project binary upload\nLocation-Directory: DIN_Terminal", URL="\ref DinTerminalPage" ];
		iso_term   [ label="ISO Virtual Terminal\nFunction: Upload, layout and control of virtual terminal\nLocation-Directory: ISO_Terminal", URL="\ref XMLspec" ];
		process   [ label="Process Data\nFunction: Process-Data\nLocation-Directory: Process", URL="\ref ProcDataPage" ];
		iso_proc   [ label="ISO Format\nFunction: Format data for ISO 11783\nLocation-Directory: Process", URL="\ref ProcDataDinIsoSec" ];
		din_proc   [ label="DIN Format\nFunction: Format data for DIN 9684\nLocation-Directory: Process", URL="\ref ProcDataDinIsoSec" ];

		can         -> root ;
		root        -> scheduler ;
		scheduler   -> system_mgmt ;
		system_mgmt -> base ;
		base        -> iso_base ;
		base        -> din_base ;
		system_mgmt -> iso_system ;
		eeprom      -> iso_system [ label="store and reload dynamic SA", style=dotted ];
		system_mgmt -> din_system ;
		system_mgmt -> multi_msg ;
		system_mgmt -> process ;
		eeprom      -> process [ label="store and reload accumulated values" ];
		system_mgmt -> multi_msg ;
		process     -> iso_proc ;
		process     -> din_proc ;
		multi_msg   -> din_term [ label="Binary Upload" ];
		process     -> din_term [ label="Synchronisation Information\nData Exchange"];
		multi_msg   -> iso_term ;

  }
  \enddot
	* @section CommOverLbs Overview on Scheduler
	* <b>Detailed Description:</b> \ref SchedulerPage
	* The class Scheduler_c works as a central scheduler and data coordinator for all IsoAgLib components.
	* The application has motly only contanct to the main scheduling function IsoAgLib::iScheduler_c::timeEvent
	* which is responsible to dedicate periodically execution time to all components with the
	* guarantee to return at the requested time.
	*
	* @section CommOverLbsSystem Overview on System Management
	* <b>Detailed Description:</b> \ref SystemMgmtPage
	* The class IsoAgLib::iSystemMgmt_c has a list of all local identities, where an address claim is performed at the
	* System-Startup and which must send responses on several network management requests.
	* Additionally IsoAgLib::iSystemMgmt_c provides functions to search for a network node corresponding to its
	* GetyPos_c ( device type ) or its address. Thereby both DIN 9684 and ISO 11783 monitor lists are
	* searched - as far as the individual protocol is activated/compiled for the project.
	*
	* @subsection CommOverIsoSystem Overview on ISO 11783 Monitor List
	* <b>Detailed Description:</b> \ref SystemMgmtAnnIso
	* The class IsoAgLib::iISOMonitor_c has a monitor list of all ISO 11783 network nodes.
	* It's mostly used by the application to search for a device by its device type - and to get its SA.
	* It's also used internally to claim address and manage local ISO 11783 identities ( \ref IsoAgLib::iISOItem_c ).
	* If EEPROM store and reload ( \ref USE_EEPROM_IO_YN ) is activated and compiled in the project, a
	* dynamic aquired SA is automatically stred and reloaded at specified EEPROM address - as suggested by the standard.
	*
	* @subsection CommOverDinSystem Overview on DIN 9684 Monitor List
	* <b>Detailed Description:</b> \ref SystemMgmtAnnDin
	* The class IsoAgLib::iDINMonitor_c has a monitor list of all DIN 9684 network nodes.
	* It's mostly used by the application to search for a device by its GETYPOS - and to get its dynamic address.
	* It's also used internally to claim address and manage local DIN 9684 identities ( \ref IsoAgLib::iDINItem_c ).
	*
	* @section CommOverLbsBase Overview on Base Data
	* <b>Detailed Description:</b> \ref BaseDataPage
	* The class IsoAgLib::iBase_c stores all received base data information to allow
	* the application developer to access the data at the time, where the local controlled
	* execution flow needs input.
	* All information which is defined in DIN 9684 and ISO 11783 can be accessed with the same function,
	* as the information is stored internally with one variable for both protocols - API is independent from active protocol.
	*
	* @subsection CommOverIsoBase Overview on ISO 11783 Data
	* <b>Detailed Description:</b> \ref BaseDataIsoType
	* ISO 11783 provides more information than DIN 9684 - read more at the link above.
	*
	* @subsection CommOverDinBase Overview on DIN 9684 Data
	* <b>Detailed Description:</b> \ref BaseDataBothTypes
	* DIN 9684 provides Base Data Type 1,2 and calendar information. If IsoAgLib is
	* used for network combinations with new ( newer than 2003 ) Varioterminals of AGCO-Fendt,
	* their additional data can be also accessed.
	*
	* @section CommOverLbsMulti Overview on Data Streams
	* <b>Detailed Description:</b> \ref MultiMsgPage
	* The IsoAgLib implements currently only the send of data streams with more than eight data byte.
	* But this is enough for the main targets LBS+ terminal project upload and ISO Virutal Terminal
	* access for handling of user interface. The implementation adapts to the different variations
	* from the common protocol.
	*
	* @subsection CommOverIsoTerminal Overview on ISO 11783 Virtual Terminal
	* <b>Detailed Description:</b> \ref XMLspec
	* The IsoAgLib provides a complete toolchain for user interface management based
	* on ISO 11783 virtual terminal. The layout with including definition of handles
	* for data exchange and display control is based on XML. This approach is comparable
	* to user interface resource description of other PC operating systems.
	* The individual DTD ( data type description ) for ISO 11783 in IsoAgLib is
	* described in \ref XMLspec . The usage of this mechanism is best demonstrated with an
	* example like imiIsoMaskupload.cpp .
	* The IsoAgLib contains the build utilities which are needed to generate the constant
	* variable array definitions which are used as data source to upload the object pool.
	* These utilities are tested with Linux, but as they use only some pure C libraries without
	* any GUI part, they should be easily ported to another development operating system.
	*
	* @subsection CommOverDinTerminal Overview on LBS+ Terminalproject upload
	* <b>Detailed Description:</b> \ref DinTerminalPage
	* The interaction with LBS+ terminals ( AGCO-Fendt Varioterminal and Agrocom ACT-DIN )
	* is based on both project binary upload ( process data for synchronisation and data stream for upload )
	* and process data for data exchange.
	*
	* @section CommOverLbsProc Overview on Process Data
	* <b>Detailed Description:</b> \ref ProcDataPage
	* The main difference between simple periodically sent base data and process data
	* is that each process data type has a consistent API for capable interactions.
	* This allows to set not only an exact setpoint, but also to define an intervall,
	* so that the commanded ECU has more flexibility to combine this request with
	* environmental and mechanical constraints or with other direct or indirect
	* dependent setpoints ( example: PTO speed and driving speed are indirect dependent for most tractors ).
	* In addition to flexible control with setpoints, measurement programs can be used to retrieve
	* specific information like MIN/MAX/AVG... measured value. This helps to avoid sending of a fine grained
	* chain of single measurement values which are only used by the receiver to get the MIN/MAX/AVG... .
	* Therefore the BUS Load can be reduced enormnous.
	*
	* @subsection CommOverIsoProc Overview on ISO Format
	* <b>Detailed Description:</b> \ref ProcDataDinIsoSec
	* The IsoAgLib can automatically detect based on the active protocol, how the
	* process data messages must be formatted. As ISO 11783 Process Data was at least till
	* Mid 2003 compatible to DIN 9684, the API is 100% independent from the protocol.<br>
	* <b>Possible Problems of Current ISO Activities:<br>
	* As the process data part of ISO 11783 is under heavy change, the IsoAgLib can't guarantee
	* the compatibility for the final - whenever it comes - ISO 11783 release.<br>
	* The main problem:<br>
	* The IsoAgLib process data handling strategy is based on the object oriented approach, which
	* was defined by the creator of the corresponding DIN 9684 part. This is also comparable to
	* the ISO 11783 virtual terminal part.
	* This means, that each process data instance has a standard API for doing jobs like:
	* - setting a excat, minimum, maximum and percentual setpoint for control
	* - retrieving current setpoint ( exact, min, max, percentual) and measurement values
	* - access to derived measurement values like MIN, MAX, AVG which is best calculated at the measuring ECU,
	*		so that less data is sent on the BUS
	* - use of measurement programs to get automatic value updates
	* As soon as no standard - for all process data types useable - algorithm can be defined to
	* access all these functions, the IsoAgLib has to try to use several builtin lookup tables
	* to connect them in a uniform way.
	* This would cause runtime and space overhead.
	* The other problem of this approach is the fact, that not all access services are created for
	* each process data, so that some ECUs which would for example only need to limit a process data
	* by setting a MAX setpoint, are then forced to send an exact setpoint.
	* This induces very tight dependencies, which aren't needed, and will probably cause resource conflicts.
	* Please contact a.spangler@osb-ag:de if you have questions on the problems of the currently planned changes.
	* Please have also a look at the various tutorial examples which provide some initial insight in the topic,
	* and look at the topic \ref InfAgentNetwork .
	* </b>
	*
	* @subsection CommOverDinProc Overview on DIN Format
	* <b>Detailed Description:</b> \ref ProcDataDinIsoSec
	* The IsoAgLib provides an API to all interaction possibilities of process data.
	* Even some incompatibilities between different process data interpretation
	* are solved automatically with some meta information ( mostly compile time defines ).
	*/
