/*
  igenericdata_c.h: Class to independently handle any length of
    data-message (along with CAN-identifier)

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "igenericdata_c.h"

#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>


// Begin Namespace IsoAgLib
namespace IsoAgLib
{
  /** Constructor which has nothing to do */
  iGenericData_c::iGenericData_c()
    : vec_data(), ui32_ident(0)
  { // nothing to do.
  }

  /** Destructor is clearing the vector */
  iGenericData_c::~iGenericData_c()
  { // explicitly clear the vector, although it should be done
    // by the default-called destructor of the stl-vector
    clearVector();
  }

  /** storing data
    @param aui16_pos position to store data in vector
    @param aui8_data data byte
  */
  void iGenericData_c::setDataUi8( uint16_t aui16_pos, uint8_t aui8_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( aui16_pos, sizeof(aui8_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( aui8_data, vec_data, aui16_pos);
  }

  /** storing data
    @param aui16_pos position to store data in vector
    @param ai8_data data byte
  */
  void iGenericData_c::setDataI8( uint16_t aui16_pos, int8_t ai8_data)
  {
      /** first check size of vector */
    CheckSizeOfVectorForWrite( aui16_pos, sizeof(ai8_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( ai8_data, vec_data, aui16_pos);
  }

  /** storing data
    @param aui16_pos position to store data in vector
    @param aui16_data data bytes
  */
  void iGenericData_c::setDataUi16( uint16_t aui16_pos, uint16_t aui16_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( aui16_pos, sizeof(aui16_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( aui16_data, vec_data, aui16_pos);
  }

  /** storing data
    @param aui16_pos position to store data in vector
    @param ai16_data data bytes
  */
  void iGenericData_c::setDataI16( uint16_t aui16_pos, int16_t ai16_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( aui16_pos, sizeof(ai16_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( ai16_data, vec_data, aui16_pos);
  }

  /** storing data
    @param aui16_pos position to store data in vector
    @param aui32_data data bytes
  */
  void iGenericData_c::setDataUi32( uint16_t aui16_pos, uint32_t aui32_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( aui16_pos, sizeof(aui32_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( aui32_data, vec_data, aui16_pos);
  }

  /** storing data
    @param aui16_pos position to store data in vector
    @param ai32_data data bytes
  */
  void iGenericData_c::setDataI32( uint16_t aui16_pos, int32_t ai32_data)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( aui16_pos, sizeof(ai32_data) );
    /** overloaded generic method to set the right data */
    __IsoAgLib::number2LittleEndianString( ai32_data, vec_data, aui16_pos);
  }


  /** deliver data from vector
    @param  aui16_pos position of byte in vector
    @return unsigned data byte at position aui16_pos
  */
  uint8_t iGenericData_c::getDataUi8( uint16_t aui16_pos) const
  {
    if ( CheckSizeOfVectorForRead( aui16_pos, sizeof(uint8_t) ) )
    { /** element is existing */
      return __IsoAgLib::convertLittleEndianStringUi8 (&vec_data[ aui16_pos ]);
    }
    else
    { /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::ProprietaryCan);
      return(0);
    }
  }

  /** deliver data from vector
    @param  aui16_pos position of byte in vector
    @return signed data byte at position aui16_pos
  */
  int8_t iGenericData_c::getDataI8( uint16_t aui16_pos) const
  {
    if ( CheckSizeOfVectorForRead( aui16_pos, sizeof(int8_t) ) )
    { /** element is existing */
      return __IsoAgLib::convertLittleEndianStringI8 (&vec_data[ aui16_pos ]);
    }
    else
    { /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::ProprietaryCan);
      return(0);
    }
  }

  /** deliver data from vector
    @param  aui16_pos position of byte in vector
    @return unsigned data bytes at position aui16_pos
  */
  uint16_t iGenericData_c::getDataUi16( uint16_t aui16_pos) const
  {
    if ( CheckSizeOfVectorForRead( aui16_pos, sizeof(uint16_t) ) )
    { /** element is existing */
      return __IsoAgLib::convertLittleEndianStringUi16 (&vec_data[ aui16_pos ]);
    }
    else
    { /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::ProprietaryCan);
      return(0);
    }
  }

  /** deliver data from vector
    @param  aui16_pos position of byte in vector
    @return signed data bytes at position aui16_pos
  */
  int16_t iGenericData_c::getDataI16( uint16_t aui16_pos) const
  {
    if (CheckSizeOfVectorForRead( aui16_pos, sizeof(int16_t) ) )
    { /** element is existing */
      return __IsoAgLib::convertLittleEndianStringI16 (&vec_data[ aui16_pos ]);
    }
    else
    { /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::ProprietaryCan);
      return(0);
    }
  }

  /** deliver data from vector
    @param  aui16_pos position of byte in vector
    @return unsigned data bytes at position aui16_pos
  */
  uint32_t iGenericData_c::getDataUi32( uint16_t aui16_pos) const
  {
    if ( CheckSizeOfVectorForRead( aui16_pos, sizeof(uint32_t) ) )
    { /** element is existing */
      return __IsoAgLib::convertLittleEndianStringUi32 (&vec_data[ aui16_pos ]);
    }
    else
    { /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::ProprietaryCan);
      return(0);
    }
  }

  /** deliver data from vector
    @param  aui16_pos position of byte in vector
    @return signed data bytes at position aui16_pos
  */
  int32_t iGenericData_c::getDataI32( uint16_t aui16_pos) const
  {
    if ( CheckSizeOfVectorForRead( aui16_pos, sizeof(int32_t) ) )
    { /** element is existing */
      return __IsoAgLib::convertLittleEndianStringI32 (&vec_data[ aui16_pos ]);
    }
    else
    { /** in case of an error */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Range, IsoAgLib::iLibErr_c::ProprietaryCan);
      return(0);
    }
  }

  /** set data stream to vector vec_data
    @param aui16_pos position of byte in vector
    @param arui8_data reference to data for read
    @param aui16_dataLength num of data bytes at position aui16_pos
  */
  void iGenericData_c::setDataStream(uint16_t aui16_pos, const uint8_t& arui8_data, uint16_t aui16_dataLength)
  {
    /** first check size of vector */
    CheckSizeOfVectorForWrite( aui16_pos, aui16_dataLength );
    /** then copy all the bytes over */
    CNAMESPACE::memcpy (&vec_data[ aui16_pos ], &arui8_data, aui16_dataLength);
  }

  /** set data stream to vector vec_data
      Convenience function for those who find a POINTER more intuitive than a reference!
    @param aui16_pos position of byte in vector
    @param apui8_data pointer to data for read
    @param aui16_dataLength num of data bytes at position aui16_pos
  */
  void iGenericData_c::setDataStream(uint16_t aui16_pos, const uint8_t* apui8_data, uint16_t aui16_dataLength)
  { 
    if (apui8_data != NULL )
    { /** apui8_data exist */
      setDataStream (aui16_pos, *apui8_data, aui16_dataLength);
    }
    else
    { /** caught NULL pointer */
      IsoAgLib::getILibErrInstance().registerError(IsoAgLib::iLibErr_c::Precondition, IsoAgLib::iLibErr_c::ProprietaryCan);
    }
  }

  /** deliver a pointer to the wanted data
    NO boundary-check is performed here assuming you know what you access!
    @param aui16_bytePos position of data
    @return uint8_t* pointer to data - will NEVER be NULL
  */
  const uint8_t* iGenericData_c::getDataStream(uint16_t aui16_bytePos) const
  {
    return( &(vec_data[ aui16_bytePos ]) );
  }

  /** check whether the write position is out of range
      @param aui16_pos position in vector to write
      @param aui16_size size of data
  */
  void iGenericData_c::CheckSizeOfVectorForWrite( uint16_t aui16_pos, uint16_t aui16_size )
  {
    const STL_NAMESPACE::vector<uint8_t>::size_type csize_vector = vec_data.size();
    const uint16_t cui16_limit = (aui16_pos + aui16_size);

    /** size of the vector is less than what is needed for this write */
    if (csize_vector < cui16_limit)
    { /** initialize new elements of the vector*/
      vec_data.resize (cui16_limit, 0x00);
    }
  }

  /** check whether the read position is out of range
      @param aui16_pos position in vector to read
      @param aui16_size size of data
      @return false if data does'nt exist otherwise true
   */
  bool iGenericData_c::CheckSizeOfVectorForRead( uint16_t aui16_pos, uint16_t aui16_size ) const
  {
    /** have a look to the size of the vector */
    if ( static_cast<uint32_t>(aui16_pos + aui16_size) > vec_data.size() )
    { /** data at position aui16_pos not existent */
      return(false);
    }
    else
    { /** data ok */
      return(true);
    }
  }

  /** return the length of the data vector */
  uint16_t iGenericData_c::getLen() const
  {
    return vec_data.size();
  }


};

