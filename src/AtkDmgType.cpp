#include "AtkDmgType.h"

const std::vector<std::vector<double>> ArmorTypeVSDmgType =
{
    { 1.0, 0.75, 0.5,  0.85 },
    { 1.0, 1.0,  1.5,  1.0  },
    { 0.5, 0.7,  0.35, 2.5  }
};

const std::map<std::string, AtkDmgType> str2AtkDmgTypes =
{
    {"Hero", AtkDmgType::Hero },
    {"Basic",AtkDmgType::Basic},
    {"Pierce",AtkDmgType::Pierce},
    {"Siege",AtkDmgType::Siege }
};
const std::map<std::string, ArmorType> str2ArmorType = 
{ 
    {"Hero",ArmorType::Hero},
    {"Basic",ArmorType::Basic},
    {"Structure",ArmorType::Structure}
};
const std::map<std::string, AtkType> str2AtkType = 
{ 
    {"melee",AtkType::melee},
    {"ranged", AtkType::ranged}
};

