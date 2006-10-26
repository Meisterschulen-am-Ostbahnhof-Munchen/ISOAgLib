/***************************************************************************
                          ibasetypes.h  - types for base data handling
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
#ifndef IBASE_TYPES_H
#define IBASE_TYPES_H
/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
//#include <Application_Config/isoaglib_config.h>
#include <IsoAgLib/typedef.h>

namespace IsoAgLib {

/** use an enum typedef for the different states of ISO flags */
typedef enum {
  IsoInactive = 0,    ///< Flag is marked explicit as not active
  IsoActive = 1,      ///< corresponding function is active
  IsoError = 2,       ///< correspnding function is in error state, and thus not useable
  IsoNotAvailable = 3 ///< corresponding function is not available/not implemented in service provider
} IsoActiveFlag_t;

/** use an enum typedef for the different states of ISO flags */
typedef enum {
  IsoNoDataRequested = 0, ///< no lighting data is requested from all implements
  IsoDataRequested = 1,   ///< lighting data is requested from all implements
  IsoReservedData = 2,        ///< reserved field
  IsoDontCare = 3         ///< corresponding function is not available/not implemented in service provider
} IsoDataReq_t;

/** GPS GNSS Method */
typedef enum {
  IsoNoGps           = 0,
  IsoGnssFix         = 1,
  IsoDgnssFix        = 2,
  IsoGnssPrecise     = 3,
  IsoRtkFixedInteger = 4,
  IsoRtkFloat        = 5,
  IsoDrEstimated     = 6,
  IsoGnssManual      = 7,
  IsoGnssSimulated   = 8,
  IsoGnssError       = 14,
  IsoGnssNull        = 15,
  IsoGnssMethodMAX   = 15
} IsoGnssMethod_t;

/** type of GNSS system */
typedef enum {
  IsoGnssGps            = 0,
  IsoGnssGlonass        = 1,
  IsoGnssGpsGlonass     = 2,
  IsoGnssGpsSbas        = 3,
  IsoGnssGpsSbasGlonass = 4,
  IsoGnssTypeMAX        = 4
} IsoGnssType_t;

/** different lighting types; implements and tractor*/
typedef enum {
  daytimeRunning =        1,
  alternateHead =         2,
  lowBeamHead =           3,
  highBeamHead =          4,
  frontFog =              5,
  beacon =                6,
  rightTurn =             7,
  leftTurn =              8,
  backUpLightAlarmHorn =  9,
  centerStop =           10,
  rightStop =            11,
  leftStop =             12,
  implClearance =        13,
  tracClearance =        14,
  implMarker =           15,
  tracMarker =           16,
  rearFog =              17,
  undersideWork =        18,
  rearLowWork =          19,
  rearHighWork =         20,
  sideLowWork =          21,
  sideHighWork =         22,
  frontLowWork =         23,
  frontHighWork =        24,
  implOEMOpt2 =          25,
  implOEMOpt1 =          26,
  implRightForwardWork = 27,
  implLeftForwardWork =  28,
  dataMsgReq =           29,    //reserved in Lighting data
  implRightFacingWork =  30,
  implLeftFacingWork =   31,
  implRearWork =         32,
} IsoCommandFlag_t;

typedef enum {
  implInTransport = 1,
  implInPark =      2,
  implInWork =      3,
} IsoMaintainPower_t;

/** use an extended enum for the different states of ISO flags for auxiliary valve */
typedef enum {
  IsoBlocked = 0,            ///< corresponding valve is closed
  IsoExtend = 1,             ///< corresponding valve is controlled from valves extended port
  IsoRetract = 2,            ///< corresponding valve is controlled from valves retract port
  IsoFloating = 3,           ///< corresponding valve has no control from or to the valve
  IsoErrorAuxExt = 14,       ///< corresponding function is in error state, and thus not useable
  IsoNotAvailableAuxExt = 15 ///< corresponding function is not available/not implemented in service provider
} IsoAuxFlagExtended_t;

