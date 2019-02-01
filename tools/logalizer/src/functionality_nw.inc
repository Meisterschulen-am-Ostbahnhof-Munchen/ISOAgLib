
#include <iostream>
#include <vector>
#include <map>

#define BITS_PER_BYTE 8
#define LENGTH_OUTPUT_STRING 20   //19 characters + space

//Table updated on 2017-11-30
const char* manufacturerCodeArray[] = {
  "Reserved",
  "Bendix Commercial Vehicle Systems LLC (formerly Allied Signal Inc.) Elyria, OH USA",
  "Allison Transmission, GMC  Indianapolis, IN USA",
  "Ametek, US Gauge Division  Sellersville, PA USA",
  "Ametek-Dixson  Grand Junction, CO USA",
  "AMP Inc.  Harrisburg, PA USA",
  "Berifors Electronics AB  Stockholm, Sweden",
  "Case Corp.  Burr Ridge, IL USA",
  "Caterpillar Inc.  Peoria, IL USA",
  "Chrysler Corp.  Auburn Hills, MI USA",
  "Cummins Engine Co  Columbus, IN USA",
  "Dearborn Group Inc.  Indianapolis, IN & Farmington Hills, MI USA",
  "Deere & Company, Precision Farming  East Moline, IL USA",
  "Delco Electronics  Kokomo, IN USA",
  "Detroit Diesel Corporation  Detroit, MI USA",
  "Dickey-john Corp.  Auburn, IL USA",
  "Eaton Corp  Southfield, MI USA",
  "Eaton Corp, Corp Res & Dev  Milwaukee, WI USA",
  "Eaton Corp, Transmission Div.  Kalamazoo, MI USA",
  "Eaton Corp. Trucking Info Services  Clemmons, NC USA",
  "Eaton Ltd  Worsley, England",
  "Echlin Inc., Midland Brake Inc.  Kansas City, MO USA",
  "Ford Motor Co., Electronic Concepts & Systems  Dearborn, MI USA",
  "Ford Motor Co., Heavy Truck  Dearborn, MI USA",
  "Ford Motor Co., Vehicle Controls  Dearborn, MI USA",
  "Daimler Trucks North America LLC (formerly Freightliner LLC)  Portland, OR USA",
  "General Motors Corp, Service Technology Grp  Romulus, MI USA",
  "GMC  Troy, MI USA",
  "Grote Ind. Inc.  Madison, IN USA",
  "Hino Motors Ltd.  Tokyo, Japan",
  "Isuzu Motors Ltd  Kawasaki, Japan",
  "J Pollak Corp  Boston, MA USA",
  "Jacobs Vehicle Systems  Bloomfield, CT USA",
  "John Deere  Waterloo, IA USA",
  "Kelsey Hayes Co.  Livonia, MI USA",
  "Kenworth Truck Co.  Kirkland, WA USA",
  "Lucas Ind.  Solihull West Midland, England",
  "Mack Trucks Inc.  Hagerstown, MD USA",
  "Micro Processor Systems Inc.  Sterling Hts, MI USA",
  "Microfirm Inc.  Stillwater, OK USA",
  "Motorola AIEG Inc.  Northbrook, IL USA",
  "Motorola Inc.  Schaumburg, IL USA",
  "International Truck and Engine Corporation – Engine Electronics Warrenville, IL USA",
  "International Truck and Engine Corporation – Vehicle Electronics Warrenville, IL USA",
  "Nippondenso Co. Ltd.  Kariya Aichi, Japan",
  "PACCAR  Mount Vernon, WA USA",
  "Noregon Systems, Inc Winston Salem, NC USA",
  "Phillips Semiconductor  Sunnyvale, CA USA",
  "Pollak Alphabet  El Paso, TX USA",
  "RE America Inc.  Cleveland, OH USA",
  "Robert Bosch Corp  Broadview, IL USA",
  "Robert Bosch GmbH  Stuttgart, Germany",
  "Meritor Automotive, Inc. Troy, MI USA",
  "Continental Automotive Systems US, Inc. Cedar Rapids, IA USA",
  "Meritor Wabco  Troy, MI USA",
  "Ryder System Inc.  Miami, FL USA",
  "SAIC  San Diego, CA USA",
  "Danfoss Minneapolis, MN USA",
  "SPX Corporation, OTC Div  Owatonna, MN USA",
  "VES Inc.  Rock Hill, SC USA",
  "Volvo Trucks North America Inc.  Greensboro, NC USA",
  "Volvo Truck Corp.  Gothenburg, Sweden",
  "Wabco  Hanover, Germany",
  "ZF Industries Inc.  Vernon Hills, IL USA",
  "unused (formerly SpectraPrecision Laserplane, then SpectraPhysics)",
  "MAN Nutzfahrzeuge AG  Munich, Germany",
  "John Deere Construction Equipment Division  Dubuque, IA USA",
  "John Deere Coffeyville Works Coffeyville, KS USA",
  "Scania  Södertälje, Sweden",
  "Trimble Navigation",
  "Flex-coil Limited  Saskatoon, SK Canada",
  "Vansco Electronics Ltd.  Winnipeg, AB Canada",
  "Sisu Corporation  ESPOO, Finland",
  "LeTourneau, Inc.  Longview, TX USA",
  "Eaton Axle-Brake Division  Kalamazoo, MI USA",
  "Deere & Co, Agricultural Division",
  "Unused (formerly Deere & Co, Construction Division)",
  "Deere Power Systems Group",
  "Frank W. Murphy Manufacturing, Inc  Tulsa, OK USA",
  "Daimler Benz AG - Engine Division (PBM)  Stuttgart, Germany",
  "Twin Disc, Inc.  Racine, WI USA",
  "Fire Research Corp. Nesconset, NY USA",
  "Bobcat/Ingersoll-Rand (formerly Melroe/Ingersoll-Rand)  Fargo, ND USA",
  "Bendix VORAD Technologies (formerly Eaton VORAD Technologies) San Diego, CA USA",
  "New Holland UK Limited  Basildon, Essex, UK",
  "Kohler Co  Kohler, WI USA",
  "C. E. Niehoff & Company  Evanston, IL USA",
  "J.C. Bamford Excavators Ltd (JCB)  Rocester, Staffordshire, UK",
  "Satloc Precision GPS  Scottsdale, AZ USA",
  "Kverneland Group, Electronics Division  Nieuw-Vennep, Netherlands",
  "Knorr-Bremse SfN GmbH  Munich, Germany",
  "BSG Bodensee Steuergeraete GmbH  Immenstaad, Germany",
  "Ag-Chem Equipment Co., Inc.  Minnetonka, MN USA",
  "Perkins Engines Company Ltd.  Peterborough, UK",
  "CNH Global N.V.  Racine, WI USA",
  "Pacific Insight Electronics Corp.  Nelson, BC Canada",
  "Mech@tronic IT GmbH  Hetzenhausen, Germany",
  "Ag Leader Technology, Inc.  Ames, IA USA",
  "Mueller-Elektronik GmbH & Co  Salzkotten, Germany",
  "International Transmissions Ltd (ITL)  Wrexham, North Wales, UK",
  "VDO Technik AG  Rüthi, Switzerland",
  "Sensoria  San Diego, CA USA",
  "AGCO GmbH & Co.  Marktoberdorf, Germany",
  "Agrocom GmbH & Co. Agrarsystem KG  Bielefeld, Germany",
  "Claas Selbstfahrende Erntemaschinen GmbH  Harsewinkel, Germany",
  "Kiepe Elektrik GmbH & Co. KG  Duesseldorf, Germany",
  "BAE Systems Controls, Inc.  Johnson City, NY USA",
  "Grimme Landmaschinen GmbH & Co. KG  Damme, Germany",
  "WTK-Elektronik GmbH  Neustadt, Germany",
  "TeeJet Technologies Denmark (formerly LH Technologies ApS ) Aabybro, Denmark",
  "EPIQ Sensor-Nite  Fenton, MI USA",
  "Maschinenfabrik Bernhard Krone GmbH  Spelle, Germany",
  "MECALAC  Annecy le Vieux, France",
  "Stress-Tek, Inc.  Kent, WA USA",
  "EControls, Inc.  San Antonio, TX USA",
  "NACCO Materials Handling Group, Inc.  Portland, OR USA",
  "BEELINE Technologies  Brisbane, QLD Australia",
  "HUSCO International  Waukesha, WI USA",
  "Intron GmbH S chwaebisch Hall, Germany",
  "IntegriNautics  Menlo Park, CA USA",
  "RDS Technology Ltd  Minchinhampton, Stroud, UK",
  "HED (Hydro Electronic Devices, Inc.)  Hartford, WI USA",
  "FG Wilson (Engineering) Limited  Larne, County Antrim, UK",
  "Basler Electric  Highland, IL USA",
  "Hydac Electronic  Saarbruecken, Germany",
  "Nevada Automotive Test Center  Carson City, NV USA",
  "Driver Tech  Salt Lake City, UT USA",
  "Holland USA  Holland, MI USA",
  "Gerhard Duecker GmbH & Co. KG  Stadtlohn, Germany",
  "OMNEX Control Systems Inc.  Port Coquitlam, BC, Canada",
  "Nido - Universal Machines B.V.  Holten, Netherlands",
  "ITT Industries  Eden Prarie, MN USA",
  "Mulag-Fahrzeugwerk  Oppenau, Germany",
  "Bucher Schoerling GmbH  Hannover, Germany",
  "Iris Technology Ltd  Lancaster, Lancs, UK",
  "Airmar Technology Corporation  Milford, NH USA",
  "Komatsu Ltd Hiratsuka,  Kanagawa, Japan",
  "Maretron  Phoenix AZ USA",
  "Georg Fritzmeier GmbH & Co. KG  Grosshelfendorf, Germany",
  "Caterpillar Trimble Control Technologies (CTCT), LLC  Dayton, OH USA",
  "Lowrance Electronics, Inc.  Tulsa, OK USA",
  "Thales Navigation Ltd.  Surrey, UK",
  "TRW Automotive (Commercial Steering Systems)  Lafayette, IN USA",
  "W. Gmeiner GmbH & Co.  Kummersbruck, Germany",
  "Mercury Marine  Fond du Lac, WI USA",
  "MurCal Controls  Palmdale, CA USA",
  "Maxima Technologies  Lancaster, PA USA",
  "Nautibus electronic GmbH  Quern, Germany",
  "Blue Water Data, Inc.  Salem, NJ USA",
  "Holset  Charleston, SC USA",
  "Fleetguard  Nashville, TN USA",
  "Raven Industries Inc. - Flow Controls Division  Sioux Falls, SD USA",
  "elobau GmbH & Co. KG Leutkirch, Germany",
  "Woodward, Industrial Controls Division  Fort Collins, CO USA",
  "Westerbeke Corporation  Taunton, MA USA",
  "Vetronix Corporation  Santa Barbara, CA",
  "ITT Industries - Cannon  Shakopee, MN USA",
  "ISSPRO Inc.  Portland, OR USA",
  "Firestone Industrial Products Company  Carmel, IN USA",
  "NTech Industries Inc  Ukiah, CA USA",
  "Nido Nijbroek, The Netherlands",
  "Offshore Systems (UK) Ltd  New Milton, Hampshire, UK",
  "Axiomatic Technologies  Mississauga, ON Canada",
  "BRP Inc.  Waukegan, IL USA",
  "MTU Friedrichshafen GmbH Friedrichshafen, Germany",
  "CPAC Systems AB  Vastra Frolunda, Sweden",
  "Phoenix International  Fargo, ND USA",
  "JLG Industries Inc  McConnellsburg, PA USA",
  "Xantrex  Burnaby, BC Canada",
  "Marlin Technologies Inc.  Horicon, WI USA",
  "Computronics Corporation Ltd.  Bentley, WA Australia",
  "Wachendorff Elektronik GmbH Geisenheim, Germany",
  "Yanmar Marine USA Adairsville, GA USA",
  "Ryeso, Inc. Palmdale, CA USA",
  "AB Volvo Penta  Goteburg, Sweden",
  "Veris Technologies, Inc. Salina, KS USA",
  "Moritz Aerospace  Dublin, PA USA",
  "Diagnostic Systems Associates  Kalamazoo, MI USA",
  "Siemens VDO Automotive AG Regensburg, Germany",
  "TeeJet Technologies Springfield, IL USA",
  "Smart Power Systems Reed City, MI USA",
  "Coretronics, Inc. Eagle, ID USA",
  "Vehicle Systems Engineering B.V.  Veenendaal, Netherlands",
  "KDS Controls, Inc  Troy, MI USA",
  "EIA Electronics Aartselaar, Belguim",
  "Beede Electrical Instrument Company Penacock, NH USA",
  "Altronic, Inc  Girard, OH USA",
  "Air-Weigh  Eugene, OR USA",
  "EMP Corp  Escanaba, MI USA",
  "QUALCOMM  San Diego, CA USA",
  "Hella KGaA Hueck & Co  Lippstadt, Germany",
  "XRS Corporation (formerly XATA Corporation) Eden Prairie, MN USA",
  "Floscan  Seattle, WA USA",
  "Jeppesen Marine  Portland, OR USA",
  "TriMark Corporation  New Hampton, IA USA",
  "General Engine Products  Livonia, MI USA",
  "LEMKEN GmbH & Co KG  Alpen, Germany",
  "Mechron Power Systems  Ottawa, ON Canada",
  "Mystic Valley Communications  Mystic, CT USA",
  "Actia Corp  Elkhart, IN USA",
  "MGM Brakes  Charlotte, NC USA",
  "Disenos y Tecnologia S.A. Barcelona, Spain",
  "Curtis Instruments, Inc  Mount Kisco, NY USA",
  "MILtronik GmbH  Hilden, Germany",
  "The Morey Corporation  Woodridge, IL USA",
  "SmarTire Systems Inc  Richmond, BC Canada",
  "port GmbH  Halle, Germany",
  "Otto Engineering  Carpentersville, IL USA",
  "Drew Technologies, Inc  Whitmore Lake, MI USA",
  "Bell Equip. Co. SA (PTY) LTD  Richards Bay, South Africa",
  "Iteris, Inc.  Anaheim, CA USA",
  "DNA Group  Raleigh, NC USA",
  "Sure Power Industries, Inc  Tualatin, OR USA",
  "CNH Belgium N.V.  Zedelgem, Belgium",
  "MC elettronica Srl Fiesso Umbertiano, Rovigo, Italy",
  "Aetna Engineering/Fireboy-Xintex Grand Rapids, MI USA",
  "Paneltronics Inc.  Hialeah Gardens, FL USA",
  "RM Michaelides Software & Elektronik GmbH Fulda, Germany",
  "Gits Manufacturing Company  Creston, IA USA",
  "Cat OEM Solutions  Mossville, IL USA",
  "Beede Electrical Instrument Company, Inc Penacook, NH USA",
  "SiE  Kempten/Allgaeu, Germany",
  "Generac Power Systems, Inc.  Waukesha, WI, USA",
  "Vaueo Retarder Co., Ltd.  Shajin, Baoan, Shen Zhen, China",
  "EMMI Network, S.L.  Gandia, Spain",
  "SKF  Gothenburg, Sweden",
  "Monaco Coach Corporation  Bend, OR USA",
  "Lykketronic A/S  Loegstoer, Denmark",
  "ZF Marine Electronics  Mukilteo, WA USA",
  "Garmin International Inc.  Olathe, KS USA",
  "Saucon Technologies  Bethlehem, PA USA",
  "Topcon Positioning Systems, Inc.  Livermore, CA USA",
  "TSD Integrated Controls  Minneapolis, MN USA",
  "Yacht Monitoring Solutions Inc.  Corpus Christi, TX USA",
  "Mondial electronic GmbH  Eggelsberg, Austria",
  "SailorMade Marine Telemetry - Tetra Technology Ltd. Florianopolis, Brazil",
  "Norac Systems International Inc  Saskatoon, SK, Canada",
  "Agtron Enterprises Inc  Saskatoon, SK, Canada",
  "ZF Friedrichshafen AG  Friedrichshafen, Germany",
  "May & Scofield Ltd  Basingstoke, Hants, UK",
  "Vanair Mfg  New Buffalo, MI USA",
  "Schneider Automation SAS Sophia Antipolis, France",
  "Kokusandenki Co. Ltd  Numazu, Shizuoka, Japan",
  "eRide, Inc.  San Francisco, CA USA",
  "Techno-Matic Aars, Denmark",
  "Capstan Ag Systems, Inc. Topeka, KS USA",
  "Class , Inc  Ocala, FL USA",
  "ePULSE  Troy, MI USA",
  "Cooper Standard Automotive Active Systems Group  Mitchell, ON Canada",
  "Schaltbau GmbH  Munich, Germany",
  "Kuhn Group  Saverne, France",
  "German Agricultural Society, Test Center (DLG)  Gross-Ultstadt, Germany",
  "Sensor-Technik Wiedemann GmbH  Kaufbeuren, Germany",
  "Mobile Control Systems  Brussels, Belgium",
  "GE Sensing  St. Marys, PA USA",
  "MEAS France  Toulouse, France",
  "Tyco Electronics AMP  Osan-Si, Kyunggi-Do, Korea",
  "Honda Motor Co., Ltd  Wako-city, Saitama, Japan",
  "ARAG  Rubiera, Italy",
  "Jetter AG Ludwigsburg, Germany",
  "Reichhardt GmbH Steuerungstechnik  Hungen, Germany",
  "Red Dot Corporation  Seattle, WA USA",
  "HydraForce, Inc  Lincolnshire, IL USA",
  "IMMI  Westfield, IN USA",
  "Autolync  Clayton, Victoria, Australia",
  "MTS Sensor Technologie GmbH  Luedenscheid, Germany",
  "International Thermal Research Ltd  Richmond, BC Canada",
  "Red Lion Controls, Inc  York, PA USA",
  "Accurate Technologies  Wixom, MI USA",
  "Saft America Inc., Space & Defense Division  Cockeysville, MD USA",
  "Tennant  Minneapolis, MN USA",
  "Cole Hersee  Boston, MA USA",
  "Gross Mechanical Laboratories, Inc Hanover, MD USA",
  "Active Research Limited Poole, Dorset, UK",
  "LTW Technology Co., LTD  Shinjuang City, Taipei Hsien, Taiwan, R.O.C.",
  "Navico Egersund AS  Egersund, Norway",
  "Aqua-Hot Heating Systems  Fort Lupton, CO USA",
  "LHP Telematics  Columbus, IN USA",
  "Takata Electronics  Farmington Hills, MI USA",
  "Geometris LP  Houston, TX USA",
  "Leica Geosystems Pty Ltd  Brisbane, Queensland Australia",
  "Precision Governors LLC  Rockford, IL USA",
  "Medallion Instrumentation Systems Spring Lake, MI  USA",
  "CWF Hamilton & Co Ltd Christchurch, New Zealand",
  "Mobile Tech Chile Concepcion, Bio Bio, Chile",
  "Sea Recovery Corp Rancho Dominguez, CA   USA",
  "Coelmo srl Acerra, Italy",
  "NTech Industries, Inc Ukiah, CA   USA",
  "Mitsubishi FUSO Truck & Bus Corp. Kawasaki, Kanagawa, Japan",
  "Watlow St. Louis, MO   USA",
  "Kuebler GmbH VS-Schwenningen, Germany",
  "Groeneveld Transport Efficiency BV Gorinchem, Netherlands",
  "IKUSI - Angel Iglesias S.A. San Sebastian, Guipuzcoa, Spain",
  "Spyder Controls Corp. Lacombe, AB   Canada",
  "Grayhill Inc. LaGrange, IL   USA",
  "BEP Marine Albany, North Shore City, New Zealand",
  "micro dynamics GmbH Cologne, Germany",
  "Zonar Systems Inc Seattle, WA   USA",
  "Holley Performance Bowling Green, KY   USA",
  "Rauch Landmaschinen GmbH Sinzheim, Germany",
  "Systron Donner Automotive Concord, CA   USA",
  "Parker Hannifin (UK) Ltd FDE Group Thetford, Norfolk, UK",
  "Nissin Kogyo Co., LTD Ueda-city, Nagano prefecture, Japan",
  "CTS Corporation Elkhart, IN   USA",
  "EmpirBus AB Uddevalla, Sweden",
  "NovAtel Inc Calgary, AB   Canada",
  "Sleipner Motor AB Fredrikstad, Norway",
  "MAS Technologies Harleysville, PA  USA",
  "Cyntrx De Pere, WI   USA",
  "Krauss-Maffei Wegmann GmbH & Co. KG Munich, Germany",
  "TECNORD srl Modena, Italy",
  "Patrick Power Products Elkridge, MD   USA",
  "Lectronix Inc.  Lansing MI USA",
  "Ilmor Engineering Inc  Plymouth, MI USA",
  "CSM GmbH  Filderstadt, Germany",
  "Icom Incorporated  Osaka, Japan",
  "ITT Flow Control  Santa Ana, CA USA",
  "Navtronics Bvba  Geel, Belgium",
  "SAT Plan  Beauvais Tillé, Oise, France",
  "Cadec Global  Manchester, NH USA ",
  "Miedema Landbouwwerktuigenfabriek B.V.  Winsum, Friesland, Netherlands ",
  "Ultra Electronics, Electrics  Cheltenham, UK",
  "MICHENKA s.r.o.  Koprivnice, Czech Republic ",
  "Mobileye Vision Technologies Ltd.  Jerusalem, Israel",
  "Snap-on Diagnostics  Lincolnshire, IL USA",
  "ASM Automation Sensorik Messtechnik GmbH  Moosinning, Germany ",
  "Akron Brass Company  Wooster, OH USA",
  "Sonceboz SA  Sonceboz, Switzerland ",
  "Qwerty Electronik AB Bagarmossen, Sweden",
  "Deif A/S Skive, Denmark",
  "Kidde Aerospace and Defense Wilson, NC USA",
  "Horton, Inc. Roseville, MN USA",
  "HWH Corporation Moscow, IA USA",
  "Hadley Products Corporation Grandville, MI USA",
  "Takata-Petri AG Aschaffenburg, Germany",
  "Evo Electric Ltd Woking, Surrey, UK",
  "APE s.r.o. Bradlec, Czech Republic",
  "Carraro S.p.A. Campodarsego, Italy",
  "GRAF-SYTECO Tuningen, Germany",
  "Competence Center ISOBUS e.V.  Wallenhorst, Germany",
  "Continental AG  Hannover, Germany",
  "Boning GmbH & Co Ganderkesee, Germany",
  "Thomas Magnete GmbH Herdorf, Germany",
  "Baumer Group Villingen-Schwenningen, Germany",
  "Parvus Corporation Salt Lake City, UT USA",
  "Korean Maritime University  Busan, Korea",
  "Control Solutions Aurora, IL USA",
  "Honeywell Morristown, NJ USA",
  "Amazonen Werke H. Dreyer Hasbergen, Germany",
  "Suonentieto Kuopio, Finland",
  "Noris Marine Systems GmbH&Co KG Nuremberg, Germany",
  "Thrane & Thrane Gentofte, Denmark",
  "Same Deutz-Fahr Treviglio, Italy",
  "Hegemon Electronics, Inc. Sterling Heights, MI USA",
  "Junkkari OY Yliharma, Finland",
  "Mastervolt International B.V. Amsterdam, Netherlands",
  "Fischer Panda Generators, Inc Oakland Park, FL USA",
  "Hardi International A/S Taastrup, Denmark",
  "Victron Energy BV Almere-Haven, Netherlands",
  "Ludwig Bergmann GmbH Goldenstedt, Germany",
  "HJS Emission Technology GmbH & Co KG Menden, Germany",
  "InMach Ulm, Germany",
  "Alois Poettinger Maschinenfabrik GmbH Grieskirchen, Austria",
  "BEI Duncan Goleta, CA USA",
  "OEM Controls Inc. Shelton, CT USA",
  "Digi-Star, LLC Fort Atkinson, WI USA",
  "Viewnyx Corp. ata, ON Canada",
  "Fliegl Agrartechnik Muehldorf am Inn, Germany",
  "HANSENHOF_electronic Reifland, Germany",
  "Power Torque Engineering Ltd Coventry, UK",
  "Rolls-Royce Marine AS Aalesund, Norway",
  "Heinzmann GmbH & Co. KG Schoenau, Germany",
  "Delphi Auburn Hills, MI USA",
  "Electronic Design Inc. Sheboygan, WI USA",
  "Northern Lights, Inc. Seattle, WA USA",
  "Williams Controls Inc. Portland, OR USA",
  "Quake Global San Diego, CA USA",
  "ifm electronic gmbh Essen, Germany",
  "Glendinning Marine Products Conway, SC USA",
  "Yamabiko Corporation Iwate, Japan",
  "Suntech International Ltd. Seoul, Korea",
  "B & G Romsey, UK",
  "National Agriculture Research Organization Ibaraki, Japan",
  "MCL Industries Pulaski, WI USA",
  "Camano Light Camano Island, WA USA",
  "Johnson Outdoor Marine Electronics Alpharetta, GA USA",
  "JLG Automation BVBA Gent, Belgium",
  "Orscheln Products, LLC Moberly, MO USA",
  "Innomatix, LLC Iowa City, IA USA",
  "Benchmark Electronics, Minnesota Division Rochester, MN USA",
  "Partech Inc. Lake Orion, MI USA",
  "Electronic Design for Industry, Inc Belpre, OH USA",
  "Tianyuan Technology Co Ltd. Shijiazhuang, China",
  "Harvest Tec, Inc Hudson, WI USA",
  "Capi  Nederland BV Nuenen, Netherlands",
  "GENTEC S.R.L. Villa Constitucion, Argentina",
  "Beyond Measure Appleton, WI  USA",
  "Sanyo-kiki Co Ltd. Okayama, Japan",
  "Hilite International Marktheidenfeld, Germany",
  "ISEKI & Co., Ltd. Ehime, Japan",
  "Livorsi Marine Grayslake, IL  USA",
  "Torqeedo GmbH Stamberg, Germany",
  "Simma Software, Inc. Terre Haute, IN   USA",
  "Trackwell ADS, Inc. Houston, TX  USA",
  "Com Nav Marine Ltd Richmond, BC  Canada",
  "Wema System AS Bergen, Norway",
  "Vecima Networks Inc. Saskatoon, SK  Canada",
  "Comtech Mobile Datacom Germantown, MD  USA",
  "Corvus Energy Ltd. Richmond, BC Canada",
  "Transfluid S.r.L. Gallarate, Italy",
  "COBO S.p.A. Divisione B Castelletto sopra Ticino, Italy",
  "Hy-Drive Technologies Ltd. Mississauga, ON  Canada",
  "WebTech Wireless Inc. Burnaby, BC  Canada",
  "Datapross Nijbroek bv Nijbroek, Netherlands",
  "Cattron Group International Sharpsvillle, PA USA",
  "Valid Manufacturing Ltd. Salmon Arm, BC Canada",
  "Kubota Corporation Sakai, Japan",
  "KVZalve Greenwood, NE  USA",
  "Intellistick Inc Santa Ana, CA USA",
  "Fusion Electronics Ltd Auckland, New Zealand",
  "Vermeer Corporation, ACS Group Pella, IA USA",
  "Vertex Standard Co, Ltd  Tokyo, Japan",
  "True Heading AB  Danderyd, Sweden",
  "BSM Wireless, Inc.  Toronto, ON Canada",
  "Odyne LLC  Waukesha, WI USA",
  "Methode Electronics Inc, MDI  Chicago, IL USA",
  "Not Assigned",
  "Not Assigned",
  "Not Assigned",
  "Rota Engineering Ltd.  Bury, Manchester, UK",
  "Auteq Telematica S.A.  Sao Paulo, Brazil",
  "Tohatsu Corporation  Tokyo, Japan",
  "S&A Systems, Inc.  Rockwall, TX USA",
  "Rowe Electronics  Norwalk, IA USA",
  "Stored Energy Systems  Longmont, CO USA",
  "Zunhammer GmbH Traunreut, Germany",
  "Kinze Manufacturing  Williamsburg, IA",
  "Digital Yacht Limited  Bristol, UK",
  "Comar Systems Ltd  Cowes, Isle of Wight, UK",
  "Hyundai Heavy Industries  Ulsan, South Korea",
  "Cummins Power Generation  Fridley, MN USA",
  "PTG Reifendruckregelsysteme GmbH Neuss, Germany",
  "Horsch Maschinen GmbH Schwandorf, Germany",
  "SignalQuest, Inc. Lebanon, NH   USA",
  "ITT Power Solutions West Springfield, MA   USA",
  "KAT MECHATRONIC, Electronic Product Division Istanbul, Turkey",
  "CertTech, L.L.C. Lenexa, KS  USA",
  "Great Plains Mfg. Salina, KS   USA",
  "Stanadyne Corporation, Electronics Systems Windsor, CT   USA",
  "Polaris Industries Inc Medina, MN   USA",
  "Dycor Technologies Ltd. Edmonton, AB   Canada",
  "Parker Hannifin Corp Gardena, CA   USA",
  "WIKA Alexander Wiegand SE & Co. KG Klingenberg, Germany",
  "Cooper Bussmann Tualatin, OR   USA",
  "NGK Spark Plug Co. Ltd Komaki, Japan",
  "ADZ NAGANO GmbH Ottendorf-Okrilla, Germany",
  "General Kinetics Manchester, NH   USA",
  "RUSELPROM-ElectricDrive Ltd Moscow, Russian Federation",
  "Control Solutions, Inc. St. Paul, MN   USA",
  "Alltek Marine Electronics Corp Neihu, Taiwan",
  "San Giorgio S.E.I.N. Genova, Italy",
  "HAWE Hydraulik SE Munich, Germany",
  "IHI Shibaura Machinery Corporation Matsumoto, Japan",
  "PROBOTIQ Cuijk, Netherlands",
  "Leach International Corporation Buena Park, CA   USA",
  "Ashcroft Inc Stratford, CT   USA",
  "Veethree Electronics and Marine LLC Sarasota, FL   USA",
  "Lely Industries N.V. Maassluis, Netherlands",
  "Tyco Fire Protection Products Marinette, WI  USA",
  "RA Consulting GmbH Bruchsal, Germany",
  "SI-TEX Marine Electronics Riverhead, NY   USA",
  "Sea Cross Marine AB Udingo, Sweden",
  "Tenneco Inc. Grass Lake, MI   USA",
  "Boss Industries, Inc. La Porte, IN   USA",
  "Persen Technologies Inc. Winnipeg, MB   Canada",
  "GME New South Wales, Australia",
  "Hummingbird Marine Electronics Alpharetta, GA  USA",
  "OilQuick AB Hudiksvall, Sweden",
  "OceanSat BV Nieuwendijk, Netherlands",
  "Vapor Bus International Buffalo Grove, IL  USA",
  "EnerDel, Inc. Indianapolis, IN   USA",
  "Chetco Digital Instruments Brookings, OR  USA",
  "Tricon Electronics Kolding, Denmark",
  "Valeo Creteil, France",
  "Headsight Inc. Bremen, IN   USA",
  "MATT automotive Koscian, Poland",
  "Westport Innovations Inc. Vancouver, BC  Canada",
  "DSE Test Solutions A/S Horsens, Denmark",
  "The Charles Machine Works, Inc. Perry, OK   USA",
  "Appareo Systems, LLC Fargo, ND   USA",
  "QuikQ Franklin, TN   USA",
  "Penny and Giles Ltd Wales, UK",
  "Inergy Automotive Systems Troy, MI   USA",
  "Watcheye Gouda, Netherlands",
  "Synerject Toulouse, France",
  "HOLMER Maschinenbau GmbH Schierling, Germany",
  "W. Gessmann GmbH Leingarten, Germany",
  "SENTRON Sistemas Embarcados Sao Carlos, Brazil",
  "Innovative Design Solutions, Inc. Troy, MI  USA",
  "LCJ Capteurs Vertou, France",
  "Oxbo International Corporation Byron, NY   USA",
  "Agrotronix S.A. Douvrin, France",
  "Attwood Corporation Lowell, MI USA",
  "Naviop S.R.L. Forli, Italy",
  "Vesper Marine Auckland, New Zealand",
  "Yetter Farm Equipment Colchester, IL USA",
  "IHI STAR Machinery Corporation Hokkaido, Japan",
  "ISOBUS Test Center Wallenhorst, Germany",
  "Transtech Innovations Varennes, QC Canada",
  "MOTORTECH GmbH Celle, Germany",
  "Marinesoft Co. Ltd Busan, South Korea",
  "Sulky Chateaubourg, France",
  "Inpower LLC Lewis Center, OH USA",
  "FarmscanAg Toowoomba, QLD Australia",
  "DISTek Integration, Inc. Cedar Falls, IA   USA",
  "GINAF Trucks Nederland B.V. Veenendaal, Netherlands",
  "AVAT Automation GmbH Tuebingen, Germany",
  "Noland Engineering Melbourne, FL   USA",
  "Transas USA Inc. Bothell, WA   USA",
  "Peeters Landbouwmachines b.v. Etten-Leur, Netherlands",
  "Trapeze Cedar Rapids, IA   USA",
  "Clever Devices Ltd. Woodbury, NY   USA",
  "Nebraska Tractor Test Laboratory Lincoln, NE  USA",
  "Reggio Emilia Innovazione Reggio Emilia, Italy",
  "Vomax Instrumentation Pty. Ltd. Marleston, SA  Australia",
  "Rust Sales Inc Harwood, ND  USA",
  "LOFA Industries, Inc Roswell, GA  USA",
  "GKN Walterscheid GmbH Lohmar, Germany",
  "Hoganas AB, Electric Drive Systems Hoganas, Sweden",
  "National Instruments Korea Seoul, South Korea",
  "NMEA Severna Park, MD  USA",
  "Genge & Thoma AG Bern, Switzerland",
  "Onwa Marine Electronics Co Ltd. Hong Kong, China",
  "Doran Manufacturing, LLC Cincinnati, OH  USA",
  "Webasto Thermo & Comfort SE Gilching, Germany",
  "MOTORPAL, a.s. Jihlava, Czech Republic",
  "SSI Technologies Janesville, WI  USA",
  "Schrader Electronics Ltd. Antrim, N. Ireland",
  "Crop Ventures, Inc. Omaha, NE  USA",
  "Mobileview Costa Mesa, CA  USA",
  "Dinex A/S Middelfart, Denmark",
  "Total Fire Systems, Inc. Youngsville, NC  USA",
  "Dinamica Generale s.p.a. Poggio Rusco, Italy",
  "BAUER Maschinen GmbH Schrobenhausen, Germany",
  "Au Group Electronics Windsor, ON   Canada",
  "GS Hydraulics New Berlin, WI  USA",
  "Maruyama Mfg Co, Inc. Togane, Chiba, Japan",
  "Thomson Linear LLC Marengo, IL  USA",
  "TM Inc. Boucherville, QC  Canada",
  "ROAD Deutschland GmbH Bretten, Germany",
  "SUN-A Corporation Hiroshima, Japan",
  "Wexler CSD Ltd. Rishon LeZion, Israel",
  "Matsuyama Plow Mfg Co, Ltd Nagano, Japan",
  "KIB Electronics Elkhart, IN  USA",
  "iris-GmbH infrared & intelligent sensors Berlin, Germany",
  "Sasaki Corporation Towada City, Japan",
  "Doosan Infracore Norway Elnesvagen, Norway",
  "Rockson Automation GmbH Kiel, Germany",
  "Davis Instruments Corp. Hayward, CA  USA",
  "Four Peaks Navigation Overgaard, AZ  USA",
  "Iowa State University, Agricultural and Biosystems Engineering Ames, IA   USA",
  "b-plus GmbH Deggendorf, Germany",
  "Bombardier Transportation GmbH Mannheim, Germany",
  "LOHR Sistemas Eletronicos LTDA Caxias do Sul, Brazil",
  "Auto Power Electronic Opole, Poland",
  "Micro-Trak Systems, Inc. Eagle Lake, MN  USA",
  "Geode Technology, Inc. Fredericton, NB  Canada",
  "Lithiumstart LLC Healdsburg, CA  USA",
  "Makersan Ltd. Co. Gebze, Turkey",
  "LORD MicroStrain Sensing Systems Willison, VT  USA",
  "frenzel + berg electronic GmbH & Co. KG,Ulm, Germany,--",
  "Marinecraft Co., Ltd.,Pusan, South Korea,--",
  "Fasse Valves,Kearney, NE USA,--",
  "Orolia Ltd,Portsmouth, Hampshire UK,--",
  "Vishay Precision Group,Bradford, West Yorkshire UK,--",
  "Lytx,San Diego, CA USA,--",
  "Vectia,Irura, Guipuzcoa Spain,--",
  "Denchi Power Ltd (formerly ABSL Power Solutions Limited),Thurso, Caithness Scotland,--",
  "advanSea,Loriant, France,--",
  "KVH Industries, Inc.,Middletown, RI USA,--",
  "San Jose Technology, Inc.,New Taipei City, Taiwan,--",
  "Väderstad - Verken AB,Väderstad, Sweden,--",
  "Innovative Software Engineering,Coralville, IA USA,--",
  "Yachtcontrol,Hattern, Netherlands,--",
  "CarMedialab GmbH,Bruchsal, Germany,--",
  "Industrial Electronic Controls,Rockford, IL USA,--",
  "Suzuki Motor Corp,Hamamatsu, Japan,--",
  "JCA Electronics,Winnipeg, MB  Canada,--",
  "Vignal Systems,Venissieux, France,--",
  "MICO, Inc.,North Mankato, MN USA,--",
  "ARGO-HYTOS GMBH,Kraichtal, Germany,--",
  "United States Coast Guard,New London, CT  USA,--",
  "tecsis GmbH,Offenbach, Germany,--",
  "Sensata Technologies,Attleboro, MA  USA,--",
  "Kongsberg Automotive,Kongsberg, Norway,--",
  "CustomWare,Assen, Netherlands,--",
  "Brunelco Electronic Innovators B.V.,Haaksbergen, Netherlands,--",
  "Hydac Filter Systems GmbH,Sulzbach/Saar, Germany,--",
  "ABB Turbo Systems Ltd,Baden, Switzerland,--",
  "Spudnik Equipment Co, LLC,Blackfoot, ID USA,--",
  "Aquatic AV,San Jose, CA USA,--",
  "Navitas Systems,Woodridge, IL USA,--",
  "Nomad Digital Ltd.,Newcastle upon Tyne, UK,--",
  "Kereval,Thorigne Fouillard, France,--",
  "Indexator Rototilt Systems AB,Vindeln, Sweden,--",
  "Aventics GmbH,Laatzen, Germany,--",
  "Intellian Technologies,Irvine, CA  USA,--",
  "Knappco/Civacon,Kansas City, MO  USA,--",
  "Gale Banks Engineering,Azusa, CA  USA,--",
  "Walvoil S.p.A.,Reggio Emilia, Italy,--",
  "Trail Tech,Battle Ground, WA USA,--",
  "Esterline,Coeur d'Alene, ID USA,--",
  "Samwon IT,Busan, Korea,--",
  "HKS Co., Ltd.,Shizuoka, Japan,--",
  "ARLT Technologies GmbH,Netphen, Germany,--",
  "Networkfleet, Verizon Telematics,San Diego, CA  USA,--",
  "SMART-TEC s.r.o.,Velke Porici, Czech Republic,--",
  "Zero Emission Vehicles,Palmerston North, New Zealand,--",
  "Evrard SA,Beaurainville, France,--",
  "Right Weigh Load Scales,Sherwood, OR USA,--",
  "Sevcon Ltd.,Gateshead, UK,--",
  "Hagie Manufacturing Company,Clarion, IA  USA,--",
  "Floyd Bell Inc,Columbus, OH  USA,--",
  "Xirgo Technologies,Camarillo, CA  USA,--",
  "Blackbox Machine Control Pty Ltd,Maida Vale, WA Australia,--",
  "Global MRV Inc.,Buffalo, NY USA,--",
  "AVL DiTEST GmbH,Fuerth, Germany,--",
  "Radio Ocean,Merignac, France,--",
  "Falck Schmidt Defence Systems,Odense C, Denmark,--",
  "Agri Info Design, Ltd.,Hokkaido, Japan,--",
  "SmartDrive Systems Inc.,San Diego, CA USA,--",
  "Reltima,Woburn, MA USA,--",
  "Pepperl+Fuchs GmbH,Mannheim, Germany,--",
  "TORC Robotics,Blacksburg, VA USA,--",
  "Rocky Research,Huntsville, AL USA,--",
  "Argo Tractors S.p.A.,Reggio Emilia, Italy,--",
  "Divelbiss Corporation,Fredericktown, OH USA,--",
  "Bavaria Yachtbau GmbH,Giebelstadt, Germany,--",
  "KVH Industries, Inc.,Middletown, RI USA,--",
  "Startec s.r.l.,Sesto al Reghena (PN), Italy,--",
  "Power Solutions International,Wood Dale, IL USA,--",
  "Diverse Yacht Services,Southampton, Hampshire UK,--",
  "Moog / Aspen Motion Technologies,Radford, VA  USA,--",
  "Bogballe A/S,Uldum, Denmark,--",
  "KUS USA,Ft. Lauderdale, FL USA,--",
  "esd electronic system design gmbh,Hannover, Germany,--",
  "Veenhuis Machines BV,Raalte, Netherlands,--",
  "Siloking,Tittmoning, Germany,--",
  "OJSC Ekran,Borisov, Belarus,--",
  "Control-Q BV,Vaassen, Netherlands,--",
  "Seiko Epson Corp.,Nagano, Japan,--",
  "Takakita Co, Ltd.,Nabari, Japan,--",
  "MicroControl GmbH & Co. KG,Troisdorf, Germany,--",
  "AEV, spol. s r. o.,Kromeriz, Czech Republic,--",
  "Kohler Power Systems, Detroit Engine Development Center (DEDC),Detroit, MI  USA,--",
  "Genge & Thoma AG,Lengnau, Switzerland,--",
  "PRO SOLUS do Brasil,Campo Mourão, Brazil,--",
  "Terzo Power Systems,Sacramento, CA  USA,--",
  "Shenzhen Jiuzhou Himunication Technology Co., Ltd,Shenzhen, China,--",
  "Data Panel Corp.,Plymouth, MN  USA,--",
  "Auto-Gaz Centrum,Radom, Poland,--",
  "SPAL Automotive S.r.l.,Correggio, Italy,--",
  "Kissling Elektrotechnik GmbH,Wildberg, Germany,--",
  "Delta Systems, Inc,Streetsboro, OH USA,--",
  "Level Developments Ltd,Croydon, Surrey UK,--",
  "Gebr. Bode GmbH & Co. KG,Kassel, Germany,--",
  "Schaeffler Technologies AG & Co., KG,Herzogenaurach, Germany,--",
  "Bartec,Gotteszell, Germany,--",
  "MacDon Industries Ltd.,Winnipeg, MB Canada,--",
  "Quantum Fuel Systems Technologies Worldwide, Inc,Lake Forest, CA USA,--",
  "STEMCO LP,Longview, TX USA,--",
  "Innovative Controls Inc.,East Pittsburgh, PA  USA,--",
  "OPTIMA Concept,Ruitz, France,--",
  "Caruelle Nicolas,Saint Denis de l'Hotel, France,--",
  "Yara International ASA,Oslo, Norway,--",
  "Kawasaki Motors Corp., USA,Grand Rapids, MI  USA,--",
  "Danfoss IXA A/S,Vejle, Denmark,--",
  "DSA Daten- und Systemtechnik GmbH,Aachen, Germany,--",
  "KeepTruckin, Inc.,Amherst, NY  USA,--",
  "Cimco Marine AB,Angelholm, Sweden,--",
  "AMVAC Chemical Corporation,Newport Beach, CA USA,--",
  "BEDIA Motorentechnik GmbH & Co. KG,Altdorf bei Nuernberg, Germany,--",
  "Eckelmann AG,Wiesbaden, Germany,--",
  "Fosen Elektro,Hasselvika, Norway,--",
  "KEB,Barntrup, Germany,--",
  "ANEDO Ltd.,Eydelstedt, Germany,--",
  "Taigene Electric Machinery Corp.,Taipei, Taiwan,--",
  "Flight Systems, Inc.,Mechanicsburg, PA USA,--",
  "Rockford Corp,Tempe, AZ  USA,--",
  "Aarcomm Systems Inc,Coquitlam, BC Canada,--",
  "LINAK A/S,Guderup, Denmark,--",
  "Digitroll Agricultural Electronics,Hajduszoboszlo, Hungary,--",
  "Tanhay Corporation,Incheon, South Korea,--",
  "Agility Fuel Solutions (formerly Agility Fuel Systems),Santa Ana, CA  USA,--",
  "GasTOPS Ltd.,Ottawa, ON  Canada,--",
  "Weldon Technologies,Columbus, OH  USA,--",
  "DRS Network & Imaging Systems, LLC (DRS Technologies),Huntsville, AL  USA,--",
  "Ålö AB,Umeå, Sweden,--",
  "Scorpion Technologies Ltd.,Kamloops, BC Canada,--",
  "Harman International Industries,Querétaro, Mexico,--",
  "K-Tec Earthmovers Inc.,Rosenort, MB Canada,--",
  "BigRoad Inc.,Kitchener, ON Canada,--",
  "Weichai Power Co., Ltd.,Weifang, China,--",
  "Hydro Tab Marine Engineering,Athens, Greece,--",
  "JL Audio, Inc.,Miramar, FL USA,--",
  "SVAB Hydraulik AB,Hallsberg, Sweden,--",
  "Shanghai Diesel Engine Corporation Limited,Shanghai, China,--",
  "Rochester Gauges Inc,Dallas, TX USA,--",
  "Lars Thrane A/S,Vedbaek, Denmark,--",
  "Marquardt GmbH,Rietheim-Weilheim, Germany,--",
  "Greentronics Ltd.,Elmira, ON Canada,--",
  "DAS Co., LTD,Hwaseong-si, South Korea,--",
  "LOR Manufacturing Company Inc.,Weidman, MI  USA,--",
  "US Hybrid Corporation,Torrance, CA  USA,--",
  "Kobashi Kogyo Co., Ltd.,Okayama, Japan,--",
  "Autonnic Research Ltd,Tollesbury, Essex  UK,--",
  "Eaton Control & Power Conversion Division (CPCD),Tualatin, OR  USA,--",
  "Yacht Devices Ltd,Kaliningrad, Russia,--",
  "Micronet Inc,Salt Lake City, UT  USA,--",
  "Cojali S. L.,Campo de Criptana, Spain,--",
  "WITZ Corporation,Nagoya City, Japan,--",
  "Hypro,New Brighton, MN  USA,--",
  "Contelec AG,Biel/Bienne, Switzerland,--",
  "Jabil Inc.,Auburn Hills, MI  USA,--",
  "Electronic Applications, Inc.,Phoenix, AZ  USA,--",
  "Hitachi Construction Machinery Co., Ltd.,Kandatsu-machi, Japan,--",
  "MIDORI PRECISIONS,Tokyo, Japan,--",
  "Dhoot Transmission Pvt Ltd.,Aurangabad, India,--",
  "Streumaster / Panien,Egglkofen, Germany,--",
  "Liebherr,Bulle, Switzerland,--",
  "BERTHOUD AGRICOLE,Belleville sur Saone, France,--",
  "Modine Manufacturing Company,Racine, WI  USA,--",
  "Gefran S.p.A.,Provaglio d'Iseo, Italy,--",
  "Intendia S.L.,Madrid, Spain,--",
  "REAPsystems,St Denys, Southampton UK,--",
  "AEM Performance Electronics,Hawthorne, CA USA,--",
  "Terex Aerial Work Platforms - Genie,Redmond, WA USA,--",
  "Balluff GmbH,Neuhausen, Germany,--",
  "Blue Ink Technologies,Huntington, WV USA,--",
  "LXNAV d.o.o.,Celje, Slovenia,--",
  "e-Traction,Apeldoorn, Netherlands,--",
  "Carling Technologies,Plainville, CT  USA,--",
  "EROAD,Auckland, New Zealand,--",
  "Daemyung Elevator Co., Ltd.,Ulsan, Republic of Korea,--",
  "Woosung Engineering Co., Ltd.,Gyengsangnam-do, Republic of Korea,--",
  "Cidra Corporate Services Inc.,Wallingford, CT USA,--",
  "SureFire Ag Systems, Inc.,Atwood, KS USA,--",
  "Agratronix,Streetsboro, OH USA,--",
  "ISOTTA,Occhiobello, Italy,--",
  "Chart Inc.,Ball Ground, GA  USA,--",
  "Joskin SA,Soumagne, Belgium,--",
  "Pacific Track,Newport Beach, CA  USA,--",
  "Deep Sea Electronics Plc,Hunmanby, North Yorkshire  UK,--",
  "AIROD Technologies,Toulouse, France,--",
  "Parker Hannifin Corp., Automation Group,Cleveland, OH  USA,--",
  "Firefly Integrations,Middlebury, IN USA,--",
  "Maschio Gaspardo S.P.A.,Campodarsego, Italy,--",
  "IMPCO Technologies,Calgary, AB  Canada,--",
  "Banner Engineering Corp.,Minneapolis, MN  USA,--",
  "Hydro-Gear,Sullivan, IL USA,--",
  "Bernecker + Rainer Industrie Elektronik GmbH,Eggelsberg, Austria,--",
  "MITA OLEODINAMICA S.p.A.,Tregnago, Italy,--",
  "ROJ s.r.l.,Biella, Italy,--",
  "AT-Systems BVBA,Meerle, Belgium,--",
  "Bednar FMT s.r.o.,Prague, Czech Republic,--",
  "GIGAVAC,Carpinteria, CA  USA,--",
  "Epec Oy,Seinajoki, Finland,--",
  "Alliance Wireless Technologies Inc.,Houston, TX USA,--",
  "Flores Automation, LLC,Muskego, WI USA,--",
  "Trombetta,Milwaukee, WI USA,--",
  "MONOSEM,Largeasse, France,--",
  "Shaw Development, LLC,Bonita Springs, FL USA,--",
  "Blink Marine,Milano, Italy,--",
  "Clarion Corporation of America,Cypress, CA  USA,--",
  "Taisho Corporation,Mito, Japan,--",
  "ZIEHL-ABEGG Automotive GmbH,Kupferzell, Germany,--",
  "HMI Systems LLC,Lakeland, FL  USA,--",
  "Ocean Signal Ltd,Margate, Kent  UK,--",
  "Seakeeper Inc.,California, MD  USA,--",
  "RLC Electronic Systems,Sinking Spring, PA USA,--",
  "AVID Technology Ltd,Cramlington, Northumberland UK,--",
  "Poly-Planar LLC,Hanover, MD  USA,--",
  "AVR bvba,Roeselare, Belgium,--",
  "Loup Electronics Inc.,Lincoln, NE  USA,--",
  "CM Automotive Systems, Inc.,Walnut, CA  USA,--",
  "Fischer Panda GmbH,Paderborn, Germany,--",
  "Johnson Matthey Battery Systems,Milton Keynes, Bucks  UK,--",
  "Abertax Technologies Limited,Paola, Malta,--",
  "MoTeC Pty Ltd,Melbourne, Victoria  Australia,--",
  "GRADALL,New Philadelphia, OH USA,--",
  "VACALL,New Philadelphia, OH USA,--",
  "AUTEC,Caldogno, Italy,--",
  "Kostal Mexicana,Querétaro, Mexico,--",
  "James Fisher Prolec,Poole, Dorset  UK,--",
  "Shihlin Electric & Engineering Corporation,Suzhou, China,--",
  "Broyda Enterprises Pty Ltd,Sydney, Australia,--",
  "Canadian Automotive Industries Ltd,Stony Plain, AB  Canada,--",
  "Tides Marine,Deerfield Beach, FL  USA,--",
  "Lumishore Ltd,Swansea, Wales  UK,--",
  "Stillwater Designs and Audio, Inc. - KICKER,Stillwater, OK  USA,--",
  "Delta-Q Technologies,Burnaby, BC  Canada,--",
  "LOR Manufacturing Company Inc.,Weidman, MI  USA,--",
  "SPBI (Bj Technologie),Dompierre-sur-Yon, France,--",
  "Gill Sensors & Controls Limited,Lymington, Hampshire  UK,--",
  "ASA Electronics,Elkhart, IN  USA,--",
  "Gundersen & Løken AS,Oslo, Norway,--",
  "Charge Automotive Ltd,Banbury, Oxfordshire  UK,--",
  "Schneider-Electric,Burnaby, BC  Canada,--",
  "RIMEX Supply Ltd.,Surrey, BC  Canada,--",
  "HMS Industrial Networks AB,Halmstad, Sweden,--",
  "Dutch Power Company,Giessen, Netherlands,--",
  "Blue Water Desalination,Carson, CA  USA,--",
  "Torch Technologies,Huntsville, AL USA,--",
  "Thales Suisse SA,Zurich, Switzerland,--",
  "Soltec Soluzioni Tecnologiche Srl,Basaldella di Campoformido, Italy,--",
  "FLIR Systems, Inc.,Wilsonville, OR  USA,--",
  "UniStrong,Beijing, China,--",
  "TE Connectivity Sensor Solutions,Dormund, Germany,--",
  "HGNSS,Scottsdale, AZ  USA,--",
  "Preco Electronics,Boise, ID  USA,--",
  "DIaLOGIKa Gesellschaft fuer angewandte Informatik mbH,Saarbruecken, Germany,--",
  "Thorsen Teknik A/S,Farsø, Denmark,--",
  "Bren-Tronics, Inc.,Commack, NY USA,--",
  "ACEINNA, Inc (formerly MEMSIC, Inc),Andover, MA USA,--",
  "Undheim Systems AS,Egersund, Norway,--",
  "BPW Hungária,Szombathely, Hungary,--",
  "Lewmar Marine Inc,Guilford, CT  USA,--",
  "INmatix Technology Group Ltd,Hattem, Netherlands,--",
  "AgriBrink Inc.,Moorefield, ON Canada,--",
  "Drov Technologies, Inc.,Oklahoma City, OK USA,--",
  "Ultra Motion LLC,Cutchogue, NY  USA,--",
  "DOK-ING Ltd.,Zagreb, Croatia,--",
  "Heizomat Gerätebau-Energiesysteme GmbH,Gunzenhausen, Germany,--",
  "Electrum Automation AB,Umeå, Sweden,--",
  "Pioneer Microsystems, Inc.,Pittsburgh, PA  USA,--",
  "PetTrack Ltd.,Héreg, Hungary,--",
  "Signature,Coldwater, OH  USA,--",
  "Famic Technologies Inc.,Montréal, QC  Canada,--",
  "Teamsurv Ltd,Didcot, Oxfordshire  UK,--",
  "Indexator Rotator Systems AB,Vindeln, Sweden,--",
  "E.S.T.E. srl,Ferrara, Italy,--",
  "Agra-GPS,Stony Plain, AB  Canada,--",
  "DigiDevice Srl,Calvisano, Italy,--",
  "Hendrickson Truck Commercial Vehicle Systems,Woodridge, IL  USA,--",
  "FELL AS,Lier, Norway,--",
  "GMB Güstrower Maschinenbau GmbH,Güstrow, Germany,--",
  "L Magnet-Motor,Starnberg, Germany,--",
  "Oceanvolt,Vantaa, Finland,--",
  "ningupex,Dresden, Germany,--",
  "MTS Maschinentechnik Schrode AG,Hayingen, Germany,--",
  "Geoprospectors GmbH,Traiskirchen, Austria,--",
  "Novotechnik Messwertaufnehmer OHG,Ostfildern (Ruit), Germany,--",
  "Velvac Inc,New Berlin, WI  USA,--",
  "Teledyne RESON BV,Rotterdam, Netherlands,--",
  "GEMAC Chemnitz GmbH,Chemnitz, Germany,--",
  "Toshiba Infrastructure Systems & Solutions Corporation,Kawasaki, Japan,--",
  "FarmFacts GmbH,Pfarrkirchen, Germany,--",
  "Teleflex  Limerick, PA USA",
  "RayMarine  Portsmouth, Hampshire, UK",
  "Navionics  Wareham, MA USA",
  "Japan Radio Co  Seattle, WA USA",
  "Northstar Technologies  Acton, MA USA",
  "Furuno USA  Camas, WA USA",
  "Trimble  Sunnyvale, CA USA",
  "Simrad  Egersund, Norway",
  "Litton  Charlottesville, VA USA",
  "Kvasar AB  Kinnahult, Sweden",
  "MMP  Fircrest, WA USA",
  "Vector Cantech  Novi, MI USA",
  "Sanshin Shizuoka, Japan",
  "Thomas G. Faria Co. Uncasville, CT  USA",
};

const char* industryGroupArray[] = {
  "Global, applies to all",
  "On-Highway Equipment",
  "Agricultural and Forestry Equipment",
  "Construction Equipment",
  "Marine",
  "Industrial-Process Control-Stationary (Gen-Sets)",
  "Reserved for future assignment by SAE",
  "Reserved for future assignment by SAE",
};

const char* deviceClassArray[] = {
  "non-specific System",
  "Tractor",
  "Tillage",
  "Secondary Tillage",
  "Planters/Seeders",
  "Fertilizers",
  "Sprayers",
  "Harvesters",
  "Root Harvesters",
  "Forages",
  "Irrigation",
  "Transport/Trailer",
  "Farm Yard Operations",
  "Powered Auxiliary Devices",
  "Special Crops",
  "Earth Work",
  "Skidder",
  "Sensor Systems",
  "N.D.",
  "Timber Harvesters",
  "Forwarders",
  "Timber Loaders",
  "Timber Processing Machines",
  "Mulchers",
  "Utility Vehicles",
  "Slurry/Manure Applicators",
  "Feeders/Mixers",
  "Not Available",
};

const char* functionsBasicArray[] = {
  "Engine",
  "Auxiliary Power Unit (APU)",
  "Electric Propulsion Control",
  "Transmission",
  "Battery Pack Monitor",
  "Shift Control/Console",
  "Power TakeOff - (Main or Rear)",
  "Axle - Steering",
  "Axle - Drive",
  "Brakes - System Controller",
  "Brakes - Steer Axle",
  "Brakes - Drive axle",
  "Retarder - Engine",
  "Retarder - Driveline",
  "Cruise Control",
  "Fuel System",
  "Steering Controller",
  "Suspension - Steer Axle",
  "Suspension - Drive Axle",
  "Instrument Cluster",
  "Trip Recorder",
  "Cab Climate Control",
  "Aerodynamic Control",
  "Vehicle Navigation",
  "Vehicle Security",
  "Network Interconnect ECU",
  "Body Controller",
  "Power TakeOff (Secondary or Front)",
  "Off Vehicle Gateway",
  "Virtual Terminal (in cab)",
  "Management Computer",
  "Propulsion Battery Charger",
  "Headway Controller",
  "System Monitor",
  "Hydraulic Pump Controller",
  "Suspension - System Controller",
  "Pneumatic - System Controller",
  "Cab Controller",
  "Tire Pressure Control",
  "Ignition Control Module",
  "Seat Control",
  "Lighting - Operator Controls",
  "Water Pump Control",
  "Transmission Display",
  "Exhaust Emission Control",
  "Vehicle Dynamic Stability Control",
  "Oil Sensor Unit",
  "Information System Controller",
  "Ramp Control",
  "Clutch/Converter Control",
  "Auxiliary Heater",
  "Forward-Looking Collision Warning System",
  "Chassis Controller",
  "Alternator/Charging System",
  "Communications Unit, Cellular",
  "Communications Unit, Satellite",
  "Communications Unit, Radio",
  "Steering Column Unit",
  "Fan Drive Control",
  "Starter",
  "Cab Display",
  "File Server / Printer",
  "On-Board Diagnostic Unit",
  "Engine Valve Controller",
  "Endurance Braking",
  "Gas Flow Measurement",
  "I/O Controller",
  "Electrical System Controller",
  "Aftertreatment system gas measurement",
  "Engine Emission Aftertreatment System",
  "Auxiliary Regeneration Device",
  "Transfer Case Control",
  "Coolant Valve Controller",
  "Rollover Detection Control",
  "Lubrication System",
  "Supplemental Fan",
  "Temperature Sensor",
  "Fuel Properties Sensor",
  "Fire Suppression System",
  "Power Systems Manager",
  "Electric Powertrain",
  "Hydraulic Powertrain",
  "File Server",
  "Printer",
  "Start Aid Device",
};

