//
// Created by Snorri Sturluson on 29/10/2017.
//

#ifndef PROJECT_COMMANDS_H
#define PROJECT_COMMANDS_H

#include <LinearMath/btVector3.h>
#include <string>
#include "Types.h"

typedef enum
{
    cmdError = -1,
    cmdAddShip,
    cmdRemoveShip,
    cmdStepSimulation,
    cmdGetState,
    cmdSetMain,
    cmdSetViewer,
    cmdSetShipTargetLocation,
    cmdSetShipAttribute
} CommandType;

struct Params
{
    virtual ~Params() {};
};

struct ParamsError : public Params
{
    std::string message;
};

struct ParamsAddShip : public Params
{
    esUserId owner;
    esTypeId typeId;
    btVector3 position;
};

struct ParamsRemoveShip : public Params
{
    esUserId owner;
};

struct ParamsStepSimulation : public Params
{
    btScalar timestep;
};

struct ParamsSetShipTargetLocation : public Params
{
    esUserId ship;
    btVector3 location;
};

struct ParamsSetShipAttribute : public Params
{
    esUserId ship;
    std::string attribute;
    btScalar value;
};

struct Command
{
    Command() : params(nullptr) {}

    CommandType command;
    Params* params;
};

#endif //PROJECT_COMMANDS_H