/** use an enum for the different states of ISO flags for auxiliary valve */
typedef enum {
  IsoBlock = 0,          ///< corresponding valve is closed
  IsoFloat = 1,          ///< corresponding valve has no control from or to the valve
  IsoErrorAux = 2,       ///< corresponding function is in error state, and thus not useable
  IsoNotAvailableAux = 3 ///< corresponding function is not available/not implemented in service provider
} IsoAuxFlag_t;

/** use an enum for the different states of ISO flags for auxiliary valve limit status*/
typedef enum {
  IsoNotLimited = 0,          ///< corresponding limit status is not active
  IsoOperatorLimited = 1,     ///< request cannot be implemented
  IsoLimitedHigh = 2,         ///< corresponding function is in error state, and thus not useable
  IsoLimitedLow = 3,           ///< corresponding function is not available/not implemented in service provider
  IsoNonRecoverabelFault = 6, ///not recoverable from the viewpoint of the implemented
  IsoNotAvailableLimit = 7    ///paramter not supported
} IsoLimitFlag_t;

/** use an enum typedef for the different states of direction */
typedef enum {
  IsoNotReversed = 0,         ///< reported direction is not reversed from the perspective of the operator
  IsoReversed = 1,            ///< reported direction is reversed from the perspective of the operator
  IsoErrorReversed = 2,       ///< correspnding function is in error state, and thus not useable
  IsoNotAvailableReversed = 3 ///< corresponding function is not available in service provider
} IsoOperatorDirectionFlag_t;

/** use an enum typedef for the different states of a parameter whose requests are transient/temporary/one-shot */
typedef enum {
  IsoExternalReqAccepted = 0, ///< External Request accepted. No subsequent operator intervention.
  IsoControlOverride = 1,     ///< most recent external request ignored because of an operator or tractor system override
  IsoErrorReq = 2,            ///< correspnding function is in error state, and thus not useable
  IsoNotAvailableReq = 3      ///< corresponding function is not available in service provider
} IsoReqFlag_t;

/** use an enum typedef for the different states of the guidance system */
typedef enum {
  IsoNotIntendedToSteer = 0, ///< guidance system is not intending to control steering
  IsoIntendedToSteer = 1,    ///< guidance system is intending to control steering
  IsoReserved = 2,           ///< reserved field
  IsoNotAvailableSteer = 3   ///< corresponding function is not available in service provider
} IsoSteerFlag_t;

/** use an enum typedef for the different states of the request reset command status */
typedef enum {
  IsoResetNotRequired = 0, ///< reset of command status is not required
  IsoResetRequired = 1,    ///< reset of command status is required
  IsoErrorReset = 2,       ///< correspnding function is in error state, and thus not useable
  IsoNotAvailableReset = 3 ///< corresponding function is not available in service provider
} IsoResetFlag_t;

/** use an enum typedef for the steering input position status */
typedef enum {
  IsoNotCorrectPos = 0,       ///< manual steering is not in correct position for guidance to function
  IsoCorrectPos = 1,          ///< manual steering is in correct position for guidance to function
  IsoErrorSteerPos = 2,       ///< correspnding function is in error state, and thus not useable
  IsoNotAvailableSteerPos = 3 ///< corresponding function is not available in service provider
} IsoSteerPosFlag_t;

/** use an enum typedef for the steering system readiness */
typedef enum {
  IsoSystemNotReady = 0,            ///< machine steering system indicates that it is not free from faults that would preclude guidance sytstem operation
  IsoSystemReady = 1,               ///< machine steering system indicates that it is not free from faults that would preclude guidance sytstem operation
  IsoErrorSteerReadiness = 2,       ///< correspnding function is in error state, and thus not useable
  IsoNotAvailableSteerReadiness = 3 ///< corresponding function is not available in service provider
} IsoSteerReadinessFlag_t;

/** use an enum typedef for certification type */
typedef enum {
  IsoNotCert = 0,  ///< certification test is not certified
  IsoCert = 1      ///< certification test performed
} IsoCertTypeFlag_t;

