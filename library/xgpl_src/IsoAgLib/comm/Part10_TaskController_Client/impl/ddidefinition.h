/*
  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _DDIDEFINITION_H_
#define _DDIDEFINITION_H_

#define DDI_DATA_DICTIONARY_VERSION                0

#define DDI_APPRATE_VOLUME_PER_AREA_SETPOINT       1 // Units are 0.01 mm3/m2
#define DDI_APPRATE_VOLUME_PER_AREA_ACTUAL         2
#define DDI_APPRATE_VOLUME_PER_AREA_DEFAULT        3
#define DDI_APPRATE_VOLUME_PER_AREA_MINIMUM        4
#define DDI_APPRATE_VOLUME_PER_AREA_MAXIMUM        5

#define DDI_APPRATE_MASS_PER_AREA_SETPOINT         6 // Units are mg/m2
#define DDI_APPRATE_MASS_PER_AREA_ACTUAL           7
#define DDI_APPRATE_MASS_PER_AREA_DEFAULT          8
#define DDI_APPRATE_MASS_PER_AREA_MINIMUM          9
#define DDI_APPRATE_MASS_PER_AREA_MAXIMUM         10

#define DDI_APPRATE_COUNT_PER_AREA_SETPOINT       11 // Units are 0.001 count/m2
#define DDI_APPRATE_COUNT_PER_AREA_ACTUAL         12
#define DDI_APPRATE_COUNT_PER_AREA_DEFAULT        13
#define DDI_APPRATE_COUNT_PER_AREA_MINIMUM        14
#define DDI_APPRATE_COUNT_PER_AREA_MAXIMUM        15

#define DDI_APPRATE_SPACING_SETPOINT              16 // Units are mm
#define DDI_APPRATE_SPACING_ACTUAL                17
#define DDI_APPRATE_SPACING_DEFAULT               18
#define DDI_APPRATE_SPACING_MINIMUM               19
#define DDI_APPRATE_SPACING_MAXIMUM               20

#define DDI_APPRATE_VOLUME_PER_VOLUME_SETPOINT    21 // Units are mm3/m3
#define DDI_APPRATE_VOLUME_PER_VOLUME_ACTUAL      22
#define DDI_APPRATE_VOLUME_PER_VOLUME_DEFAULT     23
#define DDI_APPRATE_VOLUME_PER_VOLUME_MINIMUM     24
#define DDI_APPRATE_VOLUME_PER_VOLUME_MAXIMUM     25

#define DDI_APPRATE_MASS_PER_MASS_SETPOINT        26 // Units are mg/kg
#define DDI_APPRATE_MASS_PER_MASS_ACTUAL          27
#define DDI_APPRATE_MASS_PER_MASS_DEFAULT         28
#define DDI_APPRATE_MASS_PER_MASS_MINIMUM         29
#define DDI_APPRATE_MASS_PER_MASS_MAXIMUM         30

#define DDI_APPRATE_VOLUME_PER_MASS_SETPOINT      31 // Units are mm3/kg
#define DDI_APPRATE_VOLUME_PER_MASS_ACTUAL        32
#define DDI_APPRATE_VOLUME_PER_MASS_DEFAULT       33
#define DDI_APPRATE_VOLUME_PER_MASS_MINIMUM       34
#define DDI_APPRATE_VOLUME_PER_MASS_MAXIMUM       35

#define DDI_APPRATE_VOLUME_PER_TIME_SETPOINT      36 // Units are mm3/s
#define DDI_APPRATE_VOLUME_PER_TIME_ACTUAL        37
#define DDI_APPRATE_VOLUME_PER_TIME_DEFAULT       38
#define DDI_APPRATE_VOLUME_PER_TIME_MINIMUM       39
#define DDI_APPRATE_VOLUME_PER_TIME_MAXIMUM       40

#define DDI_APPRATE_MASS_PER_TIME_SETPOINT        41 // Units are mg/s
#define DDI_APPRATE_MASS_PER_TIME_ACTUAL          42
#define DDI_APPRATE_MASS_PER_TIME_DEFAULT         43
#define DDI_APPRATE_MASS_PER_TIME_MINIMUM         44
#define DDI_APPRATE_MASS_PER_TIME_MAXIMUM         45

#define DDI_APPRATE_COUNT_PER_TIME_SETPOINT       46 // Units are 0.001 count/s
#define DDI_APPRATE_COUNT_PER_TIME_ACTUAL         47
#define DDI_APPRATE_COUNT_PER_TIME_DEFAULT        48
#define DDI_APPRATE_COUNT_PER_TIME_MINIMUM        49
#define DDI_APPRATE_COUNT_PER_TIME_MAXIMUM        50

#define DDI_TILLAGE_DEPTH_SETPOINT                51 // Units are mm
#define DDI_TILLAGE_DEPTH_ACTUAL                  52
#define DDI_TILLAGE_DEPTH_DEFAULT                 53
#define DDI_TILLAGE_DEPTH_MINIMUM                 54
#define DDI_TILLAGE_DEPTH_MAXIMUM                 55

#define DDI_SEEDING_DEPTH_SETPOINT                56 // Units are mm
#define DDI_SEEDING_DEPTH_ACTUAL                  57
#define DDI_SEEDING_DEPTH_DEFAULT                 58
#define DDI_SEEDING_DEPTH_MINIMUM                 59
#define DDI_SEEDING_DEPTH_MAXIMUM                 60

#define DDI_WORKING_HEIGHT_SETPOINT               61 // Units are mm
#define DDI_WORKING_HEIGHT_ACTUAL                 62
#define DDI_WORKING_HEIGHT_DEFAULT                63
#define DDI_WORKING_HEIGHT_MINIMUM                64
#define DDI_WORKING_HEIGHT_MAXIMUM                65

#define DDI_WORKING_WIDTH_SETPOINT                66 // Units are mm
#define DDI_WORKING_WIDTH_ACTUAL                  67
#define DDI_WORKING_WIDTH_DEFAULT                 68
#define DDI_WORKING_WIDTH_MINIMUM                 69
#define DDI_WORKING_WIDTH_MAXIMUM                 70

#define DDI_CONTENT_VOLUME_SETPOINT               71 // Units are ml
#define DDI_CONTENT_VOLUME_ACTUAL                 72
#define DDI_CONTENT_VOLUME_MAXIMUM                73

#define DDI_CONTENT_MASS_SETPOINT                 74 // Units are g
#define DDI_CONTENT_MASS_ACTUAL                   75
#define DDI_CONTENT_MASS_MAXIMUM                  76

#define DDI_CONTENT_COUNT_SETPOINT                77 // Units are count
#define DDI_CONTENT_COUNT_ACTUAL                  78
#define DDI_CONTENT_COUNT_MAXIMUM                 79

#define DDI_APPLICATION_TOTAL_VOLUME              80 // l
#define DDI_APPLICATION_TOTAL_MASS                81 // kg
#define DDI_APPLICATION_TOTAL_COUNT               82 // count

#define DDI_YIELD_VOLUME_PER_AREA                 83 // ml/m2
#define DDI_YIELD_MASS_PER_AREA                   84 // mg/m2
#define DDI_YIELD_COUNT_PER_AREA                  85 // 0.001 count/m2

#define DDI_YIELD_VOLUME_PER_TIME                 86 // ml/s
#define DDI_YIELD_MASS_PER_TIME                   87 // mg/s
#define DDI_YIELD_COUNT_PER_TIME                  88 // 0.001 count/s

#define DDI_YIELD_TOTAL_VOLUME                    89 // ml
#define DDI_YIELD_TOTAL_MASS                      90 // g
#define DDI_YIELD_TOTAL_COUNT                     91 // count

#define DDI_CROP_LOSS_VOLUME_PER_AREA             92 // ml/m2
#define DDI_CROP_LOSS_MASS_PER_AREA               93 // mg/m2
#define DDI_CROP_LOSS_COUNT_PER_AREA              94 // 0.001 count/m2
 
#define DDI_CROP_LOSS_VOLUME_PER_TIME             95 // ml/s
#define DDI_CROP_LOSS_MASS_PER_TIME               96 // mg/s
#define DDI_CROP_LOSS_COUNT_PER_TIME              97 // 0.001 count/s
 
#define DDI_PERCENTAGE_CROP_LOSS                  98 // ppm
#define DDI_CROP_MOISTURE                         99 // ppm
#define DDI_CROP_CONTAMINATION                   100 // ppm

#define DDI_BALE_WIDTH_SETPOINT                  101 // Units are mm
#define DDI_BALE_WIDTH_ACTUAL                    102
#define DDI_BALE_WIDTH_DEFAULT                   103
#define DDI_BALE_WIDTH_MINIMUM                   104
#define DDI_BALE_WIDTH_MAXIMUM                   105

#define DDI_BALE_HEIGHT_SETPOINT                 106 // Units are mm
#define DDI_BALE_HEIGHT_ACTUAL                   107
#define DDI_BALE_HEIGHT_DEFAULT                  108
#define DDI_BALE_HEIGHT_MINIMUM                  109
#define DDI_BALE_HEIGHT_MAXIMUM                  110

#define DDI_BALE_SIZE_SETPOINT                   111 // Units are mm
#define DDI_BALE_SIZE_ACTUAL                     112
#define DDI_BALE_SIZE_DEFAULT                    113
#define DDI_BALE_SIZE_MINIMUM                    114
#define DDI_BALE_SIZE_MAXIMUM                    115

#define DDI_TOTAL_AREA                           116 // m2
#define DDI_TOTAL_EFFECTIVE_DISTANCE             117 // m
#define DDI_TOTAL_INEFFECTIVE_DISTANCE           118 // m
#define DDI_TOTAL_EFFECTIVE_TIME                 119 // s
#define DDI_TOTAL_INEFFECTIVE_TIME               120 // s
 
#define DDI_PRODUCT_DENSITY_MASS_PER_VOLUME      121 // g/l
#define DDI_PRODUCT_DENSITY_MASS_PER_COUNT       122 // mg/1000
#define DDI_PRODUCT_DENSITY_VOLUME_PER_COUNT     123 // ml/1000

#define DDI_AUXVALVE_EXTEND_SCALING              124 // 0.1 %
#define DDI_AUXVALVE_RETRACT_SCALING             125 // 0.1 %
#define DDI_AUXVALVE_EXTEND_RAMPUP               126 // ms
#define DDI_AUXVALVE_EXTEND_RAMPDOWN             127 // 0.1 ms
#define DDI_AUXVALVE_RETRACT_RAMPUP              128 // ms
#define DDI_AUXVALVE_RETRACT_RAMPDOWN            129 // ms
#define DDI_AUXVALVE_FLOAT_THRESHOLD             130 // 0.1 %
#define DDI_AUXVALVE_EXTEND_PROGRESSIVITY        131
#define DDI_AUXVALVE_RETRACT_PROGRESSIVITY       132
#define DDI_AUXVALVE_INVERT_PORTS                133

#define DDI_DEVICE_ELEMENT_OFFSET_X              134 // mm
#define DDI_DEVICE_ELEMENT_OFFSET_Y              135 // mm
#define DDI_DEVICE_ELEMENT_OFFSET_Z              136 // mm

#define DDI_DEVICE_CAPACITY_VOLUME               137 // ml
#define DDI_DEVICE_CAPACITY_MASS                 138 // g
#define DDI_DEVICE_CAPACITY_COUNT                139 // count

#define DDI_APPRATE_PERCENTAGE_SETPOINT          140 // ppm

#define DDI_WORKSTATE                            141 // 00 - disabled, 01 - enabled, 10 - error, 11 - not installed

#define DDI_TIME_LATENCY_SETPOINT                142 // ms
#define DDI_TIME_LATENCY_ACTUAL                  143 // ms
 
#define DDI_YAW_ANGLE                            144 // 0.001 degree
#define DDI_ROLL_ANGLE                           145 // 0.001 degree
#define DDI_PITCH_ANGLE                          146 // 0.001 degree
 
#define DDI_LOG_COUNT                            147

#define DDI_FUEL_CONSUMPTION_TOTAL               148 // ml
#define DDI_FUEL_CONSUMPTION_PER_TIME            149 // mm3/sec
#define DDI_FUEL_CONSUMPTION_PER_AREA            150 // mm3/m2
 
#define DDI_AREA_PER_TIME_CAPACITY               151 // mm2/s

#define DDI_PRESCRIPTION_CONTROL_STATE           158 // 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SECTION_CONTROL_STATE                160 // 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE1_16             161 // sections 1-16 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE17_32            162 // sections 17-32 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE33_48            163 // sections 33-48 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE49_64            164 // sections 49-64 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE65_80            165 // sections 65-80 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE81_96            166 // sections 81-96 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE97_112           167 // sections 97-112 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE113_128          168 // sections 113-128 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE129_144          169 // sections 129-144 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE145_160          170 // sections 145-160 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE161_176          171 // sections 161-176 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE177_192          172 // sections 177-192 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE193_208          173 // sections 193-208 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE209_224          174 // sections 209-224 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE225_240          175 // sections 225-240 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_CONDENSED_WORK_STATE241_256          176 // sections241-256 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed

#define DDI_SC_TURN_ON_TIME                      205 // ms
#define DDI_SC_TURN_OFF_TIME                     206 // ms

#define DDI_SETPOINT_WORK_STATE                  289 // 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE1_16    290 // sections 1-16 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE17_32   291 // sections 17-32 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE33_48   292 // sections 33-48 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE49_64   293 // sections 49-64 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE65_80   294 // sections 65-80 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE81_96   295 // sections 81-96 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE97_112  296 // sections 97-112 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE113_128 297 // sections 112-128 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE129_144 298 // sections 129-144 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE145_160 299 // sections 145-160 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE161_176 300 // sections 161-176 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE177_192 301 // sections 177-192 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE193_208 302 // sections 193-208 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE209_224 303 // sections 209-224 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE225_240 304 // sections 225-240 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed
#define DDI_SETPOINT_CONDENSED_WORK_STATE241_256 305 // sections 241-256 bit fields 00 - disabled, 01 - enabled, 10 - error, 11 - not installed

#define DDI_PGN_BASED_LOGGING                    0xDFFE
#define DDI_DEFAULT_PROCESS_DATA                 0xDFFF

#define DDI_INVALID_DDI                          0xFFFF

#endif // _DDIDEFINITION_H_
