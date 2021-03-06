#include "Tower.h"
#include "simulatorImp.h"
#include <unordered_map>


DEF_INIT_DATA_FN(Tower)

Tower::Tower(cppSimulatorImp* _Engine,
    Side _side, std::string type_name, pos_tup init_loc)
    :Sprite(INIT_DATA(_Engine, Tower, type_name, nullptr))
{
    Engine = _Engine;
    unit_type = UNITTYPE_TOWER;
    side = _side;

    if (side == Side::Radiant) {
        color = GET_CFG->Radiant_Colors;
    }
    else {
        color = GET_CFG->Dire_Colors;
    }

    location = init_loc;

    if (Engine->get_canvas() != NULL) {
        canvas = Engine->get_canvas();
        pos_tup p = pos_in_wnd();
        PyObject* create_rectangle = PyObject_GetAttrString(canvas, "create_rectangle");
        PyObject* args = Py_BuildValue("(dddd)",
            p.x - data.viz_radius,
            p.y + data.viz_radius,
            p.x + data.viz_radius,
            p.y - data.viz_radius);
        PyObject* kwargs = Py_BuildValue("{s:s}", "fill", color.c_str());
        v_handle = PyObject_Call(create_rectangle, args, kwargs);
        Py_DECREF(kwargs);
        Py_DECREF(args);
        Py_DECREF(create_rectangle);
    }
}

Tower::~Tower()
{
}

void Tower::step()
{
    if (isDead())
        return;
    if (isAttacking())
        return;
    auto nearby_enemy = Engine->get_nearby_enemy(this, data.SightRange);
    if (!nearby_enemy.empty()) {
        Sprite* target = nearby_enemy.front().first;
        if (nearby_enemy.front().second < data.AttackRange) {
            attack(target);
        }
    }
}

void Tower::draw()
{
}

void Tower::initTowers(cppSimulatorImp* Engine)
{
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup( -1661,-1505 )));
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup( -6254, 1823 )));
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup( 4922, -6122 )));
    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup( 1032, 359 )));
    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup( -4706, 6022 )));
    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup( 6242, -1610 )));
    //TODO they are not T1 tower
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup(-6149, -852)));
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup(-3562, -2803)));
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup(-130, -6218)));

    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup(-6571, -3431)));
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup(-4670, -4100)));
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup(-4002, -6069)));

    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup(-5691, -4804)));
    Engine->addSprite(new Tower(Engine, Side::Radiant, "Tier1Tower", pos_tup(-5410, -5208)));

    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup(11, 6013)));
    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup(2528, 2128)));
    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup(6189, 368)));

    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup(3566, 5786)));
    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup(4253, 3748)));
    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup(6294, 3016)));

    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup(4974, 4776)));
    Engine->addSprite(new Tower(Engine, Side::Dire, "Tier1Tower", pos_tup(5309, 4427)));
}
