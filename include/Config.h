#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <tuple>
#include <map>
#include <mutex>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

#include "AtkDmgType.h"


enum class Side {
    Radiant = 0,
    Dire = 1
};

#define GET_DOUBLE(doc, key)\
key = doc[#key].GetDouble()

#define GET_TUPLE(doc, key)\
do {\
    double temp_x = document[#key]["x"].GetDouble();\
    double temp_y = document[#key]["y"].GetDouble();\
    key = pos_tup(temp_x, temp_y);\
}while(0)

class pos_tup
{
public:
    pos_tup(double x = 0, double y = 0)
        :x(x), y(y){}
    double x;
    double y;
};

class SpriteData;
class Config;

class MtxGuard
{
public:
    MtxGuard(std::mutex& mtx):_mtx(mtx)
    {
        _mtx.lock();
    }
    ~MtxGuard() 
    {
        _mtx.unlock();
    }
private:
    std::mutex& _mtx;
};


template<typename CfgType>
class ConfigCacheMgr
{
public:
    static ConfigCacheMgr& getInstance()
    {
        static ConfigCacheMgr mgr;
        return mgr;
    }

    CfgType* get(std::string path,void(*p_fn)(CfgType*,std::string)  = nullptr)
    {
        MtxGuard gurad(mtx);
        auto it = m.find(path);
        if (m.end() == it)
        {
            //cache miss, load it
            CfgType* p = new CfgType(path);
            if (p_fn)
            {
                p_fn(p, path);
            }
            m[path] = p;
            return p;
        }
        return it->second;
    }

    void clear() 
    {
        MtxGuard guard(mtx);
        for (auto p : m)
        {
            delete p.second;
        }
        m.clear();
    }
private:
    std::map<std::string, CfgType*> m;
    std::mutex mtx;
};

class Config {
public:
    Config() = delete;
    double tick_per_second;
    double map_div;
    pos_tup rad_init_pos;
    pos_tup dire_init_pos;
    double velocity;
    double bound_length;
    double windows_size;
    double game2window_scale;
    std::string Radiant_Colors;
    std::string Dire_Colors;
    std::string ConfigDir;
private:
    friend class ConfigCacheMgr<Config>;
    Config(std::string json_path);
};

class SpriteData
{
public:
    virtual ~SpriteData() {}
    double MaxHP;
    double BaseHPReg;
    double MaxMP;
    double HP;
    double MP;
    double MovementSpeed;
    double BaseAttackTime;
    double AttackSpeed;
    double Armor;
    double Attack;
    double AttackRange;
    double SightRange;
    double Bounty;
    double bountyEXP;
    double LastAttackTime;
    double AttackTime;
    double AtkPoint;
    double AtkBackswing;
    double ProjectileSpeed;
    AtkType atktype;
    AtkDmgType atkDmgType;
    ArmorType armorType;
    double viz_radius;
    void* ext;//shared ptr, you should never delete it!
private:
    friend class ConfigCacheMgr<SpriteData>;
    SpriteData(std::string json_path);
};
#endif//__CONFIG_H__