/*
 * file:    tutorial_settings.h
 * created: 02/20/09 01:55:40 PM CET
 *
 * comment header added here
 *
 */

#ifndef __TUTORIALINTERNALCAN_H
#define __TUTORIALINTERNALCAN_H

#include <IsoAgLib/comm/Part3_DataLink/icanpkgext_c.h>

namespace IsoAgLibTutorial {

  static const int scui_internalCanBaud = 500;

  typedef enum {
    InternalSpeed      = 1,
    InternalDistance   = 2,
    InternalPtoFront   = 3,
    InternalPtoRear    = 4,
    InternalPower      = 5,
  } InternalIdent_t;

  static const uint8_t scui_posSpeedReal = 0;
  static const uint8_t scui_posSpeedTheor = 2;
  static const uint8_t scui_posSpeedLen = 4;

  static const uint8_t scui_posDistanceReal = 0;
  static const uint8_t scui_posDistanceTheor = 4;
  static const uint8_t scui_posDistanceLen = 8;

  static const uint8_t scui_posPtoFront = 0;
  static const uint8_t scui_posPtoFrontEngaged = 2;
  static const uint8_t scui_posPtoFrontLen = 3;

  static const uint8_t scui_posPtoRear = 0;
  static const uint8_t scui_posPtoRearEngaged = 2;
  static const uint8_t scui_posPtoRearLen = 3;

  static const uint8_t scui_posPowerKeySwitch = 0;
  static const uint8_t scui_posPowerMaxTime = 1;
  static const uint8_t scui_posPowerLen = 3;

  class TutorialInternalPkg_c : public IsoAgLib::iCanPkgExt_c {
    public:
      virtual void flags2String() {
        switch( m_ident ) {
          case InternalSpeed:
            setUint16Data( scui_posSpeedReal, mu_tracData.Speed.real );
            setUint16Data( scui_posSpeedTheor, mu_tracData.Speed.theor );
            setLen( scui_posSpeedLen );
            break;
          case InternalDistance:
            setUint32Data( scui_posDistanceReal, mu_tracData.Distance.real );
            setUint32Data( scui_posDistanceTheor, mu_tracData.Distance.theor );
            setLen( scui_posDistanceLen );
            break;
          case InternalPtoFront:
            setUint16Data( scui_posPtoFront, mu_tracData.PtoFront.front );
            setUint8Data( scui_posPtoFrontEngaged, mu_tracData.PtoFront.engaged );
            setLen( scui_posPtoFrontLen );
            break;
          case InternalPtoRear:
            setUint16Data( scui_posPtoRear, mu_tracData.PtoRear.rear );
            setUint8Data( scui_posPtoRearEngaged, mu_tracData.PtoRear.engaged );
            setLen( scui_posPtoRearLen );
            break;
          case InternalPower:
            setUint8Data( scui_posPowerKeySwitch, mu_tracData.Power.keySwitch );
            setUint16Data( scui_posPowerMaxTime, mu_tracData.Power.powerMaxTime );
            setLen( scui_posPowerLen );
            break;
        }
        setIdent( m_ident );

      }
      virtual void string2Flags() {
        switch( ident() ) {
          case InternalSpeed:
            mu_tracData.Speed.real = getUint16Data( scui_posSpeedReal );
            mu_tracData.Speed.theor = getUint16Data( scui_posSpeedTheor );
            break;
          case InternalDistance:
            mu_tracData.Distance.real = getUint32Data( scui_posDistanceReal );
            mu_tracData.Distance.theor = getUint32Data( scui_posDistanceTheor );
            break;
          case InternalPtoFront:
            mu_tracData.PtoFront.front = getUint16Data( scui_posPtoFront );
            mu_tracData.PtoFront.engaged = getUint8Data( scui_posPtoFrontEngaged );
            break;
          case InternalPtoRear:
            mu_tracData.PtoRear.rear = getUint16Data( scui_posPtoRear );
            mu_tracData.PtoRear.engaged = getUint8Data( scui_posPtoRearEngaged );
            break;
          case InternalPower:
            mu_tracData.Power.keySwitch = getUint8Data( scui_posPowerKeySwitch );
            mu_tracData.Power.powerMaxTime = getUint8Data( scui_posPowerMaxTime );
            break;
        }
      }

      union {
        struct {
          uint16_t real;
          uint16_t theor;
        } Speed;

        struct {
          uint32_t real;
          uint16_t theor;
        } Distance;

        struct {
          uint16_t front;
          uint8_t engaged;
        } PtoFront;

        struct {
          uint16_t rear;
          uint8_t engaged;
        } PtoRear;

        struct {
          uint8_t keySwitch;
          uint8_t powerMaxTime;
        } Power;
      } mu_tracData;

      InternalIdent_t m_ident;
  };

}
#endif
/* eof tutorialInternalCan.h */
