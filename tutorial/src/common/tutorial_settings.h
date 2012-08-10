/*
 * file:    tutorial_settings.h
 * created: 02/20/09 01:55:40 PM CET
 *
 * comment header added here
 *
 */

#ifndef __TUTORIAL_SETTINGS_H
#define __TUTORIAL_SETTINGS_H

namespace IsoAgLibTutorial {

  /** isobus bus number - specifies the can channel connected to the ISOBUS system */
  static const unsigned char scui_isoBusNumber = 0;

  /* channel number and baud rate of the internam bus (used in tutorial tecu and internal can) */
  static const unsigned char scui_internalBusNumber = 1;

  static const char scp_isoaglibVersion[] = { '2','.','3','.','0' };
  static const char scp_tutorialVersion[] = { '0','.','2','.','1' };

  /* proprietary can timing */
  static const unsigned int scui_propCanTimePeriod = 500;
  static const float scf_propCanEarlierTimeFactor = 0.75;
  static const float scf_propCanLatestTimeFactor = 0.5;

  /* gps sensor period timing */
  static const unsigned int scui_gpsSensorTimePeriod = 1000;
  static const float scf_gpsSensorEarlierTimeFactor = 0.75;
  static const float scf_gpsSensorLatestTimeFactor = 0.5;
}

#endif
/* eof tutorialSettings.h */
