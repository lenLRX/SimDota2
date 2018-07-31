#include "Config.h"
#include "util.h"

#include <fstream>
#include <iostream>
#include "rapidjson/error/en.h"




#define GET_STRING(doc, key)\
key = doc[#key].GetString()

using namespace rapidjson;

Config::Config(std::string json_path)
{
    ConfigDir = DirName(json_path);
    //TODO:handle error
    std::ifstream ifs(json_path);
    Document document;
    IStreamWrapper isw(ifs);
    document.ParseStream(isw);
    auto err = document.GetParseError();
    if (kParseErrorNone != err)
    {
        std::cerr << GetParseError_En(err) << std::endl;
    }
    GET_DOUBLE(document, tick_per_second);
    GET_DOUBLE(document, map_div);
    GET_TUPLE(document, rad_init_pos);
    GET_TUPLE(document, dire_init_pos);
    GET_DOUBLE(document, velocity);
    GET_DOUBLE(document, bound_length);
    GET_DOUBLE(document, windows_size);
    game2window_scale = windows_size / bound_length;
    GET_STRING(document, Radiant_Colors);
    GET_STRING(document, Dire_Colors);
}


SpriteData::SpriteData(std::string json_path)
    :exp(0),gold(0)
{
    std::ifstream ifs(json_path);
    Document document;
    IStreamWrapper isw(ifs);
    document.ParseStream(isw);
    auto err = document.GetParseError();
    if (kParseErrorNone != err)
    {
        std::cerr << GetParseError_En(err) << std::endl;
    }
    GET_DOUBLE(document, HP);
    MaxHP = HP;
    GET_DOUBLE(document, BaseHPReg);
    GET_DOUBLE(document, MP);
    MaxMP = MP;
    GET_DOUBLE(document, BaseMPReg);
    GET_DOUBLE(document, MP);
    GET_DOUBLE(document, MovementSpeed);
    GET_DOUBLE(document, BaseAttackTime);
    GET_DOUBLE(document, AttackSpeed);
    GET_DOUBLE(document, Armor);
    GET_DOUBLE(document, Attack);
    GET_DOUBLE(document, AttackRange);
    GET_DOUBLE(document, SightRange);
    GET_DOUBLE(document, Bounty);
    GET_DOUBLE(document, bountyEXP);
    GET_DOUBLE(document, LastAttackTime);
    GET_DOUBLE(document, AttackTime);
    GET_DOUBLE(document, AtkPoint);
    GET_DOUBLE(document, AtkBackswing);
    GET_DOUBLE(document, ProjectileSpeed);
    atktype = str2AtkType.at(document["atktype"].GetString());
    atkDmgType = str2AtkDmgTypes.at(document["atkDmgType"].GetString());
    armorType = str2ArmorType.at(document["armorType"].GetString());
    GET_DOUBLE(document, viz_radius);
}