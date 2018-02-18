#include "Hero.h"
#include "Creep.h"
#include "log.h"
#include "simulatorImp.h"
#include <cmath>
#include <random>

//TODO use json

static std::default_random_engine rnd_gen;
static std::uniform_int_distribution<int> pos_distribution(1, 1000);
static std::uniform_int_distribution<int> sign_distribution(-1, 1);

static SpriteDataType HeroData {
    { "ShadowFiend", {
        { "HP",new double(500) },
        { "MP",new double(273) },
        { "MovementSpeed",new double(313) },
        { "Armor", new double(0.86) },
        { "Attack", new double(47) },
        { "AttackRange",new double(500) },
        { "SightRange", new double(1800) },
        { "Bounty", new double(200) },
        { "bountyEXP", new double(0) },
        { "BaseAttackTime", new double(1.7) },
        { "AttackSpeed", new double(120) },
        { "AtkPoint", new double(0.5) },
        { "AtkBackswing", new double(0.54) },
        { "ProjectileSpeed", new double(1200) },
        { "atktype", new AtkType(ranged) },
        { "atkDmgType", new AtkDmgType(AtkDmgType::Hero) },
        { "armorType", new ArmorType(ArmorType::Hero) }
    }}
};

static int get_rand()
{
    return sign_distribution(rnd_gen) * pos_distribution(rnd_gen);
}

Hero::Hero(cppSimulatorImp* _Engine, Side _side, std::string type_name):target(nullptr)
{
    Engine = _Engine;
    unit_type = UNITTYPE_HERO;
    side = _side;
    const auto& data = HeroData[type_name];
    INIT_ATTR_BY(data);

    last_exp = 0.0;
    last_HP = HP;

    _update_para();

    viz_radius = 5;
    if (side == Side::Radiant) {
        init_loc = pos_tup(-7205 + get_rand(), -6610 + get_rand());
        color = Config::Radiant_Colors;
    }
    else {
        init_loc = pos_tup(7000 + get_rand(), 6475 + get_rand());
        color = Config::Dire_Colors;
    }

    location = init_loc;
    move_order = pos_tup(0,0);

    if (Engine->get_canvas() != NULL) {
        canvas = Engine->get_canvas();
        pos_tup p = pos_in_wnd();
        PyObject* create_rectangle = PyObject_GetAttrString(canvas, "create_oval");
        PyObject* args = Py_BuildValue("(dddd)",
            std::get<0>(p) - viz_radius,
            std::get<1>(p) + viz_radius,
            std::get<0>(p) + viz_radius,
            std::get<1>(p) - viz_radius);
        PyObject* kwargs = Py_BuildValue("{s:s}", "fill", color.c_str());
        v_handle = PyObject_Call(create_rectangle, args, kwargs);
        Py_DECREF(kwargs);
        Py_DECREF(args);
        Py_DECREF(create_rectangle);
    }
}

Hero::~Hero()
{
    //LOG << "gold:" << gold << endl;
    //Logger::getInstance().flush();
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
        auto p = pos_tup(std::get<0>(move_order) + std::get<0>(location),
            std::get<1>(move_order) + std::get<1>(location));
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
            std::get<0>(p) - viz_radius,
            std::get<1>(p) + viz_radius,
            std::get<0>(p) + viz_radius,
            std::get<1>(p) - viz_radius));
    }
    
}

void Hero::set_order(PyObject* order)
{
    PyObject* subdecision;
    if (!PyArg_ParseTuple(order, "iO", &decision, &subdecision)) {
        LOG << "Parse Arg error";
        return;
    }
    if (decisonType::noop == decision) {
        ;
    }
    else if (decisonType::move == decision) {
        int sign = side == Side::Radiant ? 1 : -1;
        double x, y;
        if (!PyArg_ParseTuple(subdecision, "dd", &x, &y)) {
            LOG << "Parse Arg error";
            return;
        }
        move_order = pos_tup(sign * x * 1000,
            sign * y * 1000);
    }
    else if (decisonType::attack == decision) {
        target = nullptr;
        int target_idx = PyLong_AsLong(subdecision);
        if (target_idx >= (int)target_list.size()) {
            LOG << "index out of range! target_list size:" << target_list.size() << "," << target_idx << endl;
            exit(4);
        }
        target = target_list[target_idx];
    }
    
}

static auto is_creep = [](Sprite* s) -> bool { return dynamic_cast<Creep*>(s) != nullptr; };

PyObject* Hero::get_state_tup()
{
    int sign = side == Side::Radiant ? 1 : -1 ;
    
    auto nearby_ally = Engine->get_nearby_ally(this, is_creep);
    size_t ally_input_size = nearby_ally.size();
    double ally_x = 0.0;
    double ally_y = 0.0;
    for (size_t i = 0; i < ally_input_size; ++i) {
        ally_x += sign * (std::get<0>(nearby_ally[i].first->get_location()) - std::get<0>(location)) / Config::map_div;
        ally_y += sign * (std::get<1>(nearby_ally[i].first->get_location()) - std::get<1>(location)) / Config::map_div;
    }

    if (0 != ally_input_size) {
        ally_x /= (double)ally_input_size;
        ally_y /= (double)ally_input_size;
    }

    auto nearby_enemy = Engine->get_nearby_enemy(this, is_creep);
    size_t enemy_input_size = nearby_enemy.size();
    double enemy_x = 0.0;
    double enemy_y = 0.0;
    double weakest_dist = 100000;
    double weakest_hp = 10000;

    for (size_t i = 0; i < enemy_input_size; ++i) {
        auto pEnemy = nearby_enemy[i].first;
        enemy_x += sign * (std::get<0>(pEnemy->get_location()) - std::get<0>(location)) / Config::map_div;
        enemy_y += sign * (std::get<1>(pEnemy->get_location()) - std::get<1>(location)) / Config::map_div;
        target_list.push_back(pEnemy);
        if (pEnemy->get_HP() < weakest_hp)
        {
            weakest_hp = pEnemy->get_HP();
            weakest_dist = S2Sdistance(*this, *pEnemy);
        }
    }

    if (0 != enemy_input_size) {
        enemy_x /= (double)enemy_input_size;
        enemy_y /= (double)enemy_input_size;
        weakest_hp = 0;
        weakest_dist = 0;
    }

    if (enemy_input_size > 0)
    {
        weakest_hp = weakest_hp / 1000;
        weakest_dist = weakest_dist / Config::map_div;
    }
    else
    {
        weakest_hp = 2;
        weakest_dist = 2;
    }

    double nearest_ally_x;
    double nearest_ally_y;

    if (ally_input_size > 0)
    {
        auto pAlly = nearby_ally[0].first;
        nearest_ally_x = sign * (std::get<0>(pAlly->get_location()) - std::get<0>(location)) / Config::map_div;
        nearest_ally_y = sign * (std::get<1>(pAlly->get_location()) - std::get<1>(location)) / Config::map_div;
    }
    else
    {
        nearest_ally_x = 2;
        nearest_ally_y = 2;
    }

    double nearest_enemy_x;
    double nearest_enemy_y;
    if (enemy_input_size > 0)
    {
        auto pEnemy = nearby_enemy[0].first;
        nearest_enemy_x = sign * (std::get<0>(pEnemy->get_location()) - std::get<0>(location)) / Config::map_div;
        nearest_enemy_y = sign * (std::get<1>(pEnemy->get_location()) - std::get<1>(location)) / Config::map_div;
    }
    else
    {
        nearest_enemy_x = 2;
        nearest_enemy_y = 2;
    }

    PyObject* env_state = Py_BuildValue("[dddidddddd]",
        sign * std::get<0>(location) / Config::map_div,
        sign * std::get<1>(location) / Config::map_div,
        Attack,
        side,
        weakest_hp,
        weakest_dist,
        nearest_ally_x,
        nearest_ally_y,
        nearest_enemy_x,
        nearest_enemy_y
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
            PyList_SET_ITEM(state_targets_list,i, Py_BuildValue("(dd)", nearby_enemy[i].first->get_HP(), nearby_enemy[i].second / AttackRange));
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

    double reward = (exp - last_exp) + (gold - last_gold);
    reward *= 0.001;

    last_exp = exp;
    last_HP = HP;
    last_gold = gold;

    PyObject* ret = Py_BuildValue("(OdO)", state, reward, _isDead ? Py_True : Py_False);

    Py_DECREF(state);

    return ret;
}

std::map<std::string, std::vector<float>> Hero::get_state_tup_native()
{
    std::map<std::string, std::vector<float>> result;
    int sign = side == Side::Radiant ? 1 : -1;

    auto nearby_ally = Engine->get_nearby_ally(this, is_creep);
    size_t ally_input_size = nearby_ally.size();
    double ally_x = 0.0;
    double ally_y = 0.0;
    for (size_t i = 0; i < ally_input_size; ++i) {
        ally_x += sign * (std::get<0>(nearby_ally[i].first->get_location()) - std::get<0>(location)) / Config::map_div;
        ally_y += sign * (std::get<1>(nearby_ally[i].first->get_location()) - std::get<1>(location)) / Config::map_div;
    }

    if (0 != ally_input_size) {
        ally_x /= (double)ally_input_size;
        ally_y /= (double)ally_input_size;
    }

    auto nearby_enemy = Engine->get_nearby_enemy(this, is_creep);
    size_t enemy_input_size = nearby_enemy.size();
    double enemy_x = 0.0;
    double enemy_y = 0.0;
    double weakest_dist = 100000;
    double weakest_hp = 10000;

    for (size_t i = 0; i < enemy_input_size; ++i) {
        auto pEnemy = nearby_enemy[i].first;
        enemy_x += sign * (std::get<0>(pEnemy->get_location()) - std::get<0>(location)) / Config::map_div;
        enemy_y += sign * (std::get<1>(pEnemy->get_location()) - std::get<1>(location)) / Config::map_div;
        target_list.push_back(pEnemy);
        if (pEnemy->get_HP() < weakest_hp)
        {
            weakest_hp = pEnemy->get_HP();
            weakest_dist = S2Sdistance(*this, *pEnemy);
        }
    }

    if (0 != enemy_input_size) {
        enemy_x /= (double)enemy_input_size;
        enemy_y /= (double)enemy_input_size;
        weakest_hp = 0;
        weakest_dist = 0;
    }

    std::vector<float> env_state;
    env_state.resize(6);

    env_state[0] = sign * std::get<0>(location) / Config::map_div;
    env_state[1] = sign * std::get<1>(location) / Config::map_div;
    env_state[2] = Attack;
    env_state[3] = (float)side;
    if (enemy_input_size > 0)
    {
        env_state[4] = weakest_hp / 1000;
        env_state[5] = weakest_dist / Config::map_div;
    }
    else
    {
        env_state[4] = 2;
        env_state[5] = 2;
    }
    
    result["env_state"] = env_state;

    std::vector<float> move_state = env_state;

    if (ally_input_size > 0)
    {
        auto pAlly = nearby_ally[0].first;
        move_state.push_back(sign * (std::get<0>(pAlly->get_location()) - std::get<0>(location)) / Config::map_div);
        move_state.push_back(sign * (std::get<1>(pAlly->get_location()) - std::get<1>(location)) / Config::map_div);
    }
    else
    {
        move_state.push_back(2);
        move_state.push_back(2);
    }

    if (enemy_input_size > 0)
    {
        auto pEnemy = nearby_enemy[0].first;
        move_state.push_back(sign * (std::get<0>(pEnemy->get_location()) - std::get<0>(location)) / Config::map_div);
        move_state.push_back(sign * (std::get<1>(pEnemy->get_location()) - std::get<1>(location)) / Config::map_div);
    }
    else
    {
        move_state.push_back(2);
        move_state.push_back(2);
    }

    result["move_state"] = move_state;
    return result;
}

const static float g_rad[] = {
    -2.356194490192345f,
    -1.5707963267948966f,
    -0.7853981633974483f,
    3.141592653589793f,
    100000.0f,
    0.0f,
    2.356194490192345f,
    1.5707963267948966f,
    0.7853981633974483f
};

DecisionTuple Hero::apply_predef_step()
{
    /*
    if (isAttacking()) {
        decision = decisonType::noop;
        return decisonType::noop;
    }
    */
    int sign = side == Side::Radiant ? 1 : -1;
    auto nearby_enemy = Engine->get_nearby_enemy(this, is_creep);
    auto nearby_enemy_size = nearby_enemy.size();
    auto targetlist_size = target_list.size();
    DecisionTuple result;
    if (targetlist_size > 0)
    {
        for (int i = 0; i < targetlist_size; ++i) {
            if (!target_list[i]->isDead() && target_list[i]->get_HP() < Attack) {
                target = target_list[i];
                decision = decisonType::attack;
                result.dtype = decisonType::attack;
                result.targetIdx = i;
                return result;
            }
        }
    }
    pos_tup ret;
    int _dis = 450;
    if (nearby_enemy.size() > 0)
    {
        ret = nearby_enemy[0].first->get_location();
        if (side == Side::Radiant) {
            ret = pos_tup(std::get<0>(nearby_enemy[0].first->get_location()) - _dis,
                std::get<1>(nearby_enemy[0].first->get_location()) - _dis);
        }
        else {
            ret = pos_tup(std::get<0>(nearby_enemy[0].first->get_location()) + _dis,
                std::get<1>(nearby_enemy[0].first->get_location()) + _dis);
        }
    }
    else {
        ret = pos_tup(-482, -400);
    }

    double dx = std::get<0>(ret) - std::get<0>(location);
    double dy = std::get<1>(ret) - std::get<1>(location);
    dx *= sign;
    dy *= sign;

    double a = std::atan2(dy, dx);
    double min_d = 10000000;
    int min_idx = -1;
    for (int i = 0; i < 9; ++i)
    {
        double _d = abs(g_rad[i] - a);
        if (_d < min_d)
        {
            min_idx = i;
            min_d = _d;
        }
    }
    auto dir = get_action(min_idx);
    move_order = pos_tup(sign * dir[0] * 1000,
        sign * dir[1] * 1000);
    decision = decisonType::move;
    result.dtype = decisonType::move;
    result.moveDir = min_idx;
    return result;
}

PyObject* Hero::predefined_step()
{
    /*
    //this is real bot should do, but it is hard to train
    if (isAttacking()) {
        Py_INCREF(Py_None);
        PyObject* obj = Py_BuildValue("(iO)", decisonType::noop, Py_None);
        return obj;
    }
    */
    int sign = side == Side::Radiant ? 1 : -1;
    auto nearby_enemy = Engine->get_nearby_enemy(this, is_creep);
    auto nearby_enemy_size = nearby_enemy.size();
    auto targetlist_size = target_list.size();
    if (targetlist_size > 0)
    {
        for (int i = 0; i < targetlist_size; ++i) {
            if (!target_list[i]->isDead() && target_list[i]->get_HP() < Attack) {
                PyObject* obj = Py_BuildValue("(ii)", decisonType::attack, i);
                return obj;
            }
        }
    }
    pos_tup ret;
    int _dis = 450;
    if (nearby_enemy.size() > 0)
    {
        ret = nearby_enemy[0].first->get_location();
        if (side == Side::Radiant) {
            ret = pos_tup(std::get<0>(nearby_enemy[0].first->get_location()) - _dis,
                std::get<1>(nearby_enemy[0].first->get_location()) - _dis);
        }
        else {
            ret = pos_tup(std::get<0>(nearby_enemy[0].first->get_location()) + _dis,
                std::get<1>(nearby_enemy[0].first->get_location()) + _dis);
        }
    }
    else {
        ret = pos_tup(-482, -400);
    }
    
    double dx = std::get<0>(ret) - std::get<0>(location);
    double dy = std::get<1>(ret) - std::get<1>(location);
    dx *= sign;
    dy *= sign;

    double a = std::atan2(dy, dx);
    PyObject* obj = Py_BuildValue("(i(dd))", decisonType::move, std::cos(a), std::sin(a));
    return obj;
}
