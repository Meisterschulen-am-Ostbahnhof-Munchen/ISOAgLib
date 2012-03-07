/*
  alivecollection.cpp

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <logenvirons.h>
#include <alivecollection.h>
#include <iomanip>

AliveCollection_c::AliveCollection_c()
{
  setName(categoryVtServerClientPeriodic, "VT Server<->Client Periodic Alive");
  setAlivePeriod(categoryVtServerClientPeriodic, 1000);

  setName(categoryTcServerClientPeriodic, "TC Server<->Client Periodic Alive");
  setAlivePeriod(categoryTcServerClientPeriodic, 2000);

  setName(categoryNotAcknowledge, "NOT Acknowledge (NACK)");
  setAlivePeriod(categoryNotAcknowledge, 0); // means NO alivePeriod!

  setName(categoryVtCommandResponse, "VT Command-Response times");
  setAlivePeriod(categoryVtCommandResponse, -1500); // means Handshaking!

  setName(categoryEtpTpCommunicationTimes, "(E)TP-communication times");
  setAlivePeriod(categoryEtpTpCommunicationTimes, -500); // means Handshaking! (damn, we have mixed periods here...)
}

void AliveCollection_c::report() const
{
  for (int i=0; i<256; i++) {
    for (Category_e loop_msg = categoryBegin;
         loop_msg != categoryEnd;
         loop_msg = nextCategory(loop_msg))
    {
      const int32_t ci32_alivePeriod = alivePeriod(loop_msg);
      if (alives(loop_msg, i).size()>0)
      {
        if (ci32_alivePeriod > 0)
        { // we have a periodic event!
          std::cout << std::endl << "ISOBUS node with SA="<<std::hex<<i<<std::dec<<" had the following alive-times for ["<<name(loop_msg)<<"] with alive-periods of "<<alivePeriod(loop_msg)<<" ms:"<<std::endl;
        }
        else if (ci32_alivePeriod < 0)
        { // we have a handshaking event!
          std::cout << std::endl << "ISOBUS node with SA="<<std::hex<<i<<std::dec<<" had the following alive-times for ["<<name(loop_msg)<<"] with alive-periods of "<<(-alivePeriod(loop_msg))<<" ms:"<<std::endl;
        }
        else
        { // sinlge events!! "== 0"
          std::cout << std::endl << "ISOBUS node with SA="<<std::hex<<i<<std::dec<<" sent out ["<<name(loop_msg)<<"] at the following times:"<<std::endl;
        }
        std::vector<msgType_e>::const_iterator type_iter=response(loop_msg, i).begin();
        for (std::vector<uint64_t>::const_iterator iter=alives(loop_msg, i).begin();
             iter != alives(loop_msg, i).end();
             iter++)
        {
          std::cout << std::setfill (' ');
          std::cout << "absolute time: "<<std::setw(10)<<(*iter/1000)<<"."<<std::setw(3)<<std::setfill('0')<<(*iter%1000)<<std::setfill(' ');
          if (iter != alives(loop_msg, i).begin())
          {
            const uint64_t cui32_delta = ( *(iter) - *(iter-1) );
            std::cout<< "  relative time: "<<std::setw(10)<<cui32_delta;

            if (ci32_alivePeriod > 0)
            { // print out the alivePeriod-deviation!
              std::cout<<" deviation: ";
              int deviation = int ((double (int32_t (cui32_delta)-ci32_alivePeriod) / ci32_alivePeriod) * 100);
              uint8_t deviation_bias = (deviation > 0) ? '+' : '-';
              deviation = (deviation < 0) ? -deviation : deviation;
              if (deviation > 100)
              {
                std::cout << "EXTREME DEVIATION(!!) OF " << std::setw(10) << deviation << "0";
              }
              else
              {
                while (deviation > 10)
                {
                  std::cout << deviation_bias;
                  deviation -= 10;
                }
              }
            }
            else if (ci32_alivePeriod < 0)
            { // Handshaking
              if (type_iter == response(loop_msg, i).end()) exit_with_error("No direction/msgType set but is expected. System failure.");
              int32_t i32_alivePeriodSpecial;
              switch (*type_iter)
              {
              case msgTypeResponse: std::cout << " Response  "; i32_alivePeriodSpecial = -ci32_alivePeriod; break;
              case msgTypeCommand:  std::cout << " Command   "; i32_alivePeriodSpecial = 0; break; // no timing-requirement here!
              case msgTypeRTS:      std::cout << " (E)TP-CONN: Request to Send (RTS)         "; i32_alivePeriodSpecial = 0; break; // no timing-requirement here!
              case msgTypeCTS:      std::cout << " (E)TP-CONN: Clear to Send (CTS)           "; i32_alivePeriodSpecial = 1250; break;
              case msgTypeDPO:      std::cout << " (E)TP-CONN: Data Packet Offset (DPO)      "; i32_alivePeriodSpecial = 1250; break; /// @todo SOON-260: set the correct values here!
              case msgTypeEOMACK:   std::cout << " (E)TP-CONN: End of Message ACK (EoMACK)   "; i32_alivePeriodSpecial = 1250; break;
              case msgTypeDATA:     std::cout << " (E)TP-DATA                                "; i32_alivePeriodSpecial = 250; break;
              case msgTypeCONNABORT:std::cout << " (E)TP-CONN: Connection Abort (CONNABORT)  "; i32_alivePeriodSpecial = -1; break; // doesn't come normally!
              default:              std::cout << " ???                                       "; i32_alivePeriodSpecial = 0; break;
              }
              if ( (*type_iter == msgTypeResponse) && (*(type_iter-1) == msgTypeResponse) ) std::cout << " - RESPONSE FOLLOWING A RESPONSE!";
              if ( (*type_iter == msgTypeCommand)  && (*(type_iter-1) == msgTypeCommand)  ) std::cout << " - COMMAND FOLLOWING A COMMAND!";
              if (i32_alivePeriodSpecial > 0)
              { // print out the time it took!
                if (cui32_delta > (unsigned int) (i32_alivePeriodSpecial)) std::cout << " *** !!! TIMEOUT - Check relative time!!!! ***";
                else
                {
                  int32_t time = int32_t ((cui32_delta*100) / i32_alivePeriodSpecial);
                  std::cout <<std::setw(2)<<time<< " percent of timeout ("<<std::setw(4)<<i32_alivePeriodSpecial<<"): (one '%' shows 10%) ";
                  while (time > 10)
                  {
                    std::cout << "%";
                    time -= 10;
                  }
                }
              }
              else if (i32_alivePeriodSpecial < 0)
              { // unsolicited messages (like CONNABORT)
                std::cout << "*** UNEXPECTED/UNSOLICITED MESSAGE ***";
              }
            }
          }
          std::cout << std::endl;
          if (type_iter != response(loop_msg, i).end()) type_iter++;
        }
      }
    }
  }
}
