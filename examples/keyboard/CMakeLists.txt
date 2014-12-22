#
# (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# create a list of required cisst libraries
set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstVector
                              cisstOSAbstraction
                              cisstMultiTask
                              cisstParameterTypes)

# find cisst and make sure the required libraries have been compiled
find_package (cisst 1.0 COMPONENTS ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # required saw components
  cisst_find_saw_component (sawKeyboard 1.0 REQUIRED)
  cisst_find_saw_component (sawTextToSpeech 1.0 REQUIRED)

  if (sawKeyboard_FOUND AND sawTextToSpeech_FOUND)

    # sawRobotIO1394 configuration
    include_directories (${sawKeyboard_INCLUDE_DIR} ${sawTextToSpeech_INCLUDE_DIR})
    link_directories (${sawKeyboard_LIBRARY_DIR} ${sawTextToSpeech_INCLUDE_DIR})

    add_executable (sawTextToSpeechExampleKeyboard main.cpp)
    set_property (TARGET sawTextToSpeechExampleKeyboard PROPERTY FOLDER "sawTextToSpeech")

    # link against non cisst libraries and cisst components
    target_link_libraries (sawTextToSpeechExampleKeyboard
                           ${sawKeyboard_LIBRARIES}
                           ${sawTextToSpeech_LIBRARIES})

    # link against cisst libraries (and dependencies)
    cisst_target_link_libraries (sawTextToSpeechExampleKeyboard ${REQUIRED_CISST_LIBRARIES})

  endif (sawKeyboard_FOUND AND sawTextToSpeech_FOUND)

endif (cisst_FOUND_AS_REQUIRED)
