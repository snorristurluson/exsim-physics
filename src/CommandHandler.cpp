//
// Created by Snorri Sturluson on 29/10/2017.
//

#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <csignal>
#include "CommandHandler.h"
#include "CommandParser.h"

void signal_handler(int signal)
{
}

void CommandHandler::start(int port)
{
    std::signal(SIGPIPE, signal_handler);

    std::cout << "Starting solarsystem on port " << port << std::endl;

    m_solarsystem = new Solarsystem;
    m_mainConnection = -1;

    fd_set readfds = {};

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr = {};
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

    struct sockaddr_in client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);

    char buffer[4096];
    ssize_t bytesRead;
    do
    {
        auto max_sd = setupFdSet(listen_fd, readfds);

        auto activity = select( max_sd + 1 , &readfds, nullptr, nullptr, nullptr);
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

        std::vector<std::vector<int>::iterator> dropped_connections;
        for(auto it = m_connections.begin(); it != m_connections.end(); ++it)
        {
            if(FD_ISSET(*it, &readfds))
            {
                bzero(buffer, sizeof(buffer));
                bytesRead = read(*it, buffer, sizeof(buffer));
                if(bytesRead)
                {
                    std::string commandString(buffer, static_cast<unsigned long>(bytesRead));
                    handleInput(commandString, *it);
                }
                else
                {
                    dropped_connections.push_back(it);
                    std::cout << "Connection closed" << std::endl;
                    if(*it == m_mainConnection)
                    {
                        std::cout << "Resetting solarsystem" << std::endl;
                        delete m_solarsystem;
                        m_solarsystem = new Solarsystem;
                        m_mainConnection = -1;
                    }
                }
            }
        }
        for(auto it: dropped_connections)
        {
            m_connections.erase(it);
        }

    } while(true);
}

int CommandHandler::setupFdSet(int listen_fd, fd_set &readfds) const
{
    auto max_sd = listen_fd;
    FD_ZERO(&readfds);
    FD_SET(listen_fd, &readfds);
    for(int fd: m_connections)
    {
        if(fd > 0)
        {
            FD_SET(fd, &readfds);
        }

        if(fd > max_sd)
        {
            max_sd = fd;
        }
    }
    return max_sd;
}

void CommandHandler::handleInput(const std::string &commandString, int connection)
{
    if((connection != m_mainConnection) && (m_mainConnection != -1))
    {
        std::cout << "Unexpected input from viewer connection" << std::endl;
        std::cout << commandString << std::endl;
        return;
    }

    m_input += commandString;

    std::string line;
    auto gotLine = getwholeline(m_input, line);
    while(gotLine)
    {
        // std::cout << line << std::endl;
        CommandParser parser;
        parser.feed(line);

        auto command = parser.parse();
        auto response = handleCommand(command);

        if(command->command == cmdSetMain)
        {
            m_mainConnection = connection;
        }

        if(!response.empty())
        {
            btClock clock;
            for(auto conn: m_connections)
            {
                write(conn, response.c_str(), response.size());
                if(response.back() != '\n')
                {
                    write(conn, "\n", 1);
                }
            }
            auto duration = clock.getTimeSeconds() * 1000.0;
            // std::cout << "write: " << duration << std::endl;

        }
        gotLine = getwholeline(m_input, line);
    }
}

std::string CommandHandler::handleCommand(Command *cmd)
{
    switch(cmd->command)
    {
        case cmdError:
            return R"({"result": "error"})";

        case cmdAddShip:
            return handleAddShip(
                    dynamic_cast<ParamsAddShip*>(cmd->params));

        case cmdRemoveShip:
            return handleRemoveShip(
                    dynamic_cast<ParamsRemoveShip*>(cmd->params));

        case cmdStepSimulation:
            return handleStepSimulation(
                    dynamic_cast<ParamsStepSimulation*>(cmd->params));

        case cmdGetState:
            return handleGetState();

        case cmdSetMain:
            return R"({"result": "ok"})";

        case cmdSetShipTargetLocation:
            std::cout << "setshiptargetlocation" << std::endl;
            return handleSetShipTargetLocation(
                    dynamic_cast<ParamsSetShipTargetLocation*>(cmd->params));

        case cmdSetShipAttribute:
            std::cout << "setshipattribute" << std::endl;
            return handleSetShipAttribute(
                    dynamic_cast<ParamsSetShipAttribute*>(cmd->params));

        default:
            return R"({"result": "error"})";
    }
}

std::string CommandHandler::handleAddShip(ParamsAddShip *params)
{
    auto ship = new Ship(params->owner, params->typeId);

    btTransform t;
    t.setIdentity();
    t.setOrigin(params->position);
    ship->setTransform(t);
    m_solarsystem->addShip(ship);

    return R"({"result": "ok"})";
}

std::string CommandHandler::handleRemoveShip(ParamsRemoveShip *params)
{
    auto ship = m_solarsystem->findShip(params->owner);
    if(!ship)
    {
        std::cout << "Ship not found: " << params->owner << std::endl;
        return R"({"result": "error"})";
    }
    m_solarsystem->removeShip(ship);
    return R"({"result": "ok"})";
}

std::string CommandHandler::handleStepSimulation(ParamsStepSimulation *params)
{
    m_solarsystem->stepSimulation(params->timestep);
    return R"({"result": "ok"})";
}

std::string CommandHandler::handleGetState()
{
    return m_solarsystem->getStateAsJson();
}

std::string CommandHandler::handleSetShipTargetLocation(ParamsSetShipTargetLocation *params)
{
    auto ship = m_solarsystem->findShip(params->ship);
    if(!ship)
    {
        return R"({"result": "error"})";
    }
    ship->setTargetLocation(params->location);
    return R"({"result": "ok"})";
}

std::string CommandHandler::handleSetShipAttribute(ParamsSetShipAttribute *params)
{
    auto ship = m_solarsystem->findShip(params->ship);
    if(!ship)
    {
        return R"({"result": "error"})";
    }
    ship->setAttribute(params->attribute, params->value);
    return R"({"result": "ok"})";
}
