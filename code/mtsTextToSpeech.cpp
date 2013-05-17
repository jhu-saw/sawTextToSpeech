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

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
#include <sapi.h>
#include <malloc.h>

class mtsTextToSpeechInternal {
public:
    HRESULT HResult;
    CLSID CLSId;
    ISpVoice * Voice;
};
#else // CISST_WINDOWS

// other OSs use system to start a command line tool
#include <stdlib.h>

#endif

CMN_IMPLEMENT_SERVICES(mtsTextToSpeech);

mtsTextToSpeech::mtsTextToSpeech(void):
    mtsTaskFromSignal("text-to-speech"),
    Internals(0)
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
#elif (CISST_OS == CISST_WINDOWS)
    Internals = new mtsTextToSpeechInternal;
#else
    #error "Sorry, we need to add the proper command line for this operating system ...   You should turn off SAW_TextToSpeech for now."
#endif
}

mtsTextToSpeech::~mtsTextToSpeech()
{
#if (CISST_OS == CISST_WINDOWS)
    delete Internals;
    Internals = 0;
#endif // CISST_WINDOWS
}

void mtsTextToSpeech::Startup(void)
{
#if (CISST_OS == CISST_WINDOWS)
    Internals->HResult = CoInitialize(0);
    Internals->HResult = CLSIDFromProgID(OLESTR("SAPI.SpVoice"), &(Internals->CLSId));

    if (FAILED(Internals->HResult)) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: failed to retrieve CLSID for COM server" << std::endl;
        return;
    }
    Internals->Voice = 0;
    Internals->HResult = CoCreateInstance(Internals->CLSId, NULL, CLSCTX_INPROC_SERVER,
                                          __uuidof(ISpVoice), (LPVOID *)&(Internals->Voice));

    if (FAILED(Internals->HResult)) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: failed to start COM server" << std::endl;
        return;
    }
    Internals->Voice->SetVolume(100);
#endif // CISST_WINDOWS
}

void mtsTextToSpeech::Run(void)
{
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}

void mtsTextToSpeech::Cleanup(void)
{
#if (CISST_OS == CISST_WINDOWS)
    Internals->Voice->Release();
    Internals->Voice = 0;
    CoUninitialize();
#endif // CISST_WINDOWS
}

void mtsTextToSpeech::StringToSpeech(const std::string & text)
{
#if (CISST_OS == CISST_WINDOWS)
    // convert to wide char string
    int size = MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, 0, 0);
    wchar_t * buffer = (wchar_t *)_malloca(size * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, buffer, size);
    Internals->Voice->Speak(buffer, SPF_IS_XML, 0);
#else
    std::stringstream command;
    command << cmnPrintf(StringToSpeechCommand.c_str()) << text;
    system(command.str().c_str());
#endif
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
