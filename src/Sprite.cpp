#include "Sprite.h"
#include "simulatorImp.h"
#include "Event.h"
#include "util.h"

#include <cmath>
#include <cstdlib>

void Sprite::update_data()
{
    double AttackPerSecond = data.AttackSpeed * 0.01 / data.BaseAttackTime;
    AttackTime = 1 / AttackPerSecond;
    double dt = 1.0 / GET_CFG->tick_per_second;
    //TODO
    double deltaHP = data.BaseHPReg * dt;
    data.HP += deltaHP;
    upperBound(data.HP, data.MaxHP);
    double deltaMP = data.BaseMPReg * dt;
    data.MP += deltaMP;
    upperBound(data.MP, data.MaxMP);
}

pos_tup Sprite::pos_in_wnd()
{
    return pos_tup(location.x * GET_CFG->game2window_scale * 0.5 + GET_CFG->windows_size * 0.5,
        location.y * GET_CFG->game2window_scale * 0.5 + GET_CFG->windows_size * 0.5);
}

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

    if (!b_move || isBuilding())
        return;

    if (isAttacking()) {
        b_move = false;
        return;
    }
        

    double dx = move_target.x - location.x;
    double dy = move_target.y - location.y;
    
    if(dx == 0.0 && dy == 0.0){
        return;
    }

    double a = std::atan2(dy, dx);

    if (std::isnan(a)) {
        printf("found nan dx = %lf dy = %lf\n",dx,dy);
        fflush(stdout);
        exit(3);
    }

    double d = data.MovementSpeed * Engine->get_deltatick();
    if (hypot(dx, dy) < d) {
        location = move_target;
        b_move = false;
    }
    else {
        location.x += d * cos(a);
        location.y += d * sin(a);
    }
    
    // Correct x-axis for out-of-bounds selection
    if (location.x > GET_CFG->bound_length) {
        location.x = GET_CFG->bound_length;
    }
    else if (location.x < -GET_CFG->bound_length) {
        location.x = -GET_CFG->bound_length;
    }

    // Correct y-axis for out-of-bounds selection
    if (location.y > GET_CFG->bound_length) {
        location.y = GET_CFG->bound_length;
    }
    else if (location.y < -GET_CFG->bound_length) {
        location.y = -GET_CFG->bound_length;
    }
}

bool Sprite::damaged(Sprite* attacker, double dmg)
{
    if (isDead()) {
        return false;
    }
    data.HP -= dmg;
    if (data.HP <= 0.0) {
        dead(attacker);
    }
    return true;
}

double Sprite::attakedDmg(Sprite* attacker, double dmg)
{
    return ArmorTypeVSDmgType[(int)data.armorType][(int)attacker->data.atkDmgType] * dmg;
}


void Sprite::dead(Sprite* attacker)
{
    _isDead = true;
    remove_visual_ent();
    for (Sprite* s : Engine->get_sprites()) {
        if (s->side != side) {
            double dis = S2Sdistance(*s, *this);
            if (dis <= 1300.0) {
                s->exp += data.bountyEXP;
            }
            //else {
            //    s->exp += bountyEXP * (dis - 1300 + 1) / Config::map_div * 0.1;
            //}
        }
    }
    if (nullptr != attacker) {
        attacker->gold += data.Bounty;
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
    double dx = s1.location.x - s2.location.x;
    double dy = s1.location.y - s2.location.y;
    return hypot(dx, dy);
}

double Sprite::TimeToDamage(const Sprite * s)
{
    double TimeToAtk = data.AtkPoint / data.AttackSpeed * 0.01;
    if (melee == data.atktype) {
        return TimeToAtk;
    }
    else {
        double _d = S2Sdistance(*this, *s);
        return TimeToAtk + _d / data.ProjectileSpeed;
    }
}
