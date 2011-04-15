/*
  isoitem_c.h: object which represents an item in an iso monitor list

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISO_ITEM_H
#define ISO_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/baseitem_c.h>

namespace IsoAgLib {
  class iIsoItem_c;
}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** item class for ISO 11783 members monitor list to manage
  local (with address claim) and remote (statistic information)
  systems; utilizes BaseItem_c for basic MonitorList
  management and IsoName_c for management of the 64bit NAME field
  @short Item with services needed for ISO11783 monitor lists.
  @author Dipl.-Inform. Achim Spangler
  @see BaseItem
  @see ISOName
*/
// forward declarations
class IdentItem_c;

class IsoItem_c : public BaseItem_c  {
private:
  // private typedef alias names
public:
  /** default constructor - all data has to be initialized with a call to "set(..)"
  */
  IsoItem_c();

  /** copy constructor for ISOItem.
    The copy constructor checks if the source item is
    a master ( i.e. the mpc_masterItem pointer points to this )
    -> it doesn't simply copy the pointer, but sets its
    own pointer also to the this-pointer of the new instance
    @param acrc_src source IsoItem_c instance
  */
  IsoItem_c(const IsoItem_c& acrc_src);

  /** assign constructor for ISOItem
    @param acrc_src source IsoItem_c object
  */
  IsoItem_c& operator=(const IsoItem_c& acrc_src);

  /** default destructor */
  virtual ~IsoItem_c();

  /** deliver the data NAME string as pointer to 8byte string
    @return const pointer to Flexible8ByteString_c union with NAME
  */
  const Flexible8ByteString_c* outputNameUnion() const {return mc_isoName.outputUnion();}

  /** get self config mode
    @return self configuration adress state
  */
  bool selfConf() const {return mc_isoName. selfConf();}

  /** get industry group code
    @return industry group of device
  */
  uint8_t indGroup() const {return mc_isoName. indGroup();}

  /** get device class instance number
    @return:device class instance number
  */
  uint8_t devClassInst() const {return mc_isoName. devClassInst();}

  /** get device class code
    @return:device class
  */
  uint8_t devClass() const {return mc_isoName. devClass();}

  /** get function code
    @return function code
  */
  uint8_t func() const {return mc_isoName. func();}

  /** get function instance code
    @return function instance code
  */
  uint8_t funcInst() const {return mc_isoName. funcInst();}

  /** get ECU instance code
    @return ECU instance code
  */
  uint8_t ecuInst() const {return mc_isoName. ecuInst();}

  /** get manufactor code
    @return manufactor code
  */
  uint16_t manufCode() const {return mc_isoName. manufCode();}

  /** get serial number
    @return serial number
  */
  uint32_t serNo() const {return mc_isoName. serNo();}

  /** set the NAME data from 8 uint8_t string
    @param apu_src pointer to 8byte source string
  */
  void inputNameUnion(const Flexible8ByteString_c* apu_src) {mc_isoName.inputUnion(apu_src);}

  /** set self config mode
    @param ab_selfConf true -> indicate sefl configuring ECU
  */
  void setSelfConf(bool ab_selfConf) {mc_isoName.setSelfConf(ab_selfConf);}

  /** set industry group code
    @param aui8_indGroup industry group of device (2 for agriculture)
  */
  void setIndGroup(uint8_t aui8_indGroup) {mc_isoName.setIndGroup(aui8_indGroup);}

  /** set device class instance number
    @param aui8_devClassInst instance number of ECU with same devClass in the network */
  void setDevClassInst(uint8_t aui8_devClassInst) {mc_isoName.setDevClassInst(aui8_devClassInst);}

  /** set device class code
    @param aui8_devClass device class of ECU
  */
  void setDevClass(uint8_t aui8_devClass) {mc_isoName.setDevClass(aui8_devClass);}

  /** set function code
    @param ab_func function of the ECU (usual 25 for network interconnect)
  */
  void setFunc(uint8_t ab_func) {mc_isoName.setFunc(ab_func);}

  /** set function instance code
    @param ab_funcInst instance number of ECU with same function and device class
        (default 0 - normally)
  */
  void setFuncInst(uint8_t ab_funcInst) {mc_isoName.setFuncInst(ab_funcInst);}

  /** set ECU instance code
    @param ab_ecuInst instance number of ECU with same function, device class and function instance
        (default 0 - normally)
  */
  void setEcuInst(uint8_t ab_ecuInst) {mc_isoName.setEcuInst(ab_ecuInst);}

  /** set manufactor code
    @param aui16_manufCode code of manufactor (11bit)
  */
  void setManufCode(uint16_t aui16_manufCode) {mc_isoName.setManufCode(aui16_manufCode);}

