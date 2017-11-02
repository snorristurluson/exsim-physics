#include "btBulletDynamicsCommon.h"
#include "Solarsystem.h"
#include "Ship.h"
#include "CommandHandler.h"

int main(int argc, char** argv)
{
    int port = atoi(argv[1]);

    CommandHandler handler;
    handler.start(port);
}
