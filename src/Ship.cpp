//
// Created by Snorri Sturluson on 28/10/2017.
//

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <iostream>
#include "Ship.h"

Ship::Ship(esUserId owner, esTypeId shipType) :
    m_owner(owner),
    m_type(shipType),
    m_body(nullptr),
    m_collisionShape(nullptr)
{
    // Todo: Details should come from shipType
    m_collisionShape = new btSphereShape(1.0);
    m_mass = btScalar(1.f);
    m_transform.setIdentity();
}

void Ship::setTransform(const btTransform &t)
{
    m_transform = t;

    // Todo: should this work after entering a solar system?
}

void Ship::prepare()
{
    btVector3 localInertia(0, 0, 0);
    m_collisionShape->calculateLocalInertia(m_mass, localInertia);

    m_motionState = new btDefaultMotionState(m_transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(
            m_mass, m_motionState, m_collisionShape, localInertia
    );
    m_body = new btRigidBody(rbInfo);
}

btCollisionShape *Ship::getCollisionShape()
{
    return m_collisionShape;
}

btRigidBody *Ship::getBody()
{
    return m_body;
}

esUserId Ship::getOwner()
{
    return m_owner;
}

esTypeId Ship::getType()
{
    return m_type;
}

btVector3 Ship::getPosition()
{
    btTransform t;
    m_motionState->getWorldTransform(t);
    return t.getOrigin();
}

btVector3 Ship::getVelocity()
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
        velocity = v.normalized() * 50.0;
    }
    else
    {
        std::cout << "Ship " << m_owner << "close to target" << std::endl;
    }

    m_body->setLinearVelocity(velocity);
}
