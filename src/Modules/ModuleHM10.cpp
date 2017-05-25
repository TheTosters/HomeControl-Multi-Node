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
#include <string.h>

#define PREAMBLE_END_CHARACTER '#'
static const char* CONNECTION_NOTIF_STR = "OK+CONN";

ModuleHM10::ModuleHM10() : Module(50), expectPreamble(true), connState(strlen(CONNECTION_NOTIF_STR)) {
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
  //this send AT control command to HM10
  char buffer[24];
  Serial.begin(9600);
  Serial.write((const uint8_t*) command, strlen(command));
  if (waitResponse) {
    Serial.setTimeout(800);
    Serial.readBytes(buffer, ARRAY_SIZE(buffer));
  }
}

void ModuleHM10::doSendCommand(const char* dataToSend) {
  //this send Node protocol encoded command to connected client
  Serial.write((const uint8_t*)dataToSend, strlen(dataToSend));
  Serial.flush();
}

void ModuleHM10::checkForConnectionNotification(const char letter) {

  if (connState <= strlen(CONNECTION_NOTIF_STR)) {
    uint8_t index = strlen(CONNECTION_NOTIF_STR) - connState;
    if (CONNECTION_NOTIF_STR[index] == letter) {
      connState--;
    } else {
      connState = strlen(CONNECTION_NOTIF_STR);
    }
  }
}

void ModuleHM10::onLoop() {
  while(Serial.available()) {
    int tmp = Serial.read();
    if (tmp == -1) {
      break;  //shouldn't happend
    }
    //DEBUG: Serial.write('0'+connState);
    if (connState != 0) {
      checkForConnectionNotification((const char)tmp);
      continue;
    }
    //on error or other sad thing
    if (swallowUntilEnd) {
      swallowUntilEnd = tmp != 13;
      continue;
    }
    //DEBUG: Serial.write('A'+expectPreamble);
    if (expectPreamble) {
      expectPreamble = tmp != PREAMBLE_END_CHARACTER;
      continue;
    }

    ParseResult result = miniInParse( (char)tmp, &incommingCommand);
    //DEBUG: Serial.write('a'+(char)result);
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
