#include "btBulletDynamicsCommon.h"
#include "Solarsystem.h"
#include "Ship.h"
#include "CommandHandler.h"

int main(int argc, char** argv)
{
    Solarsystem solarsystem;

    CommandHandler handler;
    handler.start(&solarsystem, 4041);
}
