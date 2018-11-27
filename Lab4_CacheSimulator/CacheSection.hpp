//
//  CacheSection.hpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/25/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#ifndef CacheSection_hpp
#define CacheSection_hpp

#include <stdio.h>

#include "CacheTypes.h"
#include "Evictor.hpp"

class CacheSection
{
protected:
    
    Section     mSection;
    
    uint32_t    mOffset_numbits;
    uint32_t    mSet_numbits;
    uint32_t    mTag_numbits;
    
    uint32_t    mOffset_mask;
    uint32_t    mSet_mask;
    uint32_t    mTag_mask;
    
    uint32_t    mNumSets;
    uint32_t    mNumWays;
    
    uint32_t    mHitTime; 
    
    std::vector<CacheWay>       mData;
    
    std::vector<Evictor*>       mvpSetEvictors;
    
    
public:
    CacheSection();
    CacheSection(const Section& section);
    PartitionedAddress PartitionAddress(const int& address);
    WayRange Set2WayRange(int set);
    
    bool ProcessSet(PartitionedAddress partionedAddress, bool alloc);
    
    virtual bool Replace(bool hit, PartitionedAddress partionedAddress);
};

//LRU,        // Least-uecently used
//RND,        // TBD
//NMRU,       // TBD
//FIFO,       // First in, first out



class CacheSectionLRU : public CacheSection
{
public:
    CacheSectionLRU(const Section& section);
    bool Replace(bool hit, PartitionedAddress partionedAddress);
};

class CacheSectionRND: public CacheSection
{
public:
    CacheSectionRND(const Section& section);
    bool Replace(bool hit, PartitionedAddress partionedAddress);
};

class CacheSectionNMRU: public CacheSection
{
    using CacheSection::CacheSection;
    bool Replace(bool hit, PartitionedAddress partionedAddress);
};

class CacheSectionFIFO: public CacheSection
{
    using CacheSection::CacheSection; 
    bool Replace(bool hit, PartitionedAddress partionedAddress);
};







#endif /* CacheSection_hpp */
