#include "Creep.h"
#include "simulatorImp.h"

#include <string>
#include <random>
#include <fstream>

using namespace rapidjson;

class CreepDataExtFn;

class CreepDataExt
{
private:
    friend void CreepDataExtFn(SpriteData* s, std::string json_path);
    CreepDataExt(std::string json_path)
    {
        std::ifstream ifs(json_path);
        Document document;
        IStreamWrapper isw(ifs);
        document.ParseStream(isw);
        pos_tup tmp_tup;
        double temp_x;
        double temp_y;
        temp_x = document["Radiant"]["init_loc"]["x"].GetDouble();
        temp_y = document["Radiant"]["init_loc"]["y"].GetDouble();
        init_loc[(int)Side::Radiant] = pos_tup(temp_x, temp_y);

        temp_x = document["Dire"]["init_loc"]["x"].GetDouble();
        temp_y = document["Dire"]["init_loc"]["y"].GetDouble();
        init_loc[(int)Side::Dire] = pos_tup(temp_x, temp_y);

        temp_x = document["Radiant"]["dest"]["x"].GetDouble();
        temp_y = document["Radiant"]["dest"]["y"].GetDouble();
        dest[(int)Side::Radiant] = pos_tup(temp_x, temp_y);

        temp_x = document["Dire"]["dest"]["x"].GetDouble();
        temp_y = document["Dire"]["dest"]["y"].GetDouble();
        dest[(int)Side::Dire] = pos_tup(temp_x, temp_y);
    }
public:
    pos_tup init_loc[2];
    pos_tup dest[2];
};

void CreepDataExtFn(SpriteData* s, std::string json_path)
{
    s->ext = new CreepDataExt(json_path);
};

static std::default_random_engine rnd_gen;
static std::uniform_int_distribution<int> distribution(1, 10);
static std::uniform_int_distribution<int> pos_distribution(1, 500);
static std::uniform_int_distribution<int> sign_distribution(-1, 1);

static int get_rand()
{
    return sign_distribution(rnd_gen) * pos_distribution(rnd_gen);
}


DEF_INIT_DATA_FN(Creep)

Creep::Creep(cppSimulatorImp* _Engine, Side _side, std::string type_name)
    :Sprite(INIT_DATA(_Engine, Creep, type_name, CreepDataExtFn))
{
    Engine = _Engine;
    unit_type = UNITTYPE_LANE_CREEP;
    side = _side;
    

    //random atk
    data.Attack += (distribution(rnd_gen) - 5);

    CreepDataExt* p_ext = (CreepDataExt*)data.ext;
    init_loc = p_ext->init_loc[(int)side];
    dest = p_ext->dest[(int)side];

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

Creep::~Creep()
{
}

void Creep::step()
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
        else {
            set_move(target->get_location());
        }
    }
    else {
        set_move(dest);
    }
}

void Creep::draw()
{
    if (v_handle != NULL) {
        auto p = pos_in_wnd();
        Py_XDECREF(PyObject_CallMethod(canvas,
            "coords",
            "(Odddd)",
            v_handle,
            p.x - data.viz_radius,
            p.y + data.viz_radius,
            p.x + data.viz_radius,
            p.y - data.viz_radius));
    }
}
