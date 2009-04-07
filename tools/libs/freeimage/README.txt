The files located in this directory have been modified to work with MinGW 
installations like Dev-C++. For all other development environments please use 
the original files that are located in the "mingw" directory. 

Dev-C++ Installation
====================
In order to use this version of the freeimage library with Dev-C++ you have to
disable some definitions in the header file. This has already be done in the 
file FreeImage_devcpp.h. You also have to copy the needed library:

  copy FreeImage.h c:\dev-cpp\include\freeimage.h
  copy libfreeimage.a c:\dev-cpp\lib\libfreeimage.a

As last step copy the Freeimage.dll to a directory where your application can
find it and you're all set.