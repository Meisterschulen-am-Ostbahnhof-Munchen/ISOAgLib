/*
  (C) Copyright 2009 - 2015 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _IDDIDEFINITION_H_
#define _IDDIDEFINITION_H_

// This file was auto-generated

// Paste the formula below into cell "U6" of the file "export_YYYY-MM-DD.xlsx" which can be exported using this link:
//     http://dictionary.isobus.net/isobus/exports/complete
// Column Headers (line 5)
//     DDIdentifier	DDEName	Definition	Comment	Typically used by Device Class(es)	UnitSymbol	UnitQuantity	BitResolution	SAESPN	MinimumValue	MaximumValue	SubmitBy	SubmitDate	SubmitCompany	RevisionNumber	Last Status	Last Status Date	StatusComments	Attachments
// Formula (put this formula into cell "U6")
//     =CONCATENATE("#define DDI_",SUBSTITUTE(SUBSTITUTE(SUBSTITUTE(SUBSTITUTE(SUBSTITUTE(SUBSTITUTE(SUBSTITUTE(SUBSTITUTE(SUBSTITUTE(UPPER(TRIM(B6))," - ", "_")," – ", "_")," ","_"),"(",""),")",""),"-","_"),"[",""),"]",""),"–","_"), "   ", A6, "   // ", $F$5, "=",F6,", ",$G$5,"=",G6,", ",$H$5,"=",H6,", ",$J$5,"=",J6,", ",$K$5,"=",K6)
// Copy that formula all the way down column U to the last row in the spreadsheet


#define DDI_DATA_DICTIONARY_VERSION   0   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_VOLUME_PER_AREA_APPLICATION_RATE   1   // UnitSymbol=mm³/m², UnitQuantity=Capacity per area unit, BitResolution=0.01, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_VOLUME_PER_AREA_APPLICATION_RATE   2   // UnitSymbol=mm³/m², UnitQuantity=Capacity per area unit, BitResolution=0.01, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_VOLUME_PER_AREA_APPLICATION_RATE   3   // UnitSymbol=mm³/m², UnitQuantity=Capacity per area unit, BitResolution=0.01, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_VOLUME_PER_AREA_APPLICATION_RATE   4   // UnitSymbol=mm³/m², UnitQuantity=Capacity per area unit, BitResolution=0.01, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_VOLUME_PER_AREA_APPLICATION_RATE   5   // UnitSymbol=mm³/m², UnitQuantity=Capacity per area unit, BitResolution=0.01, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_MASS_PER_AREA_APPLICATION_RATE   6   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_MASS_PER_AREA_APPLICATION_RATE   7   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_MASS_PER_AREA_APPLICATION_RATE   8   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_MASS_PER_AREA_APPLICATION_RATE   9   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_MASS_PER_AREA_APPLICATION_RATE   10   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_COUNT_PER_AREA_APPLICATION_RATE   11   // UnitSymbol=/m², UnitQuantity=Quantity per area unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_COUNT_PER_AREA_APPLICATION_RATE   12   // UnitSymbol=/m², UnitQuantity=Quantity per area unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_COUNT_PER_AREA_APPLICATION_RATE   13   // UnitSymbol=/m², UnitQuantity=Quantity per area unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_COUNTPER_AREA_APPLICATION_RATE   14   // UnitSymbol=/m², UnitQuantity=Quantity per area unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_COUNT_PER_AREA_APPLICATION_RATE   15   // UnitSymbol=/m², UnitQuantity=Quantity per area unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_SPACING_APPLICATION_RATE   16   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_SPACING_APPLICATION_RATE   17   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_SPACING_APPLICATION_RATE   18   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_SPACING_APPLICATION_RATE   19   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_SPACING_APPLICATION_RATE   20   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_VOLUME_PER_VOLUME_APPLICATION_RATE   21   // UnitSymbol=mm³/m³, UnitQuantity=Capacity per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_VOLUME_PER_VOLUME_APPLICATION_RATE   22   // UnitSymbol=mm³/m³, UnitQuantity=Capacity per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_VOLUME_PER_VOLUME_APPLICATION_RATE   23   // UnitSymbol=mm³/m³, UnitQuantity=Capacity per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_VOLUME_PER_VOLUME_APPLICATION_RATE   24   // UnitSymbol=mm³/m³, UnitQuantity=Capacity per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_VOLUME_PER_VOLUME_APPLICATION_RATE   25   // UnitSymbol=mm³/m³, UnitQuantity=Capacity per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_MASS_PER_MASS_APPLICATION_RATE   26   // UnitSymbol=mg/kg, UnitQuantity=Mass per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_MASS_PER_MASS_APPLICATION_RATE   27   // UnitSymbol=mg/kg, UnitQuantity=Mass per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_MASS_PER_MASSA_PPLICATION_RATE   28   // UnitSymbol=mg/kg, UnitQuantity=Mass per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_MASS_PER_MASS_APPLICATION_RATE   29   // UnitSymbol=mg/kg, UnitQuantity=Mass per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUMMASS_PER_MASS_APPLICATION_RATE   30   // UnitSymbol=mg/kg, UnitQuantity=Mass per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_VOLUME_PER_MASS_APPLICATION_RATE   31   // UnitSymbol=mm³/kg, UnitQuantity=Capacity per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_VOLUME_PER_MASS_APPLICATION_RATE   32   // UnitSymbol=mm³/kg, UnitQuantity=Capacity per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_VOLUME_PER_MASS_APPLICATION_RATE   33   // UnitSymbol=mm³/kg, UnitQuantity=Capacity per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_VOLUME_PER_MASS_APPLICATION_RATE   34   // UnitSymbol=mm³/kg, UnitQuantity=Capacity per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_VOLUME_PER_MASS_APPLICATION_RATE   35   // UnitSymbol=mm³/kg, UnitQuantity=Capacity per mass unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_VOLUME_PER_TIME_APPLICATION_RATE   36   // UnitSymbol=mm³/s, UnitQuantity=Flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_VOLUME_PER_TIME_APPLICATION_RATE   37   // UnitSymbol=mm³/s, UnitQuantity=Flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_VOLUME_PER_TIME_APPLICATION_RATE   38   // UnitSymbol=mm³/s, UnitQuantity=Flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_VOLUME_PER_TIME_APPLICATION_RATE   39   // UnitSymbol=mm³/s, UnitQuantity=Flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_VOLUME_PER_TIME_APPLICATION_RATE   40   // UnitSymbol=mm³/s, UnitQuantity=Flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_MASS_PER_TIME_APPLICATION_RATE   41   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_MASS_PER_TIME_APPLICATION_RATE   42   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_MASS_PER_TIME_APPLICATION_RATE   43   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_MASS_PER_TIME_APPLICATION_RATE   44   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_MASS_PER_TIME_APPLICATION_RATE   45   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_COUNT_PER_TIME_APPLICATION_RATE   46   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_COUNT_PER_TIME_APPLICATION_RATE   47   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_COUNT_PER_TIME_APPLICATION_RATE   48   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_COUNT_PER_TIME_APPLICATION_RATE   49   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_COUNT_PER_TIME_APPLICATION_RATE   50   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_TILLAGE_DEPTH   51   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_TILLAGE_DEPTH   52   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_TILLAGE_DEPTH   53   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_TILLAGE_DEPTH   54   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_TILLAGE_DEPTH   55   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_SEEDING_DEPTH   56   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_SEEDING_DEPTH   57   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_SEEDING_DEPTH   58   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_SEEDING_DEPTH   59   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_SEEDING_DEPTH   60   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_WORKING_HEIGHT   61   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_WORKING_HEIGHT   62   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_WORKING_HEIGHT   63   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_WORKING_HEIGHT   64   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_WORKING_HEIGHT   65   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_WORKING_WIDTH   66   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_WORKING_WIDTH   67   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_WORKING_WIDTH   68   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_WORKING_WIDTH   69   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_WORKING_WIDTH   70   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_VOLUME_CONTENT   71   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_VOLUME_CONTENT   72   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_VOLUME_CONTENT   73   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_MASS_CONTENT   74   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_MASS_CONTENT   75   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MAXIMUM_MASS_CONTENT   76   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_COUNT_CONTENT   77   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_COUNT_CONTENT   78   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_COUNT_CONTENT   79   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_APPLICATION_TOTAL_VOLUME_IN_L   80   // UnitSymbol=L, UnitQuantity=Capacity count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_APPLICATION_TOTAL_MASS_IN_KG   81   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_APPLICATION_TOTAL_COUNT   82   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_VOLUME_PER_AREA_YIELD   83   // UnitSymbol=ml/m², UnitQuantity=Capacity per area large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MASS_PER_AREA_YIELD   84   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_COUNT_PER_AREA_YIELD   85   // UnitSymbol=/m², UnitQuantity=Quantity per area unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_VOLUME_PER_TIME_YIELD   86   // UnitSymbol=ml/s, UnitQuantity=Float large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MASS_PER_TIME_YIELD   87   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_COUNT_PER_TIME_YIELD   88   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_YIELD_TOTAL_VOLUME   89   // UnitSymbol=L, UnitQuantity=Quantity per volume, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_YIELD_TOTAL_MASS   90   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_YIELD_TOTAL_COUNT   91   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_VOLUME_PER_AREA_CROP_LOSS   92   // UnitSymbol=ml/m², UnitQuantity=Capacity per area large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MASS_PER_AREA_CROP_LOSS   93   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_COUNT_PER_AREA_CROP_LOSS   94   // UnitSymbol=/m², UnitQuantity=Quantity per area unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_VOLUME_PER_TIME_CROP_LOSS   95   // UnitSymbol=ml/s, UnitQuantity=Float large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MASS_PER_TIME_CROP_LOSS   96   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_COUNT_PER_TIME_CROP_LOSS   97   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_PERCENTAGE_CROP_LOSS   98   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_CROP_MOISTURE   99   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_CROP_CONTAMINATION   100   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_BALE_WIDTH   101   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_BALE_WIDTH   102   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_BALE_WIDTH   103   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_BALE_WIDTH   104   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_BALE_WIDTH   105   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_BALE_HEIGHT   106   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUALBALEHEIGHT   107   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_BALE_HEIGHT   108   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_BALE_HEIGHT   109   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_BALE_HEIGHT   110   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_BALE_SIZE   111   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_BALE_SIZE   112   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEFAULT_BALE_SIZE   113   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_BALE_SIZE   114   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_BALE_SIZE   115   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_TOTAL_AREA   116   // UnitSymbol=m², UnitQuantity=Area, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_EFFECTIVE_TOTAL_DISTANCE   117   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_INEFFECTIVE_TOTAL_DISTANCE   118   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_EFFECTIVE_TOTAL_TIME   119   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_INEFFECTIVE_TOTAL_TIME   120   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_PRODUCT_DENSITY_MASS_PER_VOLUME   121   // UnitSymbol=mg/l, UnitQuantity=Mass per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_PRODUCT_DENSITY_MASS_PERCOUNT   122   // UnitSymbol=mg/1000, UnitQuantity=1000 seed Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_PRODUCT_DENSITY_VOLUME_PER_COUNT   123   // UnitSymbol=ml/1000, UnitQuantity=Volume per quantity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_SCALING_EXTEND   124   // UnitSymbol=0, UnitQuantity=Percent, BitResolution=0, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_SCALING_RETRACT   125   // UnitSymbol=0, UnitQuantity=Percent, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_RAMP_EXTEND_UP   126   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_RAMP_EXTEND_DOWN   127   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_RAMP_RETRACT_UP   128   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_RAMP_RETRACT_DOWN   129   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_FLOAT_THRESHOLD   130   // UnitSymbol=0, UnitQuantity=Percent, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_PROGRESSIVITY_EXTEND   131   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_PROGRESSIVITY_RETRACT   132   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_AUXILIARY_VALVE_INVERT_PORTS   133   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_DEVICE_ELEMENT_OFFSET_X   134   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_DEVICE_ELEMENT_OFFSET_Y   135   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_DEVICE_ELEMENT_OFFSET_Z   136   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_DEVICE_VOLUME_CAPACITY   137   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEVICE_MASS_CAPACITY   138   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEVICE_COUNT_CAPACITY   139   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_PERCENTAGE_APPLICATION_RATE   140   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_WORK_STATE   141   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_PHYSICAL_SETPOINT_TIME_LATENCY   142   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_PHYSICAL_ACTUAL_VALUE_TIME_LATENCY   143   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_YAW_ANGLE   144   // UnitSymbol=°, UnitQuantity=Angle, BitResolution=0.001, MinimumValue=-180000, MaximumValue=180000
#define DDI_ROLL_ANGLE   145   // UnitSymbol=°, UnitQuantity=Angle, BitResolution=0.001, MinimumValue=-180000, MaximumValue=180000
#define DDI_PITCH_ANGLE   146   // UnitSymbol=°, UnitQuantity=Angle, BitResolution=0.001, MinimumValue=-180000, MaximumValue=180000
#define DDI_LOG_COUNT   147   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_TOTAL_FUEL_CONSUMPTION   148   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_INSTANTANEOUS_FUEL_CONSUMPTION_PER_TIME   149   // UnitSymbol=mm³/s, UnitQuantity=Flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_INSTANTANEOUS_FUEL_CONSUMPTION_PER_AREA   150   // UnitSymbol=mm³/m², UnitQuantity=Capacity per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_INSTANTANEOUS_AREA_PER_TIME_CAPACITY   151   // UnitSymbol=mm²/s, UnitQuantity=Area per time unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_NORMALIZED_DIFFERENCE_VEGETATIVE_INDEX_NDVI   153   // UnitSymbol=n.a., UnitQuantity=, BitResolution=0.001, MinimumValue=-1, MaximumValue=1
#define DDI_PHYSICAL_OBJECT_LENGTH   154   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_PHYSICAL_OBJECT_WIDTH   155   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_PHYSICAL_OBJECT_HEIGHT   156   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_CONNECTOR_TYPE   157   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=8
#define DDI_PRESCRIPTION_CONTROL_STATE   158   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_NUMBER_OF_SUB_UNITS_PER_SECTION   159   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SECTION_CONTROL_STATE   160   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_ACTUAL_CONDENSED_WORK_STATE_1_16   161   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_17_32   162   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_33_48   163   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_49_64   164   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_65_80   165   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_81_96   166   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_97_112   167   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_113_128   168   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_129_144   169   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_145_160   170   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_161_176   171   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_177_192   172   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_193_208   173   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_209_224   174   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_225_240   175   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_CONDENSED_WORK_STATE_241_256   176   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_LENGTH_OF_CUT   177   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483.647
#define DDI_ELEMENT_TYPE_INSTANCE   178   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=65533
#define DDI_ACTUAL_CULTURAL_PRACTICE   179   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DEVICE_REFERENCE_POINT_DRP_TO_GROUND_DISTANCE   180   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_DRY_MASS_PER_AREA_YIELD   181   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DRY_MASS_PER_TIME_YIELD   182   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_YIELD_TOTAL_DRY_MASS   183   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_REFERENCE_MOISTURE_FOR_DRY_MASS   184   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SEED_COTTON_MASS_PER_AREA_YIELD   185   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LINT_COTTON_MASS_PER_AREA_YIELD   186   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SEED_COTTON_MASS_PER_TIME_YIELD   187   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LINT_COTTON_MASS_PER_TIME_YIELD   188   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_YIELD_TOTAL_SEED_COTTON_MASS   189   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_YIELD_TOTAL_LINT_COTTON_MASS   190   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LINT_TURNOUT_PERCENTAGE   191   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_AMBIENT_TEMPERATURE   192   // UnitSymbol=mK, UnitQuantity=Temperature, BitResolution=1, MinimumValue=0, MaximumValue=1000000
#define DDI_SETPOINT_PRODUCT_PRESSURE   193   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_PRODUCT_PRESSURE   194   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MINIMUM_PRODUCT_PRESSURE   195   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MAXIMUM_PRODUCT_PRESSURE   196   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_SETPOINT_PUMP_OUTPUT_PRESSURE   197   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_PUMP_OUTPUT_PRESSURE   198   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MINIMUM_PUMP_OUTPUT_PRESSURE   199   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MAXIMUM_PUMP_OUTPUT_PRESSURE   200   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_SETPOINT_TANK_AGITATION_PRESSURE   201   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_TANK_AGITATION_PRESSURE   202   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MINIMUM_TANK_AGITATION_PRESSURE   203   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MAXIMUM_TANK_AGITATION_PRESSURE   204   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_SC_TURN_ON_TIME   205   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_SC_TURN_OFF_TIME   206   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_WIND_SPEED   207   // UnitSymbol=mm/s, UnitQuantity=Speed, BitResolution=1, MinimumValue=0, MaximumValue=100 000 000
#define DDI_WIND_DIRECTION   208   // UnitSymbol=°, UnitQuantity=Angle, BitResolution=1, MinimumValue=0, MaximumValue=359
#define DDI_AIR_HUMIDITY   209   // UnitSymbol=0, UnitQuantity=Percent, BitResolution=1, MinimumValue=0, MaximumValue=100
#define DDI_SKY_CONDITIONS   210   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_LAST_BALE_FLAKES_PER_BALE   211   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=1000
#define DDI_LAST_BALE_AVERAGE_MOISTURE   212   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=100000000
#define DDI_LAST_BALE_AVERAGE_STROKES_PER_FLAKE   213   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=1000
#define DDI_LIFETIME_BALE_COUNT   214   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_WORKING_HOURS   215   // UnitSymbol=h, UnitQuantity=Hour, BitResolution=0.05, MinimumValue=0, MaximumValue=210554060.75
#define DDI_ACTUAL_BALE_HYDRAULIC_PRESSURE   216   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LAST_BALE_AVERAGE_HYDRAULIC_PRESSURE   217   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_BALE_COMPRESSION_PLUNGER_LOAD   218   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=2000
#define DDI_ACTUAL_BALE_COMPRESSION_PLUNGER_LOAD   219   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=2000
#define DDI_LAST_BALE_AVERAGE_BALE_COMPRESSION_PLUNGER_LOAD   220   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=2000
#define DDI_LAST_BALE_APPLIED_PRESERVATIVE   221   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LAST_BALE_TAG_NUMBER   222   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LAST_BALE_MASS   223   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_DELTA_T   224   // UnitSymbol=mK, UnitQuantity=Temperature, BitResolution=1, MinimumValue=0, MaximumValue=1000000
#define DDI_SETPOINT_WORKING_LENGTH   225   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_WORKING_LENGTH   226   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_WORKING_LENGTH   227   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_WORKING_LENGTH   228   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_NET_WEIGHT   229   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_NET_WEIGHT_STATE   230   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_SETPOINT_NET_WEIGHT   231   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_GROSS_WEIGHT   232   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_GROSS_WEIGHT_STATE   233   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_MINIMUM_GROSS_WEIGHT   234   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MAXIMUM_GROSS_WEIGHT   235   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_THRESHER_ENGAGEMENT_TOTAL_TIME   236   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_HEADER_WORKING_HEIGHT_STATUS   237   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_ACTUAL_HEADER_ROTATIONAL_SPEED_STATUS   238   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_YIELD_HOLD_STATUS   239   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_ACTUAL_UNLOADING_SYSTEM_STATUS   240   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_CROP_TEMPERATURE   241   // UnitSymbol=mK, UnitQuantity=Temperature, BitResolution=1, MinimumValue=0, MaximumValue=1000000
#define DDI_SETPOINT_SIEVE_CLEARANCE   242   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_SIEVE_CLEARANCE   243   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_SIEVE_CLEARANCE   244   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_SIEVE_CLEARANCE   245   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_CHAFFER_CLEARANCE   246   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_CHAFFER_CLEARANCE   247   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_CHAFFER_CLEARANCE   248   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_CHAFFER_CLEARANCE   249   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_CONCAVE_CLEARANCE   250   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_CONCAVE_CLEARANCE   251   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_CONCAVE_CLEARANCE   252   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_CONCAVE_CLEARANCE   253   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_SEPARATION_FAN_ROTATIONAL_SPEED   254   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_SEPARATION_FAN_ROTATIONAL_SPEED   255   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_SEPARATION_FAN_ROTATIONAL_SPEED   256   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_SEPARATION_FAN_ROTATIONAL_SPEED   257   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_HYDRAULIC_OIL_TEMPERATURE   258   // UnitSymbol=mK, UnitQuantity=Temperature, BitResolution=1, MinimumValue=0, MaximumValue=2000000
#define DDI_YIELD_LAG_IGNORE_TIME   259   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_YIELD_LEAD_IGNORE_TIME   260   // UnitSymbol=ms, UnitQuantity=Time, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_AVERAGE_YIELD_MASS_PER_TIME   261   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_AVERAGE_CROP_MOISTURE   262   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_AVERAGE_YIELD_MASS_PER_AREA   263   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_CONNECTOR_PIVOT_X_OFFSET   264   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_REMAINING_AREA   265   // UnitSymbol=m², UnitQuantity=Area, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_APPLICATION_TOTAL_MASS   266   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_APPLICATION_TOTAL_COUNT   267   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_YIELD_TOTAL_VOLUME   268   // UnitSymbol=L, UnitQuantity=Quantity per volume, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_YIELD_TOTAL_MASS   269   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_YIELD_TOTAL_COUNT   270   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_TOTAL_AREA   271   // UnitSymbol=m², UnitQuantity=Area, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_TOTAL_DISTANCE   272   // UnitSymbol=m, UnitQuantity=Distance, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_INEFFECTIVE_TOTAL_DISTANCE   273   // UnitSymbol=m, UnitQuantity=Distance, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_EFFECTIVE_TOTAL_TIME   274   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_INEFFECTIVE_TOTAL_TIME   275   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_FUEL_CONSUMPTION   276   // UnitSymbol=L, UnitQuantity=Capacity count, BitResolution=0.5, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_AVERAGE_FUEL_CONSUMPTION_PER_TIME   277   // UnitSymbol=mm³/s, UnitQuantity=Flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_AVERAGE_FUEL_CONSUMPTION_PER_AREA   278   // UnitSymbol=mm³/m², UnitQuantity=Capacity per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_YIELD_TOTAL_DRY_MASS   279   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_YIELD_TOTAL_SEED_COTTON_MASS   280   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_YIELD_TOTAL_LINT_COTTON_MASS   281   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_THRESHING_ENGAGEMENT_TOTAL_TIME   282   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_PRECUT_TOTAL_COUNT   283   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_UNCUT_TOTAL_COUNT   284   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_PRECUT_TOTAL_COUNT   285   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_UNCUT_TOTAL_COUNT   286   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_WORK_STATE   289   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=3
#define DDI_SETPOINT_CONDENSED_WORK_STATE_1_16   290   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_17_32   291   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_33_48   292   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_49_64   293   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_65_80   294   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_81_96   295   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_97_112   296   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_113_128   297   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_129_144   298   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_145_160   299   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_161_176   300   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_177_192   301   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_193_208   302   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_209_224   303   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_225_240   304   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_SETPOINT_CONDENSED_WORK_STATE_241_256   305   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=4294967295
#define DDI_TRUE_ROTATION_POINT_X_OFFSET   306   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_TRUE_ROTATION_POINT_Y_OFFSET   307   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_PERCENTAGE_APPLICATION_RATE   308   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_PERCENTAGE_APPLICATION_RATE   309   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_PERCENTAGE_APPLICATION_RATE   310   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_RELATIVE_YIELD_POTENTIAL   311   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_RELATIVE_YIELD_POTENTIAL   312   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_RELATIVE_YIELD_POTENTIAL   313   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_PERCENTAGE_CROP_DRY_MATTER   314   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_AVERAGE_PERCENTAGE_CROP_DRY_MATTER   315   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_EFFECTIVE_TOTAL_FUEL_CONSUMPTION   316   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_INEFFECTIVE_TOTAL_FUEL_CONSUMPTION   317   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_EFFECTIVE_TOTAL_DIESEL_EXHAUST_FLUID_CONSUMPTION   318   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_INEFFECTIVE_TOTAL_DIESEL_EXHAUST_FLUID_CONSUMPTION   319   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LAST_LOADED_WEIGHT   320   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LAST_UNLOADED_WEIGHT   321   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LOAD_IDENTIFICATION_NUMBER   322   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=4294967296
#define DDI_UNLOAD_IDENTIFICATION_NUMBER   323   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_CHOPPER_ENGAGEMENT_TOTAL_TIME   324   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_APPLICATION_TOTAL_VOLUME   325   // UnitSymbol=L, UnitQuantity=Capacity count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_HEADER_SPEED   326   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_HEADER_SPEED   327   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_HEADER_SPEED   328   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_HEADER_SPEED   329   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_CUTTING_DRUM_SPEED   330   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_CUTTING_DRUM_SPEED   331   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_CUTTING_DRUM_SPEED   333   // UnitSymbol=/s, UnitQuantity=Quantity per time unit, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_OPERATING_HOURS_SINCE_LAST_SHARPENING   334   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_FRONT_PTO_HOURS   335   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_REAR_PTO_HOURS   336   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFETIME_FRONT_PTO_HOURS   337   // UnitSymbol=h, UnitQuantity=Hour, BitResolution=0.1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LIFTIME_REAR_PTO_HOURS   338   // UnitSymbol=h, UnitQuantity=Hour, BitResolution=0.1, MinimumValue=0, MaximumValue=2147483647
#define DDI_EFFECTIVE_LOADING_TIME   339   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_EFFECTIVE_UNLOADING_TIME   340   // UnitSymbol=s, UnitQuantity=Time count, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_GRAIN_KERNEL_CRACKER_GAP   341   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_GRAIN_KERNEL_CRACKER_GAP   342   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_GRAIN_KERNEL_CRACKER_GAP   343   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_GRAIN_KERNEL_CRACKER_GAP   344   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=0.001, MinimumValue=0, MaximumValue=2147483647
#define DDI_SETPOINT_SWATHING_WIDTH   345   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_SWATHING_WIDTH   346   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MINIMUM_SWATHING_WIDTH   347   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_MAXIMUM_SWATHING_WIDTH   348   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_NOZZLE_DRIFT_REDUCTION   349   // UnitSymbol=ppm, UnitQuantity=Parts per million, BitResolution=1, MinimumValue=0, MaximumValue=100
#define DDI_FUNCTION_TYPE   350   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_APPLICATION_TOTAL_VOLUME_IN_ML   351   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_APPLICATION_TOTAL_MASS_IN_GRAM_G   352   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_TOTAL_APPLICATION_OF_NITROGEN   353   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_TOTAL_APPLICATION_OF_AMMONIUM   354   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_TOTAL_APPLICATION_OF_PHOSPHOR   355   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_TOTAL_APPLICATION_OF_POTASSIUM   356   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_TOTAL_APPLICATION_OF_DRY_MATTER   357   // UnitSymbol=kg, UnitQuantity=Mass, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_AVERAGE_DRY_YIELD_MASS_PER_TIME   358   // UnitSymbol=mg/s, UnitQuantity=Mass flow, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_AVERAGE_DRY_YIELD_MASS_PER_AREA   359   // UnitSymbol=mg/m², UnitQuantity=Mass per area unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LAST_BALE_SIZE   360   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LAST_BALE_DENSITY   361   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_TOTAL_BALE_LENGTH   362   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_LAST_BALE_DRY_MASS   363   // UnitSymbol=g, UnitQuantity=Mass large, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_FLAKE_SIZE   364   // UnitSymbol=mm, UnitQuantity=Length, BitResolution=1, MinimumValue=0, MaximumValue=1000
#define DDI_SETPOINT_DOWNFORCE_PRESSURE   365   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_DOWNFORCE_PRESSURE   366   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_1_16   367   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_17_32   368   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_33_48   369   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_49_64   370   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_145_160   376   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_161_176   377   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_177_192   378   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_193_208   379   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_209_224   380   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_225_240   381   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_CONDENSED_SECTION_OVERRIDE_STATE_241_256   382   // UnitSymbol=not defined, UnitQuantity=not defined, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_APPARENT_WIND_DIRECTION   383   // UnitSymbol=°, UnitQuantity=Angle, BitResolution=1, MinimumValue=0, MaximumValue=359
#define DDI_APPARENT_WIND_SPEED   384   // UnitSymbol=mm/s, UnitQuantity=Speed, BitResolution=1, MinimumValue=0, MaximumValue=100000000
#define DDI_MSL_ATMOSPHERIC_PRESSURE   385   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=0, MaximumValue=2000000
#define DDI_ACTUAL_ATMOSPHERIC_PRESSURE   386   // UnitSymbol=Pa, UnitQuantity=Pressure, BitResolution=0.1, MinimumValue=0, MaximumValue=2000000
#define DDI_TOTAL_REVOLUTIONS_IN_FRACTIONAL_REVOLUTIONS   387   // UnitSymbol=r/min, UnitQuantity=Revolutions per minute, BitResolution=0.0001, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_TOTAL_REVOLUTIONS_IN_COMPLETE_REVOLUTIONS   388   // UnitSymbol=#, UnitQuantity=Quantity/Count, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_SETPOINT_REVOLUTIONS_SPECIFIED_AS_COUNT_PER_TIME   389   // UnitSymbol=r/min, UnitQuantity=Revolutions per minute, BitResolution=0.0001, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_REVOLUTIONS_PER_TIME   390   // UnitSymbol=r/min, UnitQuantity=Revolutions per minute, BitResolution=0.0001, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_DEFAULT_REVOLUTIONS_PER_TIME   391   // UnitSymbol=r/min, UnitQuantity=Revolutions per minute, BitResolution=0.0001, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MINIMUM_REVOLUTIONS_PER_TIME   392   // UnitSymbol=r/min, UnitQuantity=Revolutions per minute, BitResolution=0.0001, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MAXIMUM_REVOLUTIONS_PER_TIME   393   // UnitSymbol=r/min, UnitQuantity=Revolutions per minute, BitResolution=0.0001, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_FUEL_TANK_CONTENT   394   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=4294967296
#define DDI_ACTUAL_DIESEL_EXHAUST_FLUID_TANK_CONTENT   395   // UnitSymbol=ml, UnitQuantity=Capacity large, BitResolution=1, MinimumValue=0, MaximumValue=4294967296
#define DDI_SETPOINT_SPEED   396   // UnitSymbol=mm/s, UnitQuantity=Speed, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_ACTUAL_SPEED   397   // UnitSymbol=mm/s, UnitQuantity=Speed, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MINIMUM_SPEED   398   // UnitSymbol=mm/s, UnitQuantity=Speed, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_MAXIMUM_SPEED   399   // UnitSymbol=mm/s, UnitQuantity=Speed, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_SPEED_SOURCE   400   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=100
#define DDI_ACTUAL_APPLICATION_OF_NITROGEN   401   // UnitSymbol=mg/l, UnitQuantity=Mass per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_APPLICATION_OF_AMMONIUM   402   // UnitSymbol=mg/l, UnitQuantity=Mass per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_APPLICATION_OF_PHOSPHOR   403   // UnitSymbol=mg/l, UnitQuantity=Mass per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_APPLICATION_OF_POTASSIUM   404   // UnitSymbol=mg/l, UnitQuantity=Mass per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_ACTUAL_APPLICATION_OF_DRY_MATTER   405   // UnitSymbol=mg/l, UnitQuantity=Mass per capacity unit, BitResolution=1, MinimumValue=0, MaximumValue=2147483647
#define DDI_PGN_BASED_DATA   57342   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=-2147483648, MaximumValue=2147483647
#define DDI_REQUEST_DEFAULT_PROCESS_DATA   57343   // UnitSymbol=n.a., UnitQuantity=, BitResolution=1, MinimumValue=0, MaximumValue=0

#define DDI_PROPRIETARY_DDI_RANGE_FIRST   57344   // UnitSymbol=n.a., UnitQuantity=, BitResolution=0, MinimumValue=, MaximumValue=
#define DDI_PROPRIETARY_DDI_RANGE_LAST   65534   // UnitSymbol=n.a., UnitQuantity=, BitResolution=0, MinimumValue=, MaximumValue=
#define DDI_RESERVED   65535   // UnitSymbol=n.a., UnitQuantity=, BitResolution=0, MinimumValue=, MaximumValue=


// For backward compatibility:
#include "iddidefinition-deprecated.h"

#endif // _IDDIDEFINITION_H_
