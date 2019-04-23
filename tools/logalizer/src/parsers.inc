#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif


std::pair< int, PtrDataFrame_t >
parseLogLineCANMon( std::string const &acr_line ) //RX        4     1   CFE5182x| 98  2B  97  6F  FD  00  FF  EB
{
  unsigned arru_d[8];
  int iA, iB;
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%*s %i %*i %x%*s%x %x %x %x %x %x %x %x",
      &iA, &iB, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);

  //quick fix for standard CAN ID recognition (replace this when better solution known => i.e. is a standard identifier padded with 0s? does the ID always end with 'x|'? => then interprete CAN ID also as string as e.g. in candump)
  const bool cb_extendedFrameFormat = (iB > 0x7FF);

  int i_result = (parsed_count >= 2) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(parsed_count-2), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineJohnDeere( std::string const &acr_line ) // "r Xtd 2 1CAAF883 8 20 03 03 02 00 5C 5C FF 0   0 0060846488      17920  ....\\. "
{
  int iA, iB;
  unsigned arru_d[8];
  char msgType[10]; //Xtd for extended Identifier; hopefully string never longer than 9
  int parsed_count = sscanf(
      acr_line.c_str(),
      "r %s %*i %x %*i %x %x %x %x %x %x %x %x %*i   %*i %i      17920  %*s ",
      msgType, &iB, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7, &iA);

  const bool cb_extendedFrameFormat = (msgType[0] == 'X' && msgType[1] == 't' && msgType[2] == 'd');
  int i_result = parsed_count >= 2 ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(parsed_count-2), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineCanServer( std::string const &acr_line ) // "104916846 0 1 1 3 6 18eafffe   0   ee  0   0   0   0   0   0"
{
  long long iA; // time
  int iB; // ID
  unsigned msgType; // 0==std. !=0 ==ext.
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%lli %*i %*i %i %*i %*i %x %x %x %x %x %x %x %x %x",
      &iA, &msgType, &iB, /* busNr, msgObj, dlc, prio, */ arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);

  const bool cb_extendedFrameFormat = (1 == msgType);

  int i_result = parsed_count >= 3 ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  size_t t_sizeData = size_t(parsed_count-3);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + t_sizeData, back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineCANoe( std::string const &acr_line ) // "  18.9530 1  0CFE4980x        Rx   d 8 00 00 FF FF FF FF FF FF "
{
  float f1;
  int iB, iDb;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      " %f %*i %xx %*s %*s %u %x %x %x %x %x %x %x %x",      
      &f1, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );
  
  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iB > 0x7FF);
  uint64_t const cui64_time = uint64_t(f1*1000);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineA1ASCII( std::string const &acr_line ) // "m e 0x0cf00203 8  0xff 0x00 0x00 0xfa 0xff 0xf0 0x18 0xff       446270"
{
  char ch_baseOrExtended = '.';
  int iA, iB, iDb;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "m %c 0x%x %u 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x %i",
      &ch_baseOrExtended, &iA, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7, &iB);
  int i_result = parsed_count == (4+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );
  bool const cb_extendedFrameFormat = 'e' == ch_baseOrExtended;
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iB, iA, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}



