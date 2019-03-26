/*
  tractorenginecontroller1_c.h: class for retrieving tractor engine controller data

  (C) Copyright 2018 - 2019 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef TRACTORENGINECONTROLLER1_C_H
#define TRACTORENGINECONTROLLER1_C_H

#include <IsoAgLib/comm/Part7_ApplicationLayer/impl/tractorcommonrx_c.h>

namespace __IsoAgLib
{

    class TractorEngineController1_c: public TractorCommonRx_c
    {
    public:
        enum StarterMode_t
        {
            start_not_requested,
            starter_active_gear_not_engaged,
            starter_active_gear_engaged,
            start_finished_starter_not_active_after_having_been_actively_engaged,
            starter_inhibited_due_to_engine_already_running,
            starter_inhibited_due_to_engine_not_ready_for_start,
            starter_inhibited_due_to_driveline_engaged_or_other_transmission_inhibit,
            starter_inhibited_due_to_active_immobilizer,
            starter_inhibited_due_to_starter_over_temp,
            starter_inhibited_due_to_intake_air_shutoff_valve_being_active,
            starter_inhibited_due_to_active_SCR_inducement,
            reserved,
            starter_inhibited_reason_unknown,
            error_legacy_implementation_only_use_1110,
            error,
            starter_mode_not_available
        };

        enum TorqueMode_t
        {
            low_idle_governor_or_no_request_default_mode,
            accelerator_pedal_or_operator_selection,
            cruise_control,
            pto_governor,
            road_speed_governor,
            asr_control,
            transmission_control,
            abs_control,
            torque_limiting,
            high_speed_governor,
            braking_system,
            remote_accelerator,
            service_procedure,
            not_defined,
            other,
            torque_mode_not_available
        };

        inline bool isTorqueModeUsable() const;
        inline bool isActualPercentTorqueFractionalUsable() const;
        inline bool isDriverTargetPercentTorqueUsable() const;
        inline bool isActualPercentTorqueUsable() const;
        inline bool isRpmUsable() const;
        inline bool isStarterModeUsable() const;
        inline bool isDemandPercentTorqueUsable() const;

        /** deliver ISO engine torque mode
        * @return TorqueMode_t [0;15], 0 offset
        */
        TorqueMode_t torqueMode() const { return mt_torqueMode; }
        
        /** an additional torque in percent of the reference engine torque
        * @return [+0.000% to +0.875%];  res: .125%/bit, offset: 0 
        */
        uint8_t actualPercentTorqueFractional() const { return mui8_actualPercentTorqueFractional; }
       
        /** requested torque output of the engine by the drive
        * @return [-125%;125%], res: 1%/bit, offset: -125% 
        */
        int8_t driverTargetPercentTorque() const { return mui8_driverTargetPercentTorque - 125; }
        
        /** the calculated output torque of the engine
        * @return [-125%;125%], res: 1%/bit, offset: -125%
        */
        int8_t percentTorque() const { return mui8_actualPercentTorque  - 125; }
        
        /** Actual engine speed which is calculated over a minimum crankshaft angle of 720 degrees divided by the number of
        cylinders
        * @return [0 rpm;8,031.875 rpm], res: 0.125 rpm/bit, offset: 0 rpm 
        */
        uint16_t rpm() const { return mui16_rpm; }
        
        /** source address of the SAE J1939 device currently controlling the engine
        * @return [0;255] , offset: 0,  Operational Range: 0 to 253
        */
        uint8_t sourceAddress() const { return mui8_sourceAddressOfEngineControl; }
        
        /** starter actions / errors
        * @return StarterMode_t [0;15], 0 offset
        */
        StarterMode_t starterMode() const { return mt_starterMode; }
        
        /** target torque output of the engine
        * @return [-125%;125%], res: 1%/bit, offset: -125%
        */
        int8_t demandPercentTorque() const { return mui8_demandPercentTorque - 125; }

    private:
        TractorEngineController1_c(): TractorCommonRx_c( TractorCommonRx_c::TIMEOUT_SENDING_NODE_J1939 , ELECTRONIC_ENGINE_CONTROLLER_1_PGN ) {}

        virtual void setValues( const CanPkgExt_c& arc_data );
        virtual void resetValues();

    private:
        /**  [0; 15] , 0 offset */
        TorqueMode_t mt_torqueMode;    
        
        /**  [+0.000% to +0.875%], res: .125%/bit, offset: 0 */
        uint8_t mui8_actualPercentTorqueFractional;

        /**  [-125%;125%], res: 1%/bit, offset: -125% */
        uint8_t mui8_driverTargetPercentTorque;
        
        /**  [-125%;125%], res: 1%/bit, offset: -125% */
        uint8_t mui8_actualPercentTorque;
        
        /**  [0 rpm;8,031.875 rpm], res: 0.125 rpm/bit, offset: 0 rpm */
        uint16_t mui16_rpm;
        
        /**  [0;255] , offset: 0,  Operational Range: 0 to 253 */
        uint8_t mui8_sourceAddressOfEngineControl;                    
        
         /** [0;15], 0 offset */
        StarterMode_t mt_starterMode;
        
        /**  [-125%;125%], res: 1%/bit, offset: -125% */
        uint8_t mui8_demandPercentTorque;

        friend TractorEngineController1_c &getTractorEngineController1Instance( unsigned instance );
    };

    TractorEngineController1_c &getTractorEngineController1Instance( unsigned instance );
   
    inline bool TractorEngineController1_c::isTorqueModeUsable() const
    {
        return mt_torqueMode != torque_mode_not_available;
    }

    inline bool TractorEngineController1_c::isActualPercentTorqueFractionalUsable() const
    {
        return mui8_actualPercentTorqueFractional <= MAX_VAL_8;
    }

    inline bool TractorEngineController1_c::isDriverTargetPercentTorqueUsable() const
    {
        return mui8_driverTargetPercentTorque <= MAX_VAL_8;
    }
    
    inline bool TractorEngineController1_c::isActualPercentTorqueUsable() const
    {
        return mui8_actualPercentTorque <= MAX_VAL_8;
    }
    
    inline bool TractorEngineController1_c::isRpmUsable() const
    {
        return mui16_rpm <= MAX_VAL_16;
    }
    
    inline bool TractorEngineController1_c::isStarterModeUsable() const
    {
        return mt_starterMode != starter_mode_not_available;
    }
    
    inline bool TractorEngineController1_c::isDemandPercentTorqueUsable() const
    {
        return mui8_demandPercentTorque <= MAX_VAL_8;
    }
}
#endif
