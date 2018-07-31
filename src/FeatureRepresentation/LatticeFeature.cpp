#include "FeatureRepresentation/FeatureRepresentation.h"

static PyObject* getState(const cppSimulatorImp* engine, const std::string& side, int idx)
{
    return nullptr;
}

REG_FEATURE_FN("Lattice1", getState);
