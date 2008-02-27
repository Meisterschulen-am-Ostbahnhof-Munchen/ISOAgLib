#ifndef FSSERVERINSTANCE_C_H
#define FSSERVERINSTANCE_C_H

#include <IsoAgLib/typedef.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoitem_c.h>

//fileserver includes
#include "../ifsstructs.h"

namespace IsoAgLib
{
  //forward declaration
  class iFsServerInstance_c;
}

//start namespace __IsoAgLib
namespace __IsoAgLib
{

//Forward declaration
class FsManager_c;

class FsServerInstance_c
{
  public:
    /** Different states of the fileserver. busy_reading and busy_writing are states that are set by the fileserver itself
      * with status messages. Online, unreported and connected are internal states that are used by the FsManager_c to indicate
      * the preparation of the fileserver. When detected, the manager sets the state to online and starts requesting the fileserver's
      * properties and volumes. Once the properties and volumes have been requested is is set to unreported, meaning that it has not
      * yet reported to registered FsClientServerConmmunication_c objects. After the first report to registered
      * FsClientServerCommunication_c objects, the status is set to connected and the fileserver will only be reported to
      * FsClientServerCommunication_c objects that newly register to the FsManager_c.
      */
    enum FsState_en
    {
      busy_reading = 0x01,
      busy_writing = 0x02,
      online = 0x10,
      unreported,
      connected,
      used,
      offline,
      removed
    };

  private:
    /** the fileservers manager */
    const FsManager_c &c_fsManager;
    /** the fileserver's isoitem */
    const IsoItem_c &pc_newItem;
    /** fileserver property, busy wrinting/reading */
    FsState_en en_busy;
    /** fileserver property number of open files */
    uint8_t ui8_nrOpenFiles;
    /** the time the fileserver's properties have last been requested. */
    int32_t i32_propertiesReqeusted;
    /** last time a fileserver status message has been received. */
    int32_t i32_lastTime;
    /** have the fileserver properties set? */
    bool b_propertiesSet;

    /** The fileServer Properties */
    /** fileserver's version number*/
    uint8_t ui8_versionNumber;
    /** fileservers maximum number of simultaniously open files. */
    uint8_t ui8_maximumNrFiles;
    /** does fileserver support multiple volumes */
    bool b_isMultivolumes;

    /** the fileservers volumes*/
    std::vector<struct ::iFsDirectory *> v_volumes;

    /** initialization state of the fileserver, as described at the enum */
    FsState_en en_initStatus;

  public:
    /**
      * get the fileserver's status. the values are described in the corresponding enum.
      */
    FsState_en getInitStatus() { return en_initStatus; }
    /**
      * once the fileserver's volumes and properties have been requested and it has
      * been reported to the fsclientservercommunications know to the fsmanager at that
      * point of time.
      */
    void setReported() { en_initStatus = connected;}
    /**
      * If a fileserver is used by a client it is set to used.
      */
    void setUsed() { en_initStatus = used;}
    /**
      * If a fileserver has gone offline, this flag will be set.
      */
    void setOffline() { en_initStatus = offline;}
    /**
      * If a fileserver has gone online, this flag will be set.
      */
    void setOnline() { en_initStatus = online;}
    /**
      * If a fileserver has gone online and was removed, this flag will be set.
      */
    void setRemoved() { en_initStatus = removed;}

    /**
      * sets the fileserver's volumes. this value is set after the property-request. therefore the status of the
      * fileserver is set to unreported. next step is to report it to all known fsclientservercommunciations.
      */
    void setVolumes(std::vector<struct ::iFsDirectory*> v_inVolumes) { v_volumes = v_inVolumes; en_initStatus = unreported; }
    /**
      * Get the fileserver's volumes.
      */
    std::vector<struct ::iFsDirectory*> getFsVolumes() { return v_volumes; }
    /**
      * have to fileserver's volumes been requested successfully?
      */
    bool knowsVolumes() { return (v_volumes.size() != 0)?true:false;}

    /**
      * constructor of the class. necessary parameters are the fileserver's isoitem, isoname and the manager of the
      * fileserver.
      */
    FsServerInstance_c(const IsoItem_c &pref_newItem, const FsManager_c &ref_fsManager);

    /**
      * get the fileserver's source-addresse.
      */
    uint8_t getSa() {return pc_newItem.nr(); }

    /**
      * get the fileserver's isoitem.
      */
    const IsoItem_c &getIsoItem() {  return pc_newItem; }

    /**
      * get the fileserver's isoname
      */
    const IsoName_c &getIsoName() {  return pc_newItem.isoName(); }

    /**
      * called every time the fileserver's manager recives a fileserver status message.
      */
    void updateServerStatus(uint8_t rui8_busy, uint8_t rui8_nrOpenFiles, int32_t ri32_lastTime);

    /**
      * returns the last time a fileserver statusmessage has been received.
      */
    int32_t getLastTime() { return i32_lastTime; }

    /**
      * returns the last time a fileserver statusmessage has been received.
      */
    void resetLastTime() { i32_lastTime = -1; }

    /**
      * returns the last time the fileserver's properties have been requested.
      */
    int32_t getPropertiesReqeusted() { return i32_propertiesReqeusted; }
    /**
      * called everytime the fileserver's properties are requested. sets the time the properties have been requested to the
      * current time.
      */
    void setPropertiesReqeusted() {
      en_initStatus = online;
      i32_propertiesReqeusted = HAL::getTime ();
    }

    /**
      * sets the fileservers properties as defined in the ISO-11783 standard
      */
    void setFsProperties(uint8_t rui8_versionNumber, uint8_t rui8_maximumNrFiles, bool rb_isMultivolumes);

    uint8_t getMaxNrOpenFiles() { return ui8_maximumNrFiles; }
    /**
      * does the fileserver support multivolumes?
      */
    bool getSupportsMultiVolumes() { return b_isMultivolumes; }

    /**
      * have the properties of the fileserver set?
      */
    bool getPropertiesSet() {return b_propertiesSet;}

    /** explicit conversion to reference of interface class type */
    IsoAgLib::iFsServerInstance_c* toInterfacePointer();

    /** gets the version of the ISO-standard used by the fileserver
      * @return -1 if property has not been requested yet, else number of standard used as defined in Part 13 Filserver
      */
   int8_t getStandardVersion() { return b_propertiesSet?(int8_t)ui8_versionNumber:-1;}

};

//end namespace
}

#endif