std::pair< int, PtrDataFrame_t >
parseLogLineRte( std::string const &acr_line ) // "[0] HW             97.41  X   9f80182 8 67 34 b0 1c 54 01 e6 06"
{
  int iB, iDb = 0; // "%i* %x
  unsigned arru_d[8];
  long long unsigned iA; //min. uint64_t
  static int const sci_minimal_expected_parse_count = 4;
  int parsed_count = 0;
  static int remembered_mode = 0;

  for (int current_mode = remembered_mode; ; ++current_mode) {
    switch (current_mode) {
    case 0:
      break;
    case 1:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%*s %*s %lli.%*s %*i.%*s X %x %u %x %x %x %x %x %x %x %x",
          &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
      if (parsed_count <= -1 || sci_minimal_expected_parse_count <= parsed_count)
        remembered_mode = current_mode;
      break;
    case 2:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%*s %lli.%*s %*i.%*s X %x %u %x %x %x %x %x %x %x %x",
          &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
      if (remembered_mode)
        ;
      else if (-1 < parsed_count && parsed_count < sci_minimal_expected_parse_count)
        exit_with_error("wrong rte-log format. use -a for absolute timestamps!");
      else
        remembered_mode = current_mode;
      break;
    }
    if (remembered_mode)
      break; // repeat until remembered_mode is set
  }
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iB > 0x7FF);

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineRte2( std::string const &acr_line )
{
    static uint64_t sui64_rteTime = 0;
    int big;
    char *cursor;
    size_t byte;

    /* skip over any initial HW */
    for (cursor = const_cast<char *>(acr_line.c_str()); *cursor != 0 && !isspace (*cursor); cursor++)
        /* empty */;

    /*
     * must specify base 10 because of leading 0's
     */
    big = strtol (cursor, &cursor, 10);
    if (*cursor != '.') {
        fprintf (stderr, "unexpected terminator %c (%d): %s\n", *cursor, *cursor, acr_line.c_str());
        return std::make_pair( -4, PtrDataFrame_t(0) );
    }
    long ret = strtol (&cursor[1], &cursor, 10);
    (void)ret;
    while (isspace (*cursor))
        cursor++;
    if (*cursor == 'X')
        cursor++;
    else {
        fprintf (stderr, "unexpected field %c\n", *cursor);
        return std::make_pair( -5, PtrDataFrame_t(0) );
    }

    sui64_rteTime += big;
    uint64_t const cui64_time = sui64_rteTime;
    uint32_t const cui32_identifier = strtol (cursor, &cursor, 16);
    size_t t_sizeData = uint8_t (strtol (cursor, &cursor, 0));


    if (t_sizeData > 8) {
        fprintf (stderr, "protocol violation at time %lld: dlc of %d too long! (forcing to 8)\n", cui64_time, t_sizeData);
        t_sizeData = 8;
    }

    std::vector< uint8_t > vec_data(t_sizeData);

    for (byte = 0; byte < 8; byte++) {
        if (byte >= t_sizeData)
            break;

        vec_data[byte] = uint8_t (strtol (cursor, &cursor, 16));
    }
    //quick hack (see e.g. candump for better solution,but need to verify log format first)
    const bool cb_extendedFrameFormat = (cui32_identifier > 0x7FF);

    return std::make_pair( 0, new DataFrame_c( cui64_time, cui32_identifier, vec_data , cb_extendedFrameFormat) );
}


