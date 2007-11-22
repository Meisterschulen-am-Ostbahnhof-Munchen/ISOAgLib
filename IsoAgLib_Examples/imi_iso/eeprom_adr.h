// Header for positions in EEPROM

// the EEPROM is segmented in parts each 1056 uint8_t in size for a single identity
// represented by this controller

// the identifying datas are in the first 32 Byte of one part
// (offest of 0xFF for system data)
#define ADR_IDENT_ISOName (0x0+0x100)
// SHORT NAME LENGTH
#define ADR_IDENT_NAME_SHORT (0x1+0x100)
// ISO NAME flags
#define ADR_ISO_Ident_Number      268
#define ADR_ISO_Manufacturer_Code  272
#define ADR_ISO_ECU_Instance      274
#define ADR_ISO_Function_Instance  275
#define ADR_ISO_Function          276
#define ADR_ISO_Dev_Class          277
#define ADR_ISO_Dev_Class_Inst    278
#define ADR_ISO_Industry_Group    279
#define ADR_ISO_Self_Conf_Adr      280
#define ADR_ISO_SA                281


// EEPROM config specific for Task-Controller
// flag for task controller, if correct EHR values are integrated in Base_c (==1)
#define ADR_EHR_IN_BASE        282
// set max nominal force of hitch force sensor (typical 60 or 90 kN) (strored in [kN] as uint8_t)
#define ADR_MAX_THREE_POINT_FORCE          283
#define ADR_TASK_CONTROLLER_AREA_MANAGER  300
/* ******************************************************************* */
/* ***Begin of Config Settings for the recording of Task-Controller*** */
/* ******************************************************************* */
#define ADR_TASK_CONTROLLER_DEFAULT_CONFIG   1072
#define ADR_TASK_CONTROLLER_SPECIFIC_CONFIG 1100

/** 2byte : day,month of runing record task */
#define ADR_TASK_CONTROLLER_DATE_DAY            4000
#define ADR_TASK_CONTROLLER_DATE_MONTH          4001
/** offset values for tractor/task-contr -> 4xlong -> 16byte */
#define ADR_TASK_CONTROLLER_TRAC_OFFSET          4002
/**  1byte for ISOName of first implement */
#define ADR_TASK_CONTROLLER_IMPLEMENT_1_ISOName      4018
/**  8byte for name of first implement */
#define ADR_TASK_CONTROLLER_IMPLEMENT_1_NAME     4019
/**   9 int32_t offset values -> 36byte of first implement */
#define ADR_TASK_CONTROLLER_IMPLEMENT_1_OFFSET  4027

/**  1byte for ISOName of second implement */
#define ADR_TASK_CONTROLLER_IMPLEMENT_2_ISOName      4063
/**  8byte for name of second implement */
#define ADR_TASK_CONTROLLER_IMPLEMENT_2_NAME     4064
/**   9 int32_t offset values -> 36byte of second implement */
#define ADR_TASK_CONTROLLER_IMPLEMENT_2_OFFSET  4072

/** 1 float offset for total fuel consumption -> 4byte */
#define ADR_TASK_CONTROLLER_FUEL                4108


/* ********************************************************* */
/* *** Begin of Config Settings for the recording of IMI *** */
/* ********************************************************* */

// store int32_t value for constant working width [mm]
#define ADR_STATIC_WORKING_WIDTH  282
// set minimal rear PTO for working state of IMI  (0 == work state independent from PTO; uint16_t val)
#define ADR_WORK_MIN_PTO_REAR     286
// set max rear EHR position for working position of IMI (0xFF == work state independent from EHR)
#define ADR_WORK_MAX_EHR_REAR      288
#define ADR_WORK_MIN_EHR_REAR      289
// set minimal front PTO for working state of IMI  (0 == work state independent from PTO; uint16_t val)
#define ADR_WORK_MIN_PTO_FRONT    290
// set max front EHR position for working position of IMI (0xFF == work state independent from EHR)
#define ADR_WORK_MAX_EHR_FRONT    292
#define ADR_WORK_MIN_EHR_FRONT    293
// config whether IMI must drive during work
#define ADR_FORCE_WORKING_SPEED    294


// the positions of process data values
// a position is calculated by ((WERT * 0x10)+INST)*4+0x20
// working area WERT=8, INST=0
#define ADR_WORKING_AREA          300
#define ADR_TRIP_AREA              300
// remote process data for "whole distance" [m] (LIS=0, DEVCLASS=2, WERT=8, INST=1)
#define ADR_WHOLE_DIST            304
// remote process data for "working distance" [m] (LIS=0, DEVCLASS=2, WERT=8, INST=4)
#define ADR_WORKING_DIST          308
#define ADR_TRIP_DIST              308
// remote process data for "whole time" [sec] (LIS=0, DEVCLASS=2, WERT=0xA, INST=0)
#define  ADR_WHOLE_TIME            312
// remote process data for "work time" [sec] (LIS=0, DEVCLASS=2, WERT=0xA, INST=7)
#define ADR_WORKING_TIME          316
#define  ADR_TRIP_TIME              316

#define ADR_WORK_THRESHOLD        320
#define ADR_SCHMIER_CONFIG        324
#define ADR_SCHMIER_COUNTER        328
#define ADR_WHOLE_AREA            332

