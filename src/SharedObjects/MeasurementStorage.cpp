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

 MeasurementStorage.cpp
 Created on: May 22, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <SharedObjects/MeasurementStorage.h>
#include <Common.h>
#include <SharedObjects/IncommingCommands.h>

MeasurementStorage sharedStorage;

StorageNode::StorageNode(uint8_t id, uint8_t size, bool isFloat)
: next(nullptr),
  storageId(id),
  size(size),
  isFloat(isFloat),
  count(0),
  values( isFloat ? malloc(size * sizeof(float)) : malloc(size * sizeof(int32_t) ) ),
  timestamps( (uint16_t*)malloc(size * sizeof(uint16_t))) {
}

MeasurementStorage::MeasurementStorage() : listHead(nullptr) {

}

void MeasurementStorage::prepareStorage(uint8_t storageId, bool isFloat, int8_t size) {
  StorageNode* node = new StorageNode(storageId, size, isFloat);
  node->next = listHead;
  listHead = node;
}

void MeasurementStorage::addMeasurement(uint8_t storageId, float value) {
  StorageNode* node = getNode(storageId);
  if (node) {
    if (node->isFloat == false) {
      addMeasurement(storageId, static_cast<int32_t>(value));
    } else {
      uint8_t index = 0;
      if (node->size > 1) {
        if (node->count == node->size) {
          memcpy(node->values, ((uint8_t*) node->values) + sizeof(float), (node->size - 1) * sizeof(float));
          memcpy(node->timestamps, node->timestamps + 1, (node->size - 1) * sizeof(uint16_t));
          index = node->count - 1;
        } else {
          index = node->count;
          node->count++;
        }
      }
      float* tmp = (float*)node->values;
      tmp[index] = value;
      node->timestamps[index] = millis() / 1000;
    }
  }
}

void MeasurementStorage::addMeasurement(uint8_t storageId, int32_t value) {
  StorageNode* node = getNode(storageId);
  if (node) {
    if (node->isFloat == true) {
      addMeasurement(storageId, static_cast<float>(value));
    } else {
      uint8_t index = 0;
      if (node->size > 0) {
        if (node->count == node->size) {
          memcpy(node->values, ((uint8_t*) node->values) + sizeof(int32_t), (node->size - 1) * sizeof(int32_t));
          memcpy(node->timestamps, node->timestamps + 1, (node->size - 1) * sizeof(uint16_t));
          index = node->count - 1;
        } else {
          index = node->count;
          node->count++;
        }
      }
      int32_t* tmp = (int32_t*) node->values;
      tmp[index] = value;
      node->timestamps[index] = millis() / 1000;
    }
  }
}

void MeasurementStorage::getMeasurement(uint8_t storageId, int8_t index, float& floatValue, uint16_t& timestamp) {
  StorageNode* node = getNode(storageId);
  if (node) {
    if (node->isFloat) {
      innerGetMeasurement(node, index, floatValue, timestamp);
    } else {
      int32_t tmp;
      innerGetMeasurement(node, index, tmp, timestamp);
      floatValue = static_cast<float>(tmp);
    }
  } else {
    floatValue = 0;
    timestamp = 0;
  }
}

void MeasurementStorage::getMeasurement(uint8_t storageId, int8_t index, int32_t& intValue, uint16_t& timestamp) {
  StorageNode* node = getNode(storageId);
  if (node) {
    if (node->isFloat) {
      float tmp;
      innerGetMeasurement(node, index, tmp, timestamp);
      intValue = static_cast<int32_t>(tmp);
    } else {
      innerGetMeasurement(node, index, intValue, timestamp);
    }
  } else {
    intValue = 0;
    timestamp = 0;
  }
}

void MeasurementStorage::innerGetMeasurement(StorageNode* node, int8_t index, float& floatValue, uint16_t& timestamp) {
  if (index >= 0 && index < node->count) {
      timestamp = node->timestamps[node->count - index - 1];
      floatValue = ((float*) node->values)[node->count - index - 1];
    } else {
      timestamp = 0;
      floatValue = 0;
    }
}

void MeasurementStorage::innerGetMeasurement(StorageNode* node, int8_t index, int32_t& intValue, uint16_t& timestamp) {
  if (index >= 0 && index < node->count) {
    timestamp = node->timestamps[node->count - index - 1];
    intValue = ((int32_t*) node->values)[node->count - index - 1];
  } else {
    timestamp = 0;
    intValue = 0;
  }
}

uint8_t MeasurementStorage::getMeasurementCount(uint8_t storageId) {
  StorageNode* node = getNode(storageId);
  return node ? node->count : 0;
}

StorageNode* MeasurementStorage::getNode(uint8_t storageId) {
  StorageNode* tmp = listHead;
  while(tmp != nullptr) {
    if (tmp->storageId == storageId) {
      return tmp;
    }
  }

  return nullptr;
}

void MeasurementStorage::sendHistory(const char* wrappingCommand, uint8_t storageId, uint8_t count) {
  StorageNode* node = getNode(storageId);
  if (node == nullptr) {
    return;
  }
  count = count > node->count ? node->count : count;
  unsigned long now = millis() / 1000;
  remoteCommandBuilder.setCommand(wrappingCommand);
  uint16_t timestamp;
  for(int t = 0; t < count; t++) {
    remoteCommandBuilder.startSequence();
    if (node->isFloat) {
      float value;
      innerGetMeasurement(node, t, value, timestamp);
      remoteCommandBuilder.addArgument( (int32_t)(now - timestamp) );
      remoteCommandBuilder.addArgument(value);

    } else {
      int32_t value;
      innerGetMeasurement(node, t, value, timestamp);
      remoteCommandBuilder.addArgument( (int32_t)(now - timestamp) );
      remoteCommandBuilder.addArgument(value);
    }
    remoteCommandBuilder.endSequence();
  }
  remoteCommandBuilder.finalize();
}
