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
    
    return { tag, set, offset };
}

uint32_t CacheSection::JoinAddress(const uint32_t& tag, const uint32_t& set)
{
    uint32_t joinedAddress  = 0;
    joinedAddress           |= set<<mOffset_numbits;
    joinedAddress           |= tag<<(mOffset_numbits+mSet_numbits);
    
    return joinedAddress;
}

// Returns hit
EvictOut CacheSection::ProcessSet(EvictIn evictIn)
{
    auto pEvictor = mvpSetEvictors[evictIn.partitionedAddress.iSet];
    
    EvictOut evictOut = pEvictor->Access(evictIn);
    
    return evictOut;
    
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

CacheSectionDM::CacheSectionDM(const Section& section)
: CacheSection(section)
{
    //cout<<"CacheSectionDM ctor"<<endl;
    for (int i = 0; i < mNumSets; i++) {
        mvpSetEvictors.push_back(new DM_Evictor(mSection.iAssociativity));
    }
}

