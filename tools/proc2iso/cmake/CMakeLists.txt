cmake_minimum_required(VERSION 2.8.1)

#message(STATUS "(. ${.})")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

project(PROC2ISO)

add_definitions(
  -DHAL_PATH_ISOAGLIB=IsoAgLib/hal/pc
  -DHAL_PATH_ISOAGLIB_SYSTEM=IsoAgLib/hal/pc/system
  -DOPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  -DCAN_INSTANCE_CNT=1
  -DAPP_NO_THREADS
  -DXML_USE_NO_THREADS
  -DUSE_FREE_IMAGE_LIB)

include_directories(
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
	../../../library/xgpl_src/IsoAgLib/util/impl/util_funcs.cpp)

target_link_libraries(proc2iso ${ISOAGLIB_ADDITIONAL_LIBRARIES})
