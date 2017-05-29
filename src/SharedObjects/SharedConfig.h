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

 SharedConfig.h
 Created on: May 29, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef SharedConfig_hpp
#define SharedConfig_hpp

#include <Arduino.h>

class SharedConfig {
  public:
    SharedConfig();
    /**
     * Returns slotId, it should be used to read/write operations. If slotsCount > 1 then valid id's are:
     * returnedId up to returnedId+slotsCount. For example:
     * int8_t id = reserveSlots(3);
     * writeSlot(id+2, 123); //<< to write data into 3rd reserved slot.
     */
    int8_t reserveSlots(int slotsCount);
    void writeSlot(int slotId, uint32_t data);
    uint32_t readSlot(int slotId);
  private:
    uint8_t nextFreeSlot;
};

extern SharedConfig sharedConfig;
#endif /* SharedConfig_hpp */
