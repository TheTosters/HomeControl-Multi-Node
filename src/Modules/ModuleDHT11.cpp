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

 ModuleDHT11.cpp
 Created on: May 29, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */

#ifdef HW_DHT11

#include <Modules/ModuleDHT11.h>
#include <SharedObjects/MeasurementStorage.h>
#include <SharedObjects/SharedConfig.h>
#include <SharedObjects/IncommingCommands.h>
#include <MiniParser/CommandsId.hpp>
#include <MiniParser/RemoteCommandBuilder.h>
#include <Common.h>
#include <math.h>

#define DEFAULT_READ_PERIOD (5UL * 60 * 1000)
#define MIN_MEASUREMEND_PERIOD (5 * 1000)
#define MAX_MEASUREMEND_PERIOD (72UL * 60 * 1000)

#define DHT11_TEMP_STORAGE_ID  0x0e
#define DHT11_HUM_STORAGE_ID  0x0f

#ifndef DHT11_STORAGE_SIZE
#define DHT11_STORAGE_SIZE 10
#endif

//those offsets are used for config purpouses
#define TEMP_MEASUREMENT_PERIOD_CONFIG 0
#define HUM_MEASUREMENT_PERIOD_CONFIG 1

ModuleDHT11::ModuleDHT11()
: Module(10),
  sharedConfId( sharedConfig.reserveSlots(2) ),
  lastTempMeasurementTimeStamp( millis() ),
  tempMeasurementPeriod( sharedConfig.readSlot(sharedConfId + TEMP_MEASUREMENT_PERIOD_CONFIG) ),
  lastHumMeasurementTimeStamp( millis() ),
  humMeasurementPeriod( sharedConfig.readSlot(sharedConfId + HUM_MEASUREMENT_PERIOD_CONFIG) ),
  sensor(new DHT(ONE_WIRE_PIN, DHT11) ) {

  sharedStorage.prepareStorage(DHT11_TEMP_STORAGE_ID, true, DHT11_STORAGE_SIZE);
  sharedStorage.prepareStorage(DHT11_HUM_STORAGE_ID, true, DHT11_STORAGE_SIZE);
  sensor->begin();
}

void ModuleDHT11::onLoop() {
  if (millis() - lastTempMeasurementTimeStamp >= tempMeasurementPeriod) {
    doTempMeasurement();
  }

  if (millis() - lastHumMeasurementTimeStamp >= humMeasurementPeriod) {
    doHumMeasurement();
  }
}

void ModuleDHT11::doTempMeasurement() {
  float tmp = sensor->readTemperature();
  if (isnan(tmp) == false) {
    sharedStorage.addMeasurement(DHT11_TEMP_STORAGE_ID, tmp);
    lastTempMeasurementTimeStamp = millis();
  }
}

void ModuleDHT11::doHumMeasurement() {
  float tmp = sensor->readHumidity();
  if (isnan(tmp) == false) {
    sharedStorage.addMeasurement(DHT11_HUM_STORAGE_ID, tmp);
    lastHumMeasurementTimeStamp = millis();
  }
}

bool ModuleDHT11::handleCommand() {
  bool result = false;
  switch (incommingCommand.cmd) {
    case CMD_READ_TEMPERATURE_MESUREMENT:
      result = handleTemperatureMeasurement();
      break;

    case CMD_READ_HUMIDITY_MESUREMENT:
      result = handleHumidityMeasurement();
      break;

    case CMD_CONFIG_TEMP_READING_PERIOD:
      result = handleTempConfigPeriod();
      break;

    case CMD_CONFIG_HUMIDITY_READING_PERIOD:
      result = handleHumidityConfigPeriod();
      break;
  }

  return result;
}

bool ModuleDHT11::handleHumidityMeasurement() {
  bool result = false;
  if (incommingCommand.outParamType == OutParamType::NONE) {
    //do measurement right now, store in history and return it
    doHumMeasurement();
    sharedStorage.sendHistory(S_CMD_DELIVER_HUMIDITY_HISTORY, DHT11_HUM_STORAGE_ID, 1);
    result = true;

  } else if (incommingCommand.outParamType == OutParamType::INT_DIGIT) {
    //request some portion of historical data
    sharedStorage.sendHistory(S_CMD_DELIVER_HUMIDITY_HISTORY, DHT11_HUM_STORAGE_ID, incommingCommand.integerValue);
    result = true;
  }
  return result;
}

bool ModuleDHT11::handleTemperatureMeasurement() {
  bool result = false;
  if (incommingCommand.outParamType == OutParamType::NONE) {
    //do measurement right now, store in history and return it
    doTempMeasurement();
    sharedStorage.sendHistory(S_CMD_DELIVER_TEMP_HISTORY, DHT11_TEMP_STORAGE_ID, 1);
    result = true;

  } else if (incommingCommand.outParamType == OutParamType::INT_DIGIT) {
    //request some portion of historical data
    sharedStorage.sendHistory(S_CMD_DELIVER_TEMP_HISTORY, DHT11_TEMP_STORAGE_ID, incommingCommand.integerValue);
    result = true;
  }
  return result;
}

bool ModuleDHT11::handleHumidityConfigPeriod() {
  bool result = false;
  if (incommingCommand.outParamType == OutParamType::NONE) {
    //this was query, respond to it
    remoteCommandBuilder.setCommand(S_CMD_CONFIG_HUMIDITY_READING_PERIOD);
    remoteCommandBuilder.addArgument( (int32_t) humMeasurementPeriod / 1000);
    remoteCommandBuilder.finalize();
    result = true;

  } else if (incommingCommand.outParamType == OutParamType::INT_DIGIT) {
    //this was set
    humMeasurementPeriod = incommingCommand.integerValue * 1000;
    humMeasurementPeriod = humMeasurementPeriod < MIN_MEASUREMEND_PERIOD ? MIN_MEASUREMEND_PERIOD : humMeasurementPeriod;
    sharedConfig.writeSlot(sharedConfId + HUM_MEASUREMENT_PERIOD_CONFIG, humMeasurementPeriod);
    result = true;
  }
  return result;
}

bool ModuleDHT11::handleTempConfigPeriod() {
  bool result = false;
  if (incommingCommand.outParamType == OutParamType::NONE) {
    //this was query, respond to it
    remoteCommandBuilder.setCommand(S_CMD_CONFIG_TEMP_READING_PERIOD);
    remoteCommandBuilder.addArgument( (int32_t) tempMeasurementPeriod / 1000);
    remoteCommandBuilder.finalize();
    result = true;

  } else if (incommingCommand.outParamType == OutParamType::INT_DIGIT) {
    //this was set
    tempMeasurementPeriod = incommingCommand.integerValue * 1000;
    tempMeasurementPeriod = tempMeasurementPeriod < MIN_MEASUREMEND_PERIOD ? MIN_MEASUREMEND_PERIOD : tempMeasurementPeriod;
    sharedConfig.writeSlot(sharedConfId + TEMP_MEASUREMENT_PERIOD_CONFIG, tempMeasurementPeriod);
    result = true;
  }
  return result;
}
#endif //HW_DHT11
