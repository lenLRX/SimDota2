#include "FeatureRepresentation/FeatureRepresentation.h"
#include "simulatorImp.h"
#include "Hero.h"
#include "Creep.h"

//no longer working ,just a example
static PyObject* getState(cppSimulatorImp* Engine, const std::string& side, int idx)
{
    Hero* hero = Engine->getHero(side, idx);
    int sign = side == "Radiant" ? 1 : -1;

    const auto& data = hero->getData();
    const auto& location = hero->get_location();
    
    auto nearby_ally = Engine->get_nearby_ally(hero, data.SightRange, is_creep);
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

    auto nearby_enemy = Engine->get_nearby_enemy(hero, data.SightRange, is_creep);
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

    PyObject* state = Py_BuildValue("{s:O}", "env_input", env_state);
    Py_XDECREF(env_state);

    double reward = 0;

    PyObject* ret = Py_BuildValue("(OdO)", state, reward, hero->isDead() ? Py_True : Py_False);

    Py_DECREF(state);

    return ret;
}

REG_FEATURE_FN("origin1", getState);