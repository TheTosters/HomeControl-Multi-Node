/*
 BSD 3-Clause License

 Copyright (c) 2017, The Tosters
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ModuleHM10.cpp
 Created on: May 21, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifdef HW_HM10

#include <Arduino.h>
#include <Common.h>
#include <Modules/ModuleHM10.h>
#include <SharedObjects/IncommingCommands.h>
#include <MiniParser/MiniInParser.h>
#include <ModulesControl.h>

#define PREAMBLE_END_CHARACTER '#'

ModuleHM10::ModuleHM10() : Module(50), expectPreamble(true) {
  Serial.begin(9600);

  //init bluetooth
  sendBTCommand("AT+RENEW", true);
  sendBTCommand("AT+MODE0", true);
  sendBTCommand("AT+ADVI8", true);
  sendBTCommand("AT+NAME", false);
  sendBTCommand("HC_M_NODE", true);
  sendBTCommand("AT+POWE3", true);
  sendBTCommand("AT+NOTI1", true);

  sendBTCommand("AT+RESET", true);
  //Lets try to keep it default and go to sleep. Perhapts the LED will go off anyway
  sendBTCommand("AT+PIO11", true);  //put LED off when not commencted
  sendBTCommand("AT+SLEEP", true);
}

void ModuleHM10::sendBTCommand(const char * command, bool waitResponse) {
    char buffer[24];
    Serial.begin(9600);
    Serial.write((const uint8_t*)command, strlen(command));
    if (waitResponse) {
        Serial.setTimeout(800);
        Serial.readBytes(buffer, ARRAY_SIZE(buffer));
    }
}

void ModuleHM10::onLoop() {
  while(Serial.available()) {
    int tmp = Serial.read();
    if (tmp == -1) {
      break;  //shouldn't happend
    }

    //on error or other sad thing
    if (swallowUntilEnd) {
      swallowUntilEnd = tmp != 13;
      continue;
    }

    if (expectPreamble) {
      expectPreamble = tmp != PREAMBLE_END_CHARACTER;
      continue;
    }

    ParseResult result = miniInParse( (char)tmp, &incommingCommand);
    switch(result) {
      case ParseResult::WILL_CONTINUE:
        break;

      case ParseResult::SUCCESS:
        handleCommandByModules();
        miniInParserReset();
        expectPreamble = true;
        break;

      default:
        swallowUntilEnd = true;
        expectPreamble = true;
        break;
    }
  }
}

#endif //HW_HM10
