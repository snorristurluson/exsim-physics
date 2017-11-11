//
// Created by Snorri Sturluson on 29/10/2017.
//

#ifndef PROJECT_TYPES_H
#define PROJECT_TYPES_H

#include <unordered_set>

typedef int64_t esTypeId;
typedef int64_t esUserId;

enum {
    esShip,
    esSensor
};

class Ship;
typedef std::unordered_set<Ship*> ShipSet;

#endif //PROJECT_TYPES_H
