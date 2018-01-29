#ifndef __ABILITY_H__
#define __ABILITY_H__

#include <string>
#include <limits>

class cppSimulatorImp;
class Sprite;

/*

check isFullyCastable
activate

wait until ability point

check isFullyCastable again
activate_i
cost mana, enter cool down deal damadge and so on...

*/

class Ability {
public:
    Ability(cppSimulatorImp* sim, Sprite* sprite, size_t id):
        ability_id(id),engine(sim),sprite(sprite),
        //lastActivatedTime(std::numeric_limits<float>::lowest()) //it will overflow?
        lastActivatedTime(-10000)
    {  }
    virtual ~Ability() { }

    //we are sure that we can cast if this is true
    virtual bool isFullyCastable();

    //cool down is ready
    virtual bool isReady();

    //when you press the key
    virtual void activate();

    //at ability point
    //when the ability is casted enter cd and cost mana
    virtual void activate_i();

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
    float lastActivatedTime;
    float cd;//tmp var
    cppSimulatorImp* engine;
    Sprite* sprite;
};

#endif //__ABILITY_H__
