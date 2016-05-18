/*
  can_messenger.cpp: Testtool to inject CAN packages in a running
    can_server_bus.

  (C) Copyright 2009 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/driver/can/impl/canpkg_c.h>
#include <IsoAgLib/scheduler/ischeduler_c.h>

#include <fstream>
#include <sstream>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class cmdline_c
{
public:
  cmdline_c ()
  : i_repeat (1)
  , i_databytes (0)
  , i_id (0)
  , i_channel (0)
  , i_period (0)
  , b_ext (false)
  {}

  int i_repeat;
  int i_databytes;
  int i_id;
  int i_channel;
  int i_period;
  int b_ext;
  std::string str_replay_file;
  uint8_t pui8_databytes [8];

  void parse (int argc, char *argv[]);

  void usage_and_exit(int ai_errorCode) const;
  
private:

  int ahextoi (char* str);
  int databytes (char* src, uint8_t* dst);

};


int cmdline_c::ahextoi (char* str)
{
  int value=0;
  char x;
  while ((x = *str++) != 0x00)
  {
    if (x == 'x') value=0; /// entering HEX mode here
    else if ((x >= '0') && (x <= '9')) value = (value << 4) | (x - '0');
    else if ((x >= 'A') && (x <= 'F')) value = (value << 4) | (x - 'A' + 10);
    else if ((x >= 'a') && (x <= 'f')) value = (value << 4) | (x - 'a' + 10);
    else { printf ("Hex number error!\n"); usage_and_exit(1); }
  }
  return value;
}


int cmdline_c::databytes (char* src, uint8_t* dst)
{
  int bytes=0;
  bool first=true;
  char x;
  while ((x = *src++) != 0x00)
  {
    int value;
    /**/ if ((x >= '0') && (x <= '9')) value = (x - '0');
    else if ((x >= 'A') && (x <= 'F')) value = (x - 'A' + 10);
    else if ((x >= 'a') && (x <= 'f')) value = (x - 'a' + 10);
    else { value=0; printf ("Hex number error!\n"); usage_and_exit(1); }
    if (first)
    {
      *dst = value << 4;
      first = false;
    }
    else
    {
      *dst++ |= value;
      bytes++;
      first = true;
    }
  }

  if (!first)
  {
    printf ("last data byte incomplete!!\n");
    usage_and_exit(1);
  }
  return bytes;
}


void cmdline_c::parse (int argc, char *argv[])
{
  char nextup [] = "  ";
  if (argc < 2) usage_and_exit(1);
  for (int i=1; i<argc; i++)
  {
    if (nextup[2-1] == ' ')
    {
      if (argv[i][1-1] != '-')
      { // error
        printf ("Specify parameter type please!\n");
        usage_and_exit(1);
      }
      nextup[2-1] = argv[i][2-1];
      continue;
    }
    else
    {
      switch (nextup[2-1])
      {
        case 'i': i_id = ahextoi(argv[i]); break;
        case 'n': i_repeat = atoi(argv[i]); break;
        case 'c': i_channel = atoi (argv[i]); break;
        case 's':
        case 'd': i_databytes = databytes(argv[i], pui8_databytes); break;
        case 'p': i_period = atoi (argv[i]); break;
        case 'x': b_ext = true; i--; break;
        case 'f': str_replay_file = std::string(argv[i]); break;
        default: printf ("Unsupported parameter!\n"); usage_and_exit(1); break;
      }
      nextup[2-1] = ' '; // free to await next parameter-pair
      continue;
    }
  }

  if (nextup[2-1] != ' ')
  { // didn't finish parameter-pair ==> error
    printf ("Incomplete parameter-type\n");
    usage_and_exit (1);
  }
}


