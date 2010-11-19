/*
  fscommand_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef FSCOMMAND_C_H
#define FSCOMMAND_C_H

// own
#include "fsserverinstance_c.h"

// ISOAgLib
#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>


namespace __IsoAgLib
{
// forward declarations
class FsClientServerCommunication_c;

/**
  * Class responsible to send and receive commands to and from a fileserver. It also takes care about the commands used by
  * the fileserver as they can differ according to the version of the standard implemented by the fileserver.
  */
class FsCommand_c : CanCustomer_c
{
  public:
    FsCommand_c(
      FsClientServerCommunication_c &rc_inCsCom,
      FsServerInstance_c &rc_inFilerserver);

    ~FsCommand_c();

  class SchedulerTaskProxy_c : public Scheduler_Task_c {
  public:
    typedef FsCommand_c Owner_t;

    SchedulerTaskProxy_c(Owner_t &art_owner) : mrt_owner(art_owner)
    {
      // explicitly set the time-period
      setTimePeriod (100); // currently use Scheduler_Task's default
    }

    virtual ~SchedulerTaskProxy_c() {}

  private:
    virtual bool timeEvent() { return mrt_owner.timeEvent(); }
#if DEBUG_SCHEDULER
    virtual const char *getTaskName() const { return "FsCommand_c\n"; }
#endif

    // SchedulerTaskProxy_c shall not be copyable. Otherwise the
    // reference to the containing object would become invalid.
    SchedulerTaskProxy_c(SchedulerTaskProxy_c const &);

    SchedulerTaskProxy_c &operator=(SchedulerTaskProxy_c const &);

    Owner_t &mrt_owner;
  };
  typedef SchedulerTaskProxy_c Task_t;

  private:
    // forbid copy construction
    FsCommand_c(const FsCommand_c& );

    /** function used to send a request. depending on the size of the request single of multi packet send is selected. **/
    enum RequestType_en { RequestInitial, RequestRetry };
    void sendRequest (RequestType_en);

    /** decodes a given attribute and saves the decoded values in the corresponding member variables **/
    void decodeAttributes(uint8_t ui8_attributes);

    /** functions to decode fileserver repsonses. **/
    void decodeGetCurrentDirectoryResponse();
    void decodeOpenFileResponse();
    void decodeSeekFileResponse();
    void decodeReadFileResponse();
    void decodeReadDirectoryResponse();

    /** clean up when done **/
    void clearDirectoryList();
    void doCleanUp();

    /** internal read file use for read file and read directory **/
    IsoAgLib::iFsCommandErrors readFile(uint8_t ui8_inFileHandle, uint16_t ui16_count, bool b_reportHiddenFiles);

    /** which command are we sending? **/
    enum commandtype_en
    {
      en_noCommand = 0x00,
      en_requestProperties = 0x01,
      en_getCurrentDirectory = 0x10,
      en_changeCurrentDirectory = 0x11,
      en_openFile = 0x20,
      en_seekFile = 0x21,
      en_readFile = 0x22,
      en_writeFile = 0x23,
      en_closeFile = 0x24,
      en_moveFile = 0x30,
      en_deleteFile = 0x31,
      en_getFileAttributes = 0x32,
      en_setFileAttributes = 0x33,
      en_getFileDateTime = 0x34,
      en_initializeVolume = 0x40
    };
    commandtype_en en_lastCommand;

  public:

    /** is the command busy, meaning waiting for a response? **/
    bool isBusy() { return !b_receivedResponse; }

    /** time event function. If no response received, resend request periodically. **/
    bool timeEvent(void);

    /** send the current command out via 8 Byte CAN */
    void sendSinglePacket();

    /** send the current command out via TP/ETP.
        In case it coulnd't be sent out right now
        set "mb_retryMultiPacketSend=true" so the timeEvent()
        will retry until it could be started... */
    void sendMultiPacketTry();

    /**
      * functions used when receiving information.
      */
    CANPkgExt_c& dataBase() { return c_data; }
    CANPkgExt_c& data() { return c_data; }

    /**
      * process received messages. decodes the received responses and forwards it by calling the XXXResponse methods of the
      * FsClientServerCommunication_c.
      */
    bool processMsg();

