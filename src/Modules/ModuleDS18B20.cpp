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

#define DEFAULT_READ_PERIOD (5UL * 60 * 1000)
#define DS18B20_STORAGE_ID  0x0d

#ifndef DS18B20_STORAGE_SIZE
#define DS18B20_STORAGE_SIZE 10
#endif

ModuleDS18B20::ModuleDS18B20()
: Module(10),
  lastMeasurementTimeStamp( millis() ),
  measurementPeriod( DEFAULT_READ_PERIOD ),
  address{0},
  sensor(new DS18B20(&oneWire) ){

  sharedStorage.prepareStorage(DS18B20_STORAGE_ID, true, DS18B20_STORAGE_SIZE);
}

void ModuleDS18B20::onLoop() {
  if (millis() - lastMeasurementTimeStamp < measurementPeriod) {
    return;
  }
  //do reading
  bool requestRet = sensor->request(address);
  if (requestRet) {
    while (!sensor->available()) {
      //timeout is included in the sensor
    }

    float result = sensor->readTemperature(address);

    if (result != TEMP_ERROR) {
      sharedStorage.addMeasurement(DS18B20_STORAGE_ID, result);
    }

  } else {
    //todo: ignore?
  }
}

bool ModuleDS18B20::handleCommand() {
  //todo: write this
  return false;
}
#endif //HW_DS18B20
