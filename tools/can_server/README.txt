For the PEAK PCAN driver read ../libs/pcan/README.txt .

For the Sontheim driver download
    http://www.s-i-e.de/daten/software/MT_API_Setup.zip
and run the contained installation program. Install to
    C:\Programme\Sontheim\MT_Api .
If it's already installed in a different path, then adapt
    can_server_sontheim_mt.vcproj .

For the Vector driver download
    http://www.vector-worldwide.com/downloads/drivers/canlib43.zip
and run the contained setup program. Install to
    c:\Programme\CANLIB .
If it's already installed in a different path, then adapt
    can_server_vector.vcproj .

For the Vector XL driver download
    http://www.vector-worldwide.com/downloads/drivers/xl_lib67.zip
and run the contained setup program. Install to
    C:\Programme\XL Driver Library .
If it's already installed in a different path, then adapt
    can_server_vector_xl.vcproj .

For the LAWICEL driver download
    http://www.canusb.com/cdm/CDM_2.04.06_WHQL_Certified_CANUSB.zip
and run the contained installation program. Install to
    C:\Programme\LAWICEL\CANUSB .
If it's already installed in a different path, then adapt
    can_server_lawicel.vcproj .

Note: Only if you chose ``Release'' rather than ``Debug'' as MSVC++
solution configuration, then the resulting can server executable will
be copied to ..\..\bin\win32\ during the build.
