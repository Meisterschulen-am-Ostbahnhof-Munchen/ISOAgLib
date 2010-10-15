cmake_minimum_required(VERSION 2.8.1)

#message(STATUS "(. ${.})")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

project(LOGALIZER)

include_directories(
  ../../../library/xgpl_src
  ../../../tools/libs/misc)

add_executable(
  logalizer
  ../src/logalizer.cpp)

target_link_libraries(logalizer)
