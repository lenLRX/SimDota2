#include "ActionSpace/ActionSpace.h"

std::map<std::string, ActionSpaceFn> ActionSpaceConfig::registy;
std::map<std::string, PredefineActionSpaceFn> ActionSpaceConfig::predefineActionRegisty;