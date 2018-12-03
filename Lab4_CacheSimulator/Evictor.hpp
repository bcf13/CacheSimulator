//
//  Evictor.hpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/27/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#ifndef Evictor_hpp
#define Evictor_hpp

#include <stdio.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
//#include <map>
//#include <set>
#include "CacheTypes.h"

class Evictor
{
protected:
    size_t  size;
public:
    Evictor(size_t size);
    virtual EvictOut Access(const EvictIn evictIn) = 0;
};

class LRU_Evictor : public Evictor
{
private:
    // store keys of cache
    std::list<EvictItem> dq;
    
    // store references of key in cache
    std::unordered_map<EvictItem, std::list<EvictItem>::iterator,EvictItemHasher> ma;
    
    //std::map<EvictItem, std::list<EvictItem>::iterator> ma;
    
public:
    LRU_Evictor(size_t size);

    EvictOut Access(const EvictIn evictIn);
};

class RND_Evictor : public Evictor
{
private:
    std::unordered_set<EvictItem,EvictItemHasher> tagSet;
    
public:
    RND_Evictor(size_t size);
    EvictOut Access(const EvictIn evictIn);
};

class DM_Evictor : public Evictor
{
private:
    uint32_t        mTag;
    bool            mValid;
    bool            mDirty;
    eInstructionID  mInstrID;
    
public:
    DM_Evictor(size_t size);
    EvictOut Access(const EvictIn evictIn);
};



#endif /* Evictor_hpp */
