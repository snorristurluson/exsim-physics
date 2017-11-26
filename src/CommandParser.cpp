//
// Created by Snorri Sturluson on 29/10/2017.
//

#include <rapidjson/istreamwrapper.h>
#include <iostream>
#include "CommandParser.h"

using namespace rapidjson;

std::string str_tolower(std::string s) {
    std::transform(
            s.begin(),
            s.end(),
            s.begin(),
            [](unsigned char c){ return std::tolower(c); }
    );
    return s;
}

void CommandParser::feed(const std::string &input)
{
    m_input << input;
}

Command *CommandParser::parse()
{
    auto result = new Command;

    IStreamWrapper s(m_input);

    GenericDocument<UTF8<> > d;
    d.ParseStream<kParseStopWhenDoneFlag, UTF8<>, IStreamWrapper>(s);

    // Peek into the stream to trigger eof so that isDone works properly
    m_input.peek();

    if(d.HasParseError())
    {
        result->command = cmdError;
        return result;
    }

    if(!d.IsObject())
    {
        result->command = cmdError;
        return result;
    }

    std::string command = d["command"].GetString();
    command = str_tolower(command);

    if(command == "addship")
    {
        parseAddShip(result, d);
    }
    else if(command == "removeship")
    {
        parseRemoveShip(result, d);
    }
    else if(command == "stepsimulation")
    {
        parseStepSimulation(result, d );
    }
    else if(command == "getstate")
    {
        parseGetState(result, d);
    }
    else if(command == "setmain")
    {
        result->command = cmdSetMain;
    }
    else if(command == "setviewer")
    {
        result->command = cmdSetViewer;
    }
    else if(command == "setshiptargetlocation")
    {
        parseSetShipTargetLocation(result, d);
    }
    else
    {
        result->command = cmdError;
    }
    return result;
}

void CommandParser::parseAddShip(Command *command, Document &d)
{
    if(!d.HasMember("params") || !d["params"].IsObject())
    {
        command->command = cmdError;
        return;
    }

    auto rawParams = d["params"].GetObject();

    if(!rawParams.HasMember("owner") || !rawParams.HasMember("typeid") || !rawParams.HasMember("position"))
    {
        command->command = cmdError;
        return;
    }

    if(!rawParams["owner"].IsInt64() || !rawParams["typeid"].IsInt64() || !rawParams["position"].IsObject())
    {
        command->command = cmdError;
        return;
    }

    command->command = cmdAddShip;
    const Value& position = rawParams["position"].GetObject();
    if(!position.HasMember("x") || !position.HasMember("y") || !position.HasMember("z"))
    {
        command->command = cmdError;
        return;
    }

    if(!position["x"].IsNumber() || !position["y"].IsNumber() || !position["z"].IsNumber())
    {
        command->command = cmdError;
        return;
    }

    auto params = new ParamsAddShip;
    params->owner = rawParams["owner"].GetInt64();
    params->typeId = rawParams["typeid"].GetInt64();

    auto x = position["x"].GetDouble();
    auto y = position["y"].GetDouble();
    auto z = position["z"].GetDouble();

    params->position = btVector3(x, y, z);

    command->params = params;
}

void CommandParser::parseStepSimulation(Command *command, rapidjson::Document &d)
{
    if(!d.HasMember("params") || !d["params"].IsObject())
    {
        command->command = cmdError;
        return;
    }

    auto rawParams = d["params"].GetObject();

    command->command = cmdStepSimulation;
    auto params = new ParamsStepSimulation;

    if(!rawParams.HasMember("timestep") || !rawParams["timestep"].IsNumber())
    {
        command->command = cmdError;
        return;
    }
    params->timestep = rawParams["timestep"].GetDouble();

    command->params = params;
}

void CommandParser::parseGetState(Command *command, rapidjson::Document &d)
{
    command->command = cmdGetState;
}

void CommandParser::parseSetShipTargetLocation(Command *command, rapidjson::Document &d)
{
    if(!d.HasMember("params") || !d["params"].IsObject())
    {
        command->command = cmdError;
        return;
    }

    auto rawParams = d["params"].GetObject();

    if(!rawParams.HasMember("shipid") || !rawParams.HasMember("location"))
    {
        command->command = cmdError;
        return;
    }

    if(!rawParams["shipid"].IsInt64())
    {
        command->command = cmdError;
        return;
    }

    const Value& location = rawParams["location"].GetObject();
    if(!location.HasMember("x") || !location.HasMember("y") || !location.HasMember("z"))
    {
        command->command = cmdError;
        return;
    }

    if(!location["x"].IsNumber() || !location["y"].IsNumber() || !location["z"].IsNumber())
    {
        command->command = cmdError;
        return;
    }

    auto params = new ParamsSetShipTargetLocation;
    params->ship = rawParams["shipid"].GetInt64();
    params->location = btVector3(
            location["x"].GetDouble(),
            location["y"].GetDouble(),
            location["z"].GetDouble()
    );
    command->command = cmdSetShipTargetLocation;
    command->params = params;
}

void CommandParser::parseRemoveShip(Command *command, rapidjson::Document& d)
{
    if(!d.HasMember("params") || !d["params"].IsObject())
    {
        command->command = cmdError;
        return;
    }

    auto rawParams = d["params"].GetObject();

    if(!rawParams.HasMember("owner"))
    {
        command->command = cmdError;
        return;
    }

    if(!rawParams["owner"].IsInt64())
    {
        command->command = cmdError;
        return;
    }

    command->command = cmdRemoveShip;

    auto params = new ParamsRemoveShip;
    params->owner = rawParams["owner"].GetInt64();
    command->params = params;
}

bool CommandParser::isDone()
{
    return m_input.eof();
}

bool getwholeline(std::string& s, std::string& line)
{
    line = std::string();
    for(int i = 0; i < s.size(); ++i)
    {
        auto next = s[i];
        if(next == '\r')
        {
            continue;
        }
        if(next == '\n')
        {
            s = s.substr(i + 1);
            return true;
        }
        line.push_back(next);
    }

    // If we made it this far it means the line is incomplete
    line = std::string();
    return false;
}

