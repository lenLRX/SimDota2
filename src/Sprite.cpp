#include "Sprite.h"
#include "simulatorImp.h"
#include "Event.h"

#include <cmath>
#include <cstdlib>

void Sprite::attack(Sprite* target)
{
    LastAttackTime = Engine->get_time();
    EventFactory::CreateAttackEvnt(this, target);
}

bool Sprite::isAttacking()
{
    return (Engine->get_time() - LastAttackTime)
        < AttackTime;
}

void Sprite::move()
{
    if (isDead())
        return;

    if (!b_move)
        return;

    if (isAttacking()) {
        b_move = false;
        return;
    }
        

    double dx = std::get<0>(move_target) - std::get<0>(location);
    double dy = std::get<1>(move_target) - std::get<1>(location);
    
    if(dx == 0.0 && dy == 0.0){
        return;
    }

    double a = std::atan2(dy, dx);

    if (std::isnan(a)) {
        printf("found nan dx = %lf dy = %lf\n",dx,dy);
        fflush(stdout);
        exit(3);
    }

    //if (!(std::get<0>(move_target) == 0.0
    //    && std::get<1>(move_target) == 0.0)) {
        double d = MovementSpeed * Engine->get_deltatick();
        if (hypot(dx, dy) < d) {
            location = move_target;
            b_move = false;
        }
        else {
            if (Engine->get_nearby_ally(this).size() > 0 && atkDmgType != AtkDmgType::Hero)
            {
                auto nearby_ally = Engine->get_nearby_ally(this).front();
                auto _nearby_loc = nearby_ally.first->get_location();
                double dx2 = std::get<0>(_nearby_loc) - std::get<0>(location);
                double dy2 = std::get<1>(_nearby_loc) - std::get<1>(location);
                double a2 = std::atan2(dy2, dx2);
                if (fabs(a2 - a) < 3.1415 / 4 && S2Sdistance(*this, *nearby_ally.first) < 50)
                {
                    location = pos_tup(std::get<0>(location) - d * cos(a2),
                        std::get<1>(location) - d * sin(a2));
                }
                else
                {
                    location = pos_tup(std::get<0>(location) + d * cos(a),
                        std::get<1>(location) + d * sin(a));
                }
            }
            else 
            {
                location = pos_tup(std::get<0>(location) + d * cos(a),
                    std::get<1>(location) + d * sin(a));
            }
            
        }
    //}

    // Correct x-axis for out-of-bounds selection
    if (std::get<0>(location) > Config::bound_length) {
        location = pos_tup(Config::bound_length, std::get<1>(location));
    }
    else if (std::get<0>(location) < -Config::bound_length) {
        location = pos_tup(-Config::bound_length, std::get<1>(location));
    }

    // Correct y-axis for out-of-bounds selection
    if (std::get<1>(location) > Config::bound_length) {
        location = pos_tup(std::get<0>(location), Config::bound_length);
    }
    else if (std::get<1>(location) < -Config::bound_length) {
        location = pos_tup(std::get<0>(location), -Config::bound_length);
    }
}

bool Sprite::damadged(Sprite* attacker, double dmg)
{
    if (isDead()) {
        return false;
    }
    HP -= dmg;
    if (HP <= 0.0) {
        dead(attacker);
    }
    return true;
}

double Sprite::attakedDmg(Sprite* attacker, double dmg)
{
    return ArmorTypeVSDmgType[(int)armorType][(int)attacker->atkDmgType] * dmg;
}


void Sprite::dead(Sprite*  attacker)
{
    _isDead = true;
    remove_visual_ent();
    for (Sprite* s : Engine->get_sprites()) {
        if (s->side != side) {
            double dis = S2Sdistance(*s, *this);
            if (dis <= 1300.0) {
                s->exp += bountyEXP;
            }
            //else {
            //    s->exp += bountyEXP * (dis - 1300 + 1) / Config::map_div * 0.1;
            //}
        }
    }
    if (nullptr != attacker) {
        attacker->gold += Bounty;
    }
}

void Sprite::remove_visual_ent()
{
    if (NULL != v_handle) {
        PyObject* delete_fn = PyObject_GetAttrString(canvas, "delete");
        PyObject* args = Py_BuildValue("(O)", v_handle);
        PyObject* kwargs = Py_BuildValue("{}");
        Py_XDECREF(PyObject_Call(delete_fn, args, kwargs));
        Py_DECREF(kwargs);
        Py_DECREF(args);
        Py_DECREF(delete_fn);
        Py_DECREF(v_handle);
        v_handle = NULL;
    }
}

double Sprite::S2Sdistance(const Sprite & s1, const Sprite & s2)
{
    double dx = std::get<0>(s1.location) - std::get<0>(s2.location);
    double dy = std::get<1>(s1.location) - std::get<1>(s2.location);
    return hypot(dx, dy);
}

double Sprite::TimeToDamage(const Sprite * s)
{
    double TimeToAtk = AtkPoint / AttackSpeed * 0.01;
    if (melee == atktype) {
        return TimeToAtk;
    }
    else {
        double _d = S2Sdistance(*this, *s);
        return TimeToAtk + _d / ProjectileSpeed;
    }
}
