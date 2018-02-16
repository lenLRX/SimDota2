#ifndef __HERO_H__
#define __HERO_H__

#include "Sprite.h"

#include <string>
#include <vector>

//forward decl;
class cppSimulatorImp;

enum decisonType {
    noop = 0,
    move = 1,
    attack = 2
};

using target_list_t = std::vector<Sprite*>;

class Hero:public Sprite {
public:
    Hero(cppSimulatorImp* _Engine,
        Side _side, std::string type_name);
    ~Hero();
    virtual void step();
    virtual void draw();
    void set_order(PyObject* order);
    PyObject* get_state_tup();
    std::vector<float> get_state_tup_native();
    decisonType apply_predef_step();
    PyObject* predefined_step();
    
    inline unsigned int get_level() { return level; }
    
    int get_max_health();
    int get_max_mana();
    
private:
    pos_tup init_loc;
    pos_tup move_order;
    int decision;
    Sprite* target;
    target_list_t target_list;
    std::vector<target_list_t> histroy_target_lists;
    double viz_radius;
    double last_gold;
    double last_exp;
    double last_HP;
    
    // Hero Specific Data (pulled from heroes.json)
    unsigned int level;
    float base_strength;
    float base_agility;
    float base_intelligence;
    float strength_gain;
    float agility_gain;
    float intelligence_gain;
    
    float attack_rate;
    unsigned int attack_range;
    unsigned int projectile_speed;
    
    unsigned int movement_speed;
    float turn_rate;
    
    std::string color;
};

static std::vector<float> get_action(int id)
{
    int x = id / 3;
    int y = id / 3;
    std::vector<float> ret;
    ret.push_back(x - 1);
    ret.push_back(y - 1);
    return ret;
}

#endif//__HERO_H__