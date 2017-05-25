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

RemoteCommandBuilder::RemoteCommandBuilder()
: elementsType(ElementType::UNKNOWN),
  isSequenceOpen(false),
  needComa(false),
  expectedNextSubsequence(false),
  sender(nullptr) {

}

void RemoteCommandBuilder::setSender(ComandSender* sender) {
  this->sender = sender;
}

void RemoteCommandBuilder::setCommand(const char* cmd) {
  sender->doSendBuffer(cmd, 3);
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
        sender->doSend(',');
    }
    String s(value);
    sender->doSendBuffer(s.c_str(), s.length());
    needComa = true;
}

void RemoteCommandBuilder::addArgument(const float value) {
  if ((elementsType == ElementType::STRING) || (expectedNextSubsequence == true)) {
    INTERNAL_FAIL();
    return;
  }
  elementsType = ElementType::DIGIT;
  if (needComa == true) {
    sender->doSend(',');
  }

  String s(value, 3);
  sender->doSendBuffer(s.c_str(), s.length());
  needComa = true;
}

void RemoteCommandBuilder::addArgument(char* value) {
  if ((elementsType == ElementType::DIGIT) || (expectedNextSubsequence == true)) {
    INTERNAL_FAIL();
    return;
  }
  elementsType = ElementType::STRING;
  if (needComa == true) {
    sender->doSend(',');
  }
  sender->doSend('"');
  sender->doSendBuffer(value, strlen(value));
  sender->doSend('"');
  needComa = true;
}

void RemoteCommandBuilder::startSequence() {
  if (isSequenceOpen == true) {
    INTERNAL_FAIL();
    return;
  }
  isSequenceOpen = true;
  sender->doSend('(');
  needComa = false;
  expectedNextSubsequence = false;
}

void RemoteCommandBuilder::endSequence() {
  if ((isSequenceOpen == false) || (needComa == false)) {
    INTERNAL_FAIL();
    return;
  }

  isSequenceOpen = false;
  sender->doSend(')');
  needComa = false;
  expectedNextSubsequence = true;
}

void RemoteCommandBuilder::finalize() {
  if (isSequenceOpen == true) {
    INTERNAL_FAIL();
  }
  sender->doSend('\r');
}
