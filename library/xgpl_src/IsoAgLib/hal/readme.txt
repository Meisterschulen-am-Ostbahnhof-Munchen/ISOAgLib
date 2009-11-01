/** @page MainHalPage Structure of HAL for Main Parts of ISOAgLib

The directory structure of the \isoaglib HAL is designed to ease quick adaptation to new platform types.

\section MainHalAdaptationNew Adaptation to New Platform
The following steps are enough for adaptation to a new hardware:
    -# create a new <tt>\#define SYSTEM_XY</tt> in the header <tt>isoaglib_config.h</tt> for your platform (compare to <tt>SYSTEM_ESX</tt>)
    -# create a new <tt>\#elif</tt> setting in <tt>isoaglib_config.h</tt> where the directory name for the HAL of the new platform is defined (compare with <tt>SYSTEM_ESX</tt>)
    -# create a copy of the directory <tt>xgpl_src/IsoAgLib/hal/esx</tt> with the directory name as specified in <tt>isoaglib_config.h</tt>
    -# do this also for <tt>xgpl_src/supplementary_driver/hal/esx</tt> if you intend to use \isoaglib for actor, rs232 or sensor
    -# adapt the files in the new directories to your specific needs
        - adapt <tt>\#include</tt> statements for platform specific library/BIOS/RTOS
        - adapt config settings in <tt>config.h</tt> (compare to <tt>esx/config.h</tt>)
        - adapt typedefs settings in <tt>typedef.h</tt> (compare to <tt>esx/typedef.h</tt>)
        - adapt the function mapping in the several files <tt>target/device/device.h</tt>
        - implement functionality which is not provided by platform library in file named like <tt>system<b>_target_extensions.h</b></tt> and <tt>system<b>_target_extensions.cpp</b></tt>
        - adapt CAN layer in <tt>hal_can_interface.h</tt> and <tt>hal_can_interface.cpp</tt> (some CAN implementations use parameter structures which can't be mapped with inline functions) 
    -# insert your new <tt>SYSTEM_XY</tt> setting in your Makefiles or project configuration
    -# send your adaptation to <a href="mailto:Achim.Spangler_at_osb-ag:de">Achim Spangler</a>, so that your adaptation can be used and possibly optimized
    within the user community

\section MainHalStructure Structure of HAL for Main Parts

\subsection MainHalImiPM167Pc Structure of HAL for ESX, IMI and PM167 of STW and for PC
\anchor MainHalEsx
\anchor MainHalPm167 
\anchor MainHalImi
\anchor MainHalPc
<img src="images/HAL_esx.png" alt="Structure of HAL for ESX of STW" title="Structure of HAL for ESX of STW"><br>
<img src="images/HAL_imi_pm167_pc.png" alt="Structure of HAL for IMI/PM167/PC" title="Structure of HAL for IMI/PM167/PC"><br>
For more information see the different header file references:
- \ref hal/config.h "hal/config.h"
- \ref hal/system.h "hal/system.h"
- \ref hal/can.h "hal/can.h"
- \ref hal/eeprom.h "hal/eeprom.h"
- \ref esx/system/system.h "esx/system/system.h"
- \ref esx/can/can.h "esx/can/can.h"
- \ref esx/eeprom/eeprom.h "esx/eeprom/eeprom.h"
- \ref esx/config.h "esx/config.h"
- \ref esx/typedef.h "esx/typedef.h"
- \ref esx/errcodes.h "esx/errcodes.h"
- \ref imi/system/system.h "imi/system/system.h"
- \ref imi/can/can.h "imi/can/can.h"
- \ref imi/eeprom/eeprom.h "imi/eeprom/eeprom.h"
- \ref imi/config.h "imi/config.h"
- \ref imi/typedef.h "imi/typedef.h"
- \ref imi/errcodes.h "imi/errcodes.h"
- \ref pm167/system/system.h "pm167/system/system.h"
- \ref pm167/can/can.h "pm167/can/can.h"
- \ref pm167/eeprom/eeprom.h "pm167/eeprom/eeprom.h"
- \ref pm167/config.h "pm167/config.h"
- \ref pm167/typedef.h "pm167/typedef.h"
- \ref pm167/errcodes.h "pm167/errcodes.h"
- \ref pc/system/system.h "pc/system/system.h"
- \ref pc/can/can.h "pc/can/can.h"
- \ref pc/eeprom/eeprom.h "pc/eeprom/eeprom.h"
- \ref pc/config.h "pc/config.h"
- \ref pc/typedef.h "pc/typedef.h"
- \ref pc/errcodes.h "pc/errcodes.h"


<!--
\dot
digraph MainHalEsxStructure 
{
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

    m_hal       [label="Hardware Abstraction Layer\nfor main library parts\npartly obligatory", color="green4", fontcolor="white"];

    hal_config  [label="Central Header for\nPlatform Configuration\nobligatory", color="green2", shape="ellipse", URL="\ref hal/config.h"];
    hal_system  [label="Central Header for\nMain Target Functions (e.g. get_time())\nobligatory", color="green2", shape="ellipse", URL="\ref hal/system.h"];
    hal_can     [label="Central Header for\nPlatform specific CAN IO\nobligatory", color="green2", shape="ellipse", URL="\ref hal/can.h"];
    hal_eeprom  [label="Central Header for\nPlatform specific EEPROM IO\noptional", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
    hal_esx     [label="HAL for ESX of STW\nobligatory for this platform", color="green2"];

    h_esx_system [label="Target Adaptation\nof Base System Calls\n(obligatory for this platform)", color="green2", URL="\ref esx/system/system.h"];
    h_esx_can   [label="Target Adaptation\nof CAN IO\n(obligatory for this platform)", color="green2", URL="\ref esx/can/can.h"];
    h_esx_eeprom [label="Target Adaptation\nof EEPROM\n(optional);needed from other features", color="greenyellow", URL="\ref esx/eeprom/eeprom.h"];
    h_esx_conf  [label="ESX specific\nConfiguration Settings\n(obligatory)", color="green2", shape="ellipse", URL="\ref esx/config.h"];
    h_esx_type  [label="ESX specific\nTypedefs\n(obligatory)", color="green2", shape="ellipse", URL="\ref esx/typedef.h"];
    h_esx_err   [label="ESX specific\nErrcode Mapping\n(obligatory)", color="green2", shape="ellipse", URL="\ref esx/errcodes.h"];

    m_hal    -> hal_config [label="config.h"];
    m_hal    -> hal_system [label="system.h"];
    m_hal    -> hal_can [label="can.h"];
    m_hal    -> hal_eeprom [label="eeprom.h"];
    m_hal    -> hal_esx [label="esx"];

    hal_esx  -> h_esx_system [label="system"];
    hal_esx  -> h_esx_can [label="can"];
    hal_esx  -> h_esx_eeprom [label="eeprom"];
    hal_esx  -> h_esx_conf [label="config.h"];
    hal_esx  -> h_esx_type [label="typedef.h"];
    hal_esx  -> h_esx_err [label="errcodes.h"];

    hal_system -> h_esx_system [label="implement depend",style="dotted"];
    hal_can  -> h_esx_can [label="implement depend",style="dotted"];
    hal_eeprom -> h_esx_eeprom [label="implement depend",style="dotted"];
}
\enddot

\dot
digraph MainHalImiStructure
{
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

    m_hal       [label="Hardware Abstraction Layer\nfor main library parts\npartly obligatory", color="green4", fontcolor="white"];

    hal_config  [label="Central Header for\nPlatform Configuration\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/config.h"];
    hal_system  [label="Central Header for\nMain Target Functions (e.g. get_time())\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/system.h"];
    hal_can     [label="Central Header for\nPlatform specific CAN IO\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/can.h"];
    hal_eeprom  [label="Central Header for\nPlatform specific EEPROM IO\n(optional)", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
    hal_imi     [label="HAL for IMI of STW\n(obligatory for this platform)", color="green2"];

    h_imi_system [label="system", color="green2", URL="\ref imi/system/system.h"];
    h_imi_can   [label="can", color="green2", URL="\ref imi/can/can.h"];
    h_imi_eeprom [label="eeprom", color="greenyellow", URL="\ref imi/eeprom/eeprom.h"];
    h_imi_conf  [label="config.h", color="green2", shape="ellipse", URL="\ref imi/config.h"];
    h_imi_type  [label="typedef.h", color="green2", shape="ellipse", URL="\ref imi/typedef.h"];
    h_imi_err   [label="errcodes.h", color="green2", shape="ellipse", URL="\ref imi/errcodes.h"];

    m_hal    -> hal_config [label="config.h"];
    m_hal    -> hal_system [label="system.h"];
    m_hal    -> hal_can [label="can.h"];
    m_hal    -> hal_eeprom [label="eeprom.h"];
    m_hal    -> hal_imi [label="imi"];

    hal_imi  -> h_imi_system [label="system"];
    hal_imi  -> h_imi_can [label="can"];
    hal_imi  -> h_imi_eeprom [label="eeprom"];
    hal_imi  -> h_imi_conf [label="config.h"];
    hal_imi  -> h_imi_type [label="typedef.h"];
    hal_imi  -> h_imi_err [label="errcodes.h"];

    hal_system -> h_imi_system [label="implement depend",style="dotted"];
    hal_can  -> h_imi_can [label="implement depend",style="dotted"];
    hal_eeprom -> h_imi_eeprom [label="implement depend",style="dotted"];
}
\enddot



\dot
digraph MainHalPm167Structure {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

    m_hal       [label="Hardware Abstraction Layer\nfor main library parts\n(partly obligatory)", color="green4", fontcolor="white"];

    hal_config  [label="Central Header for\nPlatform Configuration\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/config.h"];
    hal_system  [label="Central Header for\nMain Target Functions (e.g. get_time())\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/system.h"];
    hal_can     [label="Central Header for\nPlatform specific CAN IO\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/can.h"];
    hal_eeprom  [label="Central Header for\nPlatform specific EEPROM IO\n(optional)", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
    hal_pm167   [label="HAL for PM167 of STW\n(obligatory for this platform)", color="green2"];

    h_pm167_system [label="system", color="green2", URL="\ref pm167/system/system.h"];
    h_pm167_can   [label="can", color="green2", URL="\ref pm167/can/can.h"];
    h_pm167_eeprom [label="eeprom", color="greenyellow", URL="\ref pm167/eeprom/eeprom.h"];
    h_pm167_conf  [label="config.h", color="green2", shape="ellipse", URL="\ref pm167/config.h"];
    h_pm167_type  [label="typedef.h", color="green2", shape="ellipse", URL="\ref pm167/typedef.h"];
    h_pm167_err   [label="errcodes.h", color="green2", shape="ellipse", URL="\ref pm167/errcodes.h"];

    m_hal    -> hal_config [label="config.h"];
    m_hal    -> hal_system [label="system.h"];
    m_hal    -> hal_can [label="can.h"];
    m_hal    -> hal_eeprom [label="eeprom.h"];
    m_hal    -> hal_pm167 [label="pm167"];

    hal_pm167 -> h_pm167_system [label="system"];
    hal_pm167 -> h_pm167_can [label="can"];
    hal_pm167 -> h_pm167_eeprom [label="eeprom"];
    hal_pm167 -> h_pm167_conf [label="config.h"];
    hal_pm167 -> h_pm167_type [label="typedef.h"];
    hal_pm167 -> h_pm167_err [label="errcodes.h"];

    hal_can  -> h_pm167_can [label="implement depend",style="dotted"];
    hal_system -> h_pm167_system [label="implement depend",style="dotted"];
    hal_eeprom -> h_pm167_eeprom [label="implement depend",style="dotted"];
}
\enddot
\dot
digraph MainHalPcStructure
{
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, style=filled, fontcolor=blue];
    edge     [fontname=ARIALN, fontsize=8];

    m_hal       [label="Hardware Abstraction Layer\nfor main library parts\npartly obligatory", color="green4", fontcolor="white"];

    hal_config  [label="Central Header for\nPlatform Configuration\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/config.h"];
    hal_system  [label="Central Header for\nMain Target Functions (e.g. get_time())\nobligatory)", color="green2", shape="ellipse", URL="\ref hal/system.h"];
    hal_can     [label="Central Header for\nPlatform specific CAN IO\n(obligatory)", color="green2", shape="ellipse", URL="\ref hal/can.h"];
    hal_eeprom  [label="Central Header for\nPlatform specific EEPROM IO\n(optional)", color="greenyellow", shape="ellipse", URL="\ref hal/eeprom.h"];
    hal_pc      [label="Example HAL for PC\n(obligatory for this platform)", color="green2"];

    h_pc_system [label="system", color="green2", URL="\ref pc/system/system.h"];
    h_pc_can    [label="can", color="green2", URL="\ref pc/can/can.h"];
    h_pc_eeprom [label="eeprom", color="greenyellow", URL="\ref pc/eeprom/eeprom.h"];
    h_pc_conf   [label="config.h", color="green2", shape="ellipse", URL="\ref pc/config.h"];
    h_pc_type   [label="typedef.h", color="green2", shape="ellipse", URL="\ref pc/typedef.h"];
    h_pc_err    [label="errcodes.h", color="green2", shape="ellipse", URL="\ref pc/errcodes.h"];

    m_hal    -> hal_config [label="config.h"];
    m_hal    -> hal_system [label="system.h"];
    m_hal    -> hal_can [label="can.h"];
    m_hal    -> hal_eeprom [label="eeprom.h"];
    m_hal    -> hal_pc [label="pc"];

    hal_pc   -> h_pc_system [label="system"];
    hal_pc   -> h_pc_can [label="can"];
    hal_pc   -> h_pc_eeprom [label="eeprom"];
    hal_pc   -> h_pc_conf [label="config.h"];
    hal_pc   -> h_pc_type [label="typedef.h"];
    hal_pc   -> h_pc_err [label="errcodes.h"];

    hal_system -> h_pc_system [label="implement depend",style="dotted"];
    hal_can  -> h_pc_can [label="implement depend",style="dotted"];
    hal_eeprom -> h_pc_eeprom [label="implement depend",style="dotted"];
}
\enddot
-->

*/
