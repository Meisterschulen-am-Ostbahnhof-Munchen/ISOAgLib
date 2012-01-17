#ifndef CAN_FILTERING_H
#define CAN_FILTERING_H

#include <string>
#include "can_server.h"

// Author: Martin Wodok
namespace can_filtering
{
  std::string config( const std::string &cmd );
  bool pass( unsigned bus, uint32_t id, unsigned dlc, uint8_t *databytes );
}

#endif
