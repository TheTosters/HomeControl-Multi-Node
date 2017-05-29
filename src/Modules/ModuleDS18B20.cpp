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

 ModuleDS18B20.cpp
 Created on: May 21, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */

#ifdef HW_DS18B20

#include <Modules/ModuleDS18B20.h>
#include <SharedObjects/OneWireShared.h>
#include <SharedObjects/MeasurementStorage.h>
#include <SharedObjects/SharedConfig.h>
#include <SharedObjects/IncommingCommands.h>
#include <MiniParser/CommandsId.hpp>
#include <MiniParser/RemoteCommandBuilder.h>
#include <Common.h>

#define DEFAULT_READ_PERIOD (5UL * 60 * 1000)
#define MIN_MEASUREMEND_PERIOD (5 * 1000)
#define MAX_MEASUREMEND_PERIOD (72UL * 60 * 1000)

#define MIN_RESOLUTION 9
#define MAX_RESOLUTION 12
#define DEFAULT_RESOLUTION 12

#define DS18B20_STORAGE_ID  0x0d

#ifndef DS18B20_STORAGE_SIZE
#define DS18B20_STORAGE_SIZE 10
#endif

//those offsets are used for config purpouses
#define MEASUREMENT_PERIOD_CONFIG 0
#define RESOLUTION_CONFIG 1

ModuleDS18B20::ModuleDS18B20()
: Module(10),
  sharedConfId( sharedConfig.reserveSlots(2) ),
  lastMeasurementTimeStamp( millis() ),
  measurementPeriod( sharedConfig.readSlot(sharedConfId + MEASUREMENT_PERIOD_CONFIG) ),
  address{0},
  sensor(new DS18B20(&oneWire) ) {

  sharedStorage.prepareStorage(DS18B20_STORAGE_ID, true, DS18B20_STORAGE_SIZE);
  //validate values, in case of firs boot and no EEPROM data
  if ((measurementPeriod < MIN_MEASUREMEND_PERIOD) || (measurementPeriod >= MAX_MEASUREMEND_PERIOD)) {
    measurementPeriod = DEFAULT_READ_PERIOD;
    sharedConfig.writeSlot(sharedConfId + MEASUREMENT_PERIOD_CONFIG, measurementPeriod);
  }

}

uint8_t ModuleDS18B20::getResolution() {
  uint8_t resolution = sharedConfig.readSlot(sharedConfId + RESOLUTION_CONFIG);
  if (resolution < MIN_RESOLUTION || resolution > MAX_RESOLUTION) {
    resolution = DEFAULT_RESOLUTION;
    sharedConfig.writeSlot(sharedConfId + RESOLUTION_CONFIG, resolution);
  }
  return resolution;
}

void ModuleDS18B20::onLoop() {
  if (millis() - lastMeasurementTimeStamp < measurementPeriod) {
    return;
  }
  doMeasurement();
}

void ModuleDS18B20::doMeasurement() {
  //do reading now
  if (address[0] != 0x28) {
    findSensor();
    sensor->begin(getResolution());
  }
  bool requestRet = sensor->request(address);
  if (requestRet) {
    while (!sensor->available()) {
      //timeout is included in the sensor
    }
    float result = sensor->readTemperature(address);
    if (result != TEMP_ERROR) {
      sharedStorage.addMeasurement(DS18B20_STORAGE_ID, result);
      lastMeasurementTimeStamp = millis();
    }

  } else {
    INTERNAL_FAIL();
  }
}

bool ModuleDS18B20::handleConfigPeriod() {
  bool result = false;
  if (incommingCommand.outParamType == OutParamType::NONE) {
    //this was query, respond to it
    remoteCommandBuilder.setCommand(S_CMD_CONFIG_TEMP_READING_PERIOD);
    remoteCommandBuilder.addArgument( (int32_t) measurementPeriod / 1000);
    remoteCommandBuilder.finalize();
    result = true;

  } else if (incommingCommand.outParamType == OutParamType::INT_DIGIT) {
    //this was set
    measurementPeriod = incommingCommand.integerValue * 1000;
    measurementPeriod = measurementPeriod < MIN_MEASUREMEND_PERIOD ? MIN_MEASUREMEND_PERIOD : measurementPeriod;
    sharedConfig.writeSlot(sharedConfId + MEASUREMENT_PERIOD_CONFIG, measurementPeriod);
    result = true;
  }
  return result;
}

bool ModuleDS18B20::handleConfigResolution() {
  bool result = false;
  if (incommingCommand.outParamType == OutParamType::NONE) {
    //this was query, respond to it
    remoteCommandBuilder.setCommand(S_CMD_CONFIG_TEMP_MEASURE_RESOLUTION);
    remoteCommandBuilder.addArgument((int32_t)getResolution());
    remoteCommandBuilder.finalize();
    result = true;

  } else if (incommingCommand.outParamType == OutParamType::INT_DIGIT) {
    //this was set
    uint8_t resolution = constrain(incommingCommand.integerValue, MIN_RESOLUTION, MAX_RESOLUTION);
    sharedConfig.writeSlot(sharedConfId + RESOLUTION_CONFIG, resolution);
    sensor->begin(resolution);
    result = true;
  }
  return result;
}

bool ModuleDS18B20::handleMeasurement() {
  bool result = false;
  if (incommingCommand.outParamType == OutParamType::NONE) {
    //do measurement right now, store in history and return it
    doMeasurement();
    sharedStorage.sendHistory(S_CMD_DELIVER_TEMP_HISTORY, DS18B20_STORAGE_ID, 1);
    result = true;

  } else if (incommingCommand.outParamType == OutParamType::INT_DIGIT) {
    //request some portion of historical data
    sharedStorage.sendHistory(S_CMD_DELIVER_TEMP_HISTORY, DS18B20_STORAGE_ID, incommingCommand.integerValue);
    result = true;
  }
  return result;
}

bool ModuleDS18B20::handleCommand() {
  bool result = false;
  switch(incommingCommand.cmd) {
    case CMD_READ_TEMPERATURE_MESUREMENT:
      result = handleMeasurement();
      break;

    case CMD_CONFIG_TEMP_READING_PERIOD:
      result = handleConfigPeriod();
      break;

    case CMD_CONFIG_TEMP_MEASURE_RESOLUTION:
      result = handleConfigResolution();
      break;
  }

  return result;
}

void ModuleDS18B20::findSensor() {
  while (oneWire.search(address)) {
    //not temp sensor
    if (address[0] != 0x28) {
      continue;
    }
    //Incorrect crc
    if (OneWire::crc8(address, 7) != address[7]) {
      continue;

    } else {
      //found sensor
      return;
    }
  }
  memset(address, 0, sizeof(address));
}

#endif //HW_DS18B20