  /** set serial number (Identity Number)
    @param aui32_serNo serial no of specific device (21bit)
  */
  void setSerNo(uint32_t aui32_serNo) {mc_isoName.setSerNo(aui32_serNo);}

  /** set all element data with one call
    @param ai32_time creation time of this item instance
    @param acrc_isoName ISOName code of this item ((deviceClass << 3) | devClInst )
    @param aui8_nr number of this item
    @param aren_status state of this ident (off, claimed address, ...)
    @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
  */
  void set(int32_t ai32_time, const IsoName_c& acrc_isoName, uint8_t aui8_nr,
           itemState_t aren_status = IState_c::Active, int ai_multitonInst = 0 );

  /** this sets the back-reference to the associated IdentItem if this is a LOCAL Item!
      there's no need to unset the back-reference, because the local/remote state
      should be set at creation of the IsoItem. */
  void setIdentItem (IdentItem_c& acrc_identItem) { mpc_identItem = &acrc_identItem; }

  /** set ISOName code of this item
    @param acrc_isoName ISOName
  */
  void setISOName(const IsoName_c& acrc_isoName) {mc_isoName = acrc_isoName;}

  /** deliver ISOName code of this item
    @return ISOName
  */
  const IsoName_c& isoName() const {return mc_isoName;}

  IsoName_c& nonConstIsoName() {return mc_isoName;}

  /** deliver name
    @return pointer to the name uint8_t string (7byte)
  */
  virtual const uint8_t* name() const;

  /** deliver name as pure ASCII string
    @param pc_asciiName string where ASCII string is inserted
    @param aui8_maxLen max length for name
  */
  virtual void getPureAsciiName(int8_t *pc_asciiName, uint8_t aui8_maxLen);

  /** periodically time evented actions:
      * find free SA or check if last SA is available
      * send adress claim
    possible errors:
      * dependant error in CanIo_c during send
    @return true -> all planned time event activitie performed
  */
  bool timeEvent( void );

  /** process received ADDRESS_CLAIMED message to update data and react
      if needed and (e.g. change of SA, reclaim current address)
      Note: The parameters are needed here because the actual CAN package
            may already be rewritten to (due to e.g. sending an AC, etc.)
    @param ai32_pkgTime Original message timestamp.
    @param aui8_pkgSa Original message source address.
  */
  void processAddressClaimed (int32_t ai32_pkgTime, uint8_t aui8_pkgSa);

  /** send a SA claim message
   * - needed to respond on request for claimed SA fomr other nodes
   * - also needed when a local ident triggers a periodic request for SA
   * @return true -> this item has already a claimed SA -> it sent its SA; false -> didn't send SA, as not yet claimed or not local
   */
  bool sendSaClaim();

  /// @param ab_fromConflict false => Initial Address-Claim, so we need to go to "AddressClaim"-phase!
  ///                        true => go to "ClaimedAddress" state, no 250ms wait (for now) as we changed SA!
  /// @todo SOON-240 Do we really need this parameter? Should we wait 250ms on change of SA also???
  /// @todo SOON-240 Merge with sendSaClaim - create an enum for the three cases!
  void sendAddressClaim (bool ab_fromConflict);

#ifdef USE_WORKING_SET
  /** (Re-)Start sending the Working-Set Announce sequence
   * @return time-announce-started (=announce_key). You need this key to check for "isAnnounced(announce_key)".
   */
  int32_t startWsAnnounce();

  // check if this item is a master (i.e. the master pointer points to itself)
  // NOTE: this function need NOT to be used in combination with function getMaster()
  //       as it already checks isMaster()
  bool isMaster () const { return (mpvec_slaveIsoNames != NULL); }
  // check if this item belong to a working set -> either master or client
  // NOTE: only to be used for checking -> if master IsoItem_c is need use function getMaster()
  bool isWsmMember () { return (getMaster() != NULL); }
  /** get master of this isoItem
      @return  this if master himself; get master if client; NULL if standalone
    */
  IsoItem_c* getMaster();

  void setMasterSlaves (STL_NAMESPACE::vector<IsoName_c>* apvec_slaveIsoNames);
  void setMaster (uint8_t aui8_slaveCount, int32_t ai_time );
  void addSlave (IsoName_c const& rcc_slaveName);

  /// For checking if the WS-Announce is completed use the "announce key" returned from "startWsAnnounce()".
  bool isWsAnnounced (int32_t ai32_timeAnnounceStarted);

  bool isWsAnnouncing() { return (mi8_slavesToClaimAddress != 0); }
#endif

  /**
    set number of this item
    @param aui8_nr number
  */
  void setNr(uint8_t aui8_nr){mui8_nr = aui8_nr;}

