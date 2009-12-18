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

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>

//Fileserver includes
#include "fsserverinstance_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/identitem_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multisend_c.h>
#include <IsoAgLib/comm/Part3_DataLink/impl/multireceive_c.h>

namespace __IsoAgLib
{

class FsClientServerCommunication_c;

/**
  * Class responsible to send and receive commands to and from a fileserver. It also takes care about the commands used by
  * the fileserver as they can differ according to the version of the standard implemented by the fileserver.
  */
class FsCommand_c : Scheduler_Task_c
{
  private:
    /** the TAN counter for the fileserver communication. */
    uint8_t ui8_tan;
    /** the version of the standard used by the fileserver */
    int8_t i8_fsVersion;
    /** the reference to the fsclientservercommunication. responses will be directed to this. */
    FsClientServerCommunication_c &rc_csCom;
    /** the reference to the used fileserver */
    FsServerInstance_c &rc_filerserver;

    /** packet filter functions*/
    void registerPacketFilter();
    void deletePacketFilter();
    void registerMultiReceiveFilter();
    void deleteMultiReceiveFilter();

    /**open file maintenance message*/
    /** number of opened files */
    uint8_t ui8_nrOpenFiles;
    /** when has the last alive been sent?*/
    int32_t i32_lastAliveSent;
    /** buffer containing the standard maintenance message, content equal for all clients! */
    static uint8_t pui8_maintenanceBuffer[8];

    /**send information*/
    CANPkgExt_c c_data;
    /** send package length */
    uint8_t ui8_packetLength;
    /** buffer containing the sent message. */
    uint8_t pui8_sendBuffer[256];
    /** buffer used for multi-receive messages */
    uint8_t *pui_receiveBuffer;
    /** has a response for the sent message been received? */
    bool b_receivedResponse;
    /** when has the last request been sent? time to resend?*/
    int32_t i32_lastrequestAttempt;
    /** how many times has the request been sent?*/
    uint8_t ui8_requestAttempts;
    /** status of a sent multi-message */
    MultiSend_c::sendSuccess_t en_sendSuccessNotify;

    /**recieve information*/
    uint8_t ui8_errorCode;

    /**currentDirectory information*/
    uint8_t *pui8_currentDirectory;

    /**file handle information*/
    uint8_t ui8_fileHandle;

    /**open file information*/
    /** file to be opend. important as return for change current directory */
    uint8_t *pui8_fileName;
    /** flags used to open file */
    uint8_t ui8_flags;
    /** attributes of opend file returned after open. */
    bool b_archive;
    bool b_system;
    bool b_caseSensitive;
    bool b_removable;
    bool b_longFilenames;
    bool b_isDirectory;
    bool b_isVolume;
    bool b_hidden;
    bool b_readOnly;

    /**seek file information*/
    uint8_t ui8_possitionMode;
    int32_t i32_offset;
    uint32_t ui32_possition;

    /**read file information*/
    uint16_t ui16_count;
    bool b_reportHiddenFiles;
    uint8_t *pui8_data;
    std::vector<struct iFsDirectory *> v_dirData;

    /** set file attributes commands **/
    uint8_t ui8_hiddenAtt;
    uint8_t ui8_readOnlyAtt;

    /** file date time information **/
    uint16_t ui16_date;
    uint16_t ui16_time;

    /** function used to send a request. depending on the size of the request single of multi packet send is selected. */
    void sendRequest();
    /** decodes a given attribute and saves the decoded values in the corresponding member variables*/ 
    void decodeAttributes(uint8_t ui8_attributes);

    /** functions to decode fileserver repsonses. */
    void decodeGetCurrentDirectoryResponse();
    void decodeOpenFileResponse();
    void decodeSeekFileResponse();
    void decodeReadFileResponse();
    void decodeReadDirectoryResponse();

    /** draft version of "getFileAttributes" */
    void getFileAttributesDIS(uint8_t ui8_fileHandle);
    void setFileAttributesDIS(uint8_t ui8_fileHandle);
    void getFileDateTimeDIS(uint8_t ui8_fileHandle);

    /** which command are we sending? */
    enum commandtype_en
    {
      en_noCommand = 0x00,
      en_requestProperties = 0x01,
      en_getCurrentDirectory = 0x10,
      en_changeCurrentDirectory = 0x11,
      en_openFile = 0x20,
      en_seekFile,
      en_readFile,
      en_writeFile,
      en_closeFile,
      en_moveFile = 0x30,
      en_deleteFile,
      en_getFileAttributes,
      en_setFileAttributes,
      en_getFileDateTime,
      en_initializeVolume = 0x40
    } en_lastCommand;

  public:

    /** is the command busy, meaning waiting for a response? */
    bool isBusy() { return !b_receivedResponse; }

    /** time evnet function. If no response received, resend request periodically. */
    bool timeEvent(void);

    /**
      * functions used when receiving information.
      */
    CANPkgExt_c& dataBase(){return c_data;}
    CANPkgExt_c& data(){return c_data;}

    /**
      * process received messages. decodes the received responses and forwards it by calling the XXXResponse methods of the
      * FsClientServerCommunication_c.
      */
    bool processMsg();

