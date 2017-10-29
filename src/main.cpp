#include "btBulletDynamicsCommon.h"
#include "Solarsystem.h"
#include "Ship.h"

int main(int argc, char** argv)
{
	int i;


    Solarsystem solarsystem;

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(2, 10, 0));

    Ship* ship1 = new Ship(1, 1);
	startTransform.setOrigin(btVector3(2, 10, 0));
    ship1->setTransform(startTransform);
    solarsystem.addShip(ship1);

	Ship* ship2 = new Ship(2, 1);
	startTransform.setOrigin(btVector3(-2, 10, 0));
	ship1->setTransform(startTransform);
	solarsystem.addShip(ship1);

	for (i = 0; i < 150; i++)
	{
		solarsystem.stepSimulation(1.f / 60.f);

        int j = 0;
        for(const auto& pos: solarsystem.getPositions()) {
            printf("world pos object %d = %f,%f,%f\n", j, float(pos.getX()), float(pos.getY()), float(pos.getZ()));
            j++;
        }
	}
}
