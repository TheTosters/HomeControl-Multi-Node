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

ModulesControl.cpp
Created on: May 21, 2017
    Author: Bartłomiej Żarnowski (Toster)
*/

#include <Arduino.h>
#include <Modules/ModuleDS18B20.h>
#include <Modules/ModuleHM10.h>
#include <Modules/ModuleDHT11.h>
#include <ModulesControl.h>
#include <SharedObjects/IncommingCommands.h>

Module* modulesHead = nullptr;

static void addModule(Module* module) {

  if (modulesHead == nullptr || modulesHead->priority > module->priority) {
    module->next = modulesHead;
    modulesHead = module;

  } else {
    Module* tmp = modulesHead;
    Module* prev = nullptr;
    while(tmp != nullptr && tmp->priority < module->priority) {
      prev = tmp;
      tmp = tmp->next;
    }
    module->next = prev->next;
    prev->next = module;
  }
}

void setupModules() {

#ifdef HW_DS18B20
  addModule(new ModuleDS18B20());
#endif

#ifdef HW_DHT11
  addModule(new ModuleDHT11());
#endif

#ifdef HW_HM10
  ModuleHM10* hm10 = new ModuleHM10();
  addModule(hm10);
  remoteCommandBuilder.setSender(hm10);
#endif
}

void handleCommandByModules() {
  Module* cur = modulesHead;
  while (cur != nullptr) {
    cur->handleCommand();
    cur = cur->next;
  }
}
