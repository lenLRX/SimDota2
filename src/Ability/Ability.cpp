#include "Ability/Ability.h"
#include "simulatorImp.h"

bool Ability::isFullyCastable()
{
    return isReady();
}

bool Ability::isReady()
{
    return engine->get_time() - lastActivatedTime > cd;;
}

void Ability::activate()
{
    //TODO: output error log
}

void Ability::activate_i()
{
    //TODO: output error log
}
