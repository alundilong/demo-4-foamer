#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include "EsObjectRegistry.h"
class TopA;

typedef EsObjectRegistry<std::unordered_map<std::string, const TopA*>> EsObjectManager;
typedef EsObjectRegistry<const TopA*> EsTopAObjectManager;

#endif
