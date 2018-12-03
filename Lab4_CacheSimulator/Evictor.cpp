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

    auto tag_in         = evictIn.partitionedAddress.iTag;
    
    EvictItem evictItem;
    evictItem.mTag      = tag_in;
    evictItem.mInstrID  = evictIn.eInstrID;
    
    EvictOut evictOut;
    evictOut.bHit       = false;
    
    auto findTag        = ma.find(evictItem);
    auto inCache        = findTag != ma.end();
    
    // Hit
    if (inCache)
    {
        evictOut.bHit=true;

        bool wasDirty = ma[evictItem]->mDirty;
        bool wasWrite = ma[evictItem]->mInstrID==eInstructionID::Write;
        
        dq.erase(ma[evictItem]);
        
        dq.push_front(evictItem);
        ma[evictItem] = dq.begin();
        
        if (evictIn.bSetDirty || wasDirty)
            ma[evictItem]->mDirty=true;
        
        if (wasWrite)
            ma[evictItem]->mInstrID=eInstructionID::Write;
        
        return evictOut;
    }
    
    // Miss
    
    if (!evictIn.bAlloc)
        return evictOut;
    
    // at this point, we have miss, and we're allocating
    
    // Cache is full, get LRU details before evicting
    if (dq.size() == size)
    {
        //delete least recently used element
        EvictItem last = dq.back();
        
        evictOut.iTag               = last.mTag;
        evictOut.bCurrentlyDirty    = last.mDirty;
        evictOut.bFull              = true;
        evictOut.eInstrID           = last.mInstrID;
        
        dq.pop_back();
        ma.erase(last);
    }
    
    // update reference
    dq.push_front(evictItem);
    ma[evictItem] = dq.begin();
    
    auto newLocation = ma[evictItem];
    
    // Replace
    newLocation->mTag           = tag_in;
    newLocation->mValid         = true;
    newLocation->mInstrID       = evictIn.eInstrID;
    
    if (evictIn.eInstrID==eInstructionID::Write && evictIn.bSetDirty)
        newLocation->mDirty=true;
    else
        newLocation->mDirty= false;
    

    return evictOut;
}



EvictOut RND_Evictor::Access(const EvictIn evictIn)
{
    
    auto tag_in         = evictIn.partitionedAddress.iTag;
    
    EvictItem evictItem;
    evictItem.mTag      = tag_in;
    evictItem.mInstrID  = evictIn.eInstrID;
    
    EvictOut evictOut;
    evictOut.bHit       = false;

    auto findTag        = tagSet.find(evictItem);
    auto inCache        = findTag != tagSet.end();
    
    // Hit
    if (inCache)
    {
        evictOut.bHit=true;
        
        if (evictIn.bSetDirty)
            findTag->mDirty=true;
        
        return evictOut;
    }
    
    // Miss
    
    if (!evictIn.bAlloc)
        return evictOut;
    
    // cache is full
    if (tagSet.size() == size)
    {
        // remove random value
        int offset = std::rand()%size;
        auto it = tagSet.begin();
        std::advance(it,offset);
        auto removed = *it;
 
        evictOut.iTag               = removed.mTag;
        evictOut.bCurrentlyDirty    = removed.mDirty;
        evictOut.bFull              = true;
        evictOut.eInstrID           = removed.mInstrID;
        
        // remove
        tagSet.erase(it);
    }
    
    // finish populating evictItem
    evictItem.mTag                  = tag_in;
    evictItem.mValid               = true;
    evictItem.mInstrID             = evictIn.eInstrID;
    
    if (evictIn.eInstrID==eInstructionID::Write && evictIn.bSetDirty)
        evictItem.mDirty=true;
    else
        evictItem.mDirty= false;
    
    // add
    tagSet.insert(evictItem);
    
    
    return evictOut;
}

EvictOut DM_Evictor::Access(const EvictIn evictIn)
{
    auto tag_in     = evictIn.partitionedAddress.iTag;
    auto set        = evictIn.partitionedAddress.iSet;
    
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
        mTag                        = tag_in;
        mValid                      = true;
        mInstrID                    = evictIn.eInstrID;
        
        if (evictIn.eInstrID==eInstructionID::Write && evictIn.bSetDirty)
            mDirty=true;
        else
            mDirty= false; 
    }
    
    return evictOut;
}
