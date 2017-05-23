/*
 * RemoteCommandBuilder.cpp
 *
 *  Created on: Mar 27, 2017
 *      Author: Zarnowski
 */

#include <Arduino.h>
#include "RemoteCommandBuilder.h"
#include <Common.h>

// reverses a string 'str' of length 'len'
void reverse(char *str, int len) {
  int i = 0, j = len - 1, temp;
  while (i < j) {
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
}

// Converts a given integer x to string str[].  d is the number
// of digits required in output. If d is more than the number
// of digits in x, then 0s are added at the beginning.
int intToStr(int32_t x, char str[], int d) {
  int i = 0;
  if (x < 0) {
    str[i++] = '-';
    x = -x;
  }

  while (x) {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }

  // If number of digits required is more, then
  // add 0s at the beginning
  while (i < d)
    str[i++] = '0';

  reverse(str, i);
  str[i] = '\0';
  return i;
}

// Converts a floating point number to string.
int ftoa(float n, char *res, int afterpoint) {
  // Extract integer part
  int ipart = (int) n;

  // Extract floating part
  float fpart = n - (float) ipart;

  // convert integer part to string
  int i = intToStr(ipart, res, 0);

  // check for display option after point
  if (afterpoint != 0) {
    res[i] = '.';  // add dot

    // Get the value of fraction part upto given no.
    // of points after dot. The third parameter is needed
    // to handle cases like 233.007
    fpart = fpart * pow(10, afterpoint);
    fpart = fpart < 0 ? -fpart : fpart;

    i += intToStr((int) fpart, res + i + 1, afterpoint);
  }
  return i;
}

RemoteCommandBuilder::RemoteCommandBuilder(const char* cmd, char* outCmdBuffer, uint8_t cmdBufferSize)
: outCmd(outCmdBuffer),
  cmdBufferSize(cmdBufferSize),
  index(3), //we will copy cmd into buffer
  elementsType(ElementType::UNKNOWN),
  isSequenceOpen(false),
  needComa(false),
  expectedNextSubsequence(false) {

  memcpy(outCmd, cmd, 3);
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

    index += intToStr(value, &outCmd[index], 0);
//    outCmd += to_string(value);
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
//  stringstream str;
//  str << fixed << setprecision(3) << value;
//  outCmd += str.str();
  index += ftoa(value, &outCmd[index], 3);
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
//  outCmd += '"';
//  outCmd += value;
//  outCmd += '"';
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
  //outCmd += "(";
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
  //outCmd += ")";
  outCmd[index++] = ')';
  needComa = false;
  expectedNextSubsequence = true;
}

char* RemoteCommandBuilder::buildCommand() {
  if (isSequenceOpen == true) {
    INTERNAL_FAIL();
    return nullptr;
  }
  //tmp += "\r";
  outCmd[index++] = '\r';
  outCmd[index] = 0;

  return outCmd;
}
