/*! \page ProcDataLocalPage Description of subdirectory Local of the Process Data part
(<i>\<xgpl_src/IsoAgLib/comm/Part7_ProcessData/Local\></i>)
All interface classes and helper elements which are dedicated to local process data
( %i.e. for process data where measurement values are updated by ECU where this SW is running )
are placed in this directory. This helps to find all source files which are needed
for local Process Data ( %i.e. if only remote Process Data are used, this directory can
be safely ignored ). \n
The subdirectories of this directory are:
- \a Std -> classes ( interface and implementation ) for the most capable variant of local Process Data ( IsoAgLib::iProcDataLocal_c ).
     This class needs all files of the sister directories StdMeasureElements, StdSetpointElements and impl
- \a SimpleMeasure -> classes ( interface and implementation ) for reduced measurement value feature set
     ( IsoAgLib::iProcDataLocalSimpleMeasure_c ).
     This class needs all files of the sister directories StdSetpointElements and impl
- \a SimpleSetpoint -> classes ( interface and implementation ) for reduced setpoint value feature set
     ( IsoAgLib::iProcDataLocalSimpleSetpoint_c ).
     This class needs all files of the sister directories StdMeasureElements, SimpleSetpointElements and impl
- \a SimpleMeasureSetpoint -> classes ( interface and implementation ) for reduced setpoint and measurement value feature set
     ( IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c ).
     This class needs all files of the sister directories SimpleSetpointElements and impl
- \a impl -> base class for all local Process Data -> always include in project, if local Process Data are used
- \a StdMeasureElements -> helper classes for capable measurement ( %i.e. meausrement programs ) in IsoAgLib::iProcDataLocal_c and
     IsoAgLib::iProcDataLocalSimpleSetpoint_c
- \a StdSetpointElements -> helper classes for capable setpoint management ( %i.e. decide dependent on commanding ECU if setpoint
     is accepted or not, etc. ) in IsoAgLib::iProcDataLocal_c and IsoAgLib::iProcDataLocalSimpleMeasure_c
- \a SimpleSetpointElements -> helper classes for simple setpoint management ( %i.e. simply accept received setpoint independent
     from commanding ECU ) in IsoAgLib::iProcDataLocalSimpleSetpoint_c and IsoAgLib::iProcDataLocalSimpleSetpointSimpleMeasure_c
\n
A closer description can be found in \ref ProcDataPage .

*/
