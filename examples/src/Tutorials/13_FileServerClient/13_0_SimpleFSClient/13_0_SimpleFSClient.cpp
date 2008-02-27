//IsoAgLib includes
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>

//FileServerClient includes
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsclientservercommunication_c.h>
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsmanager_c.h>
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsclient_c.h>


#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>

#include <time.h>
#include <vector>

#include <mcheck.h>

using namespace IsoAgLib;

iFsClientServerCommunication_c *c_tut60sfsc;
bool b_fsOnline;

class MyFsClient : public IsoAgLib::iFsClient_c
{
  public:

    bool notifyOnFileServerOffline(iFsServerInstance_c &/*rc_fsServerInstance*/)
    {
      b_fsOnline = false;
      std::cout << "bool notifyOnFileServerOffline(iFsServerInstance_c &/*rc_fsServerInstance*/)" << std::endl;
      return true;
    }

    void finishedConnectingToFileserver(iFsServerInstance_c &rc_fileServer)
    {
      std::cout << "6_0::finishedConnectingToFileserver" << std::endl;
      c_tut60sfsc->requestFsConnection(rc_fileServer);
    }
    void fileserverReady()
    {
      b_fsOnline = true;
      std::cout << "6_0::fileserverReady" << std::endl;
    }
    void fileserverError(iFsError ui8_errorCode)
    {
      std::cout << "6_0::fileserverError, error code: " << (uint16_t)ui8_errorCode << std::endl;
    }

    /// FileServer access response functions

    void getFileServerPropertiesResponse(uint8_t ui8_versionNumber, uint8_t ui8_maxNumberOpenFiles, bool b_fsMultiVolumes)
    { 
      std::cout << "6_0::getFileServerPropertiesResponse version number: " << (uint16_t)ui8_versionNumber << " max open files: " << (uint16_t)ui8_maxNumberOpenFiles << " supports multivolumes: " << b_fsMultiVolumes << std::endl;
    }

    void changeCurrentDirectoryResponse(iFsError ui8_errorCode)
    {
      std::cout << "6_0::changeCurrentDirectoryResponse error code: " << (uint16_t)ui8_errorCode << std::endl;
    }

    void openFileResponse(iFsError ui8_errorCode,
                          uint8_t /*ui8_fileHandle*/,
                          bool /*b_caseSensitive*/,
                          bool /*b_removable*/,
                          bool /*b_longFilenames*/,
                          bool /*b_directory*/, 
                          bool /*b_volume*/,
                          bool /*b_hidden*/,
                          bool /*b_readOnly*/)
    {
      std::cout << "6_0::openFileResponse error code: " << (uint16_t)ui8_errorCode << std::endl;
    }
    void seekFileResponse(iFsError ui8_errorCode, uint32_t /*ui32_position*/)
    {
      std::cout << "6_0::seekFileResponse " << (uint16_t)ui8_errorCode << std::endl;
    }
    void readFileResponse(iFsError ui8_errorCode, uint16_t ui16_dataLength, uint8_t *pui8_data)
    {
      std::cout << "6_0::readFileResponse " << std::hex << (uint16_t)ui8_errorCode << " length: " << ui16_dataLength << std::endl;


      for (int i = 0; i < ui16_dataLength; i++)
        std::cout << pui8_data[i];

      std::cout << std::dec << std::endl;
    }
    void readDirectoryResponse(iFsError ui8_errorCode, std::vector<struct iFsDirectory *> /*v_directories*/)
    {
      std::cout << "6_0::readDirectoryResponse " << (uint16_t)ui8_errorCode << std::endl;
    }
    void writeFileResponse(iFsError ui8_errorCode, uint16_t /*ui16_dataWritten*/)
    {
      std::cout << "6_0::writeFileResponse " << (uint16_t)ui8_errorCode << std::endl;
    }
    void closeFileResponse(iFsError ui8_errorCode)
    {
      std::cout << "6_0::closeFileResponse" << (uint16_t)ui8_errorCode << std::endl;
    }

    void moveFileResponse(iFsError ui8_errorCode)
    {
      std::cout << "6_0::moveFileResponse " << (uint16_t)ui8_errorCode << std::endl;
    }
    void deleteFileResponse(iFsError ui8_errorCode)
    {
      std::cout << "6_0::deleteFileResponse " << (uint16_t)ui8_errorCode << std::endl;
    }
    void getFileAttributesResponse(iFsError ui8_errorCode,
                                   bool /*b_caseSensitive*/,
                                   bool /*b_removable*/,
                                   bool /*b_longFilenames*/,
                                   bool /*b_directory*/,
                                   bool /*b_volume*/,
                                   bool /*b_hidden*/,
                                   bool /*b_readOnly*/)
    {
      std::cout << "6_0::getFileAttributesResponse " << (uint16_t)ui8_errorCode << std::endl;
    }
    void setFileAttributesResponse(iFsError ui8_errorCode)
    {
      std::cout << "6_0::setFileAttributesResponse " << (uint16_t)ui8_errorCode << std::endl;
    }
    void getFileDateTimeResponse(iFsError ui8_errorCode,
                                 uint16_t ui16_fileYear,
                                 uint8_t ui8_fileMonth,
                                 uint8_t ui8_fileDay,
                                 uint8_t ui8_fileHour,
                                 uint8_t ui8_fileMinute,
                                 uint8_t ui8_fileSecond)
    {
      std::cout << "6_0::getFileDateTimeResponse " << (uint16_t)ui8_errorCode << " Date: " << ui16_fileYear << "-" << (uint16_t)ui8_fileMonth << "-" << (uint16_t)ui8_fileDay << " time: " << (uint16_t)ui8_fileHour << ":" << (uint16_t)ui8_fileMinute << ":" << (uint16_t)ui8_fileSecond << std::endl;
    }

