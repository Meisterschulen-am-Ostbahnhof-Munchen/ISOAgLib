#ifndef ICANFIFO_H
#define ICANFIFO_H

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL


#ifdef SYSTEM_PC
#define NEAR
#else
#define NEAR _near
#endif

namespace HAL {

extern "C" {



const uint8_t cui8_numMsbObj = CONFIG_CAN_DEFAULT_MAX_OBJ_NR + 1;

typedef struct {

  MASK_TYPE fbMask; // ui32 or ui16 result of mask & 0x7FF if necessary
  MASK_TYPE fbFilter; //ui32 or ui16 result of the (Mask & Filter & 0x7FF if necessary )
  int32_t i32_filterBoxVecIdx; // index of the FilterBox in the array arrFilterBox
} comparableTable_s;


typedef struct {
      uint32_t dwId;                          /** Identifier */
      uint8_t bXtd;                           /** Type of Bit Identifier extended or standard*/
      uint8_t bDlc;                           /** Len of the data */
      uint8_t abData[8];                      /** Data received **/
      int32_t i32_time;                       /** time of received message */
      int32_t i32_fbIndex;
}fifoData_s;


bool updateTable(uint8_t aui8_busNum,uint8_t aui8_objNr,uint32_t aui32_numEl, comparableTable_s* p_newTable );


void freeIrqTable(uint8_t aui8_busNum);

void setIrqMinReceiveObjNr(uint8_t aui8_busNum, uint8_t aui8_minRecObjNr);

void iFifoPopMessage(uint8_t aui8_busNumber);


int32_t fifo_useMsgObjGet(uint8_t aui8_busNumber,__IsoAgLib::CanPkg_c* apc_data);

#ifdef DEBUG
void printIrqTable(uint8_t aui8_busNumber);
#endif


}


int32_t iFifoReadFbIdx(uint8_t aui8_busNumber,int32_t& ri32_fbIdx,int32_t& ri32_rcvTime,uint32_t& ri32_ident,__IsoAgLib::Ident_c::identType_t& r_msgtype);
}

#endif
#endif
