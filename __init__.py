from .cppSimulator import *
import os

#check if lib is in latest version
class LibVersionMissMatch(Exception):
    pass
_path = os.path.realpath(__file__)
_dir = os.path.dirname(_path)
default_cfg = os.path.join(_dir,"./config/Config.json")
try:
    
    src_path = os.path.join(_dir,"./src/simulator.cpp")
    with open(src_path, "r") as fp:
        _v = cppSimulator(None).get_version()
        import re
        version_matcher = re.compile("#define __ENGINE_VERSION__ \"(.*)\"")
        for line in fp:
            m = version_matcher.match(line)
            if m:
                v_src = m.groups(0)[0]
                if v_src != _v:
                    raise LibVersionMissMatch("version in lib is %s but version in src is %s"%(_v, v_src))
except LibVersionMissMatch as e:
    raise e
except Exception as e:
    pass

def Simulator(cfg_path=default_cfg, feature_name='origin1', canvas=None):
    return cppSimulator(cfg_path, feature_name, canvas)