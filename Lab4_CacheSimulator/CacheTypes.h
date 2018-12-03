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
#include <list>
#include <unordered_map>
#include <iostream>

enum class eAllocatePolicy
{
    Alloc,
    NoAlloc
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
    Read        =0,
    Write       =1,
    InsFetch    =2
};

typedef struct t_PartitionedAddress
{
    uint32_t    iTag;
    uint32_t    iSet;
    uint32_t    iOffset;
} PartitionedAddress;

typedef struct t_DineroMatrix
{
    std::vector<std::pair<uint32_t,uint32_t>> data;
} DineroMatrix;

// inclusive-range of ways corresponding to set
typedef struct t_WayRange
{
    int         start;
    int         stop;
} WayRange;

typedef struct t_CacheWay
{
    bool        bValid;
    bool        bDirty;
    uint32_t    iTag;
    t_CacheWay():
        bValid(false),bDirty(false),iTag(0){};
} CacheWay;

typedef struct t_EvictIn
{
    bool                bAlloc;
    bool                bSetDirty;
    PartitionedAddress  partitionedAddress;
    eInstructionID      eInstrID;
    t_EvictIn(bool bAlloc, bool bSetDirty, eInstructionID eID, PartitionedAddress partitionedAddress)
    :   bAlloc(bAlloc),
        bSetDirty(bSetDirty),
        eInstrID(eID),
        partitionedAddress(partitionedAddress){};
} EvictIn;

typedef struct t_EvictOut
{
    bool            bHit;
    bool            bFull;
    bool            bCurrentlyDirty;
    uint32_t        iTag;
    eInstructionID  eInstrID;
//    t_EvictOut():
//        bHit(false),bFull(false),iTag(0){};
//    t_EvictOut(bool hit, bool Full, uint32_t iTag):
//        bHit(bHit),bFull(bFull),bCurrentlyDirty()iTag(iTag){};
} EvictOut;


typedef struct t_EvictItem
{
    uint32_t        mTag;
    bool            mValid;
    bool            mDirty;
    eInstructionID  mInstrID;
    bool operator==(const t_EvictItem &other) const
    { return (mTag == other.mTag); }
} EvictItem;

struct EvictItemHasher
{
    std::size_t operator()(const EvictItem& e) const
    {
        using std::size_t;
        using std::hash;
        using std::string;
        
        return std::hash<uint32_t>()(e.mTag);
    }
};

typedef struct t_Section
{
    int         iAssociativity;
    int         iBlockSize;
    int         iCapacity;
    
    int         iHitTime;
    
    ~t_Section()
    {
        //std::cout<<"section dtor"<<std::endl;
    }
} Section;

typedef struct t_Level
{
    eMode Mode;
    
    std::vector<Section> vSections;       // 1 or 2 sections
    
    ~t_Level()
    {
        //std::cout<<"level dtor"<<std::endl;
    }
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
    int readMisses;
    int readTotal;
    
    int writeMisses;
    int writeTotal;
    
    int instrFetchMisses;
    int instrFetchTotal;
    
    std::string ID;
    
    t_CacheStats(std::string ID):
        readMisses(0),        readTotal(0),
        writeMisses(0),       writeTotal(0),
        instrFetchMisses(0),  instrFetchTotal(0),
        ID(ID)
    {};
    
    void WriteStat(bool hit, eInstructionID eInstrID)
    {
        switch (eInstrID) {
            case eInstructionID::Read:
                readMisses += (hit ? 0 : 1);
                readTotal++;
                break;
            
            case eInstructionID::Write:
                writeMisses += (hit ? 0 : 1);
                writeTotal++;
                break;
            
            case eInstructionID::InsFetch:
                instrFetchMisses += (hit ? 0 : 1);
                instrFetchTotal++;
                break;
        }
    }
    
    t_CacheStats(){};
    
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
