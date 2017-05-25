/*
 * RemoteCommandBuilder.cpp
 *
 *  Created on: Mar 27, 2017
 *      Author: Zarnowski
 */

#include <Arduino.h>
#include "RemoteCommandBuilder.h"
#include <Common.h>
#include <string.h>

RemoteCommandBuilder::RemoteCommandBuilder(char* outCmdBuffer, uint8_t cmdBufferSize)
: outCmd(outCmdBuffer),
  cmdBufferSize(cmdBufferSize),
  index(0), //we will copy cmd into buffer
  elementsType(ElementType::UNKNOWN),
  isSequenceOpen(false),
  needComa(false),
  expectedNextSubsequence(false),
  sender(nullptr) {

}

void RemoteCommandBuilder::setSender(ComandSender* sender) {
  this->sender = sender;
}

void RemoteCommandBuilder::setCommand(const char* cmd) {
  memcpy(outCmd, cmd, 3);
  index = 3;
  elementsType = ElementType::UNKNOWN;
  isSequenceOpen = false;
  needComa = false;
  expectedNextSubsequence = false;
}

void RemoteCommandBuilder::addArgument(const int32_t value) {
    if ((elementsType == ElementType::STRING) || (expectedNextSubsequence == true)) {
        INTERNAL_FAIL();
        return;
    }

    elementsType = ElementType::DIGIT;
    if (needComa == true) {
        outCmd[index++] = ',';
    }
    String s(value);
    strcpy(&outCmd[index], s.c_str());
    index += s.length();
    needComa = true;
}

void RemoteCommandBuilder::addArgument(const float value) {
  if ((elementsType == ElementType::STRING) || (expectedNextSubsequence == true)) {
    INTERNAL_FAIL();
    return;
  }
  elementsType = ElementType::DIGIT;
  if (needComa == true) {
    outCmd[index++] = ',';
  }

  String s(value, 3);
  strcpy(&outCmd[index], s.c_str());
  index += s.length();
  needComa = true;
}

void RemoteCommandBuilder::addArgument(char* value) {
  if ((elementsType == ElementType::DIGIT) || (expectedNextSubsequence == true)) {
    INTERNAL_FAIL();
    return;
  }
  elementsType = ElementType::STRING;
  if (needComa == true) {
    outCmd[index++] = ',';
  }
  outCmd[index++] = '"';
  while( (*value) != 0) {
    outCmd[index++] = *value;
    value++;
  }
  outCmd[index++] = '"';
  needComa = true;
}

void RemoteCommandBuilder::startSequence() {
  if (isSequenceOpen == true) {
    INTERNAL_FAIL();
    return;
  }
  isSequenceOpen = true;
  outCmd[index++] = '(';
  needComa = false;
  expectedNextSubsequence = false;
}

void RemoteCommandBuilder::endSequence() {
  if ((isSequenceOpen == false) || (needComa == false)) {
    INTERNAL_FAIL();
    return;
  }

  isSequenceOpen = false;
  outCmd[index++] = ')';
  needComa = false;
  expectedNextSubsequence = true;
}

void RemoteCommandBuilder::buildAndSendCommand() {
  if (isSequenceOpen == true) {
    INTERNAL_FAIL();
  }
  outCmd[index++] = '\r';
  outCmd[index] = 0;

  sender->doSendCommand(outCmd);
}
