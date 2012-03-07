/*
  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef AUX3_STATE_ENGINE_H
#define AUX3_STATE_ENGINE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include "../ivttypes.h"

#if 0

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/** class to describe asssignments from one AUX3 input */
class PreferredAssignmentPerAux3Input_c {
 public:
  PreferredAssignmentPerAux3Input_c(const IsoName_c& arc_auxInputDevice, uint16_t aui16_modelIdentificationCode)
  : mc_isoName(arc_auxInputDevice), mui16_modelIdentificationCode(aui16_modelIdentificationCode){};

  /** copy constructor */
  PreferredAssignmentPerAux3Input_c( const PreferredAssignmentPerAux3Input_c& arc_dataSrc);

  /** comparison */
  bool operator==(const PreferredAssignmentPerAux3Input_c& arc_dataSrc) const;
  /** comparison */
  bool operator!=(const PreferredAssignmentPerAux3Input_c& arc_dataSrc) const;

  /** clear vector mvec_assignmentsPerDevice */
  void clearAllAssignments();
  /** add new assignment
      @param aui16_auxInputObjId Object ID of the AUX3 Input
      @param aui16_auxFuntionObjectId Object ID of the AUX3 Function
      @return true -> assignment has been inserted as new pair into mvec_assignments
  */
  bool addAssignment( uint16_t aui16_auxInputObjId, uint16_t aui16_auxFuntionObjectId);
  /** deliver count of stored assignments */
  uint16_t getAssignmentCount() const { return mvec_assignmentsPerDevice.size();}

  /** ISONAME of the associated AUX3 input device */
  IsoName_c mc_isoName;
  /** model identification code of the AUX3 input device */
  uint16_t mui16_modelIdentificationCode;
  std::vector<std::pair<uint16_t, uint16_t>> mvec_assignmentsPerDevice; ///< vector of assignments
};

/** class which manages one set of preferred assignment */
class PreferredAssignment_c {
 public:
  PreferredAssignment_c();
  PreferredAssignment_c( const PreferredAssignment_c& arc_dataSrc);

  /** comparison
      IMPORTANT: order of ISONAME in mvec_assignments shall NOT influence the result!
  */
  bool operator==(const PreferredAssignment_c& arc_dataSrc) const;
  /** comparison
      IMPORTANT: order of ISONAME in mvec_assignments shall NOT influence the result!
  */
  bool operator!=(const PreferredAssignment_c& arc_dataSrc) const;

  /** clear mvec_assignments */
  void clearAllAssignments();

  /** add new assignment.
      add new node PreferredAssignmentPerAux3Input_s into mvec_assignments
      in case given arc_auxInputDevice is not yet handled _AND_ when aui16_auxFuntionObjectId
      is not yet assigned.
      propagate then to PreferredAssignmentPerAux3Input_c::addAssignment()
      @param arc_auxInputDevice ISONAME of AUX3 input device
      @param aui16_auxInputObjId Object ID of the AUX3 Input
      @param aui16_auxFuntionObjectId Object ID of the AUX3 Function
      @return true -> assignment has been inserted as new pair into mvec_assignments
  */
  bool addAssignment( const IsoName_c& arc_auxInputDevice, uint16_t aui16_auxInputObjId, uint16_t aui16_auxFuntionObjectId);
  /** deliver count of stored assignments.
      call PreferredAssignmentPerAux3Input_c::getAssignmentCount() for all items of mvec_assignments
  */
  uint16_t getAssignmentCount() const;

  /** vector of preferred assignments per AUX3 input device per node */
  std::vector<PreferredAssignmentPerAux3Input_s> mvec_assignments;
};

/** class for central management of AUX3 */
class Aux3PrefAssignMgmt_c
{
private:
  /** enum for location of preferred asssignment */
  typedef enum {
    PreferredAssignmentCode, ///< data is compiled in as manufacturer data set
    PreferredAssignmentEeprom, ///< data was created during runtime on field and stored in EEPROM
    PreferredAssignmentUndefined ///< undefined - default value when no data set is specified
  } PreferredAssignmentLocation_t;
public:
  /** constructor
     @param reference to VT object pool data for direct initialization of mvec_aux3FunctionObject
  */
  Aux3PrefAssignMgmt_c(IsoAgLib::iIsoTerminalObjectPool_c& arc_pool);

  /** initialize manufacturer hard coded pref assign
      and parse EEPROM stored pref assign data sets.
      read data set of contained AUX3 Functions from associated VT-Client pool data
      into mvec_aux3FunctionObject
      @post efficient later lookup for suitable setups is provided
  */
  void init();

  /** derive list of AUX3 input devices
      @param ar_inputMonitoringList reference to vector of to be monitored AUX3 ISONAMEs
  */
  void retrieveMonitoredAux3InputDevices(std::vector<IsoName_c>& ar_inputMonitoringList);

  /** add new assignment to mc_currentAssignment.
      check whether given assignment is compatible to mvec_aux3FunctionObject stored specification
      @param arc_auxInputDevice ISONAME of AUX3 input device
      @param aui16_auxInputObjId Object ID of the AUX3 Input
      @param aui16_auxFuntionObjectId Object ID of the AUX3 Function
      @return true -> assignment has been accepted and has been inserted into mc_currentAssignment
  */
  bool addAssignment( const IsoName_c& arc_auxInputDevice, uint16_t aui16_auxInputObjId, uint16_t aui16_auxFuntionObjectId);
  /** deliver count of stored assignments in mc_currentAssignment. */
  uint16_t getAssignmentCount() const;
  /** check whether all AUX3 functions from mvec_aux3FunctionObject has been assigned */
  bool check4CompleteAssignment() const;
  /** check whether ideal preferred assignment (from mc_uploadPreferredAssignment) has been accepted
      by sequence of addAssignment calls (stored in mc_currentAssignment)
  */
  bool isPreferredAssignmentCompleteAccepted() const { return (mc_currentAssignment == mc_uploadPreferredAssignment);}
  /** derive, select optimal preferred assignment set and start upload.
      clear mc_currentAssignment, set mc_uploadPreferredAssignment
      and mt_currentAssignmentStorageLocation according to optimal data set
      @return true -> suitable preferred assignment set has been found which differs from current assignment set, so that upload is runnning
   */
  bool deriveSelectOptimalPreferredAssignment();
  /** store mc_currentAssignment in case this is not identical to mc_uploadPreferredAssignment
      i.e. VT performed a different assignment in relation to the uploaded set.
  */
  void storePreferredAssignmentIfNeeded();

private:
  /** vector of local AUX3 functions, which are retrieved from associated VT-Client pool data */
  std::vector<const vtObjectAuxiliaryFunction2_c&> mvec_aux3FunctionObject;
  /** current preferred assignment as defined by assignment commands from VT */
  PreferredAssignment_c mc_currentAssignment;
  /** ideal assignment data set, that has been selected based on available AUX3 input devices
      and that is used for upload to VT
   */
  PreferredAssignment_c mc_uploadPreferredAssignment;
  /** location of active preferred assignment.
      this is initialized during selection of ideal data set.
      in case the VT commanded assignment varies from wanted,
      the new assignment has to be stored into EEPROM -> thus the
      location changes in this case to PreferredAssignmentEeprom
   */
  PreferredAssignmentLocation_t mt_currentAssignmentStorageLocation;
};

}
#endif
#endif
