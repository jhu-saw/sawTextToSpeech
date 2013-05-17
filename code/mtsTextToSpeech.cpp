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


#include <sawTextToSpeech/mtsTextToSpeech.h>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnPrintf.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstParameterTypes/prmEventButton.h>

#include <stdlib.h>

CMN_IMPLEMENT_SERVICES(mtsTextToSpeech);

mtsTextToSpeech::mtsTextToSpeech(void):
    mtsTaskFromSignal("text-to-speech")
{
    mtsInterfaceProvided * interfaceProvided = this->AddInterfaceProvided("Configuration");
    if (interfaceProvided) {
        interfaceProvided->AddCommandWrite(&mtsTextToSpeech::AddInterfaceRequiredForEventButton, this,
                                           "AddInterfaceRequiredForEventButton");
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: failed to add provided interface \"Configuration\"" << std::endl;
    }
    interfaceProvided = this->AddInterfaceProvided("Commands");
    if (interfaceProvided) {
        interfaceProvided->AddCommandWrite(&mtsTextToSpeech::StringToSpeech, this,
                                           "StringToSpeech");
        interfaceProvided->AddCommandWrite(&mtsTextToSpeech::CharacterToSpeech, this,
                                           "CharacterToSpeech");
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: failed to add provided interface \"Commands\"" << std::endl;
    }

    // MacOS
#if (CISST_OS == CISST_DARWIN)
    StringToSpeechCommand = "say \"%s\"";
#elif (CISST_OS == CISST_LINUX)
    StringToSpeechCommand = "flite -t \"%s\"";
#else
    #error "Sorry, we need to add the proper command line for Unix and use the Win32 API on Windows ...   You should turn off SAW_TextToSpeech for now."
#endif
}

void mtsTextToSpeech::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}

void mtsTextToSpeech::StringToSpeech(const std::string & text)
{
    std::stringstream command;
    command << cmnPrintf(StringToSpeechCommand.c_str()) << text;
    system(command.str().c_str());
}

void mtsTextToSpeech::CharacterToSpeech(const char & character)
{
    std::string text;
    text.push_back(character);
    StringToSpeech(text);
}

void mtsTextToSpeech::ButtonToSpeech(const prmEventButton & button)
{
    std::string text;
    switch (button.Type()) {
    case prmEventButton::PRESSED:
        text = "pressed";
        break;
    case prmEventButton::RELEASED:
        text = "released";
        break;
    default:
        text = "undefined";
    }
    StringToSpeech(text);
}

void mtsTextToSpeech::AddInterfaceRequiredForEventString(const std::string & interfaceName, const std::string & eventName)
{
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired(interfaceName);
    if (interfaceRequired) {
        interfaceRequired->AddEventHandlerWrite(&mtsTextToSpeech::StringToSpeech, this, eventName);
    }
}

void mtsTextToSpeech::AddInterfaceRequiredForEventCharacter(const std::string & interfaceName, const std::string & eventName)
{
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired(interfaceName);
    if (interfaceRequired) {
        interfaceRequired->AddEventHandlerWrite(&mtsTextToSpeech::CharacterToSpeech, this, eventName);
    }
}

void mtsTextToSpeech::AddInterfaceRequiredForEventButton(const std::string & interfaceName)
{
    mtsInterfaceRequired * interfaceRequired = this->AddInterfaceRequired(interfaceName);
    if (interfaceRequired) {
        interfaceRequired->AddEventHandlerWrite(&mtsTextToSpeech::ButtonToSpeech, this, "Button");
    }
}
