/***************************************************************************
                          actor_target_extensions.cpp - source for PC specific
                                                extensions for the HAL
                                                for actor
                             -------------------
    begin                : Sat Jan 01 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/
#define  STRICT
#define CVS_REV "$Revision: 0 $"
#define RTE_CLIENT
#define RTE_REV CVS_REV
#include "rte_global.h"
#include "actor_target_extension_rte.h"
#include <rte_client.h>
#include <rte_dout.h>
#include "actor.h"
#include "../channel_rte.h"
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/liberr_c.h>
#include <IsoAgLib/hal/pc/errcodes.h>
#include <cstdio>

namespace __HAL {

struct actor_channel_dimensions_s {
  enum index_e {
    begin = channel_dimensions_s::output_begin,
    end = channel_dimensions_s::output_end
  };
  enum count_e {
    num = end - begin
  };
  enum { range_id = 0 };
};

static struct channelOutput_s {
  bool  m_is_open;
  dout_c m_rte_channel;
  bool  m_last_value;
} channelsDigitalOutput[actor_channel_dimensions_s::num];

static void init_once()
{
  static bool is_again = false;

  if (!is_again) {
    is_again = true;
    rte_string_c const host = getenv("RTE_HOST") ? "" : "rte1";
    if (!rte_is_init()) {
      int rte_status = rte_connect(host);
      if (rte_status < 0) {
        cerr << "Unable to connect RTE server." << endl;
        exit(1);
      } else {
        cout << "Connected to RTE" << endl;
      }
    }
  }
}

/* ************************************** */
/* ****** Actor_c O BIOS functions  ******* */
/* ************************************** */
/* define the frequency of the pwm signal */
int16_t  setPwmFreq(uint8_t bOutputGroup, uint32_t dwFrequency)
{
  printf("PWM von Kanal %hi auf %i gesetzt\n", bOutputGroup, dwFrequency);
  return HAL_NO_ERR;
}

template <>
void report_bit_state<actor_channel_dimensions_s>(
  uint8_t logical_channel_number,
  bool bit_state)
{
  channelsDigitalOutput[logical_channel_number].m_last_value = bit_state;
}

static int16_t init_digout_once(uint8_t bOutput)
{
  init_once();
  channelOutput_s &r_cdo = channelsDigitalOutput[bOutput];
  if (r_cdo.m_is_open)
    ;
  else {
    rtd_channel_t rte_channel;
    bool got_channel = get_rte_channel<actor_channel_dimensions_s>(rte_channel, bOutput);
    if (got_channel) {
      dout_c &r_c = r_cdo.m_rte_channel;
      r_c.set_channel(rte_channel);
      r_c.set_state_handler(dinout_state<actor_channel_dimensions_s>);
      r_cdo.m_is_open = true;
    } else
      return HAL_RANGE_ERR;
  }
  return HAL_NO_ERR;
}

/* set pwm value 0 ... 100 %*/
int16_t setDigout(uint8_t bOutput, uint16_t wPWMValue)
{
  #if 0
	printf("digout fuer Kanal %i auf %i gesetzt\n", bOutputNo, wPWMValue);
	#endif
  init_digout_once(bOutput);
  channelOutput_s &r_cdo = channelsDigitalOutput[bOutput];
  if (r_cdo.m_is_open) {
    dout_c &r_c = r_cdo.m_rte_channel;
    bool bit_state = HAL::getMaxPwmDigout(bOutput) == wPWMValue;
    r_c.modify(bit_state, !bit_state);
  }
  else
    return HAL_CONFIG_ERR;
  return HAL_NO_ERR;
}

} // End of name space __HAL
