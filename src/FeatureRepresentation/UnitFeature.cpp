#include "FeatureRepresentation/FeatureRepresentation.h"
#include "simulatorImp.h"
#include "Hero.h"
#include "Creep.h"

#define ONEHOT_SELF     1,0,0
#define ONEHOT_ALLY     0,1,0
#define ONEHOT_ENEMY    0,0,1

//no longer working ,just a example
static PyObject* getState(cppSimulatorImp* Engine, const std::string& side, int idx)
{
    Hero* hero = Engine->getHero(side, idx);
    int sign = side == "Radiant" ? 1 : -1;

    const auto& data = hero->getData();
    const auto& prev_data = hero->getPrevData();
    const auto& location = hero->get_location();

    auto nearby_ally = Engine->get_nearby_ally(hero, data.SightRange);
    size_t ally_input_size = nearby_ally.size();

    std::vector<PyObject*> vec_feature;

    const char* input_template = "(ffiii)";
    vec_feature.push_back(Py_BuildValue(input_template,
        sign * location.x / GET_CFG->map_div,
        sign * location.y / GET_CFG->map_div,
        ONEHOT_SELF));
    
    for (size_t i = 0; i < ally_input_size; ++i) {
        auto ally_loc = nearby_ally[i].first->get_location();
        vec_feature.push_back(Py_BuildValue(input_template,
            sign * (ally_loc.x - location.x) / GET_CFG->map_div,
            sign * (ally_loc.y - location.y) / GET_CFG->map_div,
            ONEHOT_ALLY));
    }

    auto nearby_enemy = Engine->get_nearby_enemy(hero, data.SightRange);
    size_t enemy_input_size = nearby_enemy.size();

    for (size_t i = 0; i < enemy_input_size; ++i) {
        auto enemy_loc = nearby_enemy[i].first->get_location();
        vec_feature.push_back(Py_BuildValue(input_template,
            sign * (enemy_loc.x - location.x) / GET_CFG->map_div,
            sign * (enemy_loc.y - location.y) / GET_CFG->map_div,
            ONEHOT_ENEMY));
    }

    PyObject* env_state = PyList_New(vec_feature.size());

    if (NULL == env_state) {
        printf("env_state error!\n");
        return NULL;
    }
    
    for (size_t i = 0; i < vec_feature.size(); ++i)
    {
        PyList_SetItem(env_state, i, vec_feature[i]);
    }

    double reward = (data.exp > prev_data.exp) - 0.1 * (data.HP < prev_data.HP);

    PyObject* ret = Py_BuildValue("(OdO)", env_state, reward, hero->isDead() ? Py_True : Py_False);
    Py_XDECREF(env_state);

    return ret;
}

REG_FEATURE_FN("unit_test1", getState);