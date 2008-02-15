Dev-C++ Installation
====================
In order to use this version of the freeimage library with Dev-C++ you have to
disable some definitions in the header file. This has already be done in the 
file FreeImage_devcpp.h. You also have to copy the needed library:

  copy FreeImage_devcpp.h c:\dev-cpp\include\freeimage.h
  copy libfreeimage_devcpp.a c:\dev-cpp\lib\libfreeimage.a

libfreeimage.a is the static linked library for linux and therefore will not
work with Dev-Cpp!

As last step copy the Freeimage.dll to a directory where your application can
find it and you're all set.

