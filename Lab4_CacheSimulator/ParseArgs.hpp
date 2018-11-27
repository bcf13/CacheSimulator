//
//  ParseArgs.hpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/20/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#ifndef ParseArgs_hpp
#define ParseArgs_hpp

#include "CacheTypes.h"

#include <stdio.h>
#include <vector>
#include <utility>

eMode       ParseMode();
void        ParseArgs(CacheConfig& cacheConfig, DineroMatrix& dineroMatrix); 
void        ParseTraceFile(DineroMatrix& dm);
int         ParseInt(std::string prompt, int lBound, int uBound);
void        ParseAndPopulateCache(CacheConfig& cacheConfig);
void        ParseAndPopulateLevel(Level& level);
void        ParseAndPopulateSection(Section& section);
bool        ParseAllocateOnWriteMiss();
void        GetTestConfig(CacheConfig& cacheConfig, int testID);
eReplacementAlgorithm ParseReplacementAlgorithm();


#endif /* ParseArgs_hpp */

