#include "util.h"

std::string DirName(std::string source) 
{
    std::replace(source.begin(), source.end(), '\\', '/');
    source.erase(std::find(source.rbegin(), source.rend(), '/').base(), source.end());
    return source;
}