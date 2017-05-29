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

 MeasurementStorage.h
 Created on: May 22, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef MeasurementStorage_hpp
#define MeasurementStorage_hpp

#include <Arduino.h>

class StorageNode {
  public:
    StorageNode* next;
    const uint8_t storageId;
    const uint8_t size;
    const bool isFloat;
    uint8_t count;  //how many are actually inserted
    void* values;
    uint16_t* timestamps;

    StorageNode(uint8_t id, uint8_t size, bool isFloat);
};

class MeasurementStorage {
  public:
    MeasurementStorage();
    void prepareStorage(uint8_t storageId, bool isFloat, int8_t size);
    void addMeasurement(uint8_t storageId, float value);
    void addMeasurement(uint8_t storageId, int32_t value);
    void getMeasurement(uint8_t storageId, int8_t index, float& floatValue, uint16_t& timestamp);
    void getMeasurement(uint8_t storageId, int8_t index, int32_t& intValue, uint16_t& timestamp);
    uint8_t getMeasurementCount(uint8_t storageId);
    void sendHistory(const char* wrappingCommand, uint8_t storageId, uint8_t count);
  private:
    StorageNode* listHead;

    StorageNode* getNode(uint8_t storageId);
    void innerGetMeasurement(StorageNode* node, int8_t index, float& floatValue, uint16_t& timestamp);
    void innerGetMeasurement(StorageNode* node, int8_t index, int32_t& intValue, uint16_t& timestamp);
};

extern MeasurementStorage sharedStorage;
#endif /* MeasurementStorage_hpp */
