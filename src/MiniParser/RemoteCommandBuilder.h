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
    virtual void doSend(const char singleChar) = 0;
    virtual void doSendBuffer(const char* buff, uint8_t bufSize) = 0;
};

class RemoteCommandBuilder {
    public:
        RemoteCommandBuilder();

        void setSender(ComandSender* sender);
        void setCommand(const char* cmd);

        void addArgument(const int32_t value);
        void addArgument(const float value);
        void addArgument(char* value);

        void startSequence();
        void endSequence();

        void finalize();
    private:
        ElementType elementsType;
        bool isSequenceOpen;
        bool needComa;
        bool expectedNextSubsequence;
        ComandSender* sender;
};

#endif /* RemoteCommandBuilder_hpp */
