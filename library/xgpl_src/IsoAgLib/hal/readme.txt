/** @page MainHalPage Structure of HAL for Main Parts of ISOAgLib

\section MainHalStructure Structure of HAL for Main Parts

\subsection MainHalEsxPc Structure of HAL for ESX of STW and for PC
\anchor MainHalEsx
\anchor MainHalPc
<img src="images/HAL_esx.png" alt="Structure of HAL for ESX of STW" title="Structure of HAL for ESX of STW"><br>
For more information see the different header file references:
- \ref hal/hal_config.h "hal/hal_config.h"
- \ref hal/hal_system.h "hal/hal_system.h"
- \ref hal/hal_can.h "hal/hal_can.h"
- \ref hal/hal_eeprom.h "hal/hal_eeprom.h"
- \ref esx/system/system.h "esx/system/system.h"
- \ref esx/can/can.h "esx/can/can.h"
- \ref esx/eeprom/eeprom.h "esx/eeprom/eeprom.h"
- \ref esx/config.h "esx/config.h"
- \ref esx/typedef.h "esx/typedef.h"
- \ref esx/errcodes.h "esx/errcodes.h"
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
