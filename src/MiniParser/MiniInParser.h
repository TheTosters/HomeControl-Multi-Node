/*
 * MiniInParser.h
 *
 *  Created on: Mar 22, 2017
 *      Author: Zarnowski
 */

#ifndef MiniInParser_hpp
#define MiniInParser_hpp

#include <stdint.h>

enum class OutParamType : char {
    NONE,
    INT_DIGIT,     //signed!
    FLOAT_DIGIT,
    STRING,
};

struct NetCommand {
    uint32_t        cmd;
    OutParamType    outParamType;
    union{
        uint64_t    integerValue;
        float       floatValue;
    };
    char*           stringValue;
    uint8_t         stringValueMaxLen;

    NetCommand(char* strBuf, uint8_t bufSize);
};

typedef struct NetCommand Command;

enum class ParseResult : char {
    WILL_CONTINUE,
    ERROR_NO_CMD,   //Syntax error -> there was no command in stream
    ERROR_INVALID_CMD,    //Syntax error -> command is not made by [A-Z] symbols
    ERROR_STRING_OVERFLOW,    //to long string in argument
    ERROR_MALFORMED,   //General error in syntax
    ERROR_NEED_RESET_PARSER, //last command was malformed/errored call miniInParserReset()
    SUCCESS,     //Successfully parsed, logic can interpret result
};


ParseResult miniInParse(char nextChar, Command* outCmd);
void miniInParserReset();

#endif /* MiniInParser_hpp */
