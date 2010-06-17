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
  -DXML_USE_NO_THREADS
  -DHAL_PATH_ISOAGLIB=IsoAgLib/hal/pc
  -DOPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  -DUSE_FREE_IMAGE_LIB
  -DOPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  -DHAL_PATH_ISOAGLIB_SYSTEM=IsoAgLib/hal/pc/system
  -DSYSTEM_PC_VC)

include_directories(
  ../../../library/xgpl_src)

if(WIN32)
  # TODO check this:
  include_directories(
    ../../libs/boost/boost_1_38_0
    ../../libs/xerces-c/xerces-c_2_5_0-windows_nt-msvc_60/include)
  link_directories("../../libs/xerces-c/xerces-c_2_5_0-windows_nt-msvc_60/lib")
  set(ISOAGLIB_ADDITIONAL_LIBRARIES User32 Gdi32 Ws2_32 xerces-c_2 Advapi32)
else()
  set(ISOAGLIB_ADDITIONAL_LIBRARIES xerces-c)
endif(WIN32)

find_package(Boost REQUIRED)

add_executable(
  proc2iso
	../src/proc2iso.cpp
	../../../library/xgpl_src/IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.cpp
	../../../library/xgpl_src/IsoAgLib/util/impl/util_funcs.cpp)

target_link_libraries(proc2iso ${ISOAGLIB_ADDITIONAL_LIBRARIES})
