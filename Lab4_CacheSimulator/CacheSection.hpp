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

class CacheSection
{
private:
    
    Section     mSection;
    
    uint32_t    mOffset_numbits;
    uint32_t    mSet_numbits;
    uint32_t    mTag_numbits;
    
    uint32_t    mOffset_mask;
    uint32_t    mSet_mask;
    uint32_t    mTag_mask;
    
    uint32_t    mNumSets;
    uint32_t    mNumWays;
    
    std::vector<CacheWay> mData;
    
    
public:
    CacheSection();
    CacheSection(const Section& section);
    PartitionedAddress PartitionAddress(const int& address);
    WayRange Set2WayRange(int set);
    
    bool ProcessSet(PartitionedAddress partionedAddress);
    
    virtual void Replace();
};

//LRU,        // Least-uecently used
//RND,        // TBD
//NMRU,       // TBD
//FIFO,       // First in, first out



class CacheSectionLRU : public CacheSection
{
    using CacheSection::CacheSection;
    void Replace();
};

class CacheSectionRND: public CacheSection
{
    using CacheSection::CacheSection;
    void Replace();
};

class CacheSectionNMRU: public CacheSection
{
    using CacheSection::CacheSection;
    void Replace();
};

class CacheSectionFIFO: public CacheSection
{
     using CacheSection::CacheSection; 
    void Replace();
};







#endif /* CacheSection_hpp */
