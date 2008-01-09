/*! 
\page SuppHalPage Structure of HAL for Supplementary Parts of ISOAgLib
The directory structure of the \isoaglib HAL
is designed to ease quick adaptation to new platform types.
Please see \ref MainHalAdaptationNew for details.

\section SuppHalStructure Structure of HAL for Main Parts
\subsection SupplHalEsx HAL for Supplementary Drivers on ESX of STW
<img src="Images/HAL_suppl_esx.png" title="HAL for supplementary drivers of ESX of STW" alt="HAL for supplementary drivers of ESX of STW"><br>
More information can be found in the general HAL header files:
- \ref hal/actor.h "hal/actor.h"
- \ref hal/rs232.h "hal/rs232.h"
- \ref hal/sensor.h "hal/sensor.h"
and in the device specific headers:
- \ref esx/actor/actor.h "esx/actor/actor.h"
- \ref esx/rs232/rs232.h "esx/rs232/rs232.h"
- \ref esx/sensor/sensor.h "esx/sensor/sensor.h"

<!--
\dot
digraph SupplHalEsx 
{
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=8];

    sup_hal     [label="HAL for Supplementary Extended Drivers\n(optional)", color="green"];

    sup_hal_esx [label="HAL for Supplementary\nDrivers on ESX of STW\n(optional)", color="green"];
    sup_h_actor [label="Central HAL Header\nfor Supplementary Actor Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/actor.h"];
    sup_h_rs232 [label="Central HAL Header\nfor Supplementary RS232 Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/rs232.h"];
    sup_h_sens  [label="Central HAL Header\nfor Supplementary Sensor Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/sensor.h"];

    h_esx_actor [label="Target Adoption\n of Digital/PWM Output\n(optional)", color="green", URL="\ref esx/actor/actor.h"];
    h_esx_rs232 [label="Target Adoption\n of RS232 IO\n(optional)", color="green", URL="\ref esx/rs232/rs232.h"];
    h_esx_sensor [label="Target Adoption\n of Sensor Input\n(optional)", color="green", URL="\ref esx/sensor/sensor.h"];

    sup_hal  -> sup_h_actor [label="actor.h"];
    sup_hal  -> sup_h_rs232 [label="rs232.h"];
    sup_hal  -> sup_h_sens [label="sensor.h"];
    sup_hal  -> sup_hal_esx [label="esx"];

    sup_hal_esx -> h_esx_actor [label="actor"];
    sup_hal_esx -> h_esx_rs232 [label="rs232"];
    sup_hal_esx -> h_esx_sensor [label="sensor"];

    sup_h_actor -> h_esx_actor [label="implement depend",style="dotted"];
    sup_h_rs232 -> h_esx_rs232 [label="implement depend",style="dotted"];
    sup_h_sens -> h_esx_sensor [label="implement depend",style="dotted"];
}
\enddot
-->


\subsection SupplHalImi HAL for Supplementary Drivers on IMI of STW
<img src="Images/HAL_suppl_imi.png" title="HAL for Supplementary Drivers on IMI of STW" alt="HAL for Supplementary Drivers on IMI of STW"><br>
More information can be found in the general HAL header files (see links in previous subsection) and in the device specific headers:
- \ref imi/actor/actor.h "imi/actor/actor.h"
- \ref imi/rs232/rs232.h "imi/rs232/rs232.h"
- \ref imi/sensor/sensor.h "imi/sensor/sensor.h"

<!--
\dot
digraph SupplHalImi 
{
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=8];

    sup_hal     [label="HAL for Supplementary\nExtended Drivers\n(optional)", color="green"];

    sup_hal_imi [label="HAL for Supplementary\nDrivers on IMI of STW\n(optional)", color="green"];
    sup_h_actor [label="Central HAL Header\nfor Supplementary Actor Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/actor.h"];
    sup_h_rs232 [label="Central HAL Header\nfor Supplementary RS232 Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/rs232.h"];
    sup_h_sens  [label="Central HAL Header\nfor Supplementary Sensor Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/sensor.h"];

    h_imi_actor [label="Target Adoption\nof Digital/PWM Output\n(optional)", color="green", URL="\ref imi/actor/actor.h"];
    h_imi_rs232 [label="Target Adoption\nof RS232 IO\n(optional)", color="green", URL="\ref imi/rs232/rs232.h"];
    h_imi_sensor [label="Target Adoption\nof Sensor Input\n(optional)", color="green", URL="\ref imi/sensor/sensor.h"];

    sup_hal_imi -> h_imi_actor [label="actor"];
    sup_hal_imi -> h_imi_rs232 [label="rs232"];
    sup_hal_imi -> h_imi_sensor [label="sensor"];

    sup_h_actor -> h_imi_actor [label="implement depend",style="dotted"];
    sup_h_rs232 -> h_imi_rs232 [label="implement depend",style="dotted"];
    sup_h_sens -> h_imi_sensor [label="implement depend",style="dotted"];
}
\enddot
-->


\subsection SupplHalPm167 HAL for Supplementary Drivers on Pm167 of STW
<img src="Images/HAL_suppl_pm167.png" title="HAL for Supplementary Drivers on PM167" alt="HAL for Supplementary Drivers on PM167"><br>
More information can be found in the general HAL header files (see links in first subsection) and in the device specific headers:
- \ref pm167/rs232/rs232.h "pm167/rs232/rs232.h"

<!--
\dot
digraph SupplHalPm167 
{
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=8];

    sup_hal     [label="HAL for Supplementary\nExtended Drivers\noptional", color="green"];

    sup_hal_pm167 [label="HAL for Supplementary\n Drivers on PM167 of STW\n(optional)", color="green"];
    sup_h_actor [label="Central HAL Header\nfor Supplementary Actor Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/actor.h"];
    sup_h_rs232 [label="Central HAL Header\nfor Supplementary RS232 Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/rs232.h"];
    sup_h_sens  [label="Central HAL Header\nfor Supplementary Sensor Driver\n(optional)", color="green", shape="ellipse", URL="\ref hal/sensor.h"];

    h_pm167_rs232 [label="Target Adoption\nof RS232 IO\n(optional)", color="green", URL="\ref pm167/rs232/rs232.h"];

    sup_hal_pm167 -> h_pm167_rs232 [label="rs232"];

    sup_h_rs232 -> h_pm167_rs232 [label="implement depend",style="dotted"];
}
\enddot
-->


\subsection SupplHalPc HAL for Supplementary Drivers on PC
<img src="Images/HAL_suppl_pc.png" title="HAL for Supplementary Drivers on PC" alt="HAL for Supplementary Drivers on PC"><br>
More information can be found in the general HAL header files (see links in first subsection) and in the device specific headers:
- \ref pc/actor/actor.h "pc/actor/actor.h"
- \ref pc/rs232/rs232.h "pc/rs232/rs232.h"
- \ref pc/sensor/sensor.h "pc/sensor/sensor.h"

<!--
\dot
digraph SupplHalPc
{
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [/*shape=record, */fontname=ARIALN, fontsize=8, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=8];

    sup_hal     [label="HAL for Supplementary\nExtended Drivers\noptional", color="green"];

    sup_hal_pc  [label="HAL for Supplementary\nDrivers on PC\noptional", color="green"];
    sup_h_actor [label="Central HAL Header\nfor Supplementary Actor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/actor.h"];
    sup_h_rs232 [label="Central HAL Header\nfor Supplementary RS232 Driver\noptional", color="green", shape="ellipse", URL="\ref hal/rs232.h"];
    sup_h_sens  [label="Central HAL Header\nfor Supplementary Sensor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/sensor.h"];

    h_pc_actor [label="Target Adoption\nof Digital/PWM Output\noptional", color="green", URL="\ref pc/actor/actor.h"];
    h_pc_rs232 [label="Target Adoption\nof RS232 IO\noptional", color="green", URL="\ref pc/rs232/rs232.h"];
    h_pc_sensor [label="Target Adoption\nof Sensor Input\noptional", color="green", URL="\ref pc/sensor/sensor.h"];

    sup_hal_pc -> h_pc_actor [label="actor"];
    sup_hal_pc -> h_pc_rs232 [label="rs232"];
    sup_hal_pc -> h_pc_sensor [label="sensor"];

    sup_h_actor -> h_pc_actor  [label="implement depend",style="dotted"];
    sup_h_rs232 -> h_pc_rs232  [label="implement depend",style="dotted"];
    sup_h_sens -> h_pc_sensor  [label="implement depend",style="dotted"];
}
\enddot
-->
*/