void cmdline_c::usage_and_exit (int ai_errorCode) const
{
  printf ("\nCommandline-parameters are:\n   -i <CAN-Identifier as HEX>\n");
  printf ("   -n <Repeat-Count>\n");
  printf ("   -c <CAN-Channel>\n");
  printf ("   -d <Databytes (up to 8) as HEX> (for compatibility also -s can be used)\n");
  printf ("   -p <Period in ms>\n");
  printf ("   -x     (use eXtended Identifier)\n");
  printf ("   -f replay file (structure <timestamp (ms)> <ID> <up to 8 blank separated data bytes as HEX>\n");
  printf ("\n Example: can_messenger -x -n 1 -c 0 -i 1ceafffe -d a1b2c3d4e5f6affe\n\n");

  exit (ai_errorCode);
}


using namespace IsoAgLib;
using namespace __IsoAgLib;

int main( int argc, char *argv[] )
{
  cmdline_c params;

  params.parse (argc, argv);

  // Init System
  IsoAgLib::getIsystemInstance().init();

  // Initialize ISOAgLib
  getISchedulerInstance().init();

  // Initialize CAN-Bus
  getCanInstance().init (params.i_channel, 250 ); // CAN-Bus (with defaulting 250 kbit)

  CanPkg_c pkg;

  pkg.setIdent(params.i_id, (params.b_ext ? iIdent_c::ExtendedIdent : iIdent_c::StandardIdent));
  pkg.setDataFromString(0, params.pui8_databytes, params.i_databytes);
  
  if(!params.str_replay_file.empty())
  {
    std::ifstream input(params.str_replay_file.c_str());
    if (!input)
    {
      params.usage_and_exit(1);
    }

    printf ("Replaying CAN-log...\n" );

    float timestamp = 0.0;
    float last_time = 0.0;

    const uint32_t buf_size = 256;
    char buf[buf_size];
    
    uint32_t identifier = 0;
    uint32_t data_byte = 0;
    uint8_t len = 0;
        
    while (!input.eof())
    {
      input.getline(buf, buf_size);
      std::istringstream lineStream(buf);

      if (lineStream.eof())
        break;
      lineStream >> timestamp;
      
      if (lineStream.eof())
        break;
      lineStream >> std::hex >> identifier;

      len = 0;
      while (!lineStream.eof()) {
        lineStream >> data_byte;
        if(!lineStream.eof())
        {
          pkg.setUint8Data(len, data_byte & 0xFF);
          len++;
        }
        if(len >= 8)
        {
          break;
        }
      }
      
      pkg.setIdent(identifier, (identifier >= (1 << 11)) ? iIdent_c::ExtendedIdent : iIdent_c::StandardIdent);
      pkg.setLen(len);

      getCanInstance() << pkg;
      
      std::cout << std::hex << timestamp << " " << identifier << std::endl;

      timestamp *= 1000;
#ifdef WIN32
      Sleep ( uint32_t(timestamp - last_time) ); // won't be too accurate though due to bad Windows Sleep-capability.
#else
      usleep( uint32_t(timestamp - last_time) * 1000 );
#endif
      last_time = timestamp;
    } // while( !eof )
  }
  else
  {
    printf ("Sending on CAN-Bus %d %d times with a period of %d: ID=%x [%d databytes:] ", params.i_channel, params.i_repeat, 
                                                                                          params.i_period,  params.i_id,
                                                                                          params.i_databytes);
    for (int i=0; i<params.i_databytes; i++) printf (" %02x", params.pui8_databytes[i]);
    printf ("\n");

    for( int i = 0; i < params.i_repeat; ++i ) {
      getCanInstance() << pkg;
      const int32_t sleepPeriod = ( 1 != params.i_repeat ) ? params.i_period : 1;

      if( sleepPeriod )
      {
#ifdef WIN32
        Sleep ( sleepPeriod ); // won't be too accurate though due to bad Windows Sleep-capability.
#else
        usleep( sleepPeriod * 1000 );
#endif
      }
    }

    if( 1 != params.i_repeat )
#ifdef WIN32
        Sleep ( params.i_repeat ); // won't be too accurate though due to bad Windows Sleep-capability.
#else
        usleep( params.i_repeat * 1000 );
#endif
  }

  /// Shutdown Scheduler
  IsoAgLib::getISchedulerInstance().close();  

  // Shutdown System
  IsoAgLib::getIsystemInstance().close();  

  return 0;
}
