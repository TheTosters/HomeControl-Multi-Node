/*
 * Parser.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: Zarnowski
 */

#include "MiniInParser.h"


static const char endLineCharacter = 13;

enum class MiniInParserMode : char {
    EXPECT_COMMAND,
    CMD_PARSED,
    DIGIT,
    DIGIT_FIXED,
    DIGIT_SWALLOW,
    STRING,
    NEED_RESET,
} ;

enum class ParserHelperState : char {
    START_OF_MODE,
    IN_PROGRESS,
    EXPECT_EOL,
    SWALLOW_TO_EOL
};

static MiniInParserMode mode = MiniInParserMode::EXPECT_COMMAND;
static ParserHelperState parserHelper = ParserHelperState::START_OF_MODE;
static int8_t parserIndex;
static bool negative;
static uint16_t fracPart;

NetCommand::NetCommand(char* strBuf, uint8_t bufSize)
: cmd(0),
  outParamType(OutParamType::NONE),
  floatValue(0),
  stringValue(strBuf),
  stringValueMaxLen(bufSize)
{
}

void parseCmd(char nextChar, Command* outCmd, ParseResult* result) {
    if (parserHelper == ParserHelperState::START_OF_MODE) {
        outCmd->cmd = 0;
        parserHelper = ParserHelperState::IN_PROGRESS;
    }
    if (nextChar == endLineCharacter) {
        *result = ParseResult::ERROR_NO_CMD;
        mode = MiniInParserMode::NEED_RESET;
        return;
    }

    if (nextChar < 'A' || nextChar > 'Z') {
        *result = ParseResult::ERROR_INVALID_CMD;
        mode = MiniInParserMode::NEED_RESET;
        return;
    }

    outCmd->cmd |= (uint8_t)nextChar;
    outCmd->cmd <<= 8;
    parserIndex++;

    mode = parserIndex == 3 ? MiniInParserMode::CMD_PARSED : MiniInParserMode::EXPECT_COMMAND;
    *result = ParseResult::WILL_CONTINUE;
}

void finalizeParseWithSuccess(ParseResult* result) {
    *result = ParseResult::SUCCESS;
    miniInParserReset();
}

void handleStringArgument(char nextChar, Command* outCmd, ParseResult* result) {

    if (nextChar == endLineCharacter) {
        if (parserHelper != ParserHelperState::EXPECT_EOL) {
            *result = ParseResult::ERROR_MALFORMED;
            mode = MiniInParserMode::NEED_RESET;
            return;

        } else {
            //end of string
            finalizeParseWithSuccess(result);
            return;
        }
    }

    if (parserIndex >= outCmd->stringValueMaxLen - 1) {
        if (nextChar == '"') {
            outCmd->stringValue[parserIndex] = 0;
            parserHelper = ParserHelperState::EXPECT_EOL;
            *result = ParseResult::WILL_CONTINUE;
            mode = MiniInParserMode::STRING;

        } else {
            outCmd->stringValue[outCmd->stringValueMaxLen - 1] = 0;
            *result = ParseResult::ERROR_STRING_OVERFLOW;
            mode = MiniInParserMode::NEED_RESET;
        }
        return;
    }

    if (nextChar == '"') {
      if (parserHelper == ParserHelperState::EXPECT_EOL) {
          *result = ParseResult::ERROR_MALFORMED;
          mode = MiniInParserMode::NEED_RESET;
        
      } else {
          outCmd->stringValue[parserIndex] = 0;
          parserHelper = ParserHelperState::EXPECT_EOL;
          *result = ParseResult::WILL_CONTINUE;
          mode = MiniInParserMode::STRING;
      }
      return;
    }
  
    if (nextChar <' ' || nextChar > '~') {
        *result = ParseResult::ERROR_MALFORMED;
        mode = MiniInParserMode::NEED_RESET;
        return;
    }

    outCmd->stringValue[parserIndex] = nextChar;
    parserIndex++;
    *result = ParseResult::WILL_CONTINUE;
    mode = MiniInParserMode::STRING;
}

