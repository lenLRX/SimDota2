#include "FeatureRepresentation/FeatureRepresentation.h"
#include "simulatorImp.h"
#include "Hero.h"
#include "Creep.h"

const static int step_size = 50;
const static int sight = 20;// 20 x 50 -> 1000

static int xy2idx(int x, int y)
{
    int idx_x = x / (sight * step_size) + sight;
    int idx_y = y / (sight * step_size) + sight;
    int _2sight = sight * 2;
    idx_x = idx_x < _2sight ? idx_x : _2sight;
    idx_y = idx_y < _2sight ? idx_y : _2sight;
    idx_x = idx_x >= 0 ? idx_x : 0;
    idx_y = idx_y >= 0 ? idx_y : 0;
    //printf("(%d,%d) -> %d\n", x, y, idx_y * _2sight + idx_x);
    return idx_y * _2sight + idx_x;
}

/*
            0 0 0 0
            1 0 0 0
            0 0 0 0
*/

static PyObject* getNearbyUnit(const VecSpriteDist& vec, const pos_tup& hero_loc, int sign)
{
    int total_pixel = (2 * sight) * (2 * sight);
    //TODO: use numpy C api
    PyObject* py_nearby_unit = PyList_New(total_pixel);
    //we must init the list
    for (int i = 0; i < total_pixel; ++i)
    {
        PyList_SetItem(py_nearby_unit, i, PyLong_FromLong(0));
    }

    size_t sz = vec.size();

    for (size_t i = 0; i < sz; ++i) {
        auto loc = vec[i].first->get_location();
        PyList_SetItem(py_nearby_unit,
            xy2idx(sign * (loc.x - hero_loc.x), sign * (loc.y - hero_loc.y)),
            PyLong_FromLong(1));
    }
    return py_nearby_unit;
}

static PyObject* getState(cppSimulatorImp* Engine, const std::string& side, int idx)
{
    int total_pixel = (2 * sight) * (2 * sight);
    int sign = side == "Radiant" ? 1 : -1;


    Hero* hero = Engine->getHero(side, idx);
    const auto& data = hero->getData();
    const auto& prev_data = hero->getPrevData();
    const auto& location = hero->get_location();

    auto nearby_enemy = Engine->get_nearby_enemy(hero, data.SightRange);

    auto enemy_feature = getNearbyUnit(nearby_enemy, location, sign);

    auto nearby_ally = Engine->get_nearby_ally(hero, data.SightRange);

    auto ally_feature = getNearbyUnit(nearby_ally, location, sign);

    double reward = (data.exp > prev_data.exp) - 0.1 * (data.HP < prev_data.HP);
    
    PyObject* ret = Py_BuildValue("([OO]dO)", enemy_feature, ally_feature, reward, hero->isDead() ? Py_True : Py_False);
    Py_XDECREF(enemy_feature);
    Py_XDECREF(ally_feature);

    return ret;
}

REG_FEATURE_FN("Lattice1", getState);
