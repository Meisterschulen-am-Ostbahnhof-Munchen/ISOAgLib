About this commercial BIOS directory
------------------------------------

Note that working on Windows or Linux no commercial_BIOS is needed.

If you're compiling for an embedded target, you may already have the according 
BIOS available, so you would just need to pass the path into your IsoAgLib 
configuration file (conf_<projectname>).

If you'd like to use the default, then you just need to copy the commercial 
BIOS into an bios_<platform> directory (e.g. bios_esx) or you can still
checkout the subrepositories (that were formerly used) to there. In this case
please get in contact with OSB AG.

Valid directory-names for the available HALs are:
 - bios_a2
 - bios_c2c
 - bios_Dj1
 - bios_esx
 - bios_esxu
 - bios_imi
 - bios_mitron167
 - bios_pm167

