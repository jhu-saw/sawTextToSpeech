#
# (C) Copyright 2013-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

cmake_minimum_required (VERSION 3.16)
project (sawTextToSpeechExamples VERSION 1.5.0)

find_package (sawKeyboard 1.4.1 QUIET)
if (sawKeyboard_FOUND)
  add_subdirectory (keyboard)
else ()
  message ("sawTextToSpeach example requires sawKeyboard, skipped.")
endif ()
