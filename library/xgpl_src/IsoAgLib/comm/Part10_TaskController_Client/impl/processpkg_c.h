/*
  processpkg_c.h: data object for Process-Data messages

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCESS_PKG_H
#define PROCESS_PKG_H

#include <IsoAgLib/isoaglib_config.h>
#include "processcmd_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/driver/can/impl/ident_c.h>

namespace __IsoAgLib {

/**
  data object for Process-Data messages
  transforms flag formated information to/from
  CAN uint8_t string
  @author Dipl.-Inform. Achim Spangler
  @author Dipl.-Inf. Martin Wodok
*/
class ProcessPkg_c : public CanPkgExt_c  {
private:
public:
  ProcessPkg_c( const ProcessPkg_c& arc_src );
  ProcessPkg_c( const CanPkg_c& arc_src, int ai_multitonInst = 0 );
  /** default constructor which has nothing to do */
  ProcessPkg_c( int ai_multitonInst = 0 );
  /** default constructor which has nothing to do */
  ~ProcessPkg_c();

  /* ************************** */
  /* ***retreiving of values*** */
  /* ************************** */

   /**
    deliver Cmd of process msg
    @return Command value of message
  */
  uint8_t cmd()const{return bit_data.ui8_Command;}

  /**
    deliver Element Number of process msg
    @return Element Number value of message
  */
  uint16_t element()const{return bit_data.ui16_Element;}

    /**
    deliver DDI of process msg
    @return Data Dictionary Identifier value of message
  */
  uint16_t DDI()const{return bit_data.ui16_DDI;}

  /**
    check if the 4byte value of the message has a special command of type
    proc_specCmd_t: setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
    @param ren_checkCmd special command to check for
    @return true -> the given cmd was received
  */
  bool isSpecCmd(proc_specCmd_t ren_checkCmd)const;

  /**
    deliver data value as int32_t
    @return data value of message
  */
  int32_t getValue() const { return mi32_pdValue; }

  /**
    @return pointer to IsoItem_c of Receiver
  */
  IsoItem_c* receiverItem() const { return getMonitorItemForDA(); }

  /**
    @return pointer to IsoItem_c of Sender
  */
  IsoItem_c* senderItem() const { return getMonitorItemForSA(); }


  /* *********************** */
  /* ***setting of values*** */
  /* *********************** */
   /**
    set command of process msg
    @param ab_cmd command value of process data message
  */
  void set_Cmd(uint8_t ab_cmd){bit_data.ui8_Command = ab_cmd;}

   /**
    set DDI of process msg
    @param ab_DDI Data dictionary Itendifier
  */
  void set_DDI(uint16_t ab_DDI){bit_data.ui16_DDI = ab_DDI;}

   /**
    set Element Number of Process Data Message
    @param ab_Element command value of process data message
  */
  void set_Element(uint16_t ab_Element){bit_data.ui16_Element = ab_Element;}

  /**
    set data value
    @param ai32_val new data value for message
  */
  void setData(int32_t ai32_val) { mi32_pdValue = ai32_val; }

  /**
    set one of the special commands of type proc_specCmd_t:
    setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
    with specified data type specifying flag in CAN data string
    @param ren_procSpecCmd special command to send
  */
  void setData(proc_specCmd_t ren_procSpecCmd);

  /**
    extract data from ISO commands and save it to member class
    @param rl_elementDDI
  */
  bool resolveCommandTypeForISO(uint16_t aui16_ddi, bool b_isSetpoint);

  /**
    function to translate the string data into flag values;
    needed for assigning informations from another CanPkg_c or CANPkgExt
    @see CanPkg_c::operator=
    @see CanPkgExt_c::operator=
  */
  void string2Flags();

  /**
    function to translate flag values to data string;
    needed for sending informations from this object via CanIo_c on CAN BUS,
    because CanIo_c doesn't know anything about the data format of this type of msg
    so that it can only use an unformated data string from CANPkg
    @see CanPkg_c::getData
    @see CanPkgExt_c::getData
  */
  void flags2String();

  /** stores the command in generalized form */
  ProcessCmd_c mc_processCmd;

private: // Private attributes


  int32_t mi32_pdValue;

  struct _bit_data {
    _bit_data() : ui8_Command(0), ui16_Element(0), ui16_DDI(0) {}
    uint16_t ui8_Command : 4;
    uint16_t ui16_Element : 12;
    uint16_t ui16_DDI : 16;
  } bit_data;
};

}
#endif
