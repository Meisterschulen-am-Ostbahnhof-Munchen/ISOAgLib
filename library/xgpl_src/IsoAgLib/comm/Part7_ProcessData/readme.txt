/*! \page ProcDataPage Process Data handling
The \isoaglib provides a very capable management of Process Data as standard mechanism
(IsoAgLib::iProcDataLocal_c and IsoAgLib::iProcDataRemote_c).
But as this might cause a big overhead for some applications, which for example only want to
publish some measurement data on the BUS, restricted variants of them are provided.


\section ProcDataOverviewGrp Overview on type of Process Data
  <img src="Images/processData.png" alt="Process Data Hierarchy" title="Process Data Hierarchy">
  More information can be found on the detail pages and class descriptions:
  <ul>
  <li>\ref IsoAgLib::iProcDataLocal_c</li>
  <li>\ref IsoAgLib::iProcDataLocalSimpleMeasure_c</li>
  <li>\ref IsoAgLib::iProcDataLocalSimpleSetpoint_c</li>
  <li>\ref IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c</li>
  <li>\ref IsoAgLib::iProcDataRemote_c</li>
  <li>\ref IsoAgLib::iProcDataRemoteSimpleMeasure_c</li>
  <li>\ref IsoAgLib::iProcDataRemoteSimpleSetpoint_c</li>
  <li>\ref IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c</li>
  <li>\ref IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c</li>
  </ul>
  <!--\dot
  digraph ProcessDataHierarchy {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

    subgraph center {
      style="invis";
      root     [shape=point];
      FeatureLevelRoot [ shape=point ];
      root -> FeatureLevelRoot [ style=invis ];
    }

    subgraph cluster_feature {
      style=filled;
      color=lightgrey;
      label = "Feature levels of process data";
      AllFeatures [ shape=octagon, label="Measurement Programs\nCapable Setpoint Handling" ];
      SimpleMeasure [ shape=septagon, label="Simple Measurement" ];
      SimpleSetpoint [ shape=hexagon, label="Simple Setpoint" ];
      SimpleSetpointMeasure [ shape=pentagon, label="Simple Measurement and\nSimple Setpoint" ];
      SimpleSetpointMeasureCombined [ shape=pentagon, label="Simple Measurement and\nSimple Setpoint" ];
    };

    subgraph cluster_local {
      style=filled;
      color=cornflowerblue;
      label="Data of this ECU";
      Local_c [ fontcolor=white, shape=octagon, color="blue1", label="iProcDataLocal_c", URL="\ref IsoAgLib::iProcDataLocal_c" ];
      LocalSimpleMeasure_c [ fontcolor=white, shape=septagon, color="blue1", label="iLBSProcDataLocal\nSimpleMeasure_c", URL="\ref IsoAgLib::iProcDataLocalSimpleMeasure_c" ];
      LocalSimpleSetpoint_c [ fontcolor=white, shape=hexagon, color="blue1", label="iLBSProcDataLocal\nSimpleSetpoint_c", URL="\ref IsoAgLib::iProcDataLocalSimpleSetpoint_c" ];
      LocalSimpleSetpointSimpleMeasure_c [ fontcolor=white, shape=pentagon, color="blue1", label="iLBSProcDataLocal\nSimpleSetpoint\nSimpleMeasure_c",
                                          URL="\ref IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c" ];
    };

    subgraph cluster_remote {
      style=filled;
      color="chartreuse3";
      label="Data requested from other ECU";
      Remote_c [ shape=octagon, fontcolor=white, color="forestgreen", label="iProcDataRemote_c", URL="\ref IsoAgLib::iProcDataRemote_c" ];
      RemoteSimpleMeasure_c [ shape=septagon, fontcolor=white, color="forestgreen", label="iProcDataRemote_c\nSimpleMeasure_c", URL="\ref IsoAgLib::iProcDataRemoteSimpleMeasure_c" ];
      RemoteSimpleSetpoint_c [ shape=hexagon, fontcolor=white, color="forestgreen", label="iProcDataRemote_c\nSimpleSetpoint_c", URL="\ref IsoAgLib::iProcDataRemoteSimpleSetpoint_c" ];
      RemoteSimpleSetpointSimpleMeasure_c [ shape=pentagon, fontcolor=white, color="forestgreen", label="iProcDataRemote_c\nSimpleSetpoint\nSimpleMeasure_c",
                                          URL="\ref IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c" ];
      RemoteSimpleSetpointSimpleMeasureCombined_c [ shape=pentagon, fontcolor=white, color="forestgreen", label="iProcDataRemote_c\nSimpleSetpoint\nSimpleMeasure\nCombined_c",
                                          URL="\ref IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c" ];
    };
    root -> subgraph cluster_local [ color="blue" ];
    root -> subgraph cluster_remote [color="green" ];

    AllFeatures -> FeatureLevelRoot [ style="dashed", dir="back" ];
    SimpleMeasure -> FeatureLevelRoot [ style="dashed", dir="back", label="no measurement programs" ];
    SimpleSetpoint -> FeatureLevelRoot [ style="dashed", dir="back", label="no selective setpoint acceptance or rejection" ];
    SimpleSetpointMeasure -> FeatureLevelRoot [ style="dashed", dir="back", label="very restricted process data" ];
    SimpleSetpointMeasureCombined -> FeatureLevelRoot [ style="dashed", dir="back", label="most restricted process data" ];



    Local_c -> AllFeatures [ style="dashed", dir="back", color="blue" ];
    Remote_c -> AllFeatures [ style="dashed", dir="back", color="green" ];

    LocalSimpleMeasure_c -> SimpleMeasure [ style="dashed", dir="back", color="blue" ];
    RemoteSimpleMeasure_c -> SimpleMeasure [ style="dashed", dir="back", color="green" ];

    LocalSimpleSetpoint_c -> SimpleSetpoint [ style="dashed", dir="back", color="blue" ];
    RemoteSimpleSetpoint_c -> SimpleSetpoint [ style="dashed", dir="back", color="green" ];

    LocalSimpleSetpointSimpleMeasure_c -> SimpleSetpointMeasure [ style="dashed", dir="back", color="blue" ];
    RemoteSimpleSetpointSimpleMeasure_c -> SimpleSetpointMeasure [ style="dashed", dir="back", color="green" ];

    RemoteSimpleSetpointSimpleMeasureCombined_c -> SimpleSetpointMeasureCombined [ style="dashed", dir="back", color="green" ];

    Local_c -> FeatureLevelRoot [ style=invis ];
    LocalSimpleMeasure_c -> FeatureLevelRoot [ style=invis ];
    LocalSimpleSetpoint_c -> FeatureLevelRoot [ style=invis ];
  }
  \enddot-->
\subsection LocalProcDataOverview Local Process Data
Therefore the following variants are provided for local Process Data \n (<b>local means:</b> the ECU which
performs this SW is responsible for measurement and update of the value; additionally
it is the adressee for setpoint commands):
- \a IsoAgLib::iProcDataLocal_c
   - full feature set of local managed Process Data
   - allow remote ECU to start measurement program, so that a value update is sent regularly
     without individual request.
     Thereby several parallel measurement programs with different settings - %e.g. send rate -
     can be handled.
   - individual management of different received setpoint commands based on sender number,
     so that the application can decide on the setpoint to use
     -> the application has this way the possibility to evaluate all recieved values to find an
     optimal value which satisfies all - or at least most - of the received requests.
- \a IsoAgLib::iProcDataLocalSimpleMeasure_c
   - reduce resource (RAM per instance, ROM if IsoAgLib::iProcDataLocal_c is not used in the project)
     afford by reduce of measurement data feature set
   - only provide measurement values by single shot requests - NO measurement programs.
     Also no derivation of MIN, MAX, AVG etc. - just provide single measurement value per request.
   - capable setpoint handling of IsoAgLib::iProcDataLocal_c
- \a IsoAgLib::iProcDataLocalSimpleSetpoint_c
   - reduce resource (RAM per instance, ROM if IsoAgLib::iProcDataLocal_c is not used in the project)
     afford by reduce of setpoint command feature set
   - capable measurement data handling of IsoAgLib::iProcDataLocal_c
   - simply store each received setpoint and automatically send confirmation reply back to commanding
     ECU - this way NO differentiation by commanding ECU \n -> <b>NOT usable if</b>
     - specific setpoint values should by denied
     - setpoint can't be accepted dependent on internal state
     - more than one ECU could try to control the behaviour of this ECU with setpoint commands
- \a IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c
   - reduce feature set of both measurement and setpoint data
<br>Also take a look at the \ref ProcDataLocalPage.

\subsection RemoteProcDataOverview Remote Process Data
The access to remote Process Data, which corresponds to data aquired by remote ECUs provides
similar levels of capability:
- \a IsoAgLib::iProcDataRemote_c
   - full feature set of remote managed Process Data
   - provide methods to configure and use measurement programs and to request single values
   - enable commanding of setpoint values.
     The library provides the following state information for each setpoint:
     - Setpoint accepted? (%e.g. other conflicting setpoint or simply the commanded value out of bounds)
     - Setpoint currently realised? (%e.g. previously accepted setpoint can <b>not</b> be implemented on
       current machine and environment state)
     - Intersection of conflicting master setpoint and own setpoint is not empty and is implemented by commanded ECU?
     - Current master setpoint? (interesting if own setpoint wish was not accepted)
- \a IsoAgLib::iProcDataRemoteSimpleMeasure_c
   - simply use single value request to gather measurement data of remote ECU
   - capable setpoint handling of IsoAgLib::iProcDataRemote_c
- \a IsoAgLib::iProcDataRemoteSimpleSetpoint_c
   - capable measurement data handling of IsoAgLib::iProcDataRemote_c
   - only send setpoint without control of response of commanded ECU
- \a IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c
   - reduce feature set of both measurement and setpoint data
- \a IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c
   - reduce RAM resource request more than IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c
   - combine setpoint and measurement value to one variable
     (IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c handles them as two different values, so that
       a deviation between them can still be detected)
<br>Also take a look at the \ref ProcDataRemotePage.

\section ProcDataDirectoryStructureGrp Distribution of Classes and their corresponding files
The different variants of local and remote Process Data are based on some helper classes. The helper classes
are structured corresponding to the feature set levels described in the overview on types of Process Data.
The subdirectories are structured so that the needed main and helper classes for a wanted Process Data variant
can be easily found.
\n %E.%g. classes which are responsible for standard (capable) measuring programs of
a Process Data type reside in the directories named <b>StdMeasureElements</b>.
\n Comparable, the helper classes for a process data type which provides only restricted (simple)
setpoint management are placed in directories named <b>SimpleMeasureElements</b>.
\n This leads to the following subdirectory structure beneath <i>\<xgpl_src/IsoAgLib/comm/Process\></i>
Like in all directories of IsoAgLib, the internal implementation class Foo_c resides in path/impl/foo_c.h
with path/ifoo_c.h for the interface class iFoo_c. Thereby the individual elements of the \a impl
subdirectories are not described in this context - simply incorporate the contents of \<path/impl\>, if you take
any class of path. \n
<table>
<tr>
<td>Local</td><td>&nbsp;</td><td>classes for local process data types <br>
              -> ECU which runs this SW is responsible to update measurement value and react on setpoint (if used)</td>
</tr>
<tr>
<td>Remote</td><td>&nbsp;</td><td>classes for remote process data types<br>
              -> remote ECU provides services for this process data</td>
</tr>
<tr>
<td>StdMeasureElements</td><td>&nbsp;</td><td>helper classes for both remote and local measurement programs<br>
              (thereby placed in this central directory)</td>
</tr>
<tr>
<td>StdSetpointElements</td><td>&nbsp;</td><td>helper classes for both remote and local capable setpoint handling<br>
              (thereby placed in this central directory)</td>
</tr>
<tr>
<td>impl</td><td>&nbsp;</td><td>implementation files for all central classes which are needed for <b>all</b> Process Data applications<br>
              (-> include the elements of this directory always if you use Process Data)</td>
</tr>
<tr>
<td>Local</td><td>Std</td><td>files for IsoAgLib::iProcDataLocal_c <br> need files of \e Local/StdMeasureElements, \e Local/StdSetpointElements,
              \e StdMeasureElements, \e StdSetpointElements and \e impl (everything related to Process directory)</td>
</tr>
<tr>
<td>Local</td><td>SimpleMeasure</td><td>files for IsoAgLib::iProcDataLocalSimpleMeasure_c <br> need files of \e Local/StdSetpointElements,
              \e StdSetpointElements and \e impl (everything related to Process directory)</td>
</tr>
<tr>
<td>Local</td><td>SimpleSetpoint</td><td>files for IsoAgLib::iProcDataLocalSimpleSetpoint_c <br> need files of \e Local/StdMeasureElements,
              \e Local/SimpleSetpointElements, \e StdMeasureElements and \e impl (everything related to Process directory)</td>
</tr>
<tr>
<td>Local</td><td>SimpleMeasureSetpoint</td><td>files for IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c <br> need files of
              \e Local/SimpleSetpointElements and \e impl (everything related to Process directory)</td>
</tr>
<tr>
<td>Local</td><td>StdMeasureElements</td><td>helper classes for IsoAgLib::iProcDataLocal_c and IsoAgLib::iProcDataLocalSimpleSetpoint_c</td>
</tr>
<tr>
<td>Local</td><td>StdSetpointElements</td><td>helper classes for IsoAgLib::iProcDataLocal_c and IsoAgLib::iProcDataLocalSimpleMeasure_c</td>
</tr>
<tr>
<td>Local</td><td>SimpleSetpointElements</td><td>helper classes for IsoAgLib::iProcDataLocalSimpleSetpoint_c and
                                                 IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c</td>
</tr>
<tr>
<td>Local</td><td>impl</td><td>files for all local Process Data types</td>
</tr>
<tr>
<td>Remote</td><td>Std</td><td>files for IsoAgLib::iProcDataRemote_c <br> need files of \e Remote/StdMeasureElements, \e Remote/StdSetpointElements,
              \e StdMeasureElements, \e StdSetpointElements and \e impl (everything related to Process directory)</td>
</tr>
<tr>
<td>Remote</td><td>SimpleMeasure</td><td>files for IsoAgLib::iProcDataRemoteSimpleMeasure_c <br> need files of \e Remote/SimpleMeasureElements,
              \e Remote/StdSetpointElements, \e StdSetpointElements and \e impl (everything related to Process directory)</td>
</tr>
<tr>
<td>Remote</td><td>SimpleSetpoint</td><td>files for IsoAgLib::iProcDataRemoteSimpleSetpoint_c <br> need files of \e Remote/StdMeasureElements,
              \e Remote/SimpleSetpointElements, \e StdMeasureElements and \e impl (everything related to Process directory)</td>
</tr>
<tr>
<td>Remote</td><td>SimpleMeasureSetpoint</td><td>files for IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c and
              IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c <br> need files of
              \e Remote/StdMeasureElements (only for IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c),
              \e Remote/SimpleSetpointElements and \e impl (everything related to Process directory)</td>
</tr>
<tr>
<td>Remote</td><td>StdMeasureElements</td><td>helper classes for IsoAgLib::iProcDataRemote_c and IsoAgLib::iProcDataRemoteSimpleSetpoint_c</td>
</tr>
<tr>
<td>Remote</td><td>StdSetpointElements</td><td>helper classes for IsoAgLib::iProcDataRemote_c and IsoAgLib::iProcDataRemoteSimpleMeasure_c</td>
</tr>
<tr>
<td>Remote</td><td>SimpleSetpointElements</td><td>helper classes for IsoAgLib::iProcDataRemoteSimpleSetpoint_c and
                                                 IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c</td>
</tr>
<tr>
<td>Remote</td><td>SimpleMeasureElements</td><td>helper classes for IsoAgLib::iProcDataRemoteSimpleMeasure_c,
                                                 IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c and
                                                 IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c</td>
</tr>
<tr>
<td>Remote</td><td>impl</td><td>files for all remote Process Data types</td>
</tr>
</table>


\section ProcDataIsoSec Usage for ISO 11783
The \isoaglib handles Process Data to the greatest part independent from the underlying protocol.
This allows to write applications, which concentrate only on the data communication part without
the overhead of protocol specific details.
This is achieved by the CAN data formating and low-level processing class __IsoAgLib::ProcessPkg_c, which
detects dependent on the receiver (for send of msg) and/or sender (for receive of msg)
and its protocol state, which protocol format to use. This is achieved by IsoAgLib::iIsoMonitor_c
which provides functions to derive the protocol type of a ECU based on it DevKey == device_type/_instance setting.

\subsection ApplicationPrinciple Principles for the use of Process Data
In spite to the first design of IsoAgLib (version <= 0.3.0), the single variable instances
of data type "Process Data XY" (with XY from { IsoAgLib::iProcDataLocal_c, IsoAgLib::iProcDataRemote_c,
IsoAgLib::iProcDataLocalSimpleMeasure_c, IsoAgLib::iProcDataRemoteSimpleSetpoint_c, ... })
can be placed like a "normal" variable (%e.g. int, char) in the program in the variable
scope where the process data information shall be accessed. Like "normal" variables,
pointers to a process data instance can be distributed round the application.
\subsubsection ImportantDisallowed Important Note to the use of Process Data Variables
<b>Never</b> assign a value from one process data instance to another and <b>never</b>
use a process data type as function parameter.\n
<b>Instead:</b> Provide access to functions or different sub-programs by <b>POINTER</b> to a Process Data
instance. \n
<b>Reason:</b> A Process Data type occupies a lot of RAM for the management of the different sub-data
and a copy (this is also done for function call without pointer or references) of complete
Process Data instances are <b>very expensive</b> operation regarding to RAM and time.
Proc2iso should be used for generation of process information needed by the code sample below (see \ref XMLProcSpec).
However, the file DeviceDescription.xml-func.inc describes how to do this by hand.
\subsubsection ExampleForCorrectDistribution Example for Creation and Correct Distribution of Process Data Variable
\code
// declare extern example function (dummy)
extern void doSomething( IsoAgLib::iProcDataLocal_c *pc_localProc );
// set device type of local ECU: fertilizer
IsoAgLib::iIsoName_c c_myIsoName( 5, 0 );
// initialise variable upon construction/definition
// local process data for "on/off mechanical" [0/0x64] of fertilizer spreader
// creation of process data instance
IsoAgLib::iProcDataLocal_c c_myWorkState ( 0, 0, c_myIsoName, &c_myIsoName, false, 0x1234 );
// set measurement data to working == 0x64
c_myWorkState.setMasterMeasurementVal( 0x64 );

// alternate: initialise process data variable after definition
// "working width" [mm] of fertilizer spreader, store/load data at EEPROM adress 0x1238
IsoAgLib::iProcDataLocal_c c_myWorkWidth;
c_myWorkWidth.init( 0, 0, c_myIsoName, &c_myIsoName, false, 0x1238 ); 

// call function doSomething -> give pointer to variable c_myWorkState -> transfer simple adress via stack
doSomething( &c_myWorkState );
\endcode

\subsubsection StartMeasureProgram Example for Starting a Measure Program
The \isoaglib provides a capable support for measuring programs, where an ECU can
request the periodical send of a measurement value (Process Data) at a remote ECU.
This can be performed by creating so-called subprogs (>= 1 per measurement program),
where the conditions to trigger a measurement value send can be defined.
The final start command for the measurement program can then select the values, which
should be sent on each trigger event.
\n The nice on all this features is, that the ECU, which provides the data must only
update the measurement value, as all the other communication is performed by the \isoaglib
(%i.e. register measure programs and handle send of data, ...).
\code
// define device type of remote ECU ( from which we want measurement data )
IsoAgLib::IsoName_c remoteIsoName( 1, 0 );
// define remote process data to gather information of remote work state
 IsoAgLib::iProcDataRemote_c c_remoteWorkState( 0, 0, myIsoName, &myIsoName, remoteGtp, 0);
// trigger value update every 1000 msec.
c_remoteWorkState.prog().addSubprog(Proc_c::TimeProp, 1000);
// start measure program: trigger send of current measure value (and not MIN/MAX/AVG/ etc.)
c_remoteWorkState.prog().start(Proc_c::TimeProp, Proc_c::DoVal);
while( true ) {
  // use last received measurement value
  if ( c_remoteWorkState.prog().val() == 0x64 ) {
    // do something if remote ECU is in work state
  }
}

\endcode


\subsubsection EnhancedUseWithHandlerCall Enhanced Use of Process Data with call of Handler functions on Several Events
If an application has to immediately react on received setpoint or measurement update, it is needlessly time consuming
to poll always for the awaited event. The \isoaglib provides a mechanism to define a handler which can then be
registered on all Process Data variables, which should use the handler. \n
Example usage: Define function which evaluates new received setpoints for immediate response and realisation (if accepted)
in current control loop.
\code
// derive handler class frome base class of IsoAgLib
class MyProcessDataHandler_c : public IsoAgLib::ProcessDataChangeHandler_c {
  public:
    // react on new received setpoint for local process data
    // (remote system which wants to control the local system dependent on the setpoint
    // sent a new setpoint value)
    // acrc_src general event source class, which provides conversion functions to get needed event source class
    // ri32_val new value, which caused the event (for immediate access)
    // rc_callerIsoName IsoAgLib::iIsoName of calling device - i.e. which sent new setpoint
    // return true -> handler class reacted on change event
    virtual bool processSetpointSet( EventSource_c acrc_src, int32_t ri32_val, const IsoAgLib::iIsoName_c& rc_callerIsoName, bool rb_changed );
};
// implement the handler function, which is called on each received setpoint
bool MyProcessDataHandler_c::processSetpointSet( EventSource_c acrc_src, int32_t ri32_val, const IsoAgLib::iIsoName_c& rc_callerIsoName, bool rb_changed ) {
{ // %e.g. check for device type of commanding ECU
  if ( rc_callerIsoName.getDevClass() == 0x1 ) {
    // reaction on setpoints sent by device type 1
  }
  else if ( abs( ri32_val - currentVal ) < 10 )
  { // handle setpoints which are near to current value...
  }
}
// define variable instance of the handler class type
MyProcessDataHandler_c c_setpointHandler;

// define local process data, which uses the handler
IsoAgLib::iProcDataLocalSimpleSetpoint_c c_myFertilizerAmount;
// init process data with pointer to handler, which shall be called upon setpoint receive
c_myFertilizerAmount.init( 0, 0, myIsoName, &myIsoName, false, 0x123A, &c_setpointHandler );
\endcode

\subsection DefaultDataLoggingExample Example for default data logging
A Process Data Variable called RequestDefaultProcessData (<tt>DDI = 0xDFFF</tt>) can be used to start and stop 
default data logging (see section 6.6.2 in the task controller specification). 
The implements have to send the values for their default measurement method.

The default data logging is started by a request value command and stopped by a set value command with value 0. To get the request value command on <tt>DDI 0xDFFF</tt> a callback can be implemented.
In processDefaultLoggingStart() the appropriate measurement programms can be started via startDataLogging().

Extract from the 2.4 tutorial:
\code
class MyProcDataHandler_c : public IsoAgLib::ProcessDataChangeHandler_c
{
    //! This handler function is called if a remote process data instances sends a request value command for DDI 0xDFFF (default data logging DDI)
    //! can be used to start measurement programms in local process data instances
    //! Parameter:
    //! @param ac_src: encapsulated pointer to triggered process data variable
    //! @param ai32_val: data value in the request value command
    //! @param ac_callerIsoName: iso name of the sender
    bool processDefaultLoggingStart(IsoAgLib::EventSource_c ac_src,
                                    int32_t ai32_val,
                                    const IsoAgLib::iIsoName_c& ac_callerIsoName);
};

bool MyProcDataHandler_c::processDefaultLoggingStart(IsoAgLib::EventSource_c /* ac_src */, int32_t /* ai32_val */, const IsoAgLib::iIsoName_c& /* ac_callerIsoName */)
{
  if (arr_procData[cui8_indexApplicationRate].startDataLogging(IsoAgLib::Proc_c::TimeProp, 1000))
    LOG_INFO << "starting measurement application rate success!" << "\r\n";
  else
    LOG_INFO << "starting measurement application rate failure!" << "\r\n";
  return true;
}
\endcode

See the 2.7 tutorial for the sending of the request/set value commands.

See also \ref DefaultDataLogging. 

*/
