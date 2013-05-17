#
# $Id$
#
# (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# Core library
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstVector
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstParameterTypes)

find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # create/configure file for find_package (sawTextToSpeech)
  set (sawTextToSpeech_INCLUDE_DIR "${sawTextToSpeech_SOURCE_DIR}/include")
  set (sawTextToSpeech_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
  set (sawTextToSpeech_LIBRARIES sawTextToSpeech)

  include_directories (${sawTextToSpeech_INCLUDE_DIR})
  set (sawTextToSpeech_HEADER_DIR "${sawTextToSpeech_INCLUDE_DIR}/sawTextToSpeech")
  add_library (sawTextToSpeech
               mtsTextToSpeech.cpp
               ${sawTextToSpeech_HEADER_DIR}/sawTextToSpeechExport.h
               ${sawTextToSpeech_HEADER_DIR}/mtsTextToSpeech.h)
  set_property (TARGET sawTextToSpeech PROPERTY FOLDER "sawTextToSpeech")
  cisst_target_link_libraries (sawTextToSpeech ${REQUIRED_CISST_LIBRARIES})

  configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/sawTextToSpeechConfig.cmake.in"
                  "${sawTextToSpeech_BINARY_DIR}/sawTextToSpeechConfig.cmake"
                   @ONLY@)

  # Install target for headers and library
  install (DIRECTORY
           ${sawTextToSpeech_INCLUDE_DIR}/sawTextToSpeech
           DESTINATION include
           PATTERN .svn EXCLUDE)

  install (TARGETS sawTextToSpeech
           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib)

  install (FILES "${sawTextToSpeech_BINARY_DIR}/sawTextToSpeechConfig.cmake"
           DESTINATION "${CISST_CMAKE_INSTALL_SUFFIX}/saw")

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
