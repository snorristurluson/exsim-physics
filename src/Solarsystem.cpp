//
// Created by Snorri Sturluson on 27/10/2017.
//

#include <vector>
#include "btBulletDynamicsCommon.h"
#include "Solarsystem.h"

Solarsystem::Solarsystem() {
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_overlappingPairCache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver;

    m_dynamicsWorld = new btDiscreteDynamicsWorld(
            m_dispatcher,
            m_overlappingPairCache,
            m_solver,
            m_collisionConfiguration
    );

    m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

Solarsystem::~Solarsystem() {
    btCollisionObjectArray &collisionObjectArray = m_dynamicsWorld->getCollisionObjectArray();
    for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = collisionObjectArray[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    for (int j = 0; j < m_collisionShapes.size(); j++)
    {
        btCollisionShape* shape = m_collisionShapes[j];
        m_collisionShapes[j] = 0;
        delete shape;
    }

    delete m_dynamicsWorld;
    delete m_solver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void Solarsystem::addCollisionShape(btCollisionShape *shape) {
    m_collisionShapes.push_back(shape);
}

void Solarsystem::addRigidBody(btRigidBody *body) {
    m_dynamicsWorld->addRigidBody(body);
}

void Solarsystem::stepSimulation(btScalar timeStep) {
    m_dynamicsWorld->stepSimulation(timeStep);
}

std::vector<btVector3> Solarsystem::getPositions() {
    std::vector<btVector3> result;
    result.reserve(m_dynamicsWorld->getNumCollisionObjects());

    for (int j = m_dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
    {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[j];
        btRigidBody* body = btRigidBody::upcast(obj);
        btTransform trans;
        if (body && body->getMotionState())
        {
            body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
            trans = obj->getWorldTransform();
        }
        result.push_back(trans.getOrigin());
    }

    return result;
}
