//
// Created by Snorri Sturluson on 29/10/2017.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include "CommandHandler.h"
#include "CommandParser.h"

void CommandHandler::start(Solarsystem *solarsystem, int port)
{
    m_solarsystem = solarsystem;

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        return;
    }

    listen(listen_fd, 1);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int connection_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (connection_fd < 0)
    {
        return;
    }

    char buffer[4096];
    ssize_t bytesRead;
    CommandParser parser;
    do
    {
        bzero(buffer, sizeof(buffer));
        bytesRead = read(connection_fd, buffer, sizeof(buffer));
        if(bytesRead)
        {
            std::string commandString(buffer);
            auto command = parser.parse(commandString);
            std::string response = handleCommand(command);
            write(connection_fd, response.c_str(), response.size());
            write(connection_fd, "\n", 1);
        }
    } while(bytesRead > 0);
}

std::string CommandHandler::handleCommand(Command *cmd)
{
    switch(cmd->command)
    {
        case cmdError:
            return "error\n";

        case cmdAddShip:
            return handleAddShip(
                    dynamic_cast<ParamsAddShip*>(cmd->params));

        case cmdStepSimulation:
            return handleStepSimulation(
                    dynamic_cast<ParamsStepSimulation*>(cmd->params));

        case cmdGetState:
            return handleGetState();
    }
    return "error";
}

std::string CommandHandler::handleAddShip(ParamsAddShip *params)
{
    auto ship = new Ship(params->owner, params->typeId);

    btTransform t;
    t.setIdentity();
    t.setOrigin(params->position);
    ship->setTransform(t);
    m_solarsystem->addShip(ship);

    return "ok";
}

std::string CommandHandler::handleStepSimulation(ParamsStepSimulation *params)
{
    m_solarsystem->stepSimulation(params->timestep);
    return "ok";
}

std::string CommandHandler::handleGetState()
{
    return m_solarsystem->getStateAsJson();
}
