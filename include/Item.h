#ifndef __ITEM_H__
#define __ITEM_H__

// items are "abilities" for all intensive purposes in Dota2 world
#include "Ability.h"

enum PowerTreadStat
{
    ATTRIBUTE_STRENGTH,
    ATTRIBUTE_AGILITY,
    ATTRIBUTE_INTELLECT,
};

class Item : public Ability {

public:
    Item(size_t id) : Ability(id)  { }
    ~Item() { }

    inline bool isCombinedLocked() { return is_combined_locked; }
    inline PowerTreadStat getPowerTreadsStat() { return power_tread_stat; }

protected:
    size_t charges;
    size_t secondary_charges;
    bool is_combined_locked;
    size_t power_tread_stat;
};

#endif //__ITEM_H__
