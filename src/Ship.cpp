//
// Created by Snorri Sturluson on 28/10/2017.
//

#include <BulletCollision/CollisionShapes/btSphereShape.h>
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
    //m_transform = static_cast<btTransform*>(btAlignedAlloc(sizeof(m_transform), 16));
    //m_transform->setIdentity();
}

void Ship::setTransform(const btTransform &t)
{
    *m_transform = t;

    // Todo: should this work after entering a solar system?
}

void Ship::prepare()
{
    return;
    btVector3 localInertia(0, 0, 0);
    m_collisionShape->calculateLocalInertia(m_mass, localInertia);

    m_motionState = nullptr; //new btDefaultMotionState(*m_transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(
            m_mass, m_motionState, m_collisionShape, localInertia
    );
    m_body = new btRigidBody(rbInfo);
    m_body->setLinearVelocity(btVector3(15, 15, 0.0));
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
