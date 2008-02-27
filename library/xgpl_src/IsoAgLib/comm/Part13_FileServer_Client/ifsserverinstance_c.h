#ifndef IFSSERVERINSTANCE_C_H
#define IFSSERVERINSTANCE_C_H

#include "impl/fsserverinstance_c.h"

//start namespace IsoAgLib
namespace IsoAgLib
{

class iFsClientServerCommunication_c;

class iFsServerInstance_c : private __IsoAgLib::FsServerInstance_c
{
  friend class __IsoAgLib::FsServerInstance_c;
  friend class iFsClientServerCommunication_c;
};

//end namespace
}

#endif