    void initializeVolumeResponse(iFsError ui8_errorCode,
                                   bool /*b_caseSensitive*/,
                                   bool /*b_removable*/,
                                   bool /*b_longFilenames*/,
                                   bool /*b_directory*/,
                                   bool /*b_volume*/,
                                   bool /*b_hidden*/,
                                   bool /*b_readOnly*/)
    {
      std::cout << "6_0::initializeVolumeResponse " << (uint16_t)ui8_errorCode << std::endl;
    }

    /// FileServer access response functions END
};

int main()
{
  int32_t now = time(NULL);
  int32_t befehl = 0;
  b_fsOnline = false;

  getIcanInstance().init (0); // CAN-Bus 0 (with defaulting 250 kbit)

  // start address claim of the local identity/member
  iIdentItem_c c_myIdent (2,     // rui8_indGroup
                          2,     // rui8_devClass
                          0,     // rui8_devClassInst
                          41,    // rb_func
                          0x7FF, // rui16_manufCode
                          27);   // rui32_serNo
                          // further parameters use the default values as given in the constructor

  MyFsClient myClient;

  std::vector<iFsWhitelist_c *> v_fsWhitelist;

  iFsWhitelist_c tmpwlst;
  tmpwlst.b_requestRemoveableMedium = true;
  tmpwlst.i32_requestedManufacturer = -1;
  tmpwlst.ui8_minSimultaniouslyOpenFiles = 5;

  v_fsWhitelist.push_back(&tmpwlst);

  // Call to fsManager to init and register client
  c_tut60sfsc = getIFsManagerInstance().initFsClient(c_myIdent, myClient, v_fsWhitelist);

  //check if could initialize client
  if (c_tut60sfsc == NULL)
  {
    #if defined(DEBUG) && defined(SYSTEM_PC)
    EXTERNAL_DEBUG_DEVICE << "Could not init fsClient." << EXTERNAL_DEBUG_DEVICE_ENDL;
    abort();
    #else
    return 1; // Return from main() with error indicated
    #endif
  }

  /** IMPORTANT:
    - The following loop could be replaced of any repeating call of
      IsoAgLib::getISchedulerInstance().timeEvent();
      which is needed to perform all internal activities of the IsoAgLib.
    - Define the time intervall for IsoAgLib::getISchedulerInstance().timeEvent()
      in a way, that allows IsoAgLib to trigger all reactions on BUS
      in the ISO 11783 defined time resolution - especially the address
      claim process has some tight time restrictions, that suggest
      a trigger rate of at least 100msec ( you could call the function
      only during address claim, mask updload and other special
      circumstances in a high repetition rate )
    - The main loop is running until iSystem_c::canEn() is returning false.
      This function can be configured by the #define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
      in isoaglib_config.h to ignore short CAN_EN loss.
    - This explicit control of power state without automatic powerdown on CanEn loss
      can be controled with the central config define
      #define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
      or
      #define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownOnCanEnLoss
      in the header xgpl_src/Application_Config/isoaglib_config.h
    - This can be also controlled during runtime with the function call:
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
      or
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
  */
  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  {

    if (!b_fsOnline)
      now = time(NULL) - 4;

    if ((time(NULL) - now) == 5 && befehl == 0)
    {
      std::vector<struct iFsDirectory *> v_volumes = c_tut60sfsc->getFsVolumes();

      for (std::vector<struct iFsDirectory *>::iterator it_directories = v_volumes.begin();
          it_directories != v_volumes.end();
          it_directories++)
      {
        std::cout << std::endl << (*it_directories)->pui8_filename;
        std::cout << " is caseSensitive: " << (*it_directories)->b_caseSensitive;
        std::cout << " is removeable: " << (*it_directories)->b_removable;
        std::cout << " supports long filenames: " << (*it_directories)->b_longFilenames;
        std::cout << " is hidden: " << (*it_directories)->b_hidden;
        std::cout << " is readonly: " << (*it_directories)->b_readOnly;
        std::cout << " is size: " << (*it_directories)->ui32_size;
        std::cout << std::endl << std::endl;
      }
      befehl++;
    }
    if ((time(NULL) - now) == 5 && befehl == 1)
    {
      c_tut60sfsc->openFile((uint8_t *)"\\\\INTERNAL\\MCMC2047", false, false, true, false, false, true);
      befehl++;
    }
    if ((time(NULL) - now) == 6 && befehl == 2)
    {
      c_tut60sfsc->openFile((uint8_t *)"\\\\INTERNAL\\MCMC2047\\TMP", false, false, true, false, false, true);
      befehl++;
    }
    if ((time(NULL) - now) == 7 && befehl == 3)
    {
      c_tut60sfsc->openFile((uint8_t *)"\\\\INTERNAL\\MCMC2047\\TMP\\TESTDIR1", false, false, true, false, false, true);
      befehl++;
    }
    if ((time(NULL) - now) == 8 && befehl == 4)
    {
      c_tut60sfsc->openFile((uint8_t *)"\\\\INTERNAL\\MCMC2047\\TMP\\TESTDIR2", false, false, true, false, false, true);
      befehl++;
    }
    if ((time(NULL) - now) == 9 && befehl == 5)
    {
      c_tut60sfsc->closeFile(0);
      befehl++;
    }
    if ((time(NULL) - now) == 10 && befehl == 6)
    {
      c_tut60sfsc->closeFile(1);
      befehl++;
    }
    if ((time(NULL) - now) == 11 && befehl == 7)
    {
      c_tut60sfsc->closeFile(2);
      befehl++;
    }
    if ((time(NULL) - now) == 12 && befehl == 8)
    {
      c_tut60sfsc->closeFile(3);
      befehl++;
    }
    if ((time(NULL) - now) == 13 && befehl == 9)
    {
      c_tut60sfsc->changeCurrentDirectory((uint8_t *)"\\\\SDCARD");
      befehl++;
    }
    if ((time(NULL) - now) == 14 && befehl == 10)
    {
      c_tut60sfsc->openFile((uint8_t *)"\\\\INTERNAL\\bernd.txt", false, false, true, true, true, false);
      befehl++;
    }
    if ((time(NULL) - now) == 15 && befehl == 11)
    {
      c_tut60sfsc->writeFile(0, 10, (uint8_t *)"123456789a");
      befehl++;
    }
    if ((time(NULL) - now) == 16 && befehl == 12)
    {
      c_tut60sfsc->closeFile(0);
      befehl++;
    }
    if ((time(NULL) - now) == 17 && befehl == 13)
    {
      c_tut60sfsc->moveFile((uint8_t *)"\\\\INTERNAL\\BERND.TXT", (uint8_t *)"\\\\SDCARD\\BERND.TXT", false, true, true);
      befehl++;
    }
    if ((time(NULL) - now) == 18 && befehl == 14)
    {
      c_tut60sfsc->deleteFile((uint8_t *)"\\\\INTERNAL\\BERND.TXT", true, true);
      befehl++;
    }
    if ((time(NULL) - now) == 19 && befehl == 15)
    {
      c_tut60sfsc->getFileAttributes((uint8_t *)"\\\\SDCARD\\BERND.TXT");
      befehl++;
    }
    if ((time(NULL) - now) == 20 && befehl == 16)
    {
      c_tut60sfsc->setFileAttributes((uint8_t *)"\\\\SDCARD\\BERND.TXT", 1, 1);
      befehl++;
    }
    if ((time(NULL) - now) == 21 && befehl == 17)
    {
      c_tut60sfsc->getFileDateTime((uint8_t *)"\\\\SDCARD\\BERND.TXT");
      befehl++;
    }
    if ((time(NULL) - now) == 22 && befehl == 18)
    {
      c_tut60sfsc->initializeVolume((uint8_t *)"\\\\SDCARD", 12345, true, true);
      befehl++;
    }

    if ((time(NULL) - now) == 23 && befehl == 19)
    {
      c_tut60sfsc->openFile((uint8_t *)"\\\\SDCARD\\bernd.txt", false, false, true, true, false, false);
      befehl++;
    }
    if ((time(NULL) - now) == 24 && befehl == 20)
    {
      c_tut60sfsc->readFile(0, 15, true);
      befehl++;
    }
    if ((time(NULL) - now) == 25 && befehl == 21)
    {
      c_tut60sfsc->closeFile(0);
      befehl++;
    }

    // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_idleTimeSpread = getISchedulerInstance().timeEvent();

    // The following sleep mechanism will be changed with the new scheduler reporting back the idle-time
    // no need to sleep on single-task systems
    #ifdef SYSTEM_PC
     #ifdef WIN32
     if ( i32_idleTimeSpread > 0 ) Sleep( i32_idleTimeSpread );
     #else
     if ( i32_idleTimeSpread > 0 ) iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
     #endif
    #endif
  }
}
