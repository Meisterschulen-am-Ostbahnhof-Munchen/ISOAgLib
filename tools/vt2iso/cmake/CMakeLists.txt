cmake_minimum_required(VERSION 2.8.1)

#message(STATUS "(. ${.})")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

project(VT2ISO)

#add_custom_command(
#  OUTPUT build_info_dummy.h # a file which is not created by command, so that the command will always be executed!
#  if(WIN32)
#    COMMAND ./create_build_info.bat
#  else()
#    COMMAND ./create_build_info.sh
#  endif(WIN32)
#)




add_definitions(
  -DAPP_NO_THREADS
  -DXML_USE_NO_THREADS
  -D_CRT_SECURE_NO_WARNINGS
  -D_SCL_SECURE_NO_WARNINGS)

include_directories(
  ../src
  ../../libs/freeimage
  ${CMAKE_CURRENT_BINARY_DIR})

# a custom target that is always built
add_custom_target(svnheader ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/svnheader.h)

# creates svnheader.h using cmake script
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/svnheader.h
    COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/getsvn.cmake)

# svnversion.h is a generated file
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/svnversion.h
    PROPERTIES GENERATED TRUE
    HEADER_FILE_ONLY TRUE)


if(WIN32)
  set(BOOST_ROOT "../../libs/boost/boost_1_38_0")
  include_directories(
    ${BOOST_ROOT}
    ../../libs/xerces-c/xerces-c_2_5_0-windows_nt-msvc_60/include)
  link_directories(
    ../../libs/xerces-c/xerces-c_2_5_0-windows_nt-msvc_60/lib
    ../../libs/freeimage)
  set(ISOAGLIB_ADDITIONAL_LIBRARIES User32 Gdi32 Ws2_32 xerces-c_2 Advapi32)
else()
  link_directories(
    ../../libs/freeimage)
  set(ISOAGLIB_ADDITIONAL_LIBRARIES xerces-c)
endif(WIN32)


find_package(Boost REQUIRED)

add_executable(
  vt2iso
  ../src/vt2iso.cpp
  ../src/vt2iso-defines.cpp
  ../src/vt2iso-globals.cpp
  ../src/vt2iso.cpp
  ../src/vt2isoimagebase_c.cpp
  ../src/vt2isoimagefreeimage_c.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/svnversion.h)

target_link_libraries(vt2iso freeimage ${ISOAGLIB_ADDITIONAL_LIBRARIES})

# explicitly say that the executable depends on the svnheader
add_dependencies(vt2iso svnheader)
