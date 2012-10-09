TEMPLATE = app
CONFIG = warn_on debug console stl
CONFIG -= qt
CONFIG -= x11
INCPATH -= -I/usr/include

OBJECTS_DIR = objects

QMAKE_CFLAGS_DEBUG += -fno-builtin -Wall -Woverloaded-virtual -Wpointer-arith -Wstrict-prototypes

QMAKE_CXXFLAGS_DEBUG += -fno-builtin -Wall -Woverloaded-virtual -Wpointer-arith -Wstrict-prototypes
