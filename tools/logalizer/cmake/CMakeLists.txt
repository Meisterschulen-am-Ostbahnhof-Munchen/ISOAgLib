cmake_minimum_required(VERSION 2.8.1)

#message(STATUS "(. ${.})")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

project(LOGALIZER)

add_definitions(
  -DOPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN
  -DCAN_INSTANCE_CNT=1)

include_directories(
  ../src
  ../../../library/xgpl_src
  ../../../tools/libs/misc)

add_executable(
  logalizer
  ../src/logalizer.cpp
  ../src/transfercollection.cpp
  ../src/alivecollection.cpp
  ../src/addresstracker.cpp)

target_link_libraries(logalizer)
