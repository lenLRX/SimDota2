#ifndef __DLLEXPORT_H_
#define __DLLEXPORT_H_

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#endif//__DLLEXPORT_H_