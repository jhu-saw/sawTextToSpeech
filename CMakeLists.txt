#
# (C) Copyright 2011-2011 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

cmake_minimum_required(VERSION 3.10)
project (sawTextToSpeechAll VERSION 1.3.0)

if (UNIX)
  set (CPACK_PACKAGE_VENDOR "JHU")
  set (CPACK_DEBIAN_PACKAGE_MAINTAINER "anton.deguet@jhu.edu")
  set (CPACK_GENERATOR "DEB")
  set (CPACK_DEB_PACKAGE_COMPONENT ON)
  set (CPACK_DEB_COMPONENT_INSTALL ON)
  set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  set (CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
endif (UNIX)

add_subdirectory (components)

set (sawTextToSpeech_DIR "${sawTextToSpeechAll_BINARY_DIR}/components")
add_subdirectory (examples)

include (CPack)
cpack_add_component (sawTextToSpeech)
cpack_add_component (sawTextToSpeechExamples
                     DEPENDS sawTextToSpeech)
