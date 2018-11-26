//
//  CacheTypes.h
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/20/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#ifndef CacheTypes_h
#define CacheTypes_h

#include <vector>
#include <iostream>

enum class eConfig
{
    D,          // Direct-Mapped
    S,          // Set-Associative (S)
    F           //Fully-Associative
};

enum class eMode
{
    Unified,    // Data and Instruction cache shared
    Split       // Data and Instruction cache split
};

enum class eReplacementAlgorithm
{
    LRU,        // Least-uecently used
    RND,        // TBD
    NMRU,       // TBD
    FIFO,       // First in, first out
};

enum class eInstructionID
{
    Read=0,
    Write=1,
    InsFetch=2
};

typedef struct t_PartitionedAddress
{
    uint32_t iTag;
    uint32_t iSet;
    uint32_t iOffset;
} PartitionedAddress;

typedef struct t_DineroMatrix
{
    std::vector<std::pair<uint32_t,uint32_t>> data;
} DineroMatrix;

typedef struct t_Section
{
    int iAssociativity;
    int iBlockSize;
    int iCapacity;
    
    int iHitTime;
} Section;

typedef struct t_WayRange
{
    int start;
    int stop;
} WayRange;

typedef struct t_CacheWay
{
    bool bValid;
    uint32_t iTag;
    t_CacheWay():
        bValid(false),iTag(0){};
} CacheWay;

typedef struct t_Level
{
    eMode Mode;
    
    std::vector<Section> vSections;       // 1 or 2 sections
} Level;

typedef struct t_CacheConfig
{
    std::vector<Level> vLevels;         // 1 or 2 levels
    
    int iDRAM_hitTime;
    
    bool bAllocateOnWriteMiss;
    
    eReplacementAlgorithm replacementAlgorithm;
} CacheConfig;

typedef struct t_CacheStats
{
    int readHits;
    int readTotal;
    
    int writeHits;
    int writeTotal;
    
    int instrFetchHits;
    int instrFetchTotal;
    
    t_CacheStats():
        readHits(0),        readTotal(0),
        writeHits(0),       writeTotal(0),
        instrFetchHits(0),  instrFetchTotal(0)
    {};
} CacheStats;

// cache
//      level (1 or 2)
//          mode (unified, split)
//          section (1 or 2)
//              // A,B,C
//          Hit time

//      DRAM hit time

//      bAllocateOnWriteMiss
//      ReplacementAlgorithm









#endif /* CacheTypes_h */
