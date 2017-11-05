#include "gtest/gtest.h"
#include "Solarsystem.h"

TEST(Solarsystem, CanCreate)
{
    auto ss = new Solarsystem;
    delete ss;
}

TEST(Solarsystem, CanAddShip)
{
    auto ss = new Solarsystem;
    auto ship = new Ship(1, 2);
    ss->addShip(ship);
}

TEST(Solarsystem, FindShipFromOwner)
{
    auto ss = new Solarsystem;
    auto ship = new Ship(1, 2);
    ss->addShip(ship);

    auto found = ss->findShip(1);

    EXPECT_EQ(ship, found);
}

TEST(Solarsystem, FindShipFromOwnerReturnsNullWhenNotExists)
{
    auto ss = new Solarsystem;
    auto found = ss->findShip(1);

    EXPECT_EQ(nullptr, found);
}

TEST(Solarsystem, FindShipFromOwnerWithMultipleShips)
{
    auto ss = new Solarsystem;
    auto first = new Ship(esUserId(0), 2);
    ss->addShip(first);

    for(int i = 1; i < 100; ++i)
    {
        auto ship = new Ship(esUserId(i), 2);
        ss->addShip(ship);
    }

    auto found = ss->findShip(0);

    EXPECT_EQ(first, found);
    EXPECT_EQ(100, ss->getNumShips());
}

TEST(Solarsystem, AddSecondShipWithSameOwnerShouldNotAdd)
{
    auto ss = new Solarsystem;
    auto first = new Ship(42, 2);
    ss->addShip(first);

    auto second = new Ship(42, 3);
    ss->addShip(second);

    auto found = ss->findShip(42);

    EXPECT_EQ(first, found);
    EXPECT_EQ(1, ss->getNumShips());
}

TEST(Solarsystem, RemoveShip)
{
    auto ss = new Solarsystem;
    auto ship = new Ship(1, 2);
    ss->addShip(ship);
    ss->removeShip(ship);
    auto found = ss->findShip(1);

    EXPECT_EQ(nullptr, found);
    EXPECT_EQ(0, ss->getNumShips());
}

TEST(Solarsystem, RemoveShipWithMultipleShips)
{
    auto ss = new Solarsystem;

    for(int i = 0; i < 100; ++i)
    {
        auto ship = new Ship(esUserId(i), 2);
        ss->addShip(ship);
    }

    auto target = ss->findShip(37);
    ss->removeShip(target);

    EXPECT_EQ(99, ss->getNumShips());
}

TEST(Solarsystem, SetShipTargetLocation)
{
    auto ss = new Solarsystem;
    auto ship = new Ship(1, 2);
    ss->addShip(ship);
    ship->setTargetLocation(btVector3(100.0, 100.0, 0.0));

    auto v = ship->getVelocity();
    EXPECT_FLOAT_EQ(0.0, v.x());
    EXPECT_FLOAT_EQ(0.0, v.y());
    EXPECT_FLOAT_EQ(0.0, v.z());

    ss->stepSimulation(0.250);

    auto v2 = ship->getVelocity();
    v2.normalize();
    EXPECT_FLOAT_EQ(0.70710677, v2.x());
    EXPECT_FLOAT_EQ(0.70710677, v2.y());
    EXPECT_FLOAT_EQ(0.0, v.z());
}