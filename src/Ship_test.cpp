#include "gtest/gtest.h"
#include "Ship.h"

TEST(Ship, CanCreate)
{
    auto ship = new Ship(1, 1);
    delete ship;
}

TEST(Ship, NewlyCreatedShipHasNullBody)
{
    auto ship = new Ship(1, 1);
    ASSERT_EQ(nullptr, ship->getBody());
    delete ship;
}

TEST(Ship, NewlyCreatedShipHasCollisionShape)
{
    auto ship = new Ship(1, 1);
    ASSERT_NE(nullptr, ship->getCollisionShape());
    delete ship;
}

TEST(Ship, PreparedShipHasBody)
{
    auto ship = new Ship(1, 1);
    ship->prepare();
    ASSERT_NE(nullptr, ship->getBody());
    delete ship;
}

TEST(Ship, SetTargetLocation)
{
    auto ship = new Ship(1, 1);
    ship->prepare();
    auto v = ship->getVelocity();
    EXPECT_FLOAT_EQ(0.0, v.x());
    EXPECT_FLOAT_EQ(0.0, v.y());
    EXPECT_FLOAT_EQ(0.0, v.z());

    ship->setTargetLocation(btVector3(100, 100, 0));
    ship->update(1.0);

    auto v2 = ship->getVelocity();
    v2.normalize();
    EXPECT_FLOAT_EQ(0.70710677, v2.x());
    EXPECT_FLOAT_EQ(0.70710677, v2.y());
    EXPECT_FLOAT_EQ(0.0, v.z());
}

