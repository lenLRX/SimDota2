#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <algorithm>
//
std::string DirName(std::string source)
{
    std::replace(source.begin(),source.end(),'\\','/');
    source.erase(std::find(source.rbegin(), source.rend(), '/').base(), source.end());
    return source;
}

#endif//__UTIL_H__