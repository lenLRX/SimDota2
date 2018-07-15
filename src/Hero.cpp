#include "Hero.h"
#include "Creep.h"
#include "log.h"
#include "simulatorImp.h"
#include <cmath>
#include <random>

using namespace rapidjson;

class HeroDataExtFn;

class HeroDataExt
{
private:
    friend void HeroDataExtFn(SpriteData* s, std::string json_path);
    HeroDataExt(std::string json_path)
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
    }
public:
    pos_tup init_loc[2];
};

void HeroDataExtFn(SpriteData* s, std::string json_path)
{
    s->ext = new HeroDataExt(json_path);
};


static std::default_random_engine rnd_gen;
static std::uniform_int_distribution<int> pos_distribution(1, 1000);
static std::uniform_int_distribution<int> sign_distribution(-1, 1);

static int get_rand()
{
    return sign_distribution(rnd_gen) * pos_distribution(rnd_gen);
}

DEF_INIT_DATA_FN(Hero)

Hero::Hero(cppSimulatorImp* _Engine, Side _side, std::string type_name)
    :Sprite(INIT_DATA(_Engine, Hero, type_name, HeroDataExtFn)), target(nullptr)
{
    Engine = _Engine;
    unit_type = UNITTYPE_HERO;
    side = _side;

    last_exp = 0.0;
    last_HP = data.HP;

    HeroDataExt* p_ext = (HeroDataExt*)data.ext;
    init_loc = p_ext->init_loc[(int)side];
    if (side == Side::Radiant) {
        color = GET_CFG->Radiant_Colors;
    }
    else {
        color = GET_CFG->Dire_Colors;
    }

    location = init_loc;
    move_order = pos_tup(0,0);

    if (Engine->get_canvas() != NULL) {
        canvas = Engine->get_canvas();
        pos_tup p = pos_in_wnd();
        PyObject* create_rectangle = PyObject_GetAttrString(canvas, "create_oval");
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

Hero::~Hero()
{
    LOG << "gold:" << gold << endl;
    Logger::getInstance().flush();
}

int Hero::get_max_health()
{
    //return 200 + 20*(base_strength + strength_gain*level);
    return 500;
}

int Hero::get_max_mana()
{
    //return 75 + 12*(base_intelligence + intelligence_gain*level);
    return 291;
}

void Hero::step()
{
    if (isAttacking())
        return;
    if (decisonType::noop == decision) {
        ;
    }
    else if (decisonType::move == decision) {
        auto p = pos_tup(move_order.x + location.x,
            move_order.y + location.y);
        set_move(p);
    }
    else if (decisonType::attack == decision) {
        if (nullptr == target) {
            LOG << "null target!\n";
            Logger::getInstance().flush();
            exit(1);
        }
        attack(target);
    }
    
}

void Hero::draw()
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

void Hero::set_order(PyObject* order)
{
}

void Hero::set_target(Sprite* s)
{
    target = s;
}

const target_list_t& Hero::getTargetList()
{
    return target_list;
}

void Hero::set_decision(int d)
{
    decision = d;
}

PyObject* Hero::get_state_tup()
{
    int sign = side == Side::Radiant ? 1 : -1 ;
    
    auto nearby_ally = Engine->get_nearby_ally(this, data.SightRange, is_creep);
    size_t ally_input_size = nearby_ally.size();
    double ally_x = 0.0;
    double ally_y = 0.0;
    for (size_t i = 0; i < ally_input_size; ++i) {
        auto ally_loc = nearby_ally[i].first->get_location();
        ally_x += sign * (ally_loc.x - location.x) / GET_CFG->map_div;
        ally_y += sign * (ally_loc.y - location.y) / GET_CFG->map_div;
    }

    if (0 != ally_input_size) {
        ally_x /= (double)ally_input_size;
        ally_y /= (double)ally_input_size;
    }

    auto nearby_enemy = Engine->get_nearby_enemy(this, data.SightRange, is_creep);
    size_t enemy_input_size = nearby_enemy.size();
    double enemy_x = 0.0;
    double enemy_y = 0.0;
    for (size_t i = 0; i < enemy_input_size; ++i) {
        auto enemy_loc = nearby_enemy[i].first->get_location();
        enemy_x += sign * (enemy_loc.x - location.x) / GET_CFG->map_div;
        enemy_y += sign * (enemy_loc.y - location.y) / GET_CFG->map_div;
    }

    if (0 != enemy_input_size) {
        enemy_x /= (double)enemy_input_size;
        enemy_y /= (double)enemy_input_size;
    }

    PyObject* env_state = Py_BuildValue("[dddidddddd]",
        sign * location.x / GET_CFG->map_div,
        sign * location.y / GET_CFG->map_div,
        data.Attack,
        side,
        ally_x,
        ally_y,
        (double)ally_input_size,
        enemy_x,
        enemy_y,
        (double)enemy_input_size
    );

    if (NULL == env_state) {
        printf("env_state error!\n");
        return NULL;
    }

    PyObject* state_targets_list;
    if (enemy_input_size > 0) {
        target_list.clear();
        state_targets_list = PyList_New(enemy_input_size);
        for (int i = 0; i < enemy_input_size; i++) {
            PyList_SET_ITEM(state_targets_list,i, Py_BuildValue("(dd)",
                nearby_enemy[i].first->get_HP(), nearby_enemy[i].second / data.AttackRange));
            target_list.push_back(nearby_enemy[i].first);
        }
        //LOG << "target_list.size " << target_list.size() << endl;
    }
    else {
        state_targets_list = Py_BuildValue("[]");
    }

    PyObject* state = Py_BuildValue("{s:O,s:O}", "env_input", env_state, "target_input", state_targets_list);
    Py_XDECREF(env_state);
    Py_XDECREF(state_targets_list);

    double reward = (exp - last_exp) + (data.HP - last_HP) + (gold - last_gold);
    reward *= 0.001;

    last_exp = exp;
    last_HP = data.HP;
    last_gold = gold;

    PyObject* ret = Py_BuildValue("(OdO)", state, reward, _isDead ? Py_True : Py_False);

    Py_DECREF(state);

    return ret;
}

PyObject* Hero::predefined_step()
{
    Py_RETURN_NONE;
}
