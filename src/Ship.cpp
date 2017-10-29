//
// Created by Snorri Sturluson on 28/10/2017.
//

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include "Ship.h"

Ship::Ship(esUserId owner, esTypeId shipType) :
    m_body(nullptr)
{
    // Todo: Details should come from shipType
    m_collisionShape = new btSphereShape(btScalar(1.));
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

    btDefaultMotionState* myMotionState = new btDefaultMotionState(m_transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(
            m_mass, myMotionState, m_collisionShape, localInertia
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
