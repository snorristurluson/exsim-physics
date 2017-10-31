//
// Created by Snorri Sturluson on 29/10/2017.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "CommandHandler.h"
#include "CommandParser.h"

void CommandHandler::start(Solarsystem *solarsystem, int port)
{
    std::cout << "Starting solarsystem" << std::endl;

    m_solarsystem = solarsystem;

    fd_set readfds;

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "Bind failed - exiting" << std::endl;
        return;
    }

    listen(listen_fd, 5);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    char buffer[4096];
    ssize_t bytesRead;
    do
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(listen_fd, &readfds);
        auto max_sd = listen_fd;

        //add child sockets to set
        for(int fd: m_connections)
        {
            //if valid socket descriptor then add to read list
            if(fd > 0)
            {
                FD_SET( fd , &readfds);
            }

            //highest file descriptor number, need it for the select function
            if(fd > max_sd)
            {
                max_sd = fd;
            }
        }

        std::cout << "Calling select with " << m_connections.size() << " connections" << std::endl;
        int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if(activity < 0)
        {
            std::cout << "select error" << std::endl;
            break;
        }

        if(FD_ISSET(listen_fd, &readfds))
        {
            int connection_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);

            if (connection_fd < 0)
            {
                std::cout << "Accept error" << std::endl;
                continue;
            }

            std::cout << "Accepted new connection" << std::endl;
            m_connections.push_back(connection_fd);
        }

        std::vector<std::vector<int>::const_iterator> dropped_connections;
        for(auto it = m_connections.cbegin(); it != m_connections.cend(); ++it)
        {
            if(FD_ISSET(*it, &readfds))
            {
                std::cout << "Got data" << std::endl;
                bzero(buffer, sizeof(buffer));
                bytesRead = read(*it, buffer, sizeof(buffer));
                if(bytesRead)
                {
                    std::string commandString(buffer);
                    handleInput(commandString);
                }
                else
                {
                    dropped_connections.push_back(it);
                    std::cout << "Connection closed" << std::endl;
                }
            }
        }
        for(auto it: dropped_connections)
        {
            m_connections.erase(it);
        }

    } while(true);
}

void CommandHandler::handleInput(const std::string &commandString)
{
    CommandParser parser;
    auto command = parser.parse(commandString);
    auto response = handleCommand(command);

    if(response == "error")
    {
        response += ": " + commandString;
    }

    for(auto connection: m_connections)
    {
        write(connection, response.c_str(), response.size());
        if(response.back() != '\n')
        {
            write(connection, "\n", 1);
        }
    }
}

std::string CommandHandler::handleCommand(Command *cmd)
{
    switch(cmd->command)
    {
        case cmdError:
            return "error";

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
