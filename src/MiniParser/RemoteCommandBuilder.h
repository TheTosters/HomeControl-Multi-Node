/*
 * RemoteCommandBuilder.h
 *
 *  Created on: Mar 27, 2017
 *      Author: Zarnowski
 */

#ifndef RemoteCommandBuilder_hpp
#define RemoteCommandBuilder_hpp

enum class ElementType : char {UNKNOWN, DIGIT, STRING};

class ComandSender {
  public:
    virtual ~ComandSender() = default;
    virtual void doSendCommand(const char* dataToSend) = 0;
};

class RemoteCommandBuilder {
    public:
        RemoteCommandBuilder(char* outCmdBuffer, uint8_t cmdBufferSize);

        void setSender(ComandSender* sender);
        void setCommand(const char* cmd);

        void addArgument(const int32_t value);
        void addArgument(const float value);
        void addArgument(char* value);

        void startSequence();
        void endSequence();

        void buildAndSendCommand();
    private:
        char* outCmd;
        uint8_t cmdBufferSize;
        uint8_t index;
        ElementType elementsType;
        bool isSequenceOpen;
        bool needComa;
        bool expectedNextSubsequence;
        ComandSender* sender;
};

#endif /* RemoteCommandBuilder_hpp */
