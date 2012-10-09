/** @page SuppHalPage Structure of HAL for Supplementary Parts of IsoAgLib
	The directory structure of the ISO<i><sub>AgLib</sub></i> HAL
	is designed to ease quick adotion to new platform types.
	Please see \ref MainHalAdoptionNew for details.

	\section SuppHalStructure Structure of HAL for Main Parts
	\subsection SupplHalEsx HAL for Supplementary Drivers on ESX of STW
  \dot
  digraph SupplHalEsx {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [shape=record, fontname=ARIALN, fontsize=10, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=10];

		sup_hal     [label="HAL for Supplementary Extended Drivers\noptional", color="green"];

		sup_hal_esx [label="HAL for Supplementary Drivers on ESX of STW\noptional", color="green"];
		sup_h_actor [label="Central HAL Header for Supplementary Actor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/actor.h"];
		sup_h_rs232 [label="Central HAL Header for Supplementary RS232 Driver\noptional", color="green", shape="ellipse", URL="\ref hal/rs232.h"];
		sup_h_sens  [label="Central HAL Header for Supplementary Sensor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/sensor.h"];

		h_esx_actor [label="Target Adpotion of Digital/PWM Output\noptional", color="green", URL="\ref esx/actor/actor.h"];
		h_esx_rs232 [label="Target Adpotion of RS232 IO\noptional", color="green", URL="\ref esx/rs232/rs232.h"];
		h_esx_sensor [label="Target Adpotion of Sensor Input\noptional", color="green", URL="\ref esx/sensor/sensor.h"];

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

	\subsection SupplHalImi HAL for Supplementary Drivers on IMI of STW
  \dot
  digraph SupplHalImi {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [shape=record, fontname=ARIALN, fontsize=10, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=10];

		sup_hal     [label="HAL for Supplementary Extended Drivers\noptional", color="green"];

		sup_hal_imi [label="HAL for Supplementary Drivers on IMI of STW\noptional", color="green"];
		sup_h_actor [label="Central HAL Header for Supplementary Actor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/actor.h"];
		sup_h_rs232 [label="Central HAL Header for Supplementary RS232 Driver\noptional", color="green", shape="ellipse", URL="\ref hal/rs232.h"];
		sup_h_sens  [label="Central HAL Header for Supplementary Sensor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/sensor.h"];

		h_imi_actor [label="Target Adpotion of Digital/PWM Output\noptional", color="green", URL="\ref imi/actor/actor.h"];
		h_imi_rs232 [label="Target Adpotion of RS232 IO\noptional", color="green", URL="\ref imi/rs232/rs232.h"];
		h_imi_sensor [label="Target Adpotion of Sensor Input\noptional", color="green", URL="\ref imi/sensor/sensor.h"];

		sup_hal_imi -> h_imi_actor [label="actor"];
		sup_hal_imi -> h_imi_rs232 [label="rs232"];
		sup_hal_imi -> h_imi_sensor [label="sensor"];

		sup_h_actor -> h_imi_actor [label="implement depend",style="dotted"];
		sup_h_rs232 -> h_imi_rs232 [label="implement depend",style="dotted"];
		sup_h_sens -> h_imi_sensor [label="implement depend",style="dotted"];
	}
	\enddot

	\subsection SupplHalPm167 HAL for Supplementary Drivers on Pm167 of STW
  \dot
  digraph SupplHalPm167 {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [shape=record, fontname=ARIALN, fontsize=10, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=10];

		sup_hal     [label="HAL for Supplementary Extended Drivers\noptional", color="green"];

		sup_hal_pm167 [label="HAL for Supplementary Drivers on PM167 of STW\noptional", color="green"];
		sup_h_actor [label="Central HAL Header for Supplementary Actor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/actor.h"];
		sup_h_rs232 [label="Central HAL Header for Supplementary RS232 Driver\noptional", color="green", shape="ellipse", URL="\ref hal/rs232.h"];
		sup_h_sens  [label="Central HAL Header for Supplementary Sensor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/sensor.h"];

		h_pm167_rs232 [label="Target Adpotion of RS232 IO\noptional", color="green", URL="\ref pm167/rs232/rs232.h"];

		sup_hal_pm167 -> h_pm167_rs232 [label="rs232"];

		sup_h_rs232 -> h_pm167_rs232 [label="implement depend",style="dotted"];
	}
	\enddot

	\subsection SupplHalPc HAL for Supplementary Drivers on PC
  \dot
  digraph SupplHalPc {
    fontname=ARIALN;
    fontpath="/usr/X11R6/lib/X11/fonts/drakfont/ttf:/usr/share/fonts/ttf:/usr/X11R6/lib/X11/fonts/TTF";
    node     [shape=record, fontname=ARIALN, fontsize=10, fontcolor=blue, style=filled];
    edge     [fontname=ARIALN, fontsize=10];

		sup_hal     [label="HAL for Supplementary Extended Drivers\noptional", color="green"];

		sup_hal_pc  [label="HAL for Supplementary Drivers on PC\noptional", color="green"];
		sup_h_actor [label="Central HAL Header for Supplementary Actor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/actor.h"];
		sup_h_rs232 [label="Central HAL Header for Supplementary RS232 Driver\noptional", color="green", shape="ellipse", URL="\ref hal/rs232.h"];
		sup_h_sens  [label="Central HAL Header for Supplementary Sensor Driver\noptional", color="green", shape="ellipse", URL="\ref hal/sensor.h"];

		h_pc_actor [label="Target Adpotion of Digital/PWM Output\noptional", color="green", URL="\ref pc/actor/actor.h"];
		h_pc_rs232 [label="Target Adpotion of RS232 IO\noptional", color="green", URL="\ref pc/rs232/rs232.h"];
		h_pc_sensor [label="Target Adpotion of Sensor Input\noptional", color="green", URL="\ref pc/sensor/sensor.h"];

		sup_hal_pc -> h_pc_actor [label="actor"];
		sup_hal_pc -> h_pc_rs232 [label="rs232"];
		sup_hal_pc -> h_pc_sensor [label="sensor"];

		sup_h_actor -> h_pc_actor  [label="implement depend",style="dotted"];
		sup_h_rs232 -> h_pc_rs232  [label="implement depend",style="dotted"];
		sup_h_sens -> h_pc_sensor  [label="implement depend",style="dotted"];
	}
	\enddot
	*/
