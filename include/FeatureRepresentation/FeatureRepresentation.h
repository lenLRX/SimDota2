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

class FeatureConfigReg
{
public:
    static FeatureConfigReg& getInstance()
    {
        static FeatureConfigReg instance;
        return instance;
    }

    std::map<std::string, FeatureFn> registy;
};

class FeatureConfig
{
public:
    FeatureConfig() :inited(false) {}
    FeatureConfig(const std::string& featureName):inited(true)
    {
        auto& regInst = FeatureConfigReg::getInstance();
        auto fnIt = regInst.registy.find(featureName);
        if (fnIt != regInst.registy.end())
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
        FeatureConfigReg::getInstance().registy[name] = fn_;
        return 0;
    }

    operator bool ()
    {
        return inited;
    }
    
private:
    bool inited;
    FeatureFn fn;
};



#define REG_FEATURE_FN(name, fn) \
static int i_feature_reg_##fn = FeatureConfig::registerFeatureFn(name, fn);

