#ifndef __CREEP_H__
#define __CREEP_H__

#include "Sprite.h"
#include <string>
#include <unordered_map>

class Creep:public Sprite{
public:
    Creep(cppSimulatorImp* Engine,Side side,std::string type_name);
    ~Creep();
    virtual void step();
    virtual void draw();
private:
    pos_tup init_loc;
    pos_tup dest;
    std::string color;
};

static auto is_creep = [](Sprite* s) -> bool { return dynamic_cast<Creep*>(s) != nullptr; };

#endif//__CREEP_H__
