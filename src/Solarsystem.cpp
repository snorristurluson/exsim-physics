//
// Created by Snorri Sturluson on 27/10/2017.
//

#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
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

    delete m_dynamicsWorld;
    delete m_solver;
    delete m_overlappingPairCache;
    delete m_dispatcher;
    delete m_collisionConfiguration;
}

void Solarsystem::stepSimulation(btScalar timeStep)
{
    btClock clock;

    for(auto ship: m_ships)
    {
        ship->update(timeStep);
    }
    m_dynamicsWorld->stepSimulation(timeStep);

    auto dispatcher = m_dynamicsWorld->getDispatcher();
    auto numManifolds = dispatcher->getNumManifolds();

    std::map<Ship*, ShipSet> shipsInRange;
    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();

        auto typeA = obA->getUserIndex();
        auto typeB = obB->getUserIndex();
        if(typeA == esSensor && typeB == esSensor)
        {
            // Don't care about sensors overlapping
            continue;
        }

        auto shipA = reinterpret_cast<Ship*>(obA->getUserPointer());
        auto shipB = reinterpret_cast<Ship*>(obB->getUserPointer());

        if(shipA == shipB)
        {
            // Don't care about the ship and its own sensor
            continue;
        }

        if(typeA == esShip && typeB == esShip)
        {
            // Two ships colliding
            continue;
        }

        auto posA = shipA->getPosition();
        auto posB = shipB->getPosition();
        auto v = posA - posB;
        auto distance = v.length();

        if(typeA == esShip && typeB == esSensor)
        {
            if(distance < shipB->getSensorRange() + shipA->getRadius())
            {
                shipsInRange[shipB].insert(shipA);
            }
        }
        if(typeB == esShip && typeA == esSensor)
        {
            if(distance < shipA->getSensorRange() + shipB->getRadius())
            {
                shipsInRange[shipA].insert(shipB);
            }
        }
    }

    for(auto it: shipsInRange)
    {
        it.first->setInRange(it.second);
    }

    auto duration = clock.getTimeSeconds();
    // std::cout << "stepSimulation: " << duration << std::endl;
}

void Solarsystem::addShip(Ship *ship)
{
    if(findShip(ship->getOwner()))
    {
        return;
    }

    ship->addToWorld(m_dynamicsWorld);
    m_ships.push_back(ship);

    m_shipsByOwner[ship->getOwner()] = ship;
}

void Solarsystem::removeShip(Ship *ship)
{
    auto foundIt = m_shipsByOwner.find(ship->getOwner());
    if(foundIt != m_shipsByOwner.end())
    {
        m_shipsByOwner.erase(foundIt);
    }
    ship->removeFromWorld();
    m_ships.erase(std::remove(m_ships.begin(), m_ships.end(), ship));
}

Value shipSetToArray(const ShipSet &shipSet, Document &d)
{
    Value array;
    array.SetArray();

    for(auto shipInRange: shipSet)
    {
        array.PushBack((int64_t)shipInRange->getOwner(), d.GetAllocator());
    }
    return array;
}

std::string Solarsystem::getStateAsJson()
{
    btClock clock;

    Document d;

    Value state;
    state.SetObject();

    Value ships;
    ships.SetObject();
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

        Value inRange = shipSetToArray(ship->getInRange(), d);
        shipData.AddMember("inrange", inRange, d.GetAllocator());

        Value newInRange = shipSetToArray(ship->getNewInRange(), d);
        shipData.AddMember("newinrange", newInRange, d.GetAllocator());

        Value goneFromRange = shipSetToArray(ship->getGoneFromRange(), d);
        shipData.AddMember("gonefromrange", goneFromRange, d.GetAllocator());

        std::stringstream ss;
        ss << "ship_" << ship->getOwner();

        Value shipId;
        shipId.SetString(ss.str().c_str(), d.GetAllocator());
        ships.AddMember(shipId, shipData, d.GetAllocator());
    }
    state.AddMember("ships", ships, d.GetAllocator());

    d.SetObject();
    d.AddMember("state", state, d.GetAllocator());
    d.AddMember("result", "state", d.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    auto duration = clock.getTimeSeconds();
    // std::cout << "getStateAsJson: " << duration << std::endl;

    std::string result = buffer.GetString();
    // std::cout << result << std::endl;
    return result.c_str();
}

Ship *Solarsystem::findShip(esUserId owner)
{
    auto foundIt = m_shipsByOwner.find(owner);
    if(foundIt == m_shipsByOwner.end())
    {
        return nullptr;
    }
    return foundIt->second;
}

int Solarsystem::getNumShips()
{
    return m_ships.size();
}
