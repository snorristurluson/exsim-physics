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
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
}

TEST(CommandParser, UnknownCommandReturnsRerror)
{
    CommandParser parser;
    parser.feed("{\"command\": \"bingo\"}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
}

TEST(CommandParser, SimpleAddShipCommand)
{
    CommandParser parser;
    parser.feed("{\"command\": \"addship\", \"owner\": 1, \"type\": 2, \"position\": {\"x\": -10.0, \"y\": 10.0, \"z\": 0.0}}");
    auto cmd = parser.parse();

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
    parser.feed("{\"command\": \"addship\", \"bingo\": 1, \"type\": 2, \"position\": {\"x\": -10, \"y\": 10, \"z\": 0}}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, AddShipCommandWithWrongTypeForOwner)
{
    CommandParser parser;
    parser.feed("{\"command\": \"addship\", \"owner\": \"1\", \"type\": 2, \"position\": {\"x\": -10, \"y\": 10, \"z\": 0}}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, AddShipCommandWithInvalidPositionOwner)
{
    CommandParser parser;
    parser.feed("{\"command\": \"addship\", \"owner\": 1, \"type\": 2, \"position\": {\"bingo\": -10, \"y\": 10, \"z\": 0}}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, StepSimulationCommand)
{
    CommandParser parser;
    parser.feed("{\"command\": \"stepsimulation\", \"timestep\": 0.1}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdStepSimulation, cmd->command);
    auto params = dynamic_cast<ParamsStepSimulation*>(cmd->params);
    ASSERT_NE(nullptr, params);
    EXPECT_FLOAT_EQ(0.1, params->timestep);
}

TEST(CommandParser, InvalidStepSimulationCommand)
{
    CommandParser parser;
    parser.feed("{\"command\": \"stepsimulation\", \"bingo\": 0.1}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, GetStateCommand)
{
    CommandParser parser;
    parser.feed("{\"command\": \"getstate\"}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdGetState, cmd->command);
}

TEST(CommandParser, SetAsMainCommand)
{
    CommandParser parser;
    parser.feed("{\"command\": \"setmain\"}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdSetMain, cmd->command);
}

TEST(CommandParser, SetAsViewerCommand)
{
    CommandParser parser;
    parser.feed("{\"command\": \"setviewer\"}");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdSetViewer, cmd->command);
}

TEST(CommandParser, TwoCommandsInOne)
{
    CommandParser parser;
    parser.feed("{\"command\": \"setmain\"}{\"command\": \"setviewer\"}");
    auto cmd = parser.parse();
    EXPECT_EQ(cmdSetMain, cmd->command);
    cmd = parser.parse();
    EXPECT_EQ(cmdSetViewer, cmd->command);
    EXPECT_TRUE(parser.isDone());
}

TEST(CommandParser, SetShipTargetLocation)
{
    CommandParser parser;
    parser.feed("{\"command\": \"setshiptargetlocation\", \"ship\": 1, \"location\": {\"x\": 100, \"y\": 100, \"z\": 0}}");
    auto cmd = parser.parse();
    EXPECT_EQ(cmdSetShipTargetLocation, cmd->command);

    auto params = dynamic_cast<ParamsSetShipTargetLocation*>(cmd->params);
    ASSERT_NE(nullptr, params);
    EXPECT_EQ(1, params->ship);
    EXPECT_FLOAT_EQ(100.0, params->location.x());
    EXPECT_FLOAT_EQ(100.0, params->location.y());
    EXPECT_FLOAT_EQ(0.0, params->location.z());
}