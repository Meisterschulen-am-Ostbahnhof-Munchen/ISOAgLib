cmake_minimum_required(VERSION 2.8.1)

#message(STATUS "(. ${.})")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

project(PROC2ISO)

add_definitions(
  -DAPP_NO_THREADS
  -D_CRT_SECURE_NO_WARNINGS
  -DXML_USE_NO_THREADS)

include_directories(
  ../src
  ../../../library/xgpl_src)

if(WIN32)
  include_directories(
    ../../libs/xerces-c/xerces-c_2_5_0-windows_nt-msvc_60/include)
  link_directories("../../libs/xerces-c/xerces-c_2_5_0-windows_nt-msvc_60/lib")
  set(ISOAGLIB_ADDITIONAL_LIBRARIES User32 Gdi32 Ws2_32 xerces-c_2 Advapi32)
else()
  set(ISOAGLIB_ADDITIONAL_LIBRARIES xerces-c)
endif(WIN32)

add_executable(
  proc2iso
	../src/proc2iso.cpp
	../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.cpp
	../../../library/xgpl_src/IsoAgLib/util/impl/util_funcs.cpp
	../../../library/xgpl_src/IsoAgLib/util/impl/flexiblebytestrings.cpp)

target_link_libraries(proc2iso ${ISOAGLIB_ADDITIONAL_LIBRARIES})
