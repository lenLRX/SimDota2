#ifndef __ABILITY_H__
#define __ABILITY_H__

class Ability {
public:
    Ability(size_t id)  { ability_id = id; }
    ~Ability() { }

protected:
    std::string name;

    size_t ability_id;
    //size_t slot;
    size_t level;
    size_t cast_range;
    float channel_time;
    float cooldown_remaining;
    
    bool is_activated;
    bool is_toggled;
    bool is_in_ability_phase;
    bool is_channeling;
    bool is_stolen;
};

#endif //__ABILITY_H__
