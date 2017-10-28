//
// Created by Snorri Sturluson on 27/10/2017.
//

#ifndef PROJECT_SOLARSYSTEM_H
#define PROJECT_SOLARSYSTEM_H

#include <vector>
#include "btBulletDynamicsCommon.h"

class Solarsystem {
public:
    Solarsystem();
    ~Solarsystem();

    void addCollisionShape(btCollisionShape* shape);
    void addRigidBody(btRigidBody* body);
    void stepSimulation(btScalar timeStep);
    std::vector<btVector3> getPositions();

protected:
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};

#endif //PROJECT_SOLARSYSTEM_H
