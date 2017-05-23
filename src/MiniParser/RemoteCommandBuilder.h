/*
 * RemoteCommandBuilder.h
 *
 *  Created on: Mar 27, 2017
 *      Author: Zarnowski
 */

#ifndef RemoteCommandBuilder_hpp
#define RemoteCommandBuilder_hpp

enum class ElementType : char {UNKNOWN, DIGIT, STRING};

class RemoteCommandBuilder {
    public:
        RemoteCommandBuilder(const char* cmd, char* outCmdBuffer, uint8_t cmdBufferSize);

        void addArgument(const int32_t value);
        void addArgument(const float value);
        void addArgument(char* value);
        void startSequence();
        void endSequence();

        char* buildCommand();
    private:
        char* outCmd;
        uint8_t cmdBufferSize;
        uint8_t index;
        ElementType elementsType;
        bool isSequenceOpen;
        bool needComa;
        bool expectedNextSubsequence;
};

#endif /* RemoteCommandBuilder_hpp */