std::pair< int, PtrDataFrame_t >
parseLogLineTrc( std::string const &acr_line ) // "    13)       116.6  Rx     18EF808B  8  12 15 15 15 15 15 15 15"
{
  unsigned arru_d[8];
  int iA, iB, iDb;
  int parsed_count = sscanf(
      acr_line.c_str(),
      " %*s %u.%*u %*s %x %u %x %x %x %x %x %x %x %x",
      &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iB > 0x7FF);

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineTrc2( std::string const &acr_line ) // "    13)       116.6 1 Rx     18EF808B 80 8  12 15 15 15 15 15 15 15"
{
  unsigned arru_d[8];
  int iA, iB, iDb;
  int parsed_count = sscanf(
      acr_line.c_str(),
      " %*s %u.%*u %*u %*s %x %*x %u %x %x %x %x %x %x %x %x",
      &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iB > 0x7FF);

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineTrc3( std::string const &acr_line ) // "    1       116.6  DT  18EF808B RX 8  12 15 15 15 15 15 15 15"
{
  unsigned arru_d[8];
  int iA, iB, iDb;
  int parsed_count = sscanf(
      acr_line.c_str(),
      " %*s %u.%*u %*s %x %*s %u %x %x %x %x %x %x %x %x",
      &iA, &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count == (3+iDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iB > 0x7FF);

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}

std::pair< int, PtrDataFrame_t >
parseLogLineTrc4(std::string const &acr_line) // "    13)       116.6 1 Rx     18EF808B - 8  12 15 15 15 15 15 15 15"
{
    unsigned arru_d[8];
    int iA, iB, iDb;
    char canId[9]; // 8 chars for ext ID + null termination
    bool b_extendedFrameFormat = true;

    int parsed_count = sscanf(
        acr_line.c_str(),
        " %*s %u.%*u %*u %*s %x %*s %u %x %x %x %x %x %x %x %x",
        &iA, &iB, &iDb, arru_d, arru_d + 1, arru_d + 2, arru_d + 3, arru_d + 4, arru_d + 5, arru_d + 6, arru_d + 7);
    int i_result = parsed_count == (3 + iDb) ? 0 : -1;
    if (i_result < 0)
        return std::make_pair(i_result, PtrDataFrame_t(0));

    int parsed_count2 = sscanf(
        acr_line.c_str(),
        " %*s %*u.%*u %*u %*s %s",
        canId
    );

    if (parsed_count2 == 1)
    {
        if (iDb <= 0x7ff && (canId[1] == '\0' || canId[2] == '\0' || canId[3] == '\0')) //to ensure that we don't have an ext ID padded with leading zeros
        {
            //this must be a standard ID
            b_extendedFrameFormat = false;
        }
        //else: b_extendedFrameFormat already has value true
    }
    //else: sscanf returned 0; 

    std::vector< uint8_t > vec_data;
    copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));
    PtrDataFrame_t t_ptrFrame = new DataFrame_c(iA, iB, vec_data, b_extendedFrameFormat);
    return std::make_pair(i_result, t_ptrFrame);
}


std::pair< int, PtrDataFrame_t >
parseLogLineJrf( std::string const &acr_line ) // "41.19,0CFFFF2A,77,04,00,00,7D,00,64,FF"
{
  float fA;
  int iB;
  unsigned arru_d[8];
  char canId[9];    //8 chars + null termination
  bool b_extendedFrameFormat = true;
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%f,%x,%x,%x,%x,%x,%x,%x,%x,%x",
      &fA, &iB, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count >= 2 ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  const int parsed_count2 = sscanf(
      acr_line.c_str(),
      "%*f,%s,%*x",
      canId);

  if (parsed_count2 == 1)
  {
      if (iB <= 0x7ff && (canId[1] == '\0' || canId[2] == '\0' || canId[3] == '\0')) //to ensure that we don't have an ext ID padded with leading zeros
      {
          //this must be a standard ID
          b_extendedFrameFormat = false;
      }
      //else: b_extendedFrameFormat already has value true
  }
  //else: sscanf returned 0; 

  uint64_t const cui64_time = uint64_t(fA*1000);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(parsed_count-2), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iB, vec_data, b_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
defaultParseLogLine( std::string const &acr_line )
{
  (void)acr_line;
  std::pair< int, PtrDataFrame_t > result = std::make_pair(
      -1,
      PtrDataFrame_t(0) );
  exit_with_error("Unknown Log-Type!");
  return std::make_pair(1, result.second); // return just to satisfy compiler. exit_with_error will exit anyway ;)
}


std::pair< int, PtrDataFrame_t >
parseLogLineSocketCAN( std::string const &acr_line ) // (1321953173.064559) can1 10844D4C#FF7F737F73FFFFFF ( candump -l format )
{
#if DEBUG
  std::cout << acr_line << std::endl;
#endif

  unsigned int d[8];
  unsigned int t_s, t_us;
  unsigned int id;
  char canId[9]; // 8 chars for ext ID + null termination
  bool b_extendedFrameFormat = true;

  const int cnt = sscanf(
                    acr_line.c_str(),
                    "(%d.%d) can%*d %x#%02x%02x%02x%02x%02x%02x%02x%02x",
                    &t_s, &t_us, &id, d, d+1, d+2, d+3, d+4, d+5, d+6, d+7 );

  const int r = (cnt >= 3) ? 0 : -1;
  if (r < 0)
    return std::make_pair(r, PtrDataFrame_t(0));

  const int cnt2 = sscanf(
      acr_line.c_str(),
      "(%*d.%*d) can%*d %[^#]s",
      canId);

  if (cnt2 == 1)
  {
      if (id <= 0x7ff && (canId[1] == '\0' || canId[2] == '\0' || canId[3] == '\0')) //to ensure that we don't have an ext ID padded with leading zeros
      {
          //this must be a standard ID
          b_extendedFrameFormat = false;
      }
      //else: b_extendedFrameFormat already has value true
  }
  //else: sscanf returned 0; 

  const uint64_t t_ms = uint64_t((t_s*1000)+(t_us/1000));
  std::vector<uint8_t> vec_data;
  copy(d, d + size_t(cnt-3), back_inserter(vec_data));

  PtrDataFrame_t t_ptrFrame = new DataFrame_c(t_ms, id, vec_data, b_extendedFrameFormat);
  return std::make_pair(r, t_ptrFrame);
}

std::pair< int, PtrDataFrame_t >
parseLogLineSocketCAN2(std::string const &acr_line) //  (001.004013)  can1  18FEF326   [8]  34 06 30 95 A0 EF 95 3E

{
#if DEBUG
    std::cout << acr_line << std::endl;
#endif

    unsigned int d[8];
    unsigned int t_s, t_us;
    unsigned int id, dlc;
    char canId[9]; // 8 chars for ext ID + null termination
    bool b_extendedFrameFormat = true;

    const int cnt = sscanf(
        acr_line.c_str(),
        " (%d.%d) can%*d %x [%u] %x %x %x %x %x %x %x %x",
        &t_s, &t_us, &id, &dlc, d, d + 1, d + 2, d + 3, d + 4, d + 5, d + 6, d + 7);

    int i_result = (cnt == (4 + dlc) ? 0 : -1);

    if (i_result < 0)
        return std::make_pair(i_result, PtrDataFrame_t(0));

    const int cnt2 = sscanf(
        acr_line.c_str(),
        "(%*d.%*d) can%*d %s",
        canId);

    if (cnt2 == 1)
    {
        if (id <= 0x7ff && (canId[1] == '\0' || canId[2] == '\0' || canId[3] == '\0')) //to ensure that we don't have an ext ID padded with leading zeros
        {
            //this must be a standard ID
            b_extendedFrameFormat = false;
        }
        //else: b_extendedFrameFormat already has value true
    }
    //else: sscanf returned 0; leave b_extendedFrameFormat as true

    const uint64_t t_ms = uint64_t((t_s * 1000) + (t_us / 1000));
    std::vector<uint8_t> vec_data;
    copy(d, d + size_t(dlc), back_inserter(vec_data));

    PtrDataFrame_t t_ptrFrame = new DataFrame_c(t_ms, id, vec_data, b_extendedFrameFormat);
    return std::make_pair(i_result, t_ptrFrame);
}

std::pair< int, PtrDataFrame_t >
parseLogLineWTK( std::string const &acr_line ) // "0000.376 can r 18E6FFF1  8  21 00 FF FF 00 00 00 FF  0"
{
  float f1;
  int iID, inumDb, iX;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%f %*s %*s %x  %u  %x %x %x %x %x %x %x %x  %u",      
      &f1,
      &iID, 
      &inumDb, 
      arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7,
      &iX);
  int i_result = parsed_count == (4+inumDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iID > 0x7FF);

  uint64_t const cui64_time = static_cast<uint64_t>(f1*1000);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(inumDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iID, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}

std::pair< int, PtrDataFrame_t >
parseLogLineKvaserMemorator( std::string const &acr_line ) // "0.33198,1,cfffff0,4,8,55,7d,7d,7d,7d,ff,ff,ff,1,2014-05-05 15:01:08"
{
  float f1;
  int iID, inumDb;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%f,%*d,%x,%*d,%d,%x,%x,%x,%x,%x,%x,%x,%x%*s",      
      &f1,
      &iID,
      &inumDb,
      arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count == (3+inumDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iID > 0x7FF);

  uint64_t const cui64_time = static_cast<uint64_t>(f1*1000);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(inumDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iID, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineCsv( std::string const &acr_line ) // "0xCFE46F0*,54.6857,FF,FF,FF,FF,00,FF,FF,FF"
{
  float f1;
  int iID;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%x*,%f,%x,%x,%x,%x,%x,%x,%x,%x",
      &iID,
      &f1,
      arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  const int inumDb = parsed_count - 2;
  if( inumDb < 0 )
    return std::make_pair( -1, PtrDataFrame_t(0) );

  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iID > 0x7FF);

  uint64_t const cui64_time = static_cast<uint64_t>(f1*1000);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(inumDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iID, vec_data, cb_extendedFrameFormat);
  return std::make_pair( 0, t_ptrFrame );
}

std::pair< int, PtrDataFrame_t >
parseLogLineCsv2(std::string const &acr_line) // "00:27.9   1CE6FF26    FE  FF  FF  FF  FF  FF  0   FF"
{
    int min, s, ms100;
    int iID;
    unsigned arru_d[8];
    int parsed_count = sscanf(
        acr_line.c_str(),
        "%d:%d.%d %x %x %x %x %x %x %x %x %x",
        &min,
        &s,
        &ms100,
        &iID,
        arru_d, arru_d + 1, arru_d + 2, arru_d + 3, arru_d + 4, arru_d + 5, arru_d + 6, arru_d + 7);
    const int inumDb = parsed_count - 4;
    if (inumDb < 0)
        return std::make_pair(-1, PtrDataFrame_t(0));

    //quick hack (see e.g. candump for better solution,but need to verify log format first)
    const bool cb_extendedFrameFormat = (iID > 0x7FF);

    uint64_t const cui64_time = static_cast<uint64_t>(min * 60000 + s * 1000 + ms100 *100);
    std::vector< uint8_t > vec_data;
    copy(arru_d, arru_d + size_t(inumDb), back_inserter(vec_data));
    PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iID, vec_data, cb_extendedFrameFormat);
    return std::make_pair(0, t_ptrFrame);
}

std::pair< int, PtrDataFrame_t >
parseLogLineKomodo( std::string const &acr_line ) // "0:00.003.537,0x0cff05b4,0,8,01 00 01 00 01 00 C0 C0"
{
  int i1, i2;
  float f1;
  int iID, inumDb;
  unsigned arru_d[8];
  char canId[11]; // "0x" + 8 chars for extended ID + null termination
  bool b_extendedFrameFormat;
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%d:%d.%f,%x,%*d,%d,%x %x %x %x %x %x %x %x",
      &i1,
      &i2,
      &f1,
      &iID,
      &inumDb,
      arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  int i_result = parsed_count == (5+inumDb) ? 0 : -1;
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  const int parsed_count2 = sscanf(
      acr_line.c_str(),
      "%*d:%*d.%*f,%s,%*d",
      canId);

  if (parsed_count2 == 1)
  {
      if (iID <= 0x7ff && (canId[3] == '\0' || canId[4] == '\0' || canId[5] == '\0')) //to ensure that we don't have an ext ID padded with leading zeros; first 2 chars are "0x"
      {
          //this must be a standard ID
          b_extendedFrameFormat = false;
      }
      //else: b_extendedFrameFormat already has value true
  }
  //else: sscanf returned 0; 

  uint64_t const cui64_time = static_cast<uint64_t>(i1*60000 + i2*1000 + f1);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(inumDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iID, vec_data, b_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineVehicleSpy( std::string const &acr_line ) // "2,28.2609260000172,0.01300800000899471,67371012,F,F,HS CAN $CCBFFF7,HS CAN,,CCBFFF7,F,T,FE,FF,FF,FF,00,00,00,00,,,"
{
  double d1;
  char charArray[128];
  int iID;
  unsigned arru_d[8];
  int parsed_count = sscanf(
      acr_line.c_str(),
      "%*d,%lf,%*f,%*d,%*[^,],%*[^,],%*[^,],%[^,],,%x,%*[^,],%*[^,],%x,%x,%x,%x,%x,%x,%x,%x",
      &d1,
      charArray,
      &iID,
      arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7);
  const int inumDb = parsed_count - 3;
  if( inumDb < 0 )
    return std::make_pair( -1, PtrDataFrame_t(0) );
  if( strstr(charArray, "CAN") == NULL )
    return std::make_pair( -1, PtrDataFrame_t(0) );

  //quick hack (see e.g. candump for better solution,but need to verify log format first)
  const bool cb_extendedFrameFormat = (iID > 0x7FF);

  uint64_t const cui64_time = static_cast<uint64_t>(d1*1000);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(inumDb), back_inserter(vec_data));
  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time, iID, vec_data, cb_extendedFrameFormat);
  return std::make_pair( 0, t_ptrFrame );
}


std::pair< int, PtrDataFrame_t >
parseLogLineViewtoolGinkgoCsv( std::string const &acr_line ) // "Device0,CH1,Extended Frame,Data Frame,0x18EFFFAD,Receive,8,40 FF FF FF FF FF FF FF ,Success,00:05:56.126.000"
{
  int iHour, iMin, iSec, iMsec;
  char charArrExt[30];
  int iID;
  int iDlc;
  unsigned arru_d[8];

  std::string my_line = acr_line;
  std::size_t pos = my_line.find_last_of(",");
  if( std::string::npos == pos ) 
    return std::make_pair( -1, PtrDataFrame_t(0) );

  std::string my_time = my_line.substr( pos+1, std::string::npos );
  my_line = my_line.substr( 0, pos );
  pos = my_line.find_last_of(",");
  if( std::string::npos == pos ) 
    return std::make_pair( -1, PtrDataFrame_t(0) );

  my_line = my_line.substr( 0, pos );

  int cnt_time_el = sscanf(
      my_time.c_str(),
      "%d:%d:%d.%d.%*d",
      &iHour, &iMin, &iSec, &iMsec );
  if( 4 != cnt_time_el ) 
    return std::make_pair( -1, PtrDataFrame_t(0) );

  int parsed_count = sscanf(
      acr_line.c_str(),
      "%*[^,],%*[^,],%[^,],%*[^,],%x,%*[^,],%d,%x %x %x %x %x %x %x %x ", 
      charArrExt,
      &iID,
      &iDlc,
      arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7 );

  const int inumDb = parsed_count - 3;
  if( inumDb != iDlc ) // not the right number of CAN data bytes
    return std::make_pair( -1, PtrDataFrame_t(0) );

  const bool bExt = ( strstr(charArrExt, "Extended") == NULL ) ? false : true;

  uint64_t const cui64_time_ms = static_cast<uint64_t>( ( iHour * 60 * 60 * 1000 ) + ( iMin * 60 * 1000 ) + ( iSec * 1000 ) + iMsec );

  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(inumDb), back_inserter(vec_data));

  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time_ms, iID, vec_data, bExt);
  return std::make_pair( 0, t_ptrFrame );
}

