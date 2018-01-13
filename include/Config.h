#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <tuple>

enum class Side {
    Radiant = 0,
    Dire = 1
};

typedef std::tuple<double, double> pos_tup;

class Config {
public:
    Config(std::string json_path);
    Config() = delete;
    double tick_per_second;
    double map_div;
    pos_tup rad_init_pos;
    pos_tup dire_init_pos;
    double velocity;
    double bound_length;
    double windows_size;
    double game2window_scale;
    std::string Radiant_Colors;
    std::string Dire_Colors;
};

#endif//__CONFIG_H__