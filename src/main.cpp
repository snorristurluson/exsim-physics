#include "btBulletDynamicsCommon.h"
#include "Solarsystem.h"
#include "Ship.h"

int main(int argc, char** argv)
{
	int i;


    Solarsystem solarsystem;

	///create a few basic rigid bodies

	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		solarsystem.addCollisionShape(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		solarsystem.addRigidBody(body);
	}

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
