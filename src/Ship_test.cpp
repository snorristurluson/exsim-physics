#include "gtest/gtest.h"
#include "Ship.h"

TEST(Ship, CanCreate)
{
    auto* ship = new Ship(1, 1);
}

TEST(Ship, NewlyCreatedShipHasNullBody)
{
    auto* ship = new Ship(1, 1);
    ASSERT_EQ(nullptr, ship->getBody());
}

TEST(Ship, NewlyCreatedShipHasCollisionShape)
{
    auto* ship = new Ship(1, 1);
    ASSERT_NE(nullptr, ship->getCollisionShape());
}

TEST(Ship, PreparedShipHasBody)
{
    auto* ship = new Ship(1, 1);
    ship->prepare();
    ASSERT_NE(nullptr, ship->getBody());
}


