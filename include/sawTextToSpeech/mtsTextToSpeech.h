/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2013-05-15

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsTextToSpeech_h
#define _mtsTextToSpeech_h

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstParameterTypes/prmEventButton.h>

// Always include last
#include <sawTextToSpeech/sawTextToSpeechExport.h>

// forward declaration for class containing members specific to each
// OS/TTS package used
class mtsTextToSpeechInternal;

/*! */
class CISST_EXPORT mtsTextToSpeech: public mtsTaskFromSignal
{
    // declare services, requires dynamic creation
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 public:
    mtsTextToSpeech(void);
    ~mtsTextToSpeech(void);
    inline void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    void Run(void);
    void Cleanup(void);

    void AddInterfaceRequiredForEventString(const std::string & interfaceName, const std::string & eventName);
    void AddInterfaceRequiredForEventCharacter(const std::string & interfaceName, const std::string & eventName);
    void AddInterfaceRequiredForEventButton(const std::string & interfaceName);

 protected:
    mtsTextToSpeechInternal * Internals;
    std::string StringToSpeechCommand;
    void StringToSpeech(const std::string & text);
    void CharacterToSpeech(const char & character);
    void ButtonToSpeech(const prmEventButton & button);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTextToSpeech);

#endif // _mtsTextToSpeech_h