std::pair< int, PtrDataFrame_t >
parseLogLineRmCanDeviceMonitor( std::string const &acr_line ) // "18EAFFFE 1RXE    3   00  EE  00                                            6411,9285"
{
  int iS, iMsec; // time
  int iB; // ID
  int iDb = 0; // number of data bytes
  unsigned arru_d[8];

  int parsed_count = sscanf(
      acr_line.c_str(),
      "%*x %*s    %d",
      &iDb);


  // TODO: Check if 'last_off()' does a better job in grabbing the timestamp than this switch. Find out if e.g. 6411,0000 <--> 6411 is plotted if iMsec == 0?
  switch(iDb)
  {
    case 0:
      parsed_count = sscanf(
        acr_line.c_str(),
        "%x %*s    %d   %d,%d",
        &iB, &iDb, &iS, &iMsec);
    break;

    case 1:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%x %*s    %d   %x %d,%d",
          &iB, &iDb, arru_d, &iS, &iMsec);
    break;

    case 2:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%x %*s    %d   %x %x %d,%d",
          &iB, &iDb, arru_d, arru_d+1, &iS, &iMsec);
    break;

    case 3:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%x %*s    %d   %x %x %x %d,%d",
          &iB, &iDb, arru_d, arru_d+1, arru_d+2, &iS, &iMsec);
    break;

    case 4:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%x %*s    %d   %x %x %x %x %d,%d",
          &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, &iS, &iMsec);
    break;

    case 5:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%x %*s    %d   %x %x %x %x %x %d,%d",
          &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, &iS, &iMsec);
    break;

    case 6:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%x %*s    %d   %x %x %x %x %x %x %d,%d",
          &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, &iS, &iMsec);
    break;

    case 7:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%x %*s    %d   %x %x %x %x %x %x %x %d,%d",
          &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, &iS, &iMsec);
    break;

    case 8:
      parsed_count = sscanf(
          acr_line.c_str(),
          "%x %*s    %d   %x %x %x %x %x %x %x %x %d,%d",
          &iB, &iDb, arru_d, arru_d+1, arru_d+2, arru_d+3, arru_d+4, arru_d+5, arru_d+6, arru_d+7, &iS, &iMsec);
    break;

    default:
    break;
  }

  int i_result = ( (parsed_count == (4+iDb) ) ? 0 : -1);
  if (i_result < 0)
    return std::make_pair( i_result, PtrDataFrame_t(0) );

  while(iMsec >= 1000)
  {
    iMsec /= 10;
  }

  const bool cb_extendedFrameFormat = (iB > 0x7FF);	// TODO: Find out if RXE stands for extended format
  uint64_t const cui64_time_ms = (1000 * iS) + (iMsec);
  std::vector< uint8_t > vec_data;
  copy(arru_d, arru_d + size_t(iDb), back_inserter(vec_data));

  PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time_ms, iB, vec_data, cb_extendedFrameFormat);
  return std::make_pair( i_result, t_ptrFrame );
}