/** use an enum for the different revisions of the certification test*/
typedef enum {
  IsoFirstRev = 0,        ///< first revision (no suffix)
  IsoSecondRev = 1,       ///< second revision (suffix A)
  IsoThirdRev = 2,        ///< third revision (suffix B)
  IsoFourthRev = 3,       ///< fourth revision (suffix C)
  IsoNotAvailableRev = 7  ///<corresponding function is not available in service provider
} IsoCertRevisionFlag_t;

/** use an enum for the different certification laboratory types*/
typedef enum {
  IsoNonCert = 0,          ///< non-certified laboratory / self certification
  IsoEUCert = 1,           ///< EU certified laboratory
  IsoNaCert = 2,           ///< NA certified laboratory
  IsoNotAvailableCert = 7  ///< not available (not certified)
} IsoCertLabTypeFlag_t;

/** use an enum for the different states of ISO flags for the direction of travel of the chassis */
typedef enum {
  IsoReverse = 0,              ///< direction of travel of the machine is reversed
  IsoForward = 1,              ///< direction of travel of the machine is forward
  IsoErrorDirection = 2,       ///< corresponding function is in error state, and thus not useable
  IsoNotAvailableDirection = 3 ///< corresponding function is not available/not implemented in service provider
} IsoDirectionFlag_t;

/** use an enum for the different sources of speed*/
typedef enum {
  IsoWheelBasedSpeed = 0,      ///< speed source is wheel-based
  IsoGroundBasedSpeed = 1,     ///< speed source is ground-based
  IsoNavigationBasedSpeed = 2, ///< speed source is navigation-based
  IsoBlendedSpeed = 3,         ///< speed source is "blended"
  IsoSimulatedSpeed = 4,       ///< speed source is simulated
  IsoNotAvailableSpeed = 7     ///<corresponding function is not available in service provider
} IsoSpeedSourceFlag_t;

/** use an enum for the different states of ISO flags for the implement work state */
typedef enum {
  IsoNotReadyForFieldWork = 0, ///< Implement is not ready for field work
  IsoReadyForFieldWork = 1,    ///< Implement is ready for field work
  IsoErrorWork = 2,            ///< corresponding function is in error state, and thus not useable
  IsoNotAvailableWork = 3      ///< corresponding function is not available/not implemented in service provider
} IsoImplWorkFlag_t;

/** use an enum for the different states of ISO flags for the implement transport state */
typedef enum {
  IsoNotTransported = 0,       ///< Implement may not be transported
  IsoTransported = 1,          ///< Implement may be transported
  IsoErrorTransport = 2,       ///< corresponding function is in error state, and thus not useable
  IsoNotAvailableTransport = 3 ///< corresponding function is not available/not implemented in service provider
} IsoImplTransportFlag_t;

/** use an enum for the different states of ISO flags for the implement park state */
typedef enum {
  IsoNotDisconnected = 0, ///< Implement may not be disconnected
  IsoDisconnect = 1,      ///< Implement may be disconnected
  IsoErrorPark = 2,       ///< corresponding function is in error state, and thus not useable
  IsoNotAvailablePark = 3 ///< corresponding function is not available/not implemented in service provider
} IsoImplParkFlag_t;

/** use an enum for the different sources of the selected speed */
typedef enum
{
  NoSpeed = 0,
  WheelBasedSpeed = 1,
  GroundBasedSpeed = 2,
  GpsBasedSpeed = 3,
  SelectedSpeed = 4
}SpeedSource_t;

/** use an enum for the different sources of distance and direction */
typedef enum
{
  NoDistDirec = 0,
  WheelBasedDistDirec = 1,
  GroundBasedDistDirec = 2,
  SelectedDistDirec = 3
}DistanceDirectionSource_t;

/** define mode */
typedef enum {
  IdentModeTractor = 0,
  IdentModeImplement = 1
} IdentMode_t;

}; // end namespace IsoAgLib


#endif

