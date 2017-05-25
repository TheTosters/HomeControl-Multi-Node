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

 ModuleHM10.h
 Created on: May 21, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef ModuleHM10_hpp
#define ModuleHM10_hpp

#ifdef HW_HM10

#include <Modules/Module.hpp>
#include <MiniParser/RemoteCommandBuilder.h>

//One of Communication bridges, only one com bridge can be found in project
//it's responsible for reading data and execution parser on it
class ModuleHM10: public Module, public ComandSender {
  public:
    ModuleHM10();
    virtual ~ModuleHM10() = default;
    virtual void onLoop() override;
    virtual void doSendCommand(const char* dataToSend) override;
  private:
    bool swallowUntilEnd;
    bool expectPreamble;

    void sendBTCommand(const char * command, bool waitResponse);
    void executeCommand();
};

#endif //HW_HM10

#endif /* ModuleHM10_hpp */
