//
// Created by Snorri Sturluson on 29/10/2017.
//

#ifndef PROJECT_COMMANDHANDLER_H
#define PROJECT_COMMANDHANDLER_H


#include <netinet/in.h>
#include "Solarsystem.h"
#include "Commands.h"

class CommandHandler
{
public:
    void start(int port);

protected:
    Solarsystem* m_solarsystem;
    int m_mainConnection;
    std::vector<int> m_connections;
    std::string m_input;

    std::string handleCommand(Command *cmd);
    std::string handleAddShip(ParamsAddShip *params);
    std::string handleStepSimulation(ParamsStepSimulation *params);
    std::string handleGetState();

    void handleInput(const std::string &commandString, int connection);

    int setupFdSet(int listen_fd, fd_set &readfds) const;

    std::string handleSetShipTargetLocation(ParamsSetShipTargetLocation *params);
};


#endif //PROJECT_COMMANDHANDLER_H
