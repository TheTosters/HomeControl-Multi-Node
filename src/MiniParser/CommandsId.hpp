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

CommandsId.hpp
Created on: May 22, 2017
    Author: Bartłomiej Żarnowski (Toster)
*/
#ifndef CommandsId_hpp
#define CommandsId_hpp

//------- Temperature related commands ------
//----
#define CMD_READ_TEMPERATURE_MESUREMENT     0x52544800
#define S_CMD_READ_TEMPERATURE_MESUREMENT   "RTH"

#define CMD_CONFIG_TEMP_READING_PERIOD      0x43545000
#define S_CMD_CONFIG_TEMP_READING_PERIOD    "CTP"

#define CMD_CONFIG_TEMP_MEASURE_RESOLUTION   0x43545200
#define S_CMD_CONFIG_TEMP_MEASURE_RESOLUTION "CTR"

#define CMD_DELIVER_TEMP_HISTORY    0x56544d00
#define S_CMD_DELIVER_TEMP_HISTORY  "VTM"

#define CMD_READ_HUMIDITY_MESUREMENT     0x52484800
#define S_CMD_READ_HUMIDITY_MESUREMENT   "RHH"

#define CMD_DELIVER_HUMIDITY_HISTORY    0x56484d00
#define S_CMD_DELIVER_HUMIDITY_HISTORY  "VHM"

#endif /* CommandsId_hpp */
