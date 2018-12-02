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
{}

DM_Evictor::DM_Evictor(size_t size)
:   Evictor(1),
    mValid(false),
    mTag(0),
    mDirty(0)
{}



bool LRU_Evictor::Access(const uint32_t tag, eAllocatePolicy alloc)
{
    bool found = false;
    // not present in cache
    if (ma.find(tag) == ma.end())
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
        dq.erase(ma[tag]);
        found = true;
    }
    
    // update reference
    dq.push_front(tag);
    ma[tag] = dq.begin();

    return found;
}

bool RND_Evictor::Access(const uint32_t tag, eAllocatePolicy alloc)
{
    bool found = false;

    // not present in cache
    if (tagSet.find(tag) == tagSet.end())
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
        tagSet.insert(tag);
    }
    else // in cache
    {
        found=true;
    }
    
    return found;
}

bool DM_Evictor::Access(const uint32_t tag, eAllocatePolicy alloc)
{
// 1
//    if (mValid && tag==mTag)
//        return true;
//
//    mValid=true;
//    mTag=tag;
//    return false;
    
// 2
    
        if (mValid && (tag==mTag))
            return true;
    
        if (alloc==eAllocatePolicy::Alloc)
        {
            mTag=tag;
            mValid=true;
        }
    
    return false;
    
    
    
    
//    // 3
//    if (mValid && (tag==mTag))
//        return true;
//
//    if (!mValid)
//    {
//        mValid=true;
//        mTag=tag;
//        return false;
//    }
//
//    // occupied ...
//
//    if (alloc==eAllocatePolicy::Alloc)
//    {
//        mTag=tag;
//    }
//
//    return false;
}
