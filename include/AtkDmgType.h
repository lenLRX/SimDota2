#ifndef __ATKDMGTYPE_H__
#define __ATKDMGTYPE_H__

#include <vector>

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

extern const std::vector<std::vector<double>> ArmorTypeVSDmgType;

#endif//__ATKDMGTYPE_H__