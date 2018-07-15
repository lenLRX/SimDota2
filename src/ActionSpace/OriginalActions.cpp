#include "ActionSpace/ActionSpace.h"
#include "simulatorImp.h"
#include "Hero.h"
#include "Creep.h"

static void setAction(cppSimulatorImp* Engine, PyObject* order, const std::string& side, int idx)
{
    PyObject* subdecision;
    Hero* hero = Engine->getHero(side, idx);
    int decision;
    if (!PyArg_ParseTuple(order, "iO", &decision, &subdecision)) {
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
        if (!PyArg_ParseTuple(subdecision, "dd", &x, &y)) {
            LOG << "Parse Arg error";
            return;
        }
        hero->set_move_order(pos_tup(sign * x * 1000,
            sign * y * 1000));
    }
    else if (decisonType::attack == decision) {
        hero->set_target(nullptr);
        int target_idx = PyLong_AsLong(subdecision);
        auto target_list = hero->getTargetList();
        if (target_idx >= (int)target_list.size()) {
            LOG << "index out of range! target_list size:" << target_list.size() << "," << target_idx << endl;
            exit(4);
        }
        hero->set_target(target_list[target_idx]);
    }
}

static PyObject* getPredefineAction(cppSimulatorImp* Engine, const std::string& side, int idx)
{
    int sign = side == "Radiant" ? 1 : -1;
    Hero* hero = Engine->getHero(side, idx);
    const auto& data = hero->getData();
    const auto& location = hero->get_location();


    auto nearby_enemy = Engine->get_nearby_enemy(hero, data.SightRange);
    auto nearby_enemy_size = nearby_enemy.size();

    pos_tup ret;
    int _dis = 700;
    if (nearby_enemy.size() > 0)
    {
        ret = nearby_enemy[0].first->get_location();
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

    double a = std::atan2(dy, dx);
    PyObject* obj = Py_BuildValue("(i(dd))", decisonType::move, std::cos(a), std::sin(a));
    return obj;
}

REG_ActionSpace_FN("original" , setAction);
REG_PredefineAction_FN("original", getPredefineAction);
