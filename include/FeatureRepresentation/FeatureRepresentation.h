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

typedef std::function<PyObject*(cppSimulatorImp* engine, const std::string& side, int idx)> FeatureFn;

class FeatureConfig
{
public:
    FeatureConfig() :inited(false) {}
    FeatureConfig(const std::string& featureName):inited(true)
    {
        auto fnIt = registy.find(featureName);
        if (fnIt != registy.end())
        {
            fn = fnIt->second;
        }
        else
        {
            LOG << "feature " << featureName << " not found" << std::endl;
        }
    }
    
    PyObject* getState(cppSimulatorImp* engine, const std::string& side, int idx)
    {
        return fn(engine, side, idx);
    }

    static int registerFeatureFn(const std::string& name, const FeatureFn& fn_)
    {
        registy[name] = fn_;
        return 0;
    }

    operator bool ()
    {
        return inited;
    }
    
private:
    bool inited;
    static std::map<std::string, FeatureFn> registy;
    FeatureFn fn;
};

#define REG_FEATURE_FN(name, fn) \
static int i_feature_reg_##fn = FeatureConfig::registerFeatureFn(name, fn);