    /**
      * Method called by FsClientServerCommunciation_c. After the response of get current directory, the fileserver is considered to
      * be ready for use.
      */
    IsoAgLib::iFsCommandErrors getCurrentDirectory();
    /**
      * Method to change the fileserver's current directory. The response-method returns the new current directory to the
      * FsClientServerCommunication_c, as this information is saved there.
      * @param pui8_newDirectory the new current directory of the fileserver.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors changeCurrentDirectory(uint8_t *pui8_newDirectory);

    /**
      * Method to open a file and request a filehandle for the opened file.
      * @param pui8_fileName the name of the file to be openend.
      * @param b_openExclusive open file with exclusive access?
      * @param bool b_openForAppend open writeable file for append at the end, or allow random access.
      * @param bool b_createNewFile create not existing file?
      * @param bool b_openForReading open for reading?
      * @param bool b_openForWriting open for writing?
      * @param bool b_openDirectory open directory?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors openFile(uint8_t *pui8_fileName, bool b_openExclusive, bool b_openForAppend, bool b_createNewFile, bool b_openForReading, bool b_openForWriting, bool b_openDirectory);
    /**
      * Change the position pointer of a file.
      * @param ui8_fileHandle filehandle of the file to be seeked in.
      * @param ui8_possitionMode position mode of the file, 0 from beginning of file, 1 from current position, 2 from end.
      * @param i32_offset offset from position given in ui8_possitionMode
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors seekFile(uint8_t ui8_fileHandle, uint8_t ui8_possitionMode, int32_t i32_offset);
    /**
      * read content of a file
      * @param ui8_fileHandle filehandle of the file to be read.
      * @param ui16_count number of bytes to be read.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors readFile(uint8_t ui8_fileHandle, uint16_t ui16_count);
    /**
      * read content of a directory
      * @param ui8_fileHandle directoryhandle of the directory to be read.
      * @param ui16_count number of entries to be read.
      * @param b_reportHiddenFiles shall hidden files of a directory be reported?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors readDirectory(uint8_t ui8_fileHandle, uint16_t ui16_count, bool b_reportHiddenFiles);
    /**
      * write data to a file
      * @param ui8_fileHandle filehandle of the file to be written.
      * @param ui16_count number of bytes to be written.
      * @param pui8_data data that shall be written
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors writeFile(uint8_t ui8_fileHandle, uint16_t ui16_count, uint8_t *pui8_data);
    /**
      * close a file
      * @param ui8_fileHandle filehandle of the file to be closed.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors closeFile(uint8_t ui8_fileHandle);

    /**
      * move/copy a file.
      * @param pui8_sourceName source name of the file to be moved/copied
      * @param pui8_destName destination for the file
      * @param b_recursive shall a directory be moved/copied recoursivly?
      * @param b_force override existing/protected files?
      * @param b_copy shall the file be moved or copied?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors moveFile(uint8_t *pui8_sourceName, uint8_t *pui8_destName, bool b_recursive, bool b_force, bool b_copy);
    /**
      * delete a file.
      * @param pui8_fileName name of the file to be deleted.
      * @param b_recursive shall content of a directory be deleted as well?
      * @param b_force shall write-protected files be deleted?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors deleteFile(uint8_t *pui8_fileName, bool b_recursive, bool b_force);
    /**
      * get Attributes of a file.
      * @param pui8_fileName name of the requested file.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors getFileAttributes(uint8_t *pui8_fileName);
    /**
      * change attributed for a file.
      * @param pui8_fileName name of the file that's attributes have to be changed.
      * @param ui8_hiddenAtt change hidden attribute command: 0 clear, 1 set abd 3 leave as is.
      * @param ui8_readOnlyAtt change read only attribute command: 0 clear, 1 set abd 3 leave as is.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors setFileAttributes(uint8_t *pui8_fileName, uint8_t ui8_hiddenAtt, uint8_t ui8_readOnlyAtt);
    /**
      * get date and time of last change of file.
      * @param pui8_fileName name of the reqeusted file.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors getFileDateTime(uint8_t *pui8_fileName);

    /**
      * initialize volume
      * @param pui8_pathName pathname for the directory.
      * @param ui32_space space to be used.
      * @param b_createVolumeUsingSpace does all space have to be used?
      * @param b_createNewVolume shall a new volum be created?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    IsoAgLib::iFsCommandErrors initializeVolume(uint8_t *pui8_pathName, uint32_t ui32_space, bool b_createVolumeUsingSpace, bool b_createNewVolume);

    /**
      * Starts the initialisation process for a fileserver.
      */
    void requestProperties();

    /**
      * returns the fileserver of the FsCommand_c.
      */
    FsServerInstance_c &getFileserver() { return rc_fileserver; }

