//
// Created by Snorri Sturluson on 28/10/2017.
//

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <iostream>
#include "Ship.h"

Ship::Ship(esUserId owner, esTypeId shipType) :
    m_owner(owner),
    m_type(shipType),
    m_sensorRange(100.0),
    m_body(nullptr),
    m_sensor(nullptr),
    m_collisionShape(nullptr),
    m_motionState(nullptr),
    m_sensorShape(nullptr),
    m_sensorMotionState(nullptr),
    m_dynamicsWorld(nullptr)
{
    // Todo: Details should come from shipType
    m_collisionShape = new btSphereShape(10.0);
    m_mass = btScalar(1.f);
    m_transform.setIdentity();
    m_maxSpeed = 150.0f;
}

void Ship::setTransform(const btTransform &t)
{
    m_transform = t;

    // Todo: should this work after entering a solar system?
}

void Ship::addToWorld(btDiscreteDynamicsWorld *world)
{
    if(!m_dynamicsWorld)
    {
        m_dynamicsWorld = world;
    }
    else
    {
        if(m_dynamicsWorld != world)
        {
            // Something is not right
        }
    }
    prepareBody();
    prepareSensor();
}

void Ship::removeFromWorld()
{
    m_dynamicsWorld->removeRigidBody(m_body);
    m_dynamicsWorld->removeRigidBody(m_sensor);
}

void Ship::prepareSensor()
{
    if(m_sensor)
    {
        m_dynamicsWorld->removeRigidBody(m_sensor);
        delete m_sensor;
    }
    if(m_sensorShape)
    {
        delete m_sensorShape;
    }
    if(m_sensorMotionState)
    {
        delete m_sensorMotionState;
    }

    m_sensorShape = new btSphereShape(m_sensorRange);
    btVector3 localInertia(0, 0, 0);
    m_sensorShape->calculateLocalInertia(m_mass, localInertia);
    m_sensorMotionState = new btDefaultMotionState(m_transform);
    btRigidBody::btRigidBodyConstructionInfo sensorRbInfo(
            m_mass, m_sensorMotionState, m_sensorShape, localInertia
    );
    m_sensor = new btRigidBody(sensorRbInfo);

    m_sensor->setCollisionFlags(
            btCollisionObject::CF_KINEMATIC_OBJECT |
            btCollisionObject::CF_NO_CONTACT_RESPONSE
    );
    m_sensor->setUserPointer(reinterpret_cast<void*>(this));
    m_sensor->setUserIndex(esSensor);
    m_sensor->setLinearFactor(btVector3(1, 1, 0));
    m_sensor->setActivationState(DISABLE_DEACTIVATION);
    if(m_dynamicsWorld)
    {
        m_dynamicsWorld->addRigidBody(m_sensor, 2, 1);
    }
}

void Ship::prepareBody()
{
    btVector3 localInertia(0, 0, 0);
    m_collisionShape->calculateLocalInertia(m_mass, localInertia);

    m_motionState = new btDefaultMotionState(m_transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(
            m_mass, m_motionState, m_collisionShape, localInertia
    );
    m_body = new btRigidBody(rbInfo);
    m_body->setUserPointer(reinterpret_cast<void*>(this));
    m_body->setUserIndex(esShip);
    m_body->setLinearFactor(btVector3(1, 1, 0));
    m_body->setActivationState(DISABLE_DEACTIVATION);
    if(m_dynamicsWorld)
    {
        m_dynamicsWorld->addRigidBody(m_body, 1, 0xffffffff);
    }
}

btCollisionShape *Ship::getCollisionShape() const
{
    return m_collisionShape;
}

btRigidBody *Ship::getBody() const
{
    return m_body;
}

esUserId Ship::getOwner() const
{
    return m_owner;
}

esTypeId Ship::getType() const
{
    return m_type;
}

btVector3 Ship::getPosition() const
{
    btTransform t;
    m_motionState->getWorldTransform(t);
    return t.getOrigin();
}

btVector3 Ship::getVelocity() const
{
    return m_body->getLinearVelocity();
}

void Ship::setTargetLocation(const btVector3 &loc)
{
    m_targetLocation = loc;
}

void Ship::update(btScalar dt)
{
    btVector3 velocity(0, 0, 0);

    auto myPos = getPosition();
    auto v = m_targetLocation - myPos;
    auto distance = v.length();
    if(distance > 1.0)
    {
        // Speed should come from the ship type and modifiers
        velocity = v.normalized() * m_maxSpeed;
    }
    else
    {
        std::cout << "Ship " << m_owner << " close to target" << std::endl;
    }

    m_body->setLinearVelocity(velocity);

    btTransform t;
    m_motionState->getWorldTransform(t);
    m_sensorMotionState->setWorldTransform(t);
}

void Ship::setInRange(const ShipSet &ships)
{
    m_prevInRange = m_inRange;
    m_inRange = ships;
    m_newInRange.clear();
    m_goneFromRange.clear();

    for(auto s: m_inRange)
    {
        if(m_prevInRange.find(s) == m_prevInRange.end())
        {
            m_newInRange.insert(s);
        }
    }
    for(auto s: m_prevInRange)
    {
        if(m_inRange.find(s) == m_inRange.end())
        {
            m_goneFromRange.insert(s);
        }
    }
}

ShipSet Ship::getInRange() const
{
    return m_inRange;
}

ShipSet Ship::getNewInRange() const
{
    return m_newInRange;
}

ShipSet Ship::getGoneFromRange() const
{
    return m_goneFromRange;
}

btScalar Ship::getRadius() const
{
    return 10;
}

btScalar Ship::getSensorRange() const
{
    return m_sensorRange;
}

void Ship::setAttribute(const std::string &attr, double value)
{
    if(attr == "maxspeed")
    {
        m_maxSpeed = value;
    }
    else if(attr == "sensorrange") {
        m_sensorRange = value;
        prepareSensor();
    }
}
