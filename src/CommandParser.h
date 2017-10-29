//
// Created by Snorri Sturluson on 29/10/2017.
//

#ifndef PROJECT_COMMANDPARSER_H
#define PROJECT_COMMANDPARSER_H


#include <string>
#include "rapidjson/document.h"
#include "Commands.h"

class CommandParser
{
public:
    Command* parse(const std::string& input);

protected:
    void parseAddShip(Command *command, rapidjson::Document &d);

    void parseStepSimulation(Command *command, rapidjson::Document &d);

    void parseGetPositions(Command *command, rapidjson::Document &d);
};


#endif //PROJECT_COMMANDPARSER_H
