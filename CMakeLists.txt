#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# qpfgui - QPF HMI Application
#======================================================================
# Author: J C Gonzalez - 2015-2018
# Copyright (C) 2015-2018  Euclid SOC Team at ESAC
#======================================================================

cmake_minimum_required(VERSION 2.8.2)
cmake_policy (SET CMP0015 NEW)

set (MACOSX_CC clang)
set (MACOSX_CXX clang++)
set (MACOSX_CXX_FLAGS "-std=c++11 -stdlib=libc++")

set (MACOSX_DEBUG "-g -gmodules -glldb")

set (CMAKE_CC_COMPILER ${MACOSX_CXX})
set (CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS} ${MACOSX_DEBUG}")

set (CMAKE_CXX_COMPILER ${MACOSX_CXX})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MACOSX_DEBUG} ${MACOSX_CXX_FLAGS}")


project (wd)

#set (wdLib_src
#)

#set (wdLib_hdr
#)

set (wdBin_src
  main.cpp
)

add_definitions(-DCMAKE_BUILD_TYPE=Debug)

include_directories (. ..
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR})

#set (wd_LIB_DIR ${PWD})
#link_directories (${wd_LIB_DIR})

#----------------------------------------------------------------------

#add_library (wdlib SHARED ${wdLib_src})
#target_link_libraries (wdlib gd)
#target_include_directories (wdlib PUBLIC . /usr/local/include)

#----------------------------------------------------------------------

add_executable(wd ${wdBin_src})
#add_dependencies (cellular_automata wdlib)
target_include_directories (wd PUBLIC . /usr/local/include)
#target_link_libraries (wd gd cfitsio)
set_target_properties (wd PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS wd 
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
