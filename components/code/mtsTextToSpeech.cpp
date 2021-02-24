/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-15

  (C) Copyright 2013-2021 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <QThread>
#include <QTextToSpeech>
#include <QVoice>
#include <QLocale>
#include <QVector>

CMN_IMPLEMENT_SERVICES(mtsTextToSpeech);

mtsTextToSpeech::mtsTextToSpeech(void):
    mtsTaskFromSignal("text-to-speech"),
    Internals(0),
    Preemptive(false),
    LastString(""),
    LastBeep(vct3(0.0))
{
    mtsInterfaceProvided * interfaceProvided = this->AddInterfaceProvided("Configuration");
    if (interfaceProvided) {
        interfaceProvided->AddCommandWrite(&mtsTextToSpeech::AddInterfaceRequiredForEventButton, this,
                                           "AddInterfaceRequiredForEventButton");
        interfaceProvided->AddCommandWrite(&mtsTextToSpeech::SetPreemptive, this,
                                           "SetPreemptive");
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: failed to add provided interface \"Configuration\"" << std::endl;
    }
    interfaceProvided = this->AddInterfaceProvided("Commands");
    if (interfaceProvided) {
        interfaceProvided->AddCommandWrite(&mtsTextToSpeech::StringToSpeech, this,
                                           "StringToSpeech");
        interfaceProvided->AddCommandWrite(&mtsTextToSpeech::CharacterToSpeech, this,
                                           "CharacterToSpeech");
        interfaceProvided->AddCommandWrite(&mtsTextToSpeech::Beep, this,
                                           "Beep");
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "constructor: failed to add provided interface \"Commands\"" << std::endl;
    }


    // MacOS
#if (CISST_OS == CISST_DARWIN)
    StringToSpeechCommand = "say \"%s\"";
#elif (CISST_OS == CISST_LINUX)
    StringToSpeechCommand = "echo \"%s\" | espeak -s120 -k20";
#elif (CISST_OS == CISST_WINDOWS)
    Internals = new mtsTextToSpeechInternal;
#else
    #error "Sorry, we need to add the proper command line for this operating system ...   You should turn off SAW_TextToSpeech for now."
#endif


    // Qt test
    // List the available engines.
    QStringList engines = QTextToSpeech::availableEngines();
    std::cerr << "Available engines:";
    for (auto engine : engines) {
        std::cerr << "  " << engine.toStdString();
    }
    // Create an instance using the default engine/plugin.
    QTextToSpeech *speech = new QTextToSpeech();
    // List the available locales.
    std::cerr << "Available locales:";
    for (auto locale : speech->availableLocales()) {
        std::cerr << "  " << locale.name().toStdString();
    }
    // Set locale.
    speech->setLocale(QLocale(QLocale::English, QLocale::LatinScript, QLocale::UnitedStates));
    // List the available voices.
    std::cerr << "Available voices:";
    for (auto voice : speech->availableVoices()) {
        std::cerr << "  " << voice.name().toStdString();
    }
    // Display properties.
    std::cerr << "Locale:" << speech->locale().name().toStdString();
    std::cerr << "Pitch:" << speech->pitch();
    std::cerr << "Rate:" << speech->rate();
    std::cerr << "Voice:" << speech->voice().name().toStdString();
    std::cerr << "Volume:" << speech->volume();
    std::cerr << "State:" << speech->state();
    // Say something.
    speech->say("Hello, world! This is the Qt speech engine.");
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
    LastString = "";
    LastBeep.SetAll(0.0);
    ProcessQueuedCommands();
    ProcessQueuedEvents();
    if (Preemptive && (LastString != "")) {
        StringToSpeechInternal(LastString);
    }
    if (Preemptive && (LastBeep != 0.0)) {
        BeepInternal(LastBeep);
    }
}

void mtsTextToSpeech::Cleanup(void)
{
#if (CISST_OS == CISST_WINDOWS)
    Internals->Voice->Release();
    Internals->Voice = 0;
    CoUninitialize();
#endif // CISST_WINDOWS
}

void mtsTextToSpeech::StringToSpeechInternal(const std::string & text)
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
    int result = system(command.str().c_str());
    if (result < 0) {
        CMN_LOG_CLASS_RUN_ERROR << "StringToSpeechInternal: failed to execute system call for \"" << command.str() << "\"" << std::endl;
    }
#endif
}

void mtsTextToSpeech::StringToSpeech(const std::string & text)
{
    if (Preemptive) {
        LastString = text;
    }
    else {
        StringToSpeechInternal(text);
    }
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

void mtsTextToSpeech::Beep(const vct3 & durationFrequencyAmplitude)
{
    if (Preemptive) {
        LastBeep = durationFrequencyAmplitude;
    }
    else {
        BeepInternal(durationFrequencyAmplitude);
    }
}

void mtsTextToSpeech::BeepInternal(const vct3 & durationFrequencyAmplitude)
{
#if (CISST_OS == CISST_DARWIN)
    std::cerr << CMN_LOG_DETAILS << " not implemented yet " << durationFrequencyAmplitude << std::endl;
#elif (CISST_OS == CISST_LINUX)
    std::stringstream command;
    command << "play -q -n -t alsa"
            << " synth " << durationFrequencyAmplitude.Element(0)
            << " sine " << durationFrequencyAmplitude.Element(1)
            << " vol " << durationFrequencyAmplitude(2);
    int result = system(command.str().c_str());
    if (result < 0) {
        CMN_LOG_CLASS_RUN_ERROR << "StringToSpeechInternal: failed to execute system call for \"" << command.str() << "\"" << std::endl;
    }
#elif (CISST_OS == CISST_WINDOWS)
    std::cerr << CMN_LOG_DETAILS << " not implemented yet " << std::endl;
#endif
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

void mtsTextToSpeech::SetPreemptive(const bool & preemptive)
{
    Preemptive = preemptive;
}
