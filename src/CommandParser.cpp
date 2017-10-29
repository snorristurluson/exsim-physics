//
// Created by Snorri Sturluson on 29/10/2017.
//

#include "CommandParser.h"

using namespace rapidjson;

Command *CommandParser::parse(const std::string &input)
{
    auto result = new Command;

    Document d;
    if(d.Parse(input.c_str()).HasParseError())
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
    if(command == "addship")
    {
        parseAddShip(result, d);
    }
    else if(command == "stepsimulation")
    {
        parseStepSimulation(result, d );
    }
    else if(command == "getstate")
    {
        parseGetState(result, d);
    }
    else
    {
        result->command = cmdError;
    }
    return result;
}

void CommandParser::parseAddShip(Command *command, Document &d)
{
    if(!d.HasMember("owner") || !d.HasMember("type") || !d.HasMember("position"))
    {
        command->command = cmdError;
        return;
    }

    command->command = cmdAddShip;
    if(!d["owner"].IsInt64() || !d["type"].IsInt64() || !d["position"].IsObject())
    {
        command->command = cmdError;
        return;
    }

    const Value& position = d["position"].GetObject();
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
    params->owner = d["owner"].GetInt64();
    params->typeId = d["type"].GetInt64();

    auto x = position["x"].GetDouble();
    auto y = position["y"].GetDouble();
    auto z = position["z"].GetDouble();

    params->position = btVector3(x, y, z);

    command->params = params;
}

void CommandParser::parseStepSimulation(Command *command, rapidjson::Document &d)
{
    command->command = cmdStepSimulation;
    auto params = new ParamsStepSimulation;

    if(!d.HasMember("timestep") || !d["timestep"].IsNumber())
    {
        command->command = cmdError;
        return;
    }
    params->timestep = d["timestep"].GetDouble();

    command->params = params;
}

void CommandParser::parseGetState(Command *command, rapidjson::Document &d)
{
    command->command = cmdGetState;
}