// TODO: rename parsing format when format is known
std::pair< int, PtrDataFrame_t >
parseLogLinePloegerUnknownFormat(std::string const &acr_line) // " 0   419428883 X       8   2  98 255 255 255 255   1  54       0.018340 R"
{
    unsigned int CanId;
    char extended[9];  //probably long enough? -> should be -> check docu of format?
    int Dlc;
    int arr_dataBytes[8];
    float time;
    std::string substring;
    
    const int parsed_count = sscanf(
        acr_line.c_str(),
        " %*d %d %s %d %d %d %d %d %d %d %d %d",
        &CanId,
        extended,
        &Dlc,
        arr_dataBytes, arr_dataBytes + 1, arr_dataBytes + 2, arr_dataBytes + 3, arr_dataBytes + 4, arr_dataBytes + 5, arr_dataBytes + 6, arr_dataBytes + 7);

    if (parsed_count > 2)   //parse at least 3 formats
    {
        size_t pos = acr_line.find_last_of(".");
        if (std::string::npos == pos)
        {
            return std::make_pair(-1, PtrDataFrame_t(0));
        }
        else
        {
        // go 6 pos back(this should be sufficient, if not, parts of the time will not be parsed properly => leading to errors/nonesense in time summaries)
        substring = acr_line.substr(pos - 6, std::string::npos);
        }
    }
    else
    {
        return std::make_pair(-1, PtrDataFrame_t(0));
    }

    const int substring_parsed_count = sscanf(
        substring.c_str(),
        "%f",
        &time);

    const uint64_t cui64_time_ms = static_cast<uint64_t>(time * 1000);
    const bool extendedFrameFormat = (extended[0] == 'X');

    std::vector< uint8_t > vec_data;
    copy(arr_dataBytes, arr_dataBytes + size_t(Dlc), back_inserter(vec_data));

    PtrDataFrame_t t_ptrFrame = new DataFrame_c(cui64_time_ms, CanId, vec_data, extendedFrameFormat);
    return std::make_pair(0, t_ptrFrame);
}

// TODO: rename parsing format when format is known
std::pair< int, PtrDataFrame_t >
parseLogLinePloegerUnknownFormat_2(std::string const &acr_line) // "ID=99618844x,Type=D,Length=8,Data=B5FF36462692CA14"
{
    uint32_t CanId;
    int Dlc;
    char extended[5]; //no idea how big this array should be
    int arr_dataBytes[8];
    uint64_t u64DataBytes;

    const int parsed_count = sscanf(
        acr_line.c_str(),
        "ID=%d%[^,],Type=D,Length=%1d,Data=%llx",
        &CanId,
        extended,
        &Dlc,
        &u64DataBytes);

    if (parsed_count > 2)   //parse at least 3 formats
    {
        for (uint8_t i = 0; i < Dlc; ++i)
        {
            arr_dataBytes[i] = u64DataBytes >> ( (Dlc -1 - i ) * 8) & 0xFF;
        }
    }
    else
    {
        return std::make_pair(-1, PtrDataFrame_t(0));
    }

    const bool extendedFrameFormat = (extended[0] == 'x');

    std::vector< uint8_t > vec_data;
    copy(arr_dataBytes, arr_dataBytes + size_t(Dlc), back_inserter(vec_data));

    PtrDataFrame_t t_ptrFrame = new DataFrame_c(0, CanId, vec_data, extendedFrameFormat);
    return std::make_pair(0, t_ptrFrame);
}
