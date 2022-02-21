/*
 * iVtObjectAlarmMaskAdditions.h
 *
 *  Created on: 21.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTALARMMASKADDITIONS_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTALARMMASKADDITIONS_H_

namespace IsoAgLib {






/**
 * Priority
 * Priority of this alarm as follows:
 * 0 = High, operator is in danger or urgent machine malfunction;
 * 1 = Medium, normal alarm, machine is malfunctioning;
 * 2 = Low, information only.
 *
 */

enum Priority : uint8_t
{
	High   = 0,
	Medium = 1,
	Low    = 2,
};



/**
 * AcousticSignal
 *
 * Acoustic signal.
 * 0 = highest priority,
 * 1 = medium priority,
 * 2 = lowest priority,
 * 3 = none (silent).
 *
 */
enum AcousticSignal : uint8_t
{
	highestpriority = 0,
	mediumpriority  = 1,
	lowestpriority  = 2,
	nonesilent      = 3,
};


}





#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECTALARMMASKADDITIONS_H_ */
