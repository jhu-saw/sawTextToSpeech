#
# (C) Copyright 2013-2023 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

cmake_minimum_required (VERSION 3.10)
project (sawTextToSpeech VERSION 1.3.0)

# Core libraries
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstVector
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstParameterTypes)

find_package (cisst 1.1.0 COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # catkin/ROS paths
  cisst_set_output_path ()

  # create/configure file for find_package (sawTextToSpeech)
  set (sawTextToSpeech_INCLUDE_DIR
       "${sawTextToSpeech_SOURCE_DIR}/include"
       "${sawTextToSpeech_BINARY_DIR}/include")
  set (sawTextToSpeech_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
  set (sawTextToSpeech_LIBRARIES sawTextToSpeech)

  # add all config files for this component
  cisst_add_config_files (sawTextToSpeech)

  include_directories (BEFORE ${sawTextToSpeech_INCLUDE_DIR})
  set (sawTextToSpeech_HEADER_DIR "${sawTextToSpeech_SOURCE_DIR}/include/sawTextToSpeech")
  add_library (sawTextToSpeech
               code/mtsTextToSpeech.cpp
               ${sawTextToSpeech_HEADER_DIR}/sawTextToSpeechExport.h
               ${sawTextToSpeech_HEADER_DIR}/mtsTextToSpeech.h
               ${sawTextToSpeech_BINARY_DIR}/include/sawTextToSpeech/sawTextToSpeechRevision.h)

  set_target_properties (sawTextToSpeech PROPERTIES
                         VERSION ${sawTextToSpeech_VERSION}
                         FOLDER "sawTextToSpeech")

  cisst_target_link_libraries (sawTextToSpeech ${REQUIRED_CISST_LIBRARIES})

  # Install target for headers and library
  install (DIRECTORY "${sawTextToSpeech_SOURCE_DIR}/include/sawTextToSpeech"
                     "${sawTextToSpeech_BINARY_DIR}/include/sawTextToSpeech"
           DESTINATION include
           COMPONENT sawTextToSpeech-dev
           PATTERN .svn EXCLUDE)

  install (TARGETS sawTextToSpeech COMPONENT sawTextToSpeech
           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib)

else (cisst_FOUND_AS_REQUIRED)
  message ("Information: code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled, it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