    /**
      * Implemented function of ElementBase_c.
      */
    void close();
    /**
      * Implemented method of ElementBase_c.
      */
    const char *getTaskName() const { return "FsCommand_c\n"; }

    /**
      * Method called by FsClientServerCommunciation_c. After the response of get current directory, the fileserver is considered to
      * be ready for use.
      */
    iFsCommandErrors getCurrentDirectory();
    /**
      * Method to change the fileserver's current directory. The response-method returns the new current directory to the
      * FsClientServerCommunication_c, as this information is saved there.
      * @param pui8_newDirectory the new current directory of the fileserver.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors changeCurrentDirectory(uint8_t *pui8_newDirectory);

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
    iFsCommandErrors openFile(uint8_t *pui8_fileName, bool b_openExclusive, bool b_openForAppend, bool b_createNewFile, bool b_openForReading, bool b_openForWriting, bool b_openDirectory);
    /**
      * Change the position pointer of a file.
      * @param ui8_fileHandle filehandle of the file to be seeked in.
      * @param ui8_possitionMode position mode of the file, 0 from beginning of file, 1 from current position, 2 from end.
      * @param i32_offset offset from position given in ui8_possitionMode
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors seekFile(uint8_t ui8_fileHandle, uint8_t ui8_possitionMode, int32_t i32_offset);
    /**
      * read content of a file
      * @param ui8_fileHandle filehandle of the file to be read.
      * @param ui16_count number of bytes to be read.
      * @param b_reportHiddenFiles shall hidden files of a directory be reported?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors readFile(uint8_t ui8_fileHandle, uint16_t ui16_count, bool b_reportHiddenFiles);
    /**
      * write data to a file
      * @param ui8_fileHandle filehandle of the file to be written.
      * @param ui16_count number of bytes to be written.
      * @param pui8_data data that shall be written
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors writeFile(uint8_t ui8_fileHandle, uint16_t ui16_count, uint8_t *pui8_data);
    /**
      * close a file
      * @param ui8_fileHandle filehandle of the file to be closed.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors closeFile(uint8_t ui8_fileHandle);

    /**
      * move/copy a file.
      * @param pui8_sourceName source name of the file to be moved/copied
      * @param pui8_destName destination for the file
      * @param b_recursive shall a directory be moved/copied recoursivly?
      * @param b_force override existing/protected files?
      * @param b_copy shall the file be moved or copied?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors moveFile(uint8_t *pui8_sourceName, uint8_t *pui8_destName, bool b_recursive, bool b_force, bool b_copy);
    /**
      * delete a file.
      * @param pui8_fileName name of the file to be deleted.
      * @param b_recursive shall content of a directory be deleted as well?
      * @param b_force shall write-protected files be deleted?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors deleteFile(uint8_t *pui8_fileName, bool b_recursive, bool b_force);
    /**
      * get Attributes of a file.
      * @param pui8_fileName name of the requested file.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors getFileAttributes(uint8_t *pui8_fileName);
    /**
      * change attributed for a file.
      * @param pui8_fileName name of the file that's attributes have to be changed.
      * @param ui8_hiddenAtt change hidden attribute command: 0 clear, 1 set abd 3 leave as is.
      * @param ui8_readOnlyAtt change read only attribute command: 0 clear, 1 set abd 3 leave as is.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors setFileAttributes(uint8_t *pui8_fileName, uint8_t ui8_hiddenAtt, uint8_t ui8_readOnlyAtt);
    /**
      * get date and time of last change of file.
      * @param pui8_fileName name of the reqeusted file.
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors getFileDateTime(uint8_t *pui8_fileName);

    /**
      * initialize volume
      * @param pui8_pathName pathname for the directory.
      * @param ui32_space space to be used.
      * @param b_createVolumeUsingSpace does all space have to be used?
      * @param b_createNewVolume shall a new volum be created?
      * @return 0 if request was sent without problems, else an errorcode is returned.
      */
    iFsCommandErrors initializeVolume(uint8_t *pui8_pathName, uint32_t ui32_space, bool b_createVolumeUsingSpace, bool b_createNewVolume);

    /**
      * Constructor of a FsCommand_c. FsClientServerCommunication_c and FsServerInstance_c have to be provided. Sets
      * the TAN for the communication to the fileserver to 0, the number of open files as well.
      */
    FsCommand_c(FsClientServerCommunication_c &rc_inCsCom, FsServerInstance_c &rc_inFilerserver);

    ~FsCommand_c();

    /**
      * Starts the initialisation process for a fileserver.
      */
    void initFileserver();
    /**
      * returns the fileserver of the FsCommand_c.
      */
    FsServerInstance_c &getFileserver() { return rc_filerserver; }

    /// MultiReceiveClient functions BEGIN
    bool processPartStreamDataChunk(IsoAgLib::iStream_c& refc_stream, bool rb_isFirstChunk, bool rb_isLastChunkAndACKd);
    void reactOnAbort(IsoAgLib::iStream_c& refc_stream);
    bool reactOnStreamStart(const IsoAgLib::ReceiveStreamIdentifier_c& refc_ident, uint32_t rui32_totalLen);
    /// MultiReceiveClient functions END
};

}

#endif
