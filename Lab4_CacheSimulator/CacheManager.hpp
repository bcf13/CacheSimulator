//
//  CacheManager.hpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/25/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#ifndef CacheManager_hpp
#define CacheManager_hpp

#include <stdio.h>

#include "CacheTypes.h"
#include "CacheSection.hpp"

class CacheManager
{
private:
    int             mDineroInstructionIndex;
    DineroMatrix    mDineroMatrix;
    //CacheStats      mCacheStats;
    int             mDRAM_hitTime;
    bool            mbAllocateOnWriteMiss;
    size_t          mNumLevels;
    size_t          mNumLevel1_Sections;
    size_t          mNumLevel2_Sections;
    
    eReplacementAlgorithm                       mReplacementAlgorithm;
    std::vector<std::vector<CacheSection*>>     mvpCacheSections;
    std::vector<std::vector<CacheStats>>        mvpCacheStats;
    
public:
    CacheManager(const CacheConfig& cacheConfig, const DineroMatrix& dineroMatrix);
    ~CacheManager();
    bool ProcessInstruction();
    void PrintStats(); 
    
    
    
    
    
};

#endif /* CacheManager_hpp */
