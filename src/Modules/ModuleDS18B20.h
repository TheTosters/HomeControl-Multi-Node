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

 ModuleDS18B20.h
 Created on: May 21, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef ModuleDS18B20_hpp
#define ModuleDS18B20_hpp

#ifdef HW_DS18B20

#include <Modules/Module.hpp>
#include <Arduino.h>
#include <DS18B20.h>


class ModuleDS18B20: public Module {
  public:
    ModuleDS18B20();
    virtual ~ModuleDS18B20() = default;
    virtual void onLoop() override;
    virtual bool handleCommand() override;

  private:
    unsigned long lastMeasurementTimeStamp;
    unsigned long measurementPeriod;
    uint8_t  address[8];
    uint8_t  resolution; //allowed 9-12
    DS18B20* sensor;

    void doMeasurement();
    void handleConfigPeriod();
    void handleConfigResolution();
    void handleMeasurement();
    void findSensor();
};

#endif //HW_DS18B20

#endif /* ModuleDS18B20_hpp */