const char* functionsIg0Dvc0Array[] = {
  "Reserved",
  "Off-board diagnostic-service tool",
  "On-board data logger",
  "PC Keyboard",
  "Safety Restraint System",
  "Turbocharger",
  "Ground based speed sensor",
  "Keypad",
  "Humidity sensor",
  "Thermal Management System Controller",
  "Brake Stroke Alert",
  "On-board axle group scale",
  "On-board axle group display",
  "Battery Charger",
  "Turbocharger Compressor Bypass",
  "Turbocharger Wastegate",
  "Throttle",
  "Inertial Sensor",
  "Fuel Actuator",
  "Engine Exhaust Gas Recirculation",
  "Engine Exhaust Backpressure",
  "On-board bin weighing scale",
  "On-board bin weighing scale display",
  "Engine Cylinder Pressure Monitoring System",
  "Object Detection",
  "Object Detection Display",
  "Object Detection Sensor",
  "Not Available"
};

const char* functionsIg1Dvc0Array[] = {
  "Tachograph",
  "Door Controller",
  "Articulation Turntable Control",
  "Body-to-Vehicle Interface Control",
  "Slope Sensor",
  "Retarder Display",
  "Differential Lock Controller",
  "Low-Voltage Disconnect",
  "Roadway Information",
  "Not Available"
};

