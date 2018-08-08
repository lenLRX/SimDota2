#include "ActionSpace/ActionSpace.h"
#include "simulatorImp.h"
#include "Hero.h"
#include "Creep.h"

static void setAction(cppSimulatorImp* Engine, PyObject* order, const std::string& side, int idx)
{
    int dir;
    Hero* hero = Engine->getHero(side, idx);
    int decision;
    if (!PyArg_ParseTuple(order, "ii", &decision, &dir)) {
        LOG << "Parse Arg error";
        return;
    }
    hero->set_decision(decision);
    if (decisonType::noop == decision) {
        ;
    }
    else if (decisonType::move == decision) {
        int sign = side == "Radiant" ? 1 : -1;
        double x, y;

        double rad = dir * M_PI_4 - M_PI;

        x = cos(rad);
        y = sin(rad);

        hero->set_move_order(pos_tup(sign * x * 1000,
            sign * y * 1000));
    }
    else if (decisonType::attack == decision) {
        hero->set_target(nullptr);
    }
}

static PyObject* getPredefineAction(cppSimulatorImp* Engine, const std::string& side, int idx)
{
    int sign = side == "Radiant" ? 1 : -1;
    Hero* hero = Engine->getHero(side, idx);
    const auto& data = hero->getData();
    const auto& location = hero->get_location();

    int dir = 8;//(0-7) 8 dirs, 8 is stop

    auto nearby_enemy = Engine->get_nearby_enemy(hero, data.SightRange);
    auto nearby_enemy_size = nearby_enemy.size();

    pos_tup ret;
    
    const int near_distance = 200;
    const int far_distance = 500;

    bool b_stop = false;

    int _dis = 700;
    if (nearby_enemy.size() > 0)
    {
        ret = nearby_enemy[0].first->get_location();
        int dist = nearby_enemy[0].second;

        if (dist > near_distance
            && dist < far_distance)
        {
            b_stop = true;
        }
        

        if (side == "Radiant") {
            ret = pos_tup(ret.x - _dis,
                ret.y - _dis);
        }
        else {
            ret = pos_tup(ret.x + _dis,
                ret.y + _dis);
        }
    }
    else {
        ret = pos_tup(-482, -400);
    }

    double dx = ret.x - location.x;
    double dy = ret.y - location.y;
    dx *= sign;
    dy *= sign;

    double a = atan2(dy, dx);

    if (b_stop)
    {
        dir = 8;
    }
    else
    {
        dir = (a + M_PI) / M_PI_4;
        if (dir < 0)
        {
            dir = 0;
        }
        if (dir > 7)
        {
            dir = 7;
        }
    }

    PyObject* obj = Py_BuildValue("(ii)", decisonType::move, dir);
    return obj;
}

REG_ActionSpace_FN("lattice1", setAction);
REG_PredefineAction_FN("lattice1", getPredefineAction);