/**
 * \example CanServerMessenger.cpp
 * CanServerMessenger - based on IsoAgLib and "rte_can" (c) Manfred Guenzl
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_CanServerMessenger from the directory where the spec file
 *     and the script are located (default: IsoAgLib/compiler_projects/procejtGeneration/)<br>
 *     <tt>#> ./update_makefile.sh conf_CanServerMessenger</tt>
 * <li><b>Only Linux:</b><ul>
 *     <li>move to the CanServerMessenger subdirectory of the kdevelop_make directory and run make
 *     <li><tt>#> make</tt><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable CanServerMessenger in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist-CanServerMessenger.txt into the IDE of your choice
 *     <li>set <tt>-DPRJ_USE_AUTOGEN_CONFIG=config_CanServerMessenger</tt> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpecCanServerMessenger__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpecCanServerMessenger__pc_linux__msq_server_rte__rte
 *    <li> \ref PrjSpecCanServerMessenger__pc_linux__msq_server_rte__simulating
 *    <li> \ref PrjSpecCanServerMessenger__pc_linux__msq_server_rte__sys
 *    <li> \ref PrjSpecCanServerMessenger__pc_linux__simulating__rte
 *    <li> \ref PrjSpecCanServerMessenger__pc_linux__simulating__simulating
 *    <li> \ref PrjSpecCanServerMessenger__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpecCanServerMessenger__pc_win32__can_server_sock_vector_canlib__simulating
 *    <li> \ref PrjSpecCanServerMessenger__pc_win32__can_server_sock_vector_xl_drv_lib__simulating
 *    <li> \ref PrjSpecCanServerMessenger__pc_win32__can_server_sock_vector_canlib__sys
 *    <li> \ref PrjSpecCanServerMessenger__pc_win32__can_server_sock_vector_xl_drv_lib__sys
 *    <li> \ref PrjSpecCanServerMessenger__pc_win32__simulating__simulating
 *    <li> \ref PrjSpecCanServerMessenger__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpecCanServerMessenger__esx__sys__sys
 *    <li> \ref PrjSpecCanServerMessenger__imi__sys__sys
 *    <li> \ref PrjSpecCanServerMessenger__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileListsCanServerMessenger__pc_linux__simulating__simulating for needed files
 * ( filelist-CanServerMessenger-doxygen_import.txt ),
 *  with \ref SrcListCanServerMessenger__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrListCanServerMessenger__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileListsCanServerMessenger__pc_linux__msq_server_rte__rte
 *    <li> \ref FileListsCanServerMessenger__pc_linux__msq_server_rte__simulating
 *    <li> \ref FileListsCanServerMessenger__pc_linux__msq_server_rte__sys
 *    <li> \ref FileListsCanServerMessenger__pc_linux__simulating__rte
 *    <li> \ref FileListsCanServerMessenger__pc_linux__simulating__simulating
 *    <li> \ref FileListsCanServerMessenger__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileListsCanServerMessenger__pc_win32__can_server_sock_vector_canlib__simulating
 *    <li> \ref FileListsCanServerMessenger__pc_win32__can_server_sock_vector_xl_drv_lib__simulating
 *    <li> \ref FileListsCanServerMessenger__pc_win32__can_server_sock_vector_canlib__sys
 *    <li> \ref FileListsCanServerMessenger__pc_win32__can_server_sock_vector_xl_drv_lib__sys
 *    <li> \ref FileListsCanServerMessenger__pc_win32__simulating__simulating
 *    <li> \ref FileListsCanServerMessenger__pc_win32__simulating__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileListsCanServerMessenger__esx__sys__sys
 *    <li> \ref FileListsCanServerMessenger__imi__sys__sys
 *    <li> \ref FileListsCanServerMessenger__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the \#define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_CanServerMessenger ( see also at \ref PrjConfigCanServerMessenger ).
 *                                                                         */
 */

/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/driver/can/icanpkg_c.h>

//#include <iostream.h>
//#include <iomanip.h>


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
  uint8_t pui8_databytes [8];

  void parse (int argc, char *argv[]);

private:

  void usage() const;
};


int ahextoi (char* str)
{
  int value=0;
  char x;
  while ((x = *str++) != 0x00)
  {
    if (x == 'x') value=0; /// entering HEX mode here
    else if ((x >= '0') && (x <= '9')) value = (value << 4) | (x - '0');
    else if ((x >= 'A') && (x <= 'F')) value = (value << 4) | (x - 'A' + 10);
    else if ((x >= 'a') && (x <= 'f')) value = (value << 4) | (x - 'a' + 10);
    else { printf ("Hex number error!\n"); exit(1); }
  }
  return value;
}


int databytes (char* src, uint8_t* dst)
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
    else { printf ("Hex number error!\n"); exit(1); }
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
    exit(1);
  }
  return bytes;
}


void cmdline_c::parse (int argc, char *argv[])
{
  char nextup [] = "  ";
  for (int i=1; i<argc; i++)
  {
    if (nextup[2-1] == ' ')
    {
      if (argv[i][1-1] != '-')
      { // error
        printf ("Specify parameter type please!");
        exit(1);
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
        case 's': i_databytes = databytes(argv[i], pui8_databytes); break;
        case 'p': i_period = atoi (argv[i]); break;
				case 'x': b_ext = true; i--; break;
        default: printf ("Unsupported parameter!\n"); exit(1); break;
      }
      nextup[2-1] = ' '; // free to await next parameter-pair
      continue;
    }
  }
  if (nextup[2-1] != ' ')
  { // didn't finish parameter-pair ==> error
    printf ("Incomplete parameter-type\n");
    exit (1);
  }
}



using namespace IsoAgLib;

int main( int argc, char *argv[] )
{
  cmdline_c params;

  params.parse (argc, argv);

  printf ("Sending on CAN-Bus %d %d times with a period of %d: ID=%x (%dx)databytes %x %x %x %x %x %x %x %x\n",
          params.i_channel,         params.i_repeat,          params.i_period,          params.i_id, params.i_databytes,
          params.pui8_databytes[0], params.pui8_databytes[1], params.pui8_databytes[2], params.pui8_databytes[3],
          params.pui8_databytes[4], params.pui8_databytes[5], params.pui8_databytes[6], params.pui8_databytes[7]);

  // Initialize CAN-Bus
  getIcanInstance().init (params.i_channel); // CAN-Bus (with defaulting 250 kbit)

  iCanPkg_c pkg;

  pkg.setIdent(params.i_id, (params.b_ext ? iIdent_c::ExtendedIdent : iIdent_c::StandardIdent));
  pkg.setDataFromString(0, params.pui8_databytes, params.i_databytes);

  getIcanInstance() << pkg;

  return 0;
}
