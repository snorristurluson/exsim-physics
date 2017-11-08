//
// Created by Snorri Sturluson on 29/10/2017.
//

#ifndef PROJECT_COMMANDPARSER_H
#define PROJECT_COMMANDPARSER_H


#include <string>
#include <sstream>
#include "rapidjson/document.h"
#include "Commands.h"

class CommandParser
{
public:
    void feed(const std::string& input);
    Command* parse();
    bool isDone();

protected:
    std::stringstream m_input;

    void parseAddShip(Command *command, rapidjson::Document &d);
    void parseStepSimulation(Command *command, rapidjson::Document &d);
    void parseGetState(Command *command, rapidjson::Document &d);
    void parseSetShipTargetLocation(Command *command, rapidjson::Document &d);
};

bool getwholeline(std::string& s, std::string& line);

#endif //PROJECT_COMMANDPARSER_H
