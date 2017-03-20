/*
  measuresubprog_c.h

  (C) Copyright 2009 - 2016 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MEASURESUBPROG_C_H
#define MEASURESUBPROG_C_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/scheduler/impl/schedulertask_c.h>


namespace __IsoAgLib {

class ProcData_c;
class MeasureProg_c;

class MeasureDistProp_c : public SchedulerTask_c 
{
private: // non-copyable
  MeasureDistProp_c( const MeasureDistProp_c& );

public:
  MeasureDistProp_c( MeasureProg_c& measureProg );
  virtual ~MeasureDistProp_c();

  void start( uint32_t currentDistance, int32_t ai32_increment );

private:
  bool updateTrigger( uint32_t );
  int32_t nextTriggerTime( uint32_t );

  virtual void timeEvent();

private:
  MeasureProg_c &m_measureProg;
  uint32_t mui32_lastDistance;
  int32_t mi32_increment;
};


class MeasureTimeProp_c : public SchedulerTask_c 
{
private: // non-copyable
  MeasureTimeProp_c( const MeasureTimeProp_c& );

public:
  MeasureTimeProp_c( MeasureProg_c& measureProg );
  virtual ~MeasureTimeProp_c();

  void start( ecutime_t currentTime, int32_t ai32_increment );

private:
  bool updateTrigger( ecutime_t );
  int32_t nextTriggerTime( ecutime_t );

  virtual void timeEvent();

private:
  MeasureProg_c &m_measureProg;
  ecutime_t mt_lastTime;
  int32_t mi32_increment;
};


class MeasureOnChange_c
{
private: // non-copyable
  MeasureOnChange_c( const MeasureOnChange_c& );

public:
  MeasureOnChange_c( MeasureProg_c& measureProg );
  virtual ~MeasureOnChange_c() {}

  void start( int32_t ai32_lastVal, int32_t ai32_increment );

  void setValue( int32_t );

private:
  bool updateTrigger( int32_t );

private:
  MeasureProg_c &m_measureProg;
  int32_t mi32_lastVal;
  int32_t mi32_increment;
};

}

#endif
