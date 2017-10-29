//
// Created by Snorri Sturluson on 29/10/2017.
//

#include <gtest/gtest.h>
#include "CommandParser.h"

TEST(CommandParser, CanCreate)
{
    CommandParser parser;
}

TEST(CommandParser, EmptyInputReturnsError)
{
    CommandParser parser;
    auto cmd = parser.parse("");

    EXPECT_EQ(cmdError, cmd->command);
}

TEST(CommandParser, UnknownCommandReturnsRerror)
{
    CommandParser parser;
    auto cmd = parser.parse("{\"command\": \"bingo\"}");

    EXPECT_EQ(cmdError, cmd->command);
}

TEST(CommandParser, SimpleAddShipCommand)
{
    CommandParser parser;
    auto cmd = parser.parse("{\"command\": \"addship\", \"owner\": 1, \"type\": 2, \"position\": {\"x\": -10.0, \"y\": 10.0, \"z\": 0.0}}");

    EXPECT_EQ(cmdAddShip, cmd->command);
    auto paramsAddShip = dynamic_cast<ParamsAddShip*>(cmd->params);
    ASSERT_NE(nullptr, paramsAddShip);
    EXPECT_EQ(1, paramsAddShip->owner);
    EXPECT_EQ(2, paramsAddShip->typeId);
    EXPECT_FLOAT_EQ(-10, paramsAddShip->position.x());
    EXPECT_FLOAT_EQ(10, paramsAddShip->position.y());
    EXPECT_FLOAT_EQ(0, paramsAddShip->position.z());
}

TEST(CommandParser, InvalidAddShipCommand)
{
    CommandParser parser;
    auto cmd = parser.parse("{\"command\": \"addship\", \"bingo\": 1, \"type\": 2, \"position\": {\"x\": -10, \"y\": 10, \"z\": 0}}");

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, AddShipCommandWithWrongTypeForOwner)
{
    CommandParser parser;
    auto cmd = parser.parse("{\"command\": \"addship\", \"owner\": \"1\", \"type\": 2, \"position\": {\"x\": -10, \"y\": 10, \"z\": 0}}");

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, AddShipCommandWithInvalidPositionOwner)
{
    CommandParser parser;
    auto cmd = parser.parse("{\"command\": \"addship\", \"owner\": 1, \"type\": 2, \"position\": {\"bingo\": -10, \"y\": 10, \"z\": 0}}");

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, StepSimulationCommand)
{
    CommandParser parser;
    auto cmd = parser.parse("{\"command\": \"stepsimulation\", \"timestep\": 0.1}");

    EXPECT_EQ(cmdStepSimulation, cmd->command);
    auto params = dynamic_cast<ParamsStepSimulation*>(cmd->params);
    ASSERT_NE(nullptr, params);
    EXPECT_FLOAT_EQ(0.1, params->timestep);
}

TEST(CommandParser, InvalidStepSimulationCommand)
{
    CommandParser parser;
    auto cmd = parser.parse("{\"command\": \"stepsimulation\", \"bingo\": 0.1}");

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, GetPositionsCommand)
{
    CommandParser parser;
    auto cmd = parser.parse("{\"command\": \"getpositions\"}");

    EXPECT_EQ(cmdGetPositions, cmd->command);
}