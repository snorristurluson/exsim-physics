//
// Created by Snorri Sturluson on 27/10/2017.
//

#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "btBulletDynamicsCommon.h"
#include "Solarsystem.h"

using namespace rapidjson;

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

    m_dynamicsWorld->setGravity(btVector3(0, 0, 0));
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

void Solarsystem::addShip(Ship *ship)
{
    ship->prepare();
    addCollisionShape(ship->getCollisionShape());
    addRigidBody(ship->getBody());
    m_ships.push_back(ship);
}

std::string Solarsystem::getStateAsJson()
{
    Document d;

    Value ships;
    ships.SetArray();
    for(auto ship: m_ships)
    {
        Value shipData;
        shipData.SetObject();
        Value owner(ship->getOwner());
        Value type(ship->getType());
        shipData.AddMember("owner", owner, d.GetAllocator());
        shipData.AddMember("type", type, d.GetAllocator());

        Value positionData;
        positionData.SetObject();

        auto position = ship->getPosition();
        positionData.AddMember("x", Value(position.x()), d.GetAllocator());
        positionData.AddMember("y", Value(position.y()), d.GetAllocator());
        positionData.AddMember("z", Value(position.z()), d.GetAllocator());

        shipData.AddMember("position", positionData, d.GetAllocator());

        ships.PushBack(shipData, d.GetAllocator());
    }
    d.SetObject();
    d.AddMember("ships", ships, d.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    return buffer.GetString();
}
