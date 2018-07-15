#pragma once

/*
This header provides a common interface of various feature representations of game
*/

#include <Python.h>

#include <functional>
#include <string>
#include <map>

#include "Config.h"
#include "log.h"

//forward decl
class cppSimulatorImp;

typedef std::function<void(cppSimulatorImp*, PyObject*, const std::string&, int)> ActionSpaceFn;
typedef std::function<PyObject*(cppSimulatorImp*, const std::string&, int)> PredefineActionSpaceFn;

class ActionSpaceConfig
{
public:
    ActionSpaceConfig() :inited(false) {}
    ActionSpaceConfig(const std::string& actionSpaceName) :inited(true)
    {
        auto fnIt = registy.find(actionSpaceName);
        if (fnIt != registy.end())
        {
            fn = fnIt->second;
        }
        else
        {
            LOG << "ActionSpaceConfig " << actionSpaceName << " not found" << std::endl;
        }
    }

    void applyAction(cppSimulatorImp* engine, PyObject* action, const std::string& side, int idx)
    {
        return fn(engine, action, side, idx);
    }

    PyObject* getPredefineAction(cppSimulatorImp* engine, const std::string& side, int idx)
    {
        return predefineAction(engine, side, idx);
    }

    static int registerActionSpaceFn(const std::string& name, const ActionSpaceFn& fn_)
    {
        registy[name] = fn_;
        return 0;
    }

    static int registerPredefineActionFn(const std::string& name, const PredefineActionSpaceFn& fn_)
    {
        predefineActionRegisty[name] = fn_;
        return 0;
    }

    operator bool()
    {
        return inited;
    }

private:
    bool inited;
    static std::map<std::string, ActionSpaceFn> registy;
    static std::map<std::string, PredefineActionSpaceFn> predefineActionRegisty;
    ActionSpaceFn fn;
    PredefineActionSpaceFn predefineAction;
};

#define REG_ActionSpace_FN(name, fn) \
static int i_actions_space_reg_##fn = ActionSpaceConfig::registerActionSpaceFn(name, fn);


#define REG_PredefineAction_FN(name, fn) \
static int i_predefine_action_reg_##fn = ActionSpaceConfig::registerPredefineActionFn(name, fn);
