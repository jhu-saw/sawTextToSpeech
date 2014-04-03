/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*

  Author(s):  Anton Deguet
  Created on: 2013-05-15

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

// system
#include <iostream>
// cisst/saw
#include <cisstOSAbstraction/osaSleep.h>
#include <sawKeyboard/mtsKeyboard.h>
#include <sawTextToSpeech/mtsTextToSpeech.h>

int main(int CMN_UNUSED(argc), char ** CMN_UNUSED(argv))
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cerr, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);

    mtsManagerLocal * manager = mtsManagerLocal::GetInstance();

    // components
    mtsKeyboard * keyboard = new mtsKeyboard;
    mtsTextToSpeech * textToSpeech = new mtsTextToSpeech;

    // add an interface to handle keyboard events "Key"
    textToSpeech->AddInterfaceRequiredForEventCharacter("Keyboard", "Key");

    manager->AddComponent(keyboard);
    manager->AddComponent(textToSpeech);

    manager->Connect(textToSpeech->GetName(), "Keyboard",
                     keyboard->GetName(), "Keyboard");

    // create the component threads
    manager->CreateAll();
    manager->WaitForStateAll(mtsComponentState::READY, 2.0 * cmn_s);

    // start the periodic Run
    manager->StartAll();
    manager->WaitForStateAll(mtsComponentState::ACTIVE , 2.0 * cmn_s);

    std::cout << "Hit 'q' to quit.  Any other key should trigger text to speech." << std::endl;
    while (!keyboard->Done()) {
        std::cout << "." << std::flush;
        osaSleep(1.0 * cmn_s);
    }

    // cleanup
    manager->KillAll();
    manager->WaitForStateAll(mtsComponentState::FINISHED, 2.0 * cmn_s);
    manager->Cleanup();

    // delete component
    delete textToSpeech;
    delete keyboard;

    // stop all logs
    cmnLogger::Kill();

    return 0;
}
