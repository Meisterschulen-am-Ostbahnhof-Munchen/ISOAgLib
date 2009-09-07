Please note:
------------

To run the Visual Studio 2005 binaries you will need the VC8 runtime components that
are available in the Microsoft Visual C++ 2005 Redistributable Package (2.5MB):

  http://www.microsoft.com/Downloads/details.aspx?FamilyID=32bc1bee-a3f9-4c13-9c99-220b62a191ee&displaylang=en

CAN SERVER:
-----------

In order to run the can_server variants for

- PEAK
- Sontheim
- Vector Informatik

you probably need driver specific runtime libraries that can be obtained on the manufactures website:

Vector:
  http://www.vector-worldwide.com/downloads/drivers/canlib43.zip
Vector XL:
  http://www.vector.com/downloads/drivers/CANcaseXL_2k_XP_Vista_69.zip
  http://www.vector-worldwide.com/downloads/drivers/xl_lib67.zip
PEAK:
  http://p103112.typo3server.info/fileadmin/media/files/usb.zip
Sontheim:
  http://www.s-i-e.de/daten/software/MT_API_Setup.zip

The Pthread runtime library will be required in every case, and can be copied from here to your local dynamic
library search path. (e.g c:\winnt\system32)

VT2ISO:
-------

vt2iso.exe is linked against Freeimage.dll. Please make a copy if needed.