const char* functionsIg1Dvc1Array[] = {
  "Forward Road Image Processing",
  "Fifth Wheel Smart System",
  "Catalyst Fluid Sensor",
  "Adaptive Front Lighting System",
  "Idle Control System",
  "User Interface System",
  "Not Available"
};

const char* functionsIg1Dvc2Array[] = {
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc0Array[] = {
  "Non Virtual Terminal Display",
  "Operator Controls - Machine Specific",
  "Task Controller (Mapping Computer)",
  "Position Control",
  "Machine Control",
  "Foreign Object Detection",
  "Tractor ECU",
  "Sequence Control Master",
  "Product Dosing",
  "Product Treatment",
  "reserved",
  "Data Logger",
  "Decision Support",
  "Lighting Controller",
  "TIM Server",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc1Array[] = {
  "n.a",
  "Auxiliary Valve Control",
  "Rear Hitch Control",
  "Front Hitch Control",
  "Tractor Machine Control",
  "n.a",
  "Center Hitch Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc2Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Tillage Machine Control",
  "n.a",
  "n.a",
  "Tillage Depth Control",
  "Frame Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc3Array[] = {
  " n.a",
  " n.a",
  " n.a",
  " n.a",
  " Secondary Tillage Machine Control",
  " n.a",
  " n.a",
  " Secondary Tillage Depth Control",
  " Frame Control",
  " n.a.",
  " n.a.",
  " n.a.",
  " n.a.",
  " n.a.",
  " n.a.",
  " Not Available"
};

const char* functionsIg2Dvc4Array[] = {
  "Seed Rate Control",
  "Section On/Off Control",
  "n.a",
  "Position Control",
  "Planters/ Seeders Machine Control",
  "Product Flow",
  "Product Level",
  "Depth Control",
  "Frame Control",
  "Down Pressure",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc5Array[] = {
  "Fertilize Rate Control",
  "Section On/Off Control",
  "Product Pressure",
  "Position Control",
  "Fertilizers Machine Control",
  "Product Flow",
  "Product Level",
  "Height/Depth Control",
  "Frame Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc6Array[] = {
  "Spray Rate Control",
  "Section On/Off Control",
  "Product Pressure",
  "Position Control",
  "Sprayers Machine Control",
  "Product Flow",
  "Product Level",
  "Boom Height Control",
  "Frame Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc7Array[] = {
  "Tailing Monitor",
  "Header Control",
  "Product Loss Monitor",
  "Product Moisture",
  "Harvester Machine Control",
  "Product Flow",
  "Product Level",
  "Header Height Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc8Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Root Harvesters Machine Control",
  "Product Flow",
  "Product Level",
  "Depth Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc9Array[] = {
  "Twine Wrapper Control",
  "Product Packaging Control",
  "n.a.",
  "Product Moisture",
  "Forage Machine Control",
  "Product Flow",
  "n.a.",
  "Working Height Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc10Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc11Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Transport Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "Unload Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available",
};

const char* functionsIg2Dvc12Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc13Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Powered Devices Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc14Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Special Crop Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc15Array[] = {
  "Material Rate Control",
  "n.a",
  "n.a",
  "n.a",
  "Earthworks Machine Control",
  "Material Flow",
  "Material Level",
  "Depth Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc16Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Skidder Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc17Array[] = {
  "Guidance Feeler",
  "Camera System",
  "Crop Scouting",
  "Material Properties Sensing",
  "Inertial Measurement Unit (IMU)",
  "Product flow",
  "Product Level",
  "Product Mass",
  "Vibration/Knock",
  "Weather Instruments",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc18Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc19Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Timber Harvestors Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc20Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Forwarders Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc21Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Timber Loaders Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc22Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Timber Processing Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc23Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Mulcher Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc24Array[] = {
  "n.a",
  "n.a",
  "n.a",
  "n.a",
  "Utility Machine Control",
  "n.a",
  "n.a",
  "n.a",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc25Array[] = {
  "Slurry/Manure Rate Control",
  "Section On/Off Control",
  "Product Pressure",
  "n.a.",
  "Slurry/Manure Machine Control",
  "Product Flow",
  "Product Level",
  "Boom Height Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};

const char* functionsIg2Dvc26Array[] = {
  "Feeder/Mixer Rate Control",
  "Section On/Off Control",
  "Product Pressure",
  "n.a.",
  "Feeder/Mixer Machine Control",
  "Product Flow",
  "Product Level",
  "Boom Height Control",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "n.a.",
  "Not Available"
};


struct dvcFctWithSize_s {
  const char** deviceClassFunction;
  size_t arraySize;
};
dvcFctWithSize_s IG0deviceClassFunctionArray[] = {
  { functionsIg0Dvc0Array, sizeof(functionsIg0Dvc0Array) / sizeof(functionsIg0Dvc0Array[0]) }
};

dvcFctWithSize_s IG1deviceClassFunctionArray[] = {
  { functionsIg1Dvc0Array, sizeof(functionsIg1Dvc0Array) / sizeof(functionsIg1Dvc0Array[0]) },
  { functionsIg1Dvc1Array, sizeof(functionsIg1Dvc1Array) / sizeof(functionsIg1Dvc1Array[0]) },
  { functionsIg1Dvc2Array, sizeof(functionsIg1Dvc2Array) / sizeof(functionsIg1Dvc2Array[0]) }
};
dvcFctWithSize_s IG2deviceClassFunctionArray[] = {
  { functionsIg2Dvc0Array,  sizeof(functionsIg2Dvc0Array) /  sizeof(functionsIg2Dvc0Array[0]) }, 
  { functionsIg2Dvc1Array,  sizeof(functionsIg2Dvc1Array) /  sizeof(functionsIg2Dvc1Array[0]) }, 
  { functionsIg2Dvc2Array,  sizeof(functionsIg2Dvc2Array) /  sizeof(functionsIg2Dvc2Array[0]) }, 
  { functionsIg2Dvc3Array,  sizeof(functionsIg2Dvc3Array) /  sizeof(functionsIg2Dvc3Array[0]) },
  { functionsIg2Dvc4Array,  sizeof(functionsIg2Dvc4Array) /  sizeof(functionsIg2Dvc4Array[0]) }, 
  { functionsIg2Dvc5Array,  sizeof(functionsIg2Dvc5Array) /  sizeof(functionsIg2Dvc5Array[0]) }, 
  { functionsIg2Dvc6Array,  sizeof(functionsIg2Dvc6Array) /  sizeof(functionsIg2Dvc6Array[0]) },
  { functionsIg2Dvc7Array,  sizeof(functionsIg2Dvc7Array) /  sizeof(functionsIg2Dvc7Array[0]) },
  { functionsIg2Dvc8Array,  sizeof(functionsIg2Dvc8Array) /  sizeof(functionsIg2Dvc8Array[0]) }, 
  { functionsIg2Dvc9Array,  sizeof(functionsIg2Dvc9Array) /  sizeof(functionsIg2Dvc9Array[0]) }, 
  { functionsIg2Dvc10Array, sizeof(functionsIg2Dvc10Array) / sizeof(functionsIg2Dvc10Array[0]) },
  { functionsIg2Dvc11Array, sizeof(functionsIg2Dvc11Array) / sizeof(functionsIg2Dvc11Array[0]) },
  { functionsIg2Dvc12Array, sizeof(functionsIg2Dvc12Array) / sizeof(functionsIg2Dvc12Array[0]) },
  { functionsIg2Dvc13Array, sizeof(functionsIg2Dvc13Array) / sizeof(functionsIg2Dvc13Array[0]) },
  { functionsIg2Dvc14Array, sizeof(functionsIg2Dvc14Array) / sizeof(functionsIg2Dvc14Array[0]) },
  { functionsIg2Dvc15Array, sizeof(functionsIg2Dvc15Array) / sizeof(functionsIg2Dvc15Array[0]) },
  { functionsIg2Dvc16Array, sizeof(functionsIg2Dvc16Array) / sizeof(functionsIg2Dvc16Array[0]) },
  { functionsIg2Dvc17Array, sizeof(functionsIg2Dvc17Array) / sizeof(functionsIg2Dvc17Array[0]) },
  { functionsIg2Dvc18Array, sizeof(functionsIg2Dvc18Array) / sizeof(functionsIg2Dvc18Array[0]) },
  { functionsIg2Dvc19Array, sizeof(functionsIg2Dvc19Array) / sizeof(functionsIg2Dvc19Array[0]) },
  { functionsIg2Dvc20Array, sizeof(functionsIg2Dvc20Array) / sizeof(functionsIg2Dvc20Array[0]) },
  { functionsIg2Dvc21Array, sizeof(functionsIg2Dvc21Array) / sizeof(functionsIg2Dvc21Array[0]) },
  { functionsIg2Dvc22Array, sizeof(functionsIg2Dvc22Array) / sizeof(functionsIg2Dvc22Array[0]) },
  { functionsIg2Dvc23Array, sizeof(functionsIg2Dvc23Array) / sizeof(functionsIg2Dvc23Array[0]) },
  { functionsIg2Dvc24Array, sizeof(functionsIg2Dvc24Array) / sizeof(functionsIg2Dvc24Array[0]) },
  { functionsIg2Dvc25Array, sizeof(functionsIg2Dvc25Array) / sizeof(functionsIg2Dvc25Array[0]) },
  { functionsIg2Dvc26Array, sizeof(functionsIg2Dvc26Array) / sizeof(functionsIg2Dvc26Array[0]) }
};

typedef struct {
    // ui8 SA stored as ui16 for cout convenience
    std::map<uint64_t, std::vector<uint16_t> > listOfNamesAndAddresses;
    std::vector<uint8_t> listOfTcServerSas;
}TNamesAndAdressesMgmt;

TNamesAndAdressesMgmt tNamesAndAddressesMgmt;

struct namefields_s
{
    bool boSelfConfig;
    uint16_t IndustryGroup;
    uint16_t DeviceClass;
    uint16_t DeviceClassInstance;
    uint16_t ManuCode;
    uint16_t Function;
    uint16_t FunctionInstance;
    uint16_t EcuInstance;
    uint32_t SerNo;
};

namefields_s decodeName(const std::vector<uint8_t> &dataFrame)
{
    namefields_s decodedName;
    decodedName.boSelfConfig = dataFrame[7];
    decodedName.IndustryGroup = (dataFrame[7] >> 4) & 0x7;
    decodedName.DeviceClass = dataFrame[6] >> 1;
    decodedName.DeviceClassInstance = (dataFrame[7] & 0xF);
    decodedName.ManuCode = (dataFrame[3] << 3) | (dataFrame[2] >> 5);
    decodedName.Function = (dataFrame[5]);
    decodedName.FunctionInstance = (dataFrame[4] >> 3);
    decodedName.EcuInstance = (dataFrame[4] & 0x7);
    decodedName.SerNo = (dataFrame[2] & 0x1F) << 16 | (dataFrame[1] << 8) | dataFrame[0];
    return decodedName;
}

void vSetOverrideTcSa(uint8_t u8OverrideTcSa)
{
    tNamesAndAddressesMgmt.listOfTcServerSas.push_back(u8OverrideTcSa);
}

bool boSaIsTc(uint8_t u8Sa)
{
    bool boSaFound = false;
    if (tNamesAndAddressesMgmt.listOfTcServerSas.empty())
    {
        //list still empty, so no TC available yet
        return false;
    }
    //is SA already stored as TC?
    else
    {
        for (auto it = tNamesAndAddressesMgmt.listOfTcServerSas.begin(); it != tNamesAndAddressesMgmt.listOfTcServerSas.end(); ++it)
        {
            if (*it == u8Sa)
            {
                boSaFound = true;
                break;
            }
        }
    }
    return boSaFound;
}

inline const char* getFunction(const uint16_t industryGroup, const uint16_t deviceClass, const uint16_t function)
{
    if (function < 128)
    {
        if (function < sizeof(functionsBasicArray) / sizeof(functionsBasicArray[0]))
        {
            return functionsBasicArray[function];
        }
        else
        {
            return "undefined";
        }
    }
    else //function >= 128
    {
        switch (industryGroup)
        {
        case 0:
            if (deviceClass < sizeof(IG0deviceClassFunctionArray) / sizeof(IG0deviceClassFunctionArray[0]) &&
                (function - 128) < IG0deviceClassFunctionArray[deviceClass].arraySize)
            {
                return IG0deviceClassFunctionArray[deviceClass].deviceClassFunction[function - 128];
            }
            else
            {
                return "undefined";
            }
            break;
        case 1:
            if (deviceClass < sizeof(IG1deviceClassFunctionArray) / sizeof(IG1deviceClassFunctionArray[0]) &&
                (function - 128) < IG1deviceClassFunctionArray[deviceClass].arraySize)
            {
                return IG1deviceClassFunctionArray[deviceClass].deviceClassFunction[function - 128];
            }
            else
            {
                return "undefined";
            }
            break;
        case 2:
            if (deviceClass < sizeof(IG2deviceClassFunctionArray) / sizeof(IG2deviceClassFunctionArray[0]) &&
                (function - 128) < IG2deviceClassFunctionArray[deviceClass].arraySize)
            {
                return IG2deviceClassFunctionArray[deviceClass].deviceClassFunction[function - 128];
            }
            else
            {
                return "undefined";
            }
            break;
        case 3:     //break left out intentionally
        case 4:
        case 5:
        case 6:
        case 7:
            return "Function not included yet";
            break;
        default:
            return "not defined yet";
        }
    }
}

inline const char* getDeviceClass(const uint16_t deviceClass)
{
    if (deviceClass < sizeof(deviceClassArray) / (sizeof(deviceClassArray[0])))
    {
        return deviceClassArray[deviceClass];
    }
    else
    {
        return "undefined";
    }
}

inline const char* getManufacturer(const uint16_t ManuCode)
{
    if (ManuCode < sizeof(manufacturerCodeArray) / (sizeof(manufacturerCodeArray[0])))
    {
        return manufacturerCodeArray[ManuCode];
    }
    else
    {
        return "undefined";
    }
}

inline const char* getIndustryGroup(const uint16_t IndustryGroup)
{
    if (IndustryGroup < 8)
    {
        return industryGroupArray[IndustryGroup];
    }
    else
    {
        return "undefined";
    }

}
inline void printShortenedString(std::ostream& out, const char* string)
{
    char charShortened[LENGTH_OUTPUT_STRING];
    if (strlen(string) < LENGTH_OUTPUT_STRING)
    {
        out << std::setw(LENGTH_OUTPUT_STRING) << std::left << string;
    }
    else
    {
        snprintf(charShortened, LENGTH_OUTPUT_STRING, "%s", string);
        out << std::setw(LENGTH_OUTPUT_STRING) << std::left << charShortened;
    }
}

void readOutName(std::ostream& out, PtrDataFrame_t at_ptrFrame)
{
    namefields_s isobusName;
    uint16_t sourceAddress;
    std::vector<uint16_t> listOfAddresses;
    uint64_t fullName = 0;

    // Decode NAME
    isobusName = decodeName(at_ptrFrame->data());

    sourceAddress = at_ptrFrame->sourceAddress();
    for (uint8_t i = 0; i < 8; i++)
    {
        fullName |= ((uint64_t)at_ptrFrame->dataOctet(7 - i) << (7 - i) * BITS_PER_BYTE);
    }

    // Track SA changes for this NAME
    listOfAddresses.push_back(sourceAddress);
    std::pair<std::map<uint64_t, std::vector<uint16_t> >::iterator, bool> ret;
    ret = tNamesAndAddressesMgmt.listOfNamesAndAddresses.insert(std::make_pair(fullName, listOfAddresses));
    if (ret.second == false)
    {
        //Name already existed
        //check if the current source address differs from the last stored source address
        if (ret.first->second.back() != sourceAddress)
        {
            ret.first->second.push_back(sourceAddress);
        }
        //else: the address is equal to the last stored address, nothing needs to be done here
    }
    //else: the new name was inserted, nothing needs to be done here

    // Print out decoded NAME
    out << std::right << std::setfill('0') << std::setw(16) << fullName << ": " << std::left << std::setfill(' ');

    printShortenedString(std::cout, getFunction(isobusName.IndustryGroup, isobusName.DeviceClass, isobusName.Function));
    out << "/";

    printShortenedString(std::cout, getDeviceClass(isobusName.DeviceClass));
    out << "/";

    printShortenedString(std::cout, getManufacturer(isobusName.ManuCode));
    out << "/";

    out << std::setfill(' ') << std::right << std::dec 
    << " SC["   << std::setw(1) << (isobusName.boSelfConfig ? "X" : " ") << "] /"
    << " FI["   << std::setw(2) << isobusName.FunctionInstance << "] /"
    << " DCI["  << std::setw(2) << isobusName.DeviceClassInstance << "] /"
    << " ECUI[" << std::setw(1) << isobusName.EcuInstance << "]";

    //TC Server management
    if (isobusName.Function == 130 /*Task Controller*/)
    {
        if (tNamesAndAddressesMgmt.listOfTcServerSas.empty())
        {
            //vector still empty, so insert SA
            tNamesAndAddressesMgmt.listOfTcServerSas.push_back(sourceAddress);
        }        
        //is SA already stored as TC?
        else if (!boSaIsTc(sourceAddress))
        {
            tNamesAndAddressesMgmt.listOfTcServerSas.push_back(sourceAddress);
        }
        //else: SA already stored as TC, nothing needs to be done here
    }
};

void printFullNameAndAddressList(std::ostream& out) {
  using std::hex;
  namefields_s isobusName;
  std::vector<uint8_t> dataOctet;
  std::map<uint64_t, std::vector<uint16_t> >::iterator it;

  out << std::setfill(' ');
  out << std::endl << std::endl << "Summary of ISOBUS Names with their claimed source addresses: " << std::endl << std::endl;
  out << std::setw(20) <<std::left << "NAME (64bit number)" <<std::setw(30) << "NAME (LE as on CAN)"  <<std::setw(11) <<std::left 
      << "Addresses" << std::setw(100) << "Decoded" << std::endl;
  
  for (it = tNamesAndAddressesMgmt.listOfNamesAndAddresses.begin(); it != tNamesAndAddressesMgmt.listOfNamesAndAddresses.end(); ++it)
  {
    dataOctet.clear();

    //print name (as 64 bit number)
    out << std:: setfill('0') << std::setw(16) <<std::right << std::hex << it->first << "    ";

    //create data octets from full name:
    for (uint8_t i = 0; i < 8; i++)
    {
      dataOctet.push_back(0xFF & it->first >> (i * BITS_PER_BYTE));
    }

    //print name as LE as on CAN bus:
    for (uint8_t i = 0; i < 8; i++)
    {
      out << std::right << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)dataOctet[i] << " ";
    }

    out << "      ";
    //print source address
    for (uint8_t i = 0; i < it->second.size(); ++i)
    {
      if (i > 2)
      {
          out << "."; //to show that not all elements are printed
          break;    // don't print endless lines
      }
      out  << std::right << std::setfill('0') << std::setw(2) << std::hex << it->second[i];
      if (i != it->second.size() - 1)
      {
        out << ",";
      }
    }
    if (it->second.size() * 3 <= 12)
    {
        out << std::setfill(' ') << std::setw(12 - it->second.size() * 3);
    }
    out << std::setfill(' ') << " ";

    isobusName = decodeName(dataOctet);

    //print decoded NAME
    out << std::left << "  Man   DC  DCI  F   FI  EI SC  Serial IG" << std::endl;
    out << std::setw(61) << std::dec << std::right << " "
        << "[" << std::setw(4) << isobusName.ManuCode << "]"
        << "[" << std::setw(3) << isobusName.DeviceClass << "]"
        << "[" << std::setw(2) << isobusName.DeviceClassInstance << "]"
        << "[" << std::setw(3) << isobusName.Function << "]"
        << "[" << std::setw(2) << isobusName.FunctionInstance << "]"
        << "[" << std::setw(1) << isobusName.EcuInstance << "]"
        << "[" << std::setw(1) << (isobusName.boSelfConfig ? "X" : " ") << "]"
        << "[" << std::setw(7) << isobusName.SerNo << "]"
        << "[" << std::setw(1) << isobusName.IndustryGroup << "]__ ";

    out << getIndustryGroup(isobusName.IndustryGroup) << std::endl;

    out << std::setw(63) << " " << " |    |        |__________________________ ";
    out << getFunction(isobusName.IndustryGroup, isobusName.DeviceClass, isobusName.Function) << std::endl;

    out << std::setw(63) << " " << " |    |___________________________________ ";
    out << getDeviceClass(isobusName.DeviceClass) << std::endl;

    out << std::setw(63) << " " << " |________________________________________ ";
    out << getManufacturer(isobusName.ManuCode) << std::endl;
  }
}