void handleDigitArgument(char nextChar, Command* outCmd, ParseResult* result) {
    if (parserHelper == ParserHelperState::START_OF_MODE) {
        outCmd->integerValue = 0;
        negative = nextChar == '-';
        parserHelper = ParserHelperState::IN_PROGRESS;
        if (negative == true) {
            *result = ParseResult::WILL_CONTINUE;
            mode = MiniInParserMode::DIGIT;
            return;
        }
    }
    if (nextChar == endLineCharacter) {
        if (negative) {
            outCmd->integerValue = -outCmd->integerValue;
        }
        finalizeParseWithSuccess(result);
        return;
    }
    if (nextChar == '.') {
        *result = ParseResult::WILL_CONTINUE;
        mode = MiniInParserMode::DIGIT_FIXED;
        parserIndex = 0;
        fracPart = 0;
        outCmd->outParamType = OutParamType::FLOAT_DIGIT;
        return;
    }
    if (nextChar < '0' || nextChar > '9') {
        *result = ParseResult::ERROR_MALFORMED;
        mode = MiniInParserMode::NEED_RESET;
        return;
    }
    outCmd->integerValue *= 10;
    outCmd->integerValue += nextChar - '0';
    *result = ParseResult::WILL_CONTINUE;
    mode = MiniInParserMode::DIGIT;
}

void finalizeFixedDigit(Command* outCmd, ParseResult* result) {
    int base = 1;
    for(;parserIndex != 0; parserIndex --) {
        base *= 10;
    }

    outCmd->floatValue = outCmd->integerValue + (float)fracPart / base;
    if (negative) {
        outCmd->floatValue = -outCmd->floatValue;
    }
    finalizeParseWithSuccess(result);
}

void handleFracDigitArgument(char nextChar, Command* outCmd, ParseResult* result) {
    if (nextChar == endLineCharacter) {
        finalizeFixedDigit(outCmd, result);
        return;
    }

    if (nextChar < '0' || nextChar > '9') {
        *result = ParseResult::ERROR_MALFORMED;
        mode = MiniInParserMode::NEED_RESET;
        return;
    }
    if (parserHelper == ParserHelperState::IN_PROGRESS) {
        fracPart *= 10;
        fracPart += nextChar - '0';
        parserIndex++;
        parserHelper = parserIndex < 3 ? ParserHelperState::IN_PROGRESS : ParserHelperState::SWALLOW_TO_EOL;
    }
    *result = ParseResult::WILL_CONTINUE;
    mode = MiniInParserMode::DIGIT_FIXED;
}

void discoverParam(char nextChar, Command* outCmd, ParseResult* result) {
    if (nextChar == '"') {
        *result = ParseResult::WILL_CONTINUE;
        mode = MiniInParserMode::STRING;
        outCmd->outParamType = OutParamType::STRING;
        parserHelper = ParserHelperState::START_OF_MODE;
        parserIndex = 0;

    } else if (nextChar == '-' || (nextChar >= '0' && nextChar <= '9')) {
        *result = ParseResult::WILL_CONTINUE;
        mode = MiniInParserMode::DIGIT;
        outCmd->outParamType = OutParamType::INT_DIGIT;
        parserHelper = ParserHelperState::START_OF_MODE;
        parserIndex = 0;
        handleDigitArgument(nextChar, outCmd, result);

    } else if (nextChar == endLineCharacter) {
        finalizeParseWithSuccess(result);
        outCmd->outParamType = OutParamType::NONE;

    } else {
        *result = ParseResult::ERROR_MALFORMED;
        mode = MiniInParserMode::NEED_RESET;
    }
}

ParseResult miniInParse(char nextChar, Command* outCmd) {
    ParseResult result = ParseResult::ERROR_MALFORMED;
    switch(mode) {
        default:
        case MiniInParserMode::EXPECT_COMMAND:
            parseCmd(nextChar, outCmd, &result);
            break;

        case MiniInParserMode::CMD_PARSED:
            discoverParam(nextChar, outCmd, &result);
            break;

        case MiniInParserMode::STRING:
            handleStringArgument(nextChar, outCmd, &result);
            break;

        case MiniInParserMode::DIGIT:
            handleDigitArgument(nextChar, outCmd, &result);
            break;

        case MiniInParserMode::DIGIT_FIXED:
            handleFracDigitArgument(nextChar, outCmd, &result);
            break;
        
        case MiniInParserMode::NEED_RESET:
            result = ParseResult::ERROR_NEED_RESET_PARSER;
            break;
    }

    return result;
}

void miniInParserReset() {
    mode = MiniInParserMode::EXPECT_COMMAND;
    parserIndex = 0;
    negative = false;
    fracPart = 0;
    parserHelper = ParserHelperState::START_OF_MODE;
}
