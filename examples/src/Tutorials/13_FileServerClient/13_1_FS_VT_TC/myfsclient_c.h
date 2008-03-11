//IsoAgLib includes
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/comm/Scheduler/ischeduler_c.h>

//FileServerClient includes
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsclientservercommunication_c.h>
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsmanager_c.h>
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsclient_c.h>

#include <string>

#if 0
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>

#include <time.h>
#include <vector>

#include <mcheck.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#endif

using namespace IsoAgLib;


class MyFsClient_c : public IsoAgLib::iFsClient_c
{
  public:
    MyFsClient_c ():
	c_conftbl (""),
    	b_connected (false),
	b_pending_response (false),
	b_async_read_complete (true),
	fs_step (0),
	b_fsOnline (false)
    {};

    bool notifyOnFileServerOffline (iFsServerInstance_c &/*rc_fsServerInstance*/) {
        b_fsOnline = false;
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "bool notifyOnFileServerOffline(iFsServerInstance_c &/*rc_fsServerInstance*/)"
	    << std::endl;
		#endif

        return true;
    }

    void finishedConnectingToFileserver (iFsServerInstance_c &rc_fileServer) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::finishedConnectingToFileserver"
	    << std::endl;
		#endif

	if (mp_fscom == NULL) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
	    fprintf (stderr, "MyFsClient: iFsClientServerCommunication_c pointer NULL\n");
		#endif
	    CNAMESPACE::abort();
	    b_connected = false;
	}
	else {
	    mp_fscom->requestFsConnection(rc_fileServer);
	    b_connected = true;
        #if defined(DEBUG) && defined(SYSTEM_PC)
	    fprintf (stderr, "finishedConnectionToFileserver: b_fsOnline %s\n",  b_fsOnline? "true" : "false");
		#endif
	}
    }

    void fileserverReady() {
        b_fsOnline = true;
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout << "6_0::fileserverReady" << std::endl;
		#endif

        b_pending_response = false;
    }

    void fileserverError(iFsError ui8_errorCode) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::fileserverError, error code: "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }



    /// FileServer access response functions

    void getFileServerPropertiesResponse (
        uint8_t ui8_versionNumber,
	uint8_t ui8_maxNumberOpenFiles,
	bool b_fsMultiVolumes
    ) { 
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::getFileServerPropertiesResponse version number: "
	    << (uint16_t)ui8_versionNumber
	    << " max open files: "
	    << (uint16_t)ui8_maxNumberOpenFiles
	    << " supports multivolumes: "
	    << b_fsMultiVolumes
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void changeCurrentDirectoryResponse(iFsError ui8_errorCode) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::changeCurrentDirectoryResponse error code: "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void openFileResponse(
        iFsError ui8_errorCode,
	uint8_t ui8_fileHandle,
	bool /*b_caseSensitive*/,
	bool /*b_removable*/,
	bool /*b_longFilenames*/,
	bool /*b_directory*/, 
	bool /*b_volume*/,
	bool /*b_hidden*/,
	bool /*b_readOnly*/
    ) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::openFileResponse error code: "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif
      
        handle = ui8_fileHandle;

        b_pending_response = false;
    }

    void seekFileResponse(iFsError ui8_errorCode, uint32_t /*ui32_position*/) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::seekFileResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void readFileResponse (
        iFsError ui8_errorCode,
	uint16_t ui16_dataLength,
	uint8_t *pui8_data
    ) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        fprintf (stderr, "6_0::readFileResponse: status %d length: %d\n", ui8_errorCode, ui16_dataLength);
		#endif

        //std::cout << "6_0::readFileResponse " << std::hex << (uint16_t)ui8_errorCode << " length: " << ui16_dataLength << std::endl;


        received_data_len = ui16_dataLength;

        for (int i = 0; i < ui16_dataLength; i++)
	    received_data[i] = pui8_data[i];

        received_data[received_data_len] = 0;

        #if defined(DEBUG) && defined(SYSTEM_PC)
        for (int i = 0; i < ui16_dataLength; i++)
	    std::cout << pui8_data[i];

        std::cout << std::dec << std::endl;
		#endif

        b_pending_response = false;
        b_async_read_complete = true;
    }

    void readDirectoryResponse (
        iFsError ui8_errorCode,
	std::vector<struct iFsDirectory *> /*v_directories*/
    ) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::readDirectoryResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void writeFileResponse(iFsError ui8_errorCode, uint16_t /*ui16_dataWritten*/) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::writeFileResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void closeFileResponse(iFsError ui8_errorCode) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::closeFileResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void moveFileResponse(iFsError ui8_errorCode) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::moveFileResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void deleteFileResponse(iFsError ui8_errorCode) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::deleteFileResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void getFileAttributesResponse (
        iFsError ui8_errorCode,
	bool /*b_caseSensitive*/,
	bool /*b_removable*/,
	bool /*b_longFilenames*/,
	bool /*b_directory*/,
	bool /*b_volume*/,
	bool /*b_hidden*/,
	bool /*b_readOnly*/
    ) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::getFileAttributesResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void setFileAttributesResponse(iFsError ui8_errorCode) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::setFileAttributesResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void getFileDateTimeResponse (
        iFsError ui8_errorCode,
	uint16_t ui16_fileYear,
	uint8_t ui8_fileMonth,
	uint8_t ui8_fileDay,
	uint8_t ui8_fileHour,
	uint8_t ui8_fileMinute,
	uint8_t ui8_fileSecond
    ) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::getFileDateTimeResponse "
	    << (uint16_t)ui8_errorCode
	    << " Date: "
	    << ui16_fileYear
	    << "-"
	    << (uint16_t)ui8_fileMonth
	    << "-"
	    << (uint16_t)ui8_fileDay
	    << " time: "
	    << (uint16_t)ui8_fileHour
	    << ":"
	    << (uint16_t)ui8_fileMinute
	    << ":"
	    << (uint16_t)ui8_fileSecond
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void initializeVolumeResponse (
        iFsError ui8_errorCode,
	bool /*b_caseSensitive*/,
	bool /*b_removable*/,
	bool /*b_longFilenames*/,
	bool /*b_directory*/,
	bool /*b_volume*/,
	bool /*b_hidden*/,
	bool /*b_readOnly*/
    ) {
        #if defined(DEBUG) && defined(SYSTEM_PC)
        std::cout
	    << "6_0::initializeVolumeResponse "
	    << (uint16_t)ui8_errorCode
	    << std::endl;
		#endif

        b_pending_response = false;
    }

    void connect (iIdentItem_c& c_myIdent);
    const char *confvar (const char *varname);
    void confvar (const char *varname, char *value);
    void load_conftbl ();
    const char *save_conftbl () { return c_conftbl.c_str(); };

    /// FileServer access response functions END



    std::string c_conftbl;
    bool b_connected;
    bool b_pending_response;
    bool b_async_read_complete;
    int fs_step;
    bool b_fsOnline;
    uint8_t handle;
    uint8_t received_data [15 * 1024];
    int received_data_len;
    iFsClientServerCommunication_c *mp_fscom;
};