    /**
      * set connection state to keep open even after all files have been closed
      * @param b_keepOpen connection state.
      * @param b_forceClose force the connection to close even if files are open.
      */
    void setKeepConnectionOpen( bool b_keepOpen, bool b_forceClose=false );
    /**
      * get conneciton state
      * @return true if the keep connection open is current set else false is returned.
      */
    bool getKeepConnectionOpen() const { return b_keepConnectionOpen; }

    /** MultiReceiveClient functions BEGIN **/
    bool processPartStreamDataChunk (Stream_c& refc_stream, bool rb_isFirstChunk, bool rb_isLastChunkAndACKd);
    void reactOnAbort(Stream_c& refc_stream);
    bool reactOnStreamStart(const ReceiveStreamIdentifier_c& refc_ident, uint32_t rui32_totalLen);
    /** MultiReceiveClient functions END **/

    int getSingletonVecKey() const { return 0; }

  private:
    // member variable instead of multiple inheritance
    Task_t mc_schedulerTask;
    /** the TAN counter for the fileserver communication. **/
    uint8_t ui8_tan;
    /** the reference to the fsclientservercommunication. responses will be directed to this. **/
    FsClientServerCommunication_c &rc_csCom;
    /** the reference to the used fileserver **/
    FsServerInstance_c &rc_fileserver;

    /** open file maintenance message **/
    /** number of opened files **/
    uint8_t ui8_nrOpenFiles;
    /** keep connection open even after all files are closed **/
    bool b_keepConnectionOpen;
    /** when has the last alive been sent? **/
    int32_t i32_lastAliveSent;
    /** buffer containing the standard maintenance message, content equal for all clients! **/
    static uint8_t pui8_maintenanceBuffer[8];

    /** send information **/
    CANPkgExt_c c_data;
    /** send package length **/
    uint8_t ui8_packetLength;
    /** buffer containing the sent message. **/
    uint8_t pui8_sendBuffer[256];
    /** buffer used for multi-receive messages **/
    uint8_t *pui8_receiveBuffer;
    uint32_t ui32_recBufAllocSize;
    uint8_t ui8_recTan;
    /** has a response for the sent message been received? **/
    bool b_receivedResponse;
    /** when has the last request been sent? time to resend? **/
    int32_t i32_lastrequestAttempt;
    /** how many times has the request been sent? **/
    uint8_t ui8_requestAttempts;
    /** status of a sent multi-message **/
    MultiSend_c::sendSuccess_t en_sendSuccessNotify;

    /** recieve information **/
    uint8_t ui8_errorCode;

    /** currentDirectory information **/
    uint8_t *pui8_currentDirectory;
    uint16_t ui16_curDirAllocSize;

    /** file handle information **/
    uint8_t ui8_fileHandle;

    /** open file information **/
    /** file to be opend. important as return for change current directory **/
    uint8_t *pui8_fileName;
    uint16_t ui16_fileNameAllocSize;
    /** flags used to open file **/
    uint8_t ui8_flags;
    /** attributes of opend file returned after open. **/
    bool b_archive;
    bool b_system;
    bool b_caseSensitive;
    bool b_removable;
    bool b_longFilenames;
    bool b_isDirectory;
    bool b_isVolume;
    bool b_hidden;
    bool b_readOnly;

    /** seek file information **/
    uint8_t ui8_possitionMode;
    int32_t i32_offset;
    uint32_t ui32_possition;

    /** read file information */
    uint16_t ui16_count;
    bool b_reportHiddenFiles;
    uint8_t *pui8_data;
    uint16_t ui16_dataAllocSize;
    IsoAgLib::iFsDirList v_dirData;
    bool b_readDirectory;

    /** set file attributes commands **/
    uint8_t ui8_hiddenAtt;
    uint8_t ui8_readOnlyAtt;

    /** file date time information **/
    uint16_t ui16_date;
    uint16_t ui16_time;

    /** filter information **/
    bool b_receiveFilterCreated;

    /// FS Initial Query Part (Properties/Volumes)
    bool mb_initialQueryStarted;

    /// Is this instance for Initializing the FsInstance_c
    /// (call callbacks and do initialization procedure then)
    /// or is it for normal Application-triggered operation?
    bool mb_initializingFileserver;

    /// Indicate if a MultiSend was started and it
    /// needs to be waited for the Success/Abort.
    bool mb_waitForMultiSendFinish;

    /// If the MultiPacketSend couldn't be started
    /// it needs to be retried in the timeEvent()
    bool mb_retryMultiPacketSend;
};

} // __IsoAgLib

#endif
