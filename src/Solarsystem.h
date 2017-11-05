//
// Created by Snorri Sturluson on 27/10/2017.
//

#ifndef PROJECT_SOLARSYSTEM_H
#define PROJECT_SOLARSYSTEM_H

#include <vector>
#include <string>
#include <map>
#include "btBulletDynamicsCommon.h"
#include "Ship.h"

class Solarsystem
{
public:
    Solarsystem();
    ~Solarsystem();

    void addShip(Ship* ship);
    void removeShip(Ship* ship);
    Ship* findShip(esUserId);

    void stepSimulation(btScalar timeStep);

    std::string getStateAsJson();
    int getNumShips();

protected:
    void addCollisionShape(btCollisionShape* shape);
    void addRigidBody(btRigidBody* body);

protected:
    std::vector<Ship*> m_ships;
    std::map<esUserId, Ship*> m_shipsByOwner;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};

#endif //PROJECT_SOLARSYSTEM_H
