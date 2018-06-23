#include "Ability/Shadowraze.h"
#include "Hero.h"
#include "simulatorImp.h"

void Shadowraze::activate()
{
    (void)isFullyCastable();
}

void Shadowraze::activate_i()
{
    if (!isFullyCastable())
    {
        return;
    }

    sprite->cost_MP(ManaCost);
    lastActivatedTime = engine->get_time();

    const auto& nearbyEnemy = engine->get_nearby_enemy(sprite, 700);
}
