cmake_minimum_required(VERSION 2.8.1)

#message(STATUS "(. ${.})")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

project(LOGALIZER)

add_definitions(
  -D_CRT_SECURE_NO_WARNINGS)

include_directories(
  ../src
  ../../../library/xgpl_src
  ../../../tools/libs/misc)

add_executable(
  logalizer
  ../src/isoaglib_project_config.h
  ../src/logalizer.cpp
  ../src/transfercollection.cpp
  ../src/transfercollection.h
  ../src/alivecollection.cpp
  ../src/alivecollection.h
  ../src/addresstracker.cpp
  ../src/addresstracker.h
  ../src/functionality_fs.inc
  ../src/functionality_gps.inc
  ../src/functionality_nw.inc
  ../src/functionality_tc.inc
  ../src/functionality_tecu.inc
  ../src/functionality_tim.inc
  ../src/functionality_vt.inc
  ../src/dataframe.h
  ../src/inputstream.h
  ../src/logenvirons.h
  ../src/gpx_writer.inc
  ../src/checks.inc
  ../src/parsers.inc)

target_link_libraries(logalizer)
