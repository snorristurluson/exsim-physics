//
// Created by Snorri Sturluson on 29/10/2017.
//

#include <gtest/gtest.h>
#include "CommandParser.h"

TEST(Utils, getwholeline)
{
    std::string input = "bingo\nbongo";
    std::string line;
    auto result = getwholeline(input, line);
    EXPECT_EQ(result, true);
    EXPECT_EQ(line, "bingo");
}

TEST(Utils, getwholeline_IncompleteSecondLine)
{
    std::string input = "bingo\nbon";
    std::string firstLine;
    std::string secondLine;

    auto first = getwholeline(input, firstLine);
    EXPECT_EQ(first, true);
    EXPECT_EQ(firstLine, "bingo");
    EXPECT_EQ(input, "bon");

    auto second = getwholeline(input, secondLine);
    EXPECT_EQ(second, false);
    EXPECT_EQ(secondLine, "");
    EXPECT_EQ(input, "bon");
}

TEST(Utils, getwholeline_IncompleteSecondLineIsCompleted)
{
    std::string input = "bingo\nbon";
    std::string firstLine;
    std::string secondLine;
    auto first = getwholeline(input, firstLine);
    EXPECT_EQ(first, true);
    EXPECT_EQ(firstLine, "bingo");

    auto second = getwholeline(input, secondLine);
    EXPECT_EQ(second, false);
    EXPECT_EQ(secondLine, "");

    input += "go\n";
    second = getwholeline(input, secondLine);
    EXPECT_EQ(second, true);
    EXPECT_EQ(secondLine, "bongo");
    EXPECT_EQ(input, "");
}


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
    parser.feed(
            R"({"command": "addship", "params": {"owner": 1, "typeid": 2, "position": {"x": -10.0, "y": 10.0, "z": 0.0}}})");
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
    parser.feed(R"({"command": "addship", "bingo": 1, "typeid": 2, "position": {"x": -10, "y": 10, "z": 0}})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, AddShipCommandWithWrongTypeForOwner)
{
    CommandParser parser;
    parser.feed(
            R"({"command": "addship", "params": {"owner": "1", "typeid": 2, "position": {"x": -10, "y": 10, "z": 0}}})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, AddShipCommandWithInvalidPositionOwner)
{
    CommandParser parser;
    parser.feed(
            R"({"command": "addship", "params": {"owner": 1, "typeid": 2, "position": {"bingo": -10, "y": 10, "z": 0}}})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, RemoveShipCommand)
{
    CommandParser parser;
    parser.feed(R"({"command": "removeship", "params": {"owner": 1}})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdRemoveShip, cmd->command);
    auto paramsRemoveShip = dynamic_cast<ParamsRemoveShip*>(cmd->params);
    ASSERT_NE(nullptr, paramsRemoveShip);
    EXPECT_EQ(1, paramsRemoveShip->owner);
}

TEST(CommandParser, StepSimulationCommand)
{
    CommandParser parser;
    parser.feed(R"({"command": "stepsimulation", "params": {"timestep": 0.1}})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdStepSimulation, cmd->command);
    auto params = dynamic_cast<ParamsStepSimulation*>(cmd->params);
    ASSERT_NE(nullptr, params);
    EXPECT_FLOAT_EQ(0.1, params->timestep);
}

TEST(CommandParser, InvalidStepSimulationCommand)
{
    CommandParser parser;
    parser.feed(R"({"command": "stepsimulation", "params": {"bingo": 0.1}})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdError, cmd->command);
    EXPECT_EQ(nullptr, cmd->params);
}

TEST(CommandParser, GetStateCommand)
{
    CommandParser parser;
    parser.feed(R"({"command": "getstate"})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdGetState, cmd->command);
}

TEST(CommandParser, SetAsMainCommand)
{
    CommandParser parser;
    parser.feed(R"({"command": "setmain"})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdSetMain, cmd->command);
}

TEST(CommandParser, SetAsViewerCommand)
{
    CommandParser parser;
    parser.feed(R"({"command": "setviewer"})");
    auto cmd = parser.parse();

    EXPECT_EQ(cmdSetViewer, cmd->command);
}

TEST(CommandParser, TwoCommandsInOne)
{
    CommandParser parser;
    parser.feed(R"({"command": "setmain"}{"command": "setviewer"})");
    auto cmd = parser.parse();
    EXPECT_EQ(cmdSetMain, cmd->command);
    cmd = parser.parse();
    EXPECT_EQ(cmdSetViewer, cmd->command);
    EXPECT_TRUE(parser.isDone());
}

TEST(CommandParser, SetShipTargetLocation)
{
    CommandParser parser;
    parser.feed(
            R"({"command": "setshiptargetlocation", "params": {"shipid": 1, "location": {"x": 100, "y": 100, "z": 0}}})");
    auto cmd = parser.parse();
    EXPECT_EQ(cmdSetShipTargetLocation, cmd->command);

    auto params = dynamic_cast<ParamsSetShipTargetLocation*>(cmd->params);
    ASSERT_NE(nullptr, params);
    EXPECT_EQ(1, params->ship);
    EXPECT_FLOAT_EQ(100.0, params->location.x());
    EXPECT_FLOAT_EQ(100.0, params->location.y());
    EXPECT_FLOAT_EQ(0.0, params->location.z());
}

TEST(CommandParser, SetShipAttribute)
{
    CommandParser parser;
    parser.feed(
            R"({"command": "setshipattribute", "params": {"shipid": 1, "attribute": "maxspeed", "value": 100}})"
    );

    auto cmd = parser.parse();
    EXPECT_EQ(cmdSetShipAttribute, cmd->command);

    auto params = dynamic_cast<ParamsSetShipAttribute*>(cmd->params);
    ASSERT_NE(nullptr, params);

    EXPECT_EQ(params->ship, 1);
    EXPECT_EQ(params->attribute, "maxspeed");
    EXPECT_FLOAT_EQ(params->value, 100);
}