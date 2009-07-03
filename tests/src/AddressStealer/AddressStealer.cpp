#include <IsoAgLib/driver/can/icanio_c.h>
#include <IsoAgLib/scheduler/ischeduler_c.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>


class Stealer_c : public __IsoAgLib::Scheduler_Task_c
{
public:
  Stealer_c (uint8_t aui8_sa, int32_t ai32_delayTime) : mui8_sa (aui8_sa), mi32_delayTime (ai32_delayTime), mi32_stealTime (-1) {}
  virtual ~Stealer_c() {}


  virtual bool timeEvent();

  virtual bool processMsg();


  virtual void close() {}

  virtual const char* getTaskName() const { return "Stealer_c"; }
  virtual uint16_t getForcedMinExecTime() const { return 1; }
  virtual void updateEarlierAndLatestInterval() {
    setEarlierInterval(0);
    setLatestInterval(0);
  }

private:
  uint8_t mui8_sa;
  int32_t mi32_delayTime;
  int32_t mi32_stealTime;
};


bool
Stealer_c::timeEvent()
{
  if (mi32_stealTime < 0)
    return true;

  if (HAL::getTime() > mi32_stealTime)
  {
    mi32_stealTime = -1;
#ifdef DEBUG
    EXTERNAL_DEBUG_DEVICE << "==> NOW Stealing SA " << uint16_t(mui8_sa) << std::endl;
#endif
    dataBase().setIsoPri (6);
    dataBase().setIsoPgn (ADDRESS_CLAIM_PGN);
    dataBase().setIsoPs (255); // global information
    dataBase().setIsoSa (mui8_sa);
    dataBase().setUint32Data(0, 0);
    dataBase().setUint32Data(4, 0);
    __IsoAgLib::getCanInstance(1) << dataBase();
  }

  setTimePeriod(5);
  return true;
}


bool
Stealer_c::processMsg()
{
#ifdef DEBUG
  EXTERNAL_DEBUG_DEVICE << "Received ADDRESS_CLAIM for " << uint16_t (dataBase().isoSa()) << std::endl;
#endif
  if (dataBase().isoSa() == mui8_sa)
  { // we want to steal from 0x80!
#ifdef DEBUG
    EXTERNAL_DEBUG_DEVICE << " --> Scheduling Address-Steal!" << std::endl;
#endif
    mi32_stealTime = HAL::getTime() + mi32_delayTime;
  }

  return true;
}





Stealer_c* gpc_stealer = NULL;


bool ecuMain (uint8_t aui8_sa, int32_t ai32_delayTime) {

  gpc_stealer = new Stealer_c (aui8_sa, ai32_delayTime);

  /// ISOBUS on Bus 1
  if ( ! IsoAgLib::getIcanInstance(0).init( 1 ) ) {
    return false;
  }

  /// Internal on Bus 0
  if ( ! IsoAgLib::getIcanInstance(1).init( 0 ) ) {
    return false;
  }

  __IsoAgLib::getSchedulerInstance().registerClient (gpc_stealer);
  __IsoAgLib::getCanInstance(1).insertFilter (*gpc_stealer, 0x3FF0000, (ADDRESS_CLAIM_PGN<<8), true, __IsoAgLib::Ident_c::ExtendedIdent, 8);

  return true;
}


bool ecuShutdown() {

  return true;
}


int main( int argc, char** argv )
{
  /// COMMAND-LINE PARAMETERS
  uint8_t ui8_sa = 0x80;
  int32_t i32_delayTime = 2000; // 2 sec
  switch (argc)
  {
    case (2+1):
      i32_delayTime = atoi (argv[2]);
      // break left out intentionally
    case (1+1):
      ui8_sa = atoi (argv[1]);
      // break left out intentionally
    case (0+1):
      // no parameters
      break;

    default:
      // too many parameters (or not even exe-filename (argc==0))
      return 1;
  }

  ecuMain (ui8_sa, i32_delayTime);

  while ( IsoAgLib::iSystem_c::canEn() )
  {
    int32_t i32_idleTimeSpread =
        IsoAgLib::getISchedulerInstance().timeEvent();

    if ( i32_idleTimeSpread > 0 )
      IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
  }

  ecuShutdown();

  return 1;
}
