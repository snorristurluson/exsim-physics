//
// Created by Snorri Sturluson on 29/10/2017.
//

#ifndef PROJECT_COMMANDHANDLER_H
#define PROJECT_COMMANDHANDLER_H


#include "Solarsystem.h"
#include "Commands.h"

class CommandHandler
{
public:
    void start(Solarsystem* solarsystem, int port);

protected:
    Solarsystem* m_solarsystem;
    std::vector<int> m_connections;

    std::string handleCommand(Command *cmd);
    std::string handleAddShip(ParamsAddShip *params);
    std::string handleStepSimulation(ParamsStepSimulation *params);
    std::string handleGetState();

    void handleInput(const std::string &commandString);
};


#endif //PROJECT_COMMANDHANDLER_H
