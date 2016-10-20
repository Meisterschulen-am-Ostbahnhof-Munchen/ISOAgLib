Xerces-C++ XML Parser
~~~~~~~~~~~~~~~~~~~~~

This library is used in IsoAgLib at least for
  tools/vt2iso
.

= MS Windows =

Files from Xerces-C++ v2.5.0.0 should be copied in here and named:

  xerces-c_2_5_0-windows_nt-msvc_60

Download:

  http://archive.apache.org/dist/xml/xerces-c/Xerces-C_2_5_0/xerces-c_2_5_0-windows_nt-msvc_60.zip

= Linux =

Favor package libxerces-c2-dev over libxerces-c-dev. If these are no longer supported
    (as is decribed here: https://launchpad.net/ubuntu/wily/+source/xerces-c2)
on your packaging system, you can either build them by hand or you can grab the following DEB file:

    https://launchpad.net/ubuntu/+source/xerces-c2/2.8.0+deb1-3build2/+build/7763264

Then install by

    sudo dpkg -i libxerces-c28_2.8.0+deb1-3build2_i386.deb 
    sudo dpkg -i libxerces-c2