  /**
    deliver the number/adress of this item
    @return number
  */
  uint8_t nr()const{return mui8_nr;}

  /**
   * General Chandge Address function. Will set the Address and the call the necessary broadcast.
   */
  void changeAddressAndBroadcast (uint8_t aui8_newAddress);

  /**
   * some other node has stolen the Address, so we need to give up ours first.
   */
  void giveUpAddressAndBroadcast() { changeAddressAndBroadcast (0xFE); }

  /**
    lower comparison with another IsoItem_c on the rigth (compare the ISOName)
    @param acrc_right rigth parameter for lower compare
  */
  bool operator<(const IsoItem_c& acrc_right) const
    {return (isoName() < acrc_right.isoName());}

  /**
    lower comparison with ISOName uint8_t on the rigth
    @param acrc_isoName rigth parameter for lower compare
  */
  bool operator<(const IsoName_c& acrc_isoName)const{return (isoName() < acrc_isoName);}

  /**
    lower comparison between left ISOName uint8_t and right MonitorItem
    @param acrc_left ISOName uint8_t left parameter
    @param acrc_right rigth ServiceItem_c parameter
  */
  friend bool operator<(const IsoName_c& acrc_left, const IsoItem_c& acrc_right);

  /**
    lower comparison between left IsoItem_c and right ISOName uint8_t
    @param acrc_left left ServiceItem_c parameter
    @param acrc_right ISOName uint8_t right parameter
  */
  friend bool lessThan(const IsoItem_c& acrc_left, const IsoName_c& acrc_right);

  /**
    equality comparison with ISOName uint8_t on the rigth
    @param acrc_right rigth parameter for lower compare
  */
  bool operator==(const IsoName_c& acrc_right)const { return (isoName() == acrc_right);}

  /**
    difference comparison with ISOName uint8_t on the rigth
    @param acrc_right rigth parameter for lower compare
  */
  bool operator!=(const IsoName_c& acrc_right) const{ return (isoName() != acrc_right);}

  /**
    compare given number to nr of this item and return result
    @param aui8_nr compared number
    @return true -> given number equal to nr of this item
  */
  bool equalNr(const uint8_t aui8_nr)const{return (nr() == aui8_nr);}

  IdentItem_c* getIdentItem() { return mpc_identItem; }

#ifdef USE_WORKING_SET
  /** take away master state of this item -> is than standalone
      BE CAREFUL: you should know what you are doing!!!
    */
  void removeMasterState() { if( isMaster() ) { delete mpvec_slaveIsoNames; mpvec_slaveIsoNames = NULL; } }

  int32_t announceTimeStamp() { return mi32_timeAnnounceForRemoteItem; }
#endif

  /** convert function */
  IsoAgLib::iIsoItem_c& toIisoItem_c();

  /** convert function */
  const IsoAgLib::iIsoItem_c& toConstIisoItem_c() const;

protected: // methods

private: // methods
  /** calculate random wait time between 0 and 153msec. from NAME and time
    @return wait offset in msec. [0..153]
  */
  uint8_t calc_randomWait();

private: // members
#ifdef USE_WORKING_SET
  // return pointer to vector of clients
  STL_NAMESPACE::vector<IsoName_c>* getVectorOfClients() const { return mpvec_slaveIsoNames;}

  /** pointer to a list of all slave nodes represented by their ISO-Name
   * if this pointer is != NULL, this item IS a master and the list's size is the number of associated slaves.
   * if this pointer is == NULL, this item is NOT a master.
   * It may be a slave - this can be determined by searching
   * through all Items' list for this Item's IsoName!
   */
  STL_NAMESPACE::vector<IsoName_c>* mpvec_slaveIsoNames;

  /** mi8_slavesToClaimAddress
    * == -1  waiting to announce WS-master message
    *  >  0  still so many slaves to announce..
    * ==  0  idle / announcing complete
    */
  int8_t mi8_slavesToClaimAddress;

  int32_t mi32_timeLastCompletedAnnounceStarted;
  int32_t mi32_timeCurrentAnnounceStarted;

  int32_t mi32_timeAnnounceForRemoteItem;

  bool mb_repeatAnnounce;
#endif

  /** number of element */
  uint8_t mui8_nr : 8;

  bool mb_repeatClaim;

  /** If the IsoItem is LOCAL, you'll find a back-reference to the IdentItem here. */
  IdentItem_c* mpc_identItem;

  /** ISOName of element */
  IsoName_c mc_isoName;
};

/**
  lower comparison between left IsoItem_c and right ISOName uint8_t
  @param acrc_left left IsoItem_c parameter
  @param acrc_right right IsoName_c parameter
*/
bool lessThan(const IsoItem_c& acrc_left, const IsoName_c& acrc_right);

}
#endif
