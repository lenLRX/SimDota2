#ifndef __ABILITY_H__
#define __ABILITY_H__

#include <string>

class cppSimulatorImp;
class Sprite;

class Ability {
public:
    Ability(cppSimulatorImp* sim, Sprite* sprite, size_t id):
        ability_id(id),engine(sim),sprite(sprite)
    {  }
    virtual ~Ability() { }

    //we are sure that we can cast if this is true
    virtual bool isFullyCastable();

    //cool down is ready
    virtual bool isReady();

    virtual void activate();

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
private:
    cppSimulatorImp* engine;
    Sprite* sprite;
};

#endif //__ABILITY_H__
