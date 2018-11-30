//
//  CacheSection.cpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/25/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#include "CacheSection.hpp"
#include <math.h>       /* log2 */

#define ADDRESS_SIZE_BITS 24

using namespace std;

uint32_t GetMask(int numBits)
{
    return pow(2,numBits)-1; // e.g. 4 bits, 2^4=16, 16-1=15=1111
}

// Get range of ways corresponding to set index
// E.g. set 2, associativity=4, -> [8 11]
WayRange CacheSection::Set2WayRange(int set)
{
    return
    {
        set*mSection.iAssociativity,
        (set+1)*mSection.iAssociativity-1
    };
}

CacheSection::CacheSection()
{
}

CacheSection::CacheSection(const Section& section)
    :   mSection(section),
        mHitTime(section.iHitTime)
{
    // fully associative cache = (C/B)-way cache with 1 set
    
    const auto bytes_per_set = mSection.iAssociativity*mSection.iBlockSize;
    mNumSets = mSection.iCapacity / bytes_per_set;
    
    // bits
    mOffset_numbits             = log2(mSection.iBlockSize);
    mSet_numbits                = log2(mNumSets);
    mTag_numbits                = ADDRESS_SIZE_BITS-(mOffset_numbits+mSet_numbits);
    
    mOffset_mask                = GetMask(mOffset_numbits);
    mSet_mask                   = GetMask(mSet_numbits) << mOffset_numbits;
    mTag_mask                   = GetMask(mTag_numbits) << (mOffset_numbits+mSet_numbits);
    
    mNumWays                    = mNumSets*mSection.iAssociativity;
    
    CacheWay c;
    mData.assign(mNumWays, c);
}

PartitionedAddress CacheSection::PartitionAddress(const int& address)
{
    uint32_t tag    = (address & mTag_mask) >> (mOffset_numbits+mSet_numbits);
    uint32_t set    = (address & mSet_mask) >> mOffset_numbits;
    uint32_t offset = (address & mOffset_mask);
    
    return { tag,set, offset};
}

// Returns hit
bool CacheSection::ProcessSet(PartitionedAddress partionedAddress, bool alloc)
{
    auto wayRange=Set2WayRange(partionedAddress.iSet);
    
    auto tag=partionedAddress.iTag;
    
    if (mSection.iAssociativity>1)
        return Replace(alloc,partionedAddress);
    
    bool found = false; 
    
    for (int i = wayRange.start ; i <= wayRange.stop; i++)
    {
        auto valid      = mData[i].bValid;
        auto othertTag  = mData[i].iTag;
        
        if (valid && othertTag==tag)
            return true;
    }
    
    mData[wayRange.start].bValid    =true;
    mData[wayRange.start].iTag      =tag;
    
    return false;
}

CacheSectionLRU::CacheSectionLRU(const Section& section)
: CacheSection(section)
{
    //cout<<"CacheSectionLRU ctor"<<endl;
    for (int i = 0; i < mNumSets; i++) {
        mvpSetEvictors.push_back(new LRU_Evictor(mSection.iAssociativity));
    }
    

}

CacheSectionRND::CacheSectionRND(const Section& section)
: CacheSection(section)
{
    //cout<<"CacheSectionRND ctor"<<endl;
    for (int i = 0; i < mNumSets; i++) {
        mvpSetEvictors.push_back(new RND_Evictor(mSection.iAssociativity));
    }

}

bool CacheSection::Replace(bool hit, PartitionedAddress partionedAddress)
{
    //cout<<"CacheSection Replace"<<endl;
    return true;
}

bool CacheSectionLRU::Replace(bool hit, PartitionedAddress partionedAddress)
{
    //cout<<"CacheSectionLRU Replace"<<endl;
    
    auto set = partionedAddress.iSet;
    auto wayRange=Set2WayRange(set);
    
    auto pEvictor = mvpSetEvictors[set];
    
    auto tag = partionedAddress.iTag;
    
    return pEvictor->Access(tag);
}

bool CacheSectionRND::Replace(bool hit, PartitionedAddress partionedAddress)
{
    //cout<<"CacheSectionRND Replace"<<endl;
    auto set = partionedAddress.iSet;
    auto wayRange=Set2WayRange(set);
    
    auto pEvictor = mvpSetEvictors[set];
    
    auto tag = partionedAddress.iTag;
    
    return pEvictor->Access(tag);
}

bool CacheSectionNMRU::Replace(bool hit, PartitionedAddress partionedAddress)
{
        cout<<"CacheSectionNMRU Replace"<<endl;
        return true;
}

bool CacheSectionFIFO::Replace(bool hit, PartitionedAddress partionedAddress)
{
        cout<<"CacheSectionFIFO Replace"<<endl;
        return true;
}

