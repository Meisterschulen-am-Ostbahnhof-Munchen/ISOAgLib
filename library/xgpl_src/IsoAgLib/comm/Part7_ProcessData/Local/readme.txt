/*! \page ProcDataLocalPage Description of subdirectory Local of the Process Data part

<tt>\<xgpl_src/IsoAgLib/comm/Part7_ProcessData/Local\></tt>.<br>
All interface classes and helper elements which are dedicated to local process data
(%i.e. for process data where measurement values are updated by ECU where this SW is running)
are placed in this directory. This helps to find all source files which are needed
for local Process Data (%i.e. if only remote Process Data are used, this directory can
be safely ignored). \n
The subdirectories of this directory are:
- <i>Std -> classes</i> (interface and implementation) for the most capable variant of local Process Data (IsoAgLib::iProcDataLocal_c).
     This class needs all files of the sister directories StdMeasureElements, StdSetpointElements and impl
- <i>SimpleMeasure -> classes</i> (interface and implementation) for reduced measurement value feature set
     (IsoAgLib::iProcDataLocalSimpleMeasure_c).
     This class needs all files of the sister directories StdSetpointElements and impl
- <i>SimpleSetpoint -> classes</i> (interface and implementation) for reduced setpoint value feature set
     (IsoAgLib::iProcDataLocalSimpleSetpoint_c).
     This class needs all files of the sister directories StdMeasureElements, SimpleSetpointElements and impl
- <i>SimpleMeasureSetpoint -> classes</i> (interface and implementation) for reduced setpoint and measurement value feature set
     (IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c).
     This class needs all files of the sister directories SimpleSetpointElements and impl
- <i>impl -> base class</i> for all local Process Data -> always include in project, if local Process Data are used
- <i>StdMeasureElements -> helper classes</i> for capable measurement (%i.e. meausrement programs) in IsoAgLib::iProcDataLocal_c and
     IsoAgLib::iProcDataLocalSimpleSetpoint_c
- <i>StdSetpointElements -> helper classes</i> for capable setpoint management (%i.e. decide dependent on commanding ECU if setpoint
     is accepted or not, etc.) in IsoAgLib::iProcDataLocal_c and IsoAgLib::iProcDataLocalSimpleMeasure_c
- <i>SimpleSetpointElements -> helper classes</i> for simple setpoint management (%i.e. simply accept received setpoint independent
     from commanding ECU) in IsoAgLib::iProcDataLocalSimpleSetpoint_c and IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c
\n
A closer description can be found in \ref ProcDataPage .

*/
