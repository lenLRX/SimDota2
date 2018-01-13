#ifndef __ATKDMGTYPE_H__
#define __ATKDMGTYPE_H__

#include <vector>
#include <string>
#include <map>

enum class AtkDmgType
{
    Hero = 0,
    Basic = 1,
    Pierce = 2,
    Siege = 3
};

enum class ArmorType
{
    Hero = 0,
    Basic = 1,
    Structure = 2
};

enum AtkType
{
    melee = 0,
    ranged = 1
};


extern const std::vector<std::vector<double>> ArmorTypeVSDmgType;

extern const std::map<std::string, AtkDmgType> str2AtkDmgTypes;
extern const std::map<std::string, ArmorType> str2ArmorType;
extern const std::map<std::string, AtkType> str2AtkType;

#endif//__ATKDMGTYPE_H__