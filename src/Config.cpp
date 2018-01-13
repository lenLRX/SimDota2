#include "Config.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

#include <fstream>


#define GET_DOUBLE(doc, key)\
key = doc[#key].GetDouble()

#define GET_TUPLE(doc, key)\
do {\
    double temp_x = document[#key]["x"].GetDouble();\
    double temp_y = document[#key]["y"].GetDouble();\
    key = pos_tup(temp_x, temp_y);\
}while(0)

#define GET_STRING(doc, key)\
key = doc[#key].GetString()

using namespace rapidjson;

Config::Config(std::string json_path)
{
    std::ifstream ifs(json_path);
    Document document;
    IStreamWrapper isw(ifs);
    document.ParseStream(isw);
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
