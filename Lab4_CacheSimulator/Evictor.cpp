//
//  Evictor.cpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/27/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#include "Evictor.hpp"

Evictor::Evictor(size_t size)
: size(size)
{}

LRU_Evictor::LRU_Evictor(size_t size)
:   Evictor(size)
{}

RND_Evictor::RND_Evictor(size_t size)
:   Evictor(size)
{
    std::srand(42); 
}

DM_Evictor::DM_Evictor(size_t size)
:   Evictor(1),
    mValid(false),
    mTag(0),
    mDirty(0)
{}

// New functionality:
// Inputs: write (set dirty = 0)
// Outputs: tag
// Internal: don't disrupt LRU unncessarily 

EvictOut LRU_Evictor::Access(const EvictIn evictIn)
{
//    // store keys of cache
//    std::list<int> dq;
//
//    // store references of key in cache
//    std::unordered_map<int, std::list<int>::iterator> ma;
    
    auto tag_in = evictIn.partitionedAddress.iTag;
    
    EvictOut evictOut;
    evictOut.bHit=false;
    
    // not present in cache
    if (ma.find(tag_in) == ma.end())
    {
        // cache is full
        if (dq.size() == size)
        {
            //delete least recently used element
            int last = dq.back();
            dq.pop_back();
            ma.erase(last);
        }
    }
    // present in cache
    else
    {
        dq.erase(ma[tag_in]);
        evictOut.bHit=true;
    }
    
    // update reference
    dq.push_front(tag_in);
    ma[tag_in] = dq.begin();

    return evictOut;
}



EvictOut RND_Evictor::Access(const EvictIn evictIn)
{
    
    auto tag_in = evictIn.partitionedAddress.iTag;
    
    EvictOut evictOut;
    evictOut.bHit=false;

    // not present in cache
    if (tagSet.find(tag_in) == tagSet.end())
    {
        // cache is full
        if (tagSet.size() == size)
        {
            // remove random value
            int offset = std::rand()%size;
            auto it = tagSet.begin(); 
            std::advance(it,offset);
            tagSet.erase(it);
        }
        // add to cache
        tagSet.insert(tag_in);
    }
    else // in cache
    {
        evictOut.bHit=true;
    }
    
    return evictOut;
}

EvictOut DM_Evictor::Access(const EvictIn evictIn)
{
    auto tag_in = evictIn.partitionedAddress.iTag;
    
    EvictOut evictOut;
    evictOut.bHit=false;
    
    // Hit
    if (mValid && (tag_in==mTag))
    {
        evictOut.bHit=true;
        if (evictIn.bSetDirty)
            mDirty=true; 
        return evictOut;
    }

    // Miss
    
    //  If allocate, add to cache
    if (evictIn.bAlloc)
    {
        evictOut.iTag               = mTag;
        evictOut.bCurrentlyDirty    = mDirty;
        evictOut.bFull              = mValid;
        evictOut.eInstrID           = mInstrID; 
        
        // Replace 
        mTag=tag_in;
        mValid=true;
    }
    
    return evictOut;
}
