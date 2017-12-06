//
// Created by Snorri Sturluson on 28/10/2017.
//

#ifndef PROJECT_SHIP_H
#define PROJECT_SHIP_H

#include <cstdint>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <LinearMath/btDefaultMotionState.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "Types.h"

class Ship
{
public:
    Ship(esUserId owner, esTypeId shipType);

    void setTransform(const btTransform& t);

    void addToWorld(btDiscreteDynamicsWorld *world);
    void removeFromWorld();

    btCollisionShape* getCollisionShape() const;
    btRigidBody* getBody() const;

    esUserId getOwner() const;
    esTypeId getType() const;
    btVector3 getPosition() const;
    btVector3 getVelocity() const;

    void setTargetLocation(const btVector3& loc);
    void update(btScalar dt);

    void setInRange(const ShipSet& ships);
    ShipSet getInRange() const;
    ShipSet getNewInRange() const;
    ShipSet getGoneFromRange() const;

    btScalar getSensorRange() const;
    btScalar getRadius() const;

    void setAttribute(const std::string &attr, double value);

protected:
    ATTRIBUTE_ALIGNED16(btTransform m_transform);
    esUserId m_owner;
    esTypeId m_type;
    btScalar m_mass;
    btScalar m_maxSpeed;
    btScalar m_sensorRange;
    btCollisionShape* m_collisionShape;
    btCollisionShape* m_sensorShape;
    btDefaultMotionState* m_motionState;
    btDefaultMotionState* m_sensorMotionState;
    btRigidBody* m_body;
    btRigidBody* m_sensor;
    btDiscreteDynamicsWorld* m_dynamicsWorld;

    btVector3 m_targetLocation;
    ShipSet m_inRange;
    ShipSet m_prevInRange;
    ShipSet m_newInRange;
    ShipSet m_goneFromRange;

    void prepareBody();
    void prepareSensor();
};


#endif //PROJECT_SHIP_H
