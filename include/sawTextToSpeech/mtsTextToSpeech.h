/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-15

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <sawTextToSpeech/sawTextToSpeechRevision.h>

// Always include last
#include <sawTextToSpeech/sawTextToSpeechExport.h>

// forward declaration for class containing members specific to each
// OS/TTS package used
class mtsTextToSpeechInternal;

/*! Provides a simple component to convert text to speech.  The
  current implementation uses command lines tools on Mac OS (say) and
  Linux (flite).  On Windows, this component uses the native API.

  The component has two default provided interfaces.  The
  "Configuration" interface provides the command "SetPreemptive" (see
  method with same name).  The interface "Commands" provides the
  commands "StringToSpeech" and "CharacterToSpeech".

  This component can also be configured by adding required interface
  with event observers, see methods
  AddInterfaceRequiredForEventString,
  AddInterfaceRequiredForEventCharacter and
  AddInterfaceRequiredForEventButton.  All these methods add a
  required interface (unless said interface already exists) and an
  event handler.
*/
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

    /*! Add a required interface with an event observer using a string
      as payload.  If the interface already exists, the event is added
      to the existing interface. */
    void AddInterfaceRequiredForEventString(const std::string & interfaceName, const std::string & eventName);

    /*! Add a required interface with an event observer using a
      character as payload.  If the interface already exists, the
      event is added to the existing interface. */
    void AddInterfaceRequiredForEventCharacter(const std::string & interfaceName, const std::string & eventName);

    /*! Add a required interface with an event observer using a
      prmEventButton payload.  If the interface already exists, the
      event is added to the existing interface. */
    void AddInterfaceRequiredForEventButton(const std::string & interfaceName);

    /*! Turn on or off the preemptive mode, i.e. at each Run
      execution, all messages received are dequeued and only the
      last one is used for text to speech. */
    void SetPreemptive(const bool & preemptive);

 protected:
    mtsTextToSpeechInternal * Internals;
    std::string StringToSpeechCommand;
    void StringToSpeech(const std::string & text);
    void StringToSpeechInternal(const std::string & text);
    void CharacterToSpeech(const char & character);
    void ButtonToSpeech(const prmEventButton & button);
    bool Preemptive;
    std::string LastString;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTextToSpeech);

#endif // _mtsTextToSpeech_h
