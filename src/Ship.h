//
// Created by Snorri Sturluson on 28/10/2017.
//

#ifndef PROJECT_SHIP_H
#define PROJECT_SHIP_H

#include <cstdint>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <LinearMath/btDefaultMotionState.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "Types.h"

class Ship
{
public:
    Ship(esUserId owner, esTypeId shipType);

    void setTransform(const btTransform& t);

    void prepare();

    btCollisionShape* getCollisionShape();
    btRigidBody* getBody();
protected:
    btTransform m_transform;
    btScalar m_mass;
    btCollisionShape* m_collisionShape;
    btDefaultMotionState* m_motionState;
    btRigidBody* m_body;
};


#endif //PROJECT_SHIP_H
