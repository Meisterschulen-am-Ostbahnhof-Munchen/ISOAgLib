/** @page ProcDataRemotePage Description of subdirectory \a Remote of the Process part of IsoAgLib
  * ( <i>\<xgpl_src/IsoAgLib/comm/Process/Remote\></i> )
  * All interface classes and helper elements which are dedicated to remote process data
  * ( %i.e. for process data where measurement values are updated by remote ECU, this ECU can request
  * values and can command setpoints )
  * are placed in this directory. This helps to find all source files which are needed
  * for remote Process Data ( %i.e. if only local Process Data are used, this directory can
  * be safely ignored ). \n
  * The subdirectories of this directory are:
  * - \a Std -> classes ( interface and implementation ) for the most capable variant of remote Process Data ( IsoAgLib::iProcDataRemote_c ).
  *      This class needs all files of the sister directories StdMeasureElements, StdSetpointElements and impl
  * - \a SimpleMeasure -> classes ( interface and implementation ) for reduced measurement value feature set ( IsoAgLib::iProcDataRemoteSimpleMeasure_c ).
  *      This class needs all files of the sister directories StdSetpointElements and impl
  * - \a SimpleSetpoint -> classes ( interface and implementation ) for reduced setpoint value feature set ( IsoAgLib::iProcDataRemoteSimpleSetpoint_c ).
  *      This class needs all files of the sister directories StdMeasureElements, SimpleSetpointElements and impl
  * - \a SimpleMeasureSetpoint -> classes ( interface and implementation ) for reduced setpoint and measurement value feature set
  *      ( IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c ).
  *      This class needs all files of the sister directories SimpleSetpointElements and impl
  * - \a impl -> base class for all remote Process Data -> always include in project, if remote Process Data are used
  * - \a StdMeasureElements -> helper classes for capable measurement ( %i.e. meausrement programs ) in IsoAgLib::iProcDataRemote_c and
  *      IsoAgLib::iProcDataRemoteSimpleSetpoint_c
  * - \a StdSetpointElements -> helper classes for capable setpoint management ( %i.e. decide dependend on commanding ECU if setpoint is accepted or not, etc. )
  *      in IsoAgLib::iProcDataRemote_c and IsoAgLib::iProcDataRemoteSimpleMeasure_c
  * - \a SimpleSetpointElements -> helper classes for simple setpoint management ( %i.e. simply accept received setpoint independend from commanding ECU )
  *      in IsoAgLib::iProcDataRemoteSimpleSetpoint_c and IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c
  * - \a SimpleMeasureElements -> helper classes for simple measurement value management ( %i.e. simply use single shot value requests )
  *      in IsoAgLib::iProcDataRemoteSimpleMeasure_c, IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasure_c and
  *      IsoAgLib::iProcDataRemoteSimpleSetpointSimpleMeasureCombined_c
  * \n
  * A closer description can be found in \ref ProcDataPage .
  *
  */