//
//  CacheManager.cpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/25/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#include "CacheManager.hpp"
using namespace std; 

std::string GetSectionID(int levelIndex, int sectionIndex)
{
    std::string ID = levelIndex==0 ? "Level-1 " : "Level-2 ";
    switch (sectionIndex) {
        case -1:
            return ID + "Unified-Cache";
        case 0:
            return ID + "D-Cache";
        case 1:
            return ID + "I-Cache";
        default:
            return ID;
    }
}

CacheManager::CacheManager(const CacheConfig& cacheConfig, const DineroMatrix& dineroMatrix)
    :mDineroInstructionIndex(0),
    mDineroMatrix(dineroMatrix),
    mReplacementAlgorithm(cacheConfig.replacementAlgorithm),
    mDRAM_hitTime(cacheConfig.iDRAM_hitTime),
    mbAllocateOnWriteMiss(cacheConfig.bAllocateOnWriteMiss)
{

    mNumLevels          = cacheConfig.vLevels.size();
    mNumLevel1_Sections = cacheConfig.vLevels[0].vSections.size();
    mNumLevel2_Sections = mNumLevels>1 ? cacheConfig.vLevels[1].vSections.size() : 0;
    
    cout<<mNumLevels<<" , "<<mNumLevel1_Sections<<" , "<<mNumLevel2_Sections<<endl;
    
    mvpCacheSections.assign(mNumLevels, {});
    mvpCacheStats.assign(mNumLevels, {});
    
    int levelIndex=0;
    for (auto level : cacheConfig.vLevels)
    {
        int sectionIndex=0;
        for (auto section : level.vSections)
        {
            if (section.iAssociativity==1)
                mvpCacheSections[levelIndex].push_back(new CacheSectionDM(section));
            else
            {
                switch (cacheConfig.replacementAlgorithm) {
                    case eReplacementAlgorithm::LRU:
                    {
                        mvpCacheSections[levelIndex].push_back(new CacheSectionLRU(section)); break;
                    }
                    case eReplacementAlgorithm::RND:
                    {
                        mvpCacheSections[levelIndex].push_back(new CacheSectionRND(section)); break;
                    }
                    case eReplacementAlgorithm::NMRU:
                    {
                        mvpCacheSections[levelIndex].push_back(new CacheSectionNMRU(section)); break;
                    }
                    case eReplacementAlgorithm::FIFO:
                    {
                        mvpCacheSections[levelIndex].push_back(new CacheSectionFIFO(section)); break;
                    }
                }
            }
            int sectionIndexInput = level.vSections.size()==1 ? -1 : sectionIndex; // handles unified case
            mvpCacheStats[levelIndex].emplace_back(GetSectionID(levelIndex,sectionIndexInput));
            sectionIndex++; 
        }
        levelIndex++;
    }
}

CacheManager::~CacheManager()
{}

// returns whether instruction was processed
bool CacheManager::ProcessInstruction()
{
    if (mDineroInstructionIndex>mDineroMatrix.data.size()-1)
        return false;
    
    std::pair<int,int> instruction = mDineroMatrix.data[mDineroInstructionIndex++];
    
    auto instrID          =std::get<0>(instruction);
    auto address                =std::get<1>(instruction);
    
    eInstructionID eInstrId;
    if (instrID==0)
        eInstrId =eInstructionID::Read;
    else if (instrID==1)
        eInstrId =eInstructionID::Write;
    else eInstrId =eInstructionID::InsFetch;
    
    //////////////////////////////// L1 : I ////////////////////////////////
    
    auto L1_sectionIndex        = (mNumLevel1_Sections==1 || (eInstrId != eInstructionID::InsFetch)) ? 0 : 1;
    CacheSection*   section_L1  = mvpCacheSections[0][L1_sectionIndex];
    CacheStats* cacheStats_L1   = &mvpCacheStats[0][L1_sectionIndex];
    
                                // not-write or allocate-on-write
    bool alloc                  = !(eInstrId==eInstructionID::Write && !mbAllocateOnWriteMiss);
    
    auto partitionedAddress_L1  = section_L1->PartitionAddress(address);

    bool setDirty_L1            = eInstrId==eInstructionID::Write;
    
    bool alloc_L1               = alloc && mNumLevels==1; // we don't add to L1 on a miss if L2 exists
    
    EvictIn evictIn_L1          = {alloc_L1, setDirty_L1, eInstrId, partitionedAddress_L1};
    
    auto evictOut_L1            = section_L1->ProcessSet(evictIn_L1);
    
    cacheStats_L1->WriteStat(evictOut_L1.bHit, eInstrId);
    
    if (evictOut_L1.bHit || mNumLevels==1)
        return true;
    
    //////////////////////////////// L2 : II ////////////////////////////////
    
    eInstructionID eInstrId_L2_II = eInstrId;
    if (eInstrId==eInstructionID::Write && mbAllocateOnWriteMiss)
        eInstrId_L2_II =eInstructionID::Read;
    
    auto L2_sectionIndex        = (mNumLevel2_Sections==1 || (eInstrId!=eInstructionID::InsFetch)) ? 0 : 1;
    CacheSection*   section_L2  = mvpCacheSections[1][L2_sectionIndex];
    CacheStats* cacheStats_L2   = &mvpCacheStats[1][L2_sectionIndex];
    
    auto partitionedAddress_L2  = section_L2->PartitionAddress(address);
    
    bool alloc_L2               = alloc; // not dependent on level
    
    bool setDirty_L2            = (eInstrId==eInstructionID::Write) && !alloc; // set L2 dirty if write and not allocating to L1 subsequently
    
    // Question: does instructionID matter here?
    EvictIn evictIn_L2          = {alloc_L2, setDirty_L2, eInstrId, partitionedAddress_L2}; // never write dirty to L2
    
    auto evictOut_L2            = section_L2->ProcessSet(evictIn_L2);
    
    auto L2_hit                 = evictOut_L2.bHit;
    
    cacheStats_L2->WriteStat(L2_hit, eInstrId_L2_II);
    
    // If write, and alloc=false, we already set dirty, so we can move on
    if (setDirty_L2)
        return true;
    
    //////////////////////////////// L1 : III ////////////////////////////////
    
    // we have the value from L2, need to add to L1
    
    bool alloc_L1_III           = alloc;
    
    auto setDirty_L1_III        = setDirty_L1; // same
    
    EvictIn evictIn_L1_III      = {alloc_L1_III, setDirty_L1_III, eInstrId, partitionedAddress_L1};
    
    auto evictOut_L1_III        = section_L1->ProcessSet(evictIn_L1_III);
    
    
    // cacheStats_L1->WriteStat(evictOut_L1.bHit, instructionID); don't update stats here?
    
    // If it wasn't full, or wasn't dirty, we're done
    if (!evictOut_L1_III.bFull || !evictOut_L1_III.bCurrentlyDirty)
        return true;

    //////////////////////////////// L2 : IV ////////////////////////////////
    
    auto evictedL1_InstrID          = evictOut_L1_III.eInstrID;
    
    //if (evictOut_L1_III.bCurrentlyDirty)
        //evictedL1_InstrID           = eInstructionID::Write;
    
    // Need to determine L2 section:
    auto L2_sectionIndex_IV         = (mNumLevel2_Sections==1 || (evictedL1_InstrID != eInstructionID::InsFetch)) ? 0 : 1;
    CacheSection*   section_L2_IV   = mvpCacheSections[1][L2_sectionIndex_IV];
    CacheStats* cacheStats_L2_IV    = &mvpCacheStats[1][L2_sectionIndex_IV];
    

    // Need to translate L1 address into L2 address (we already know the set)
    auto translated_L1_address      = section_L1->JoinAddress(evictOut_L1_III.iTag, partitionedAddress_L1.iSet);
    auto partitionedAddress_L2_IV   = section_L2_IV->PartitionAddress(translated_L1_address);
    
    bool alloc_L2_IV                = alloc; // not dependent on level
    
    bool setDirty_L2_IV             = (eInstrId==eInstructionID::Write) && !alloc;

    EvictIn evictIn_L2_IV           = {alloc_L2_IV, setDirty_L2_IV, evictedL1_InstrID, partitionedAddress_L2_IV}; // never write dirty to L2
    
    auto evictOut_L2_IV             = section_L2_IV->ProcessSet(evictIn_L2_IV);
    
    auto L2_hit_IV                  = evictOut_L2_IV.bHit;
    
    cacheStats_L2_IV->WriteStat(L2_hit_IV, evictedL1_InstrID);
    
    return true;
}

void CacheManager::PrintStats()
{
    for (auto level : mvpCacheStats)
    {
        // Reverse order to print I, then D cache
        for (auto s = level.rbegin(); s != level.rend(); ++s)
        {
            auto section = *s; 
            
            cout<<section.ID<<endl;
            cout<<endl;
            
            printf(    " Metrics                     Total         Instrn          Data          Read         Write          Misc\n");
            printf(    " -------                     ------        ------          ----          ----         -----          ----\n");
            
            float Total                 = section.readTotal+section.writeTotal+section.instrFetchTotal;
            float Total_Misses          = section.readMisses+section.writeMisses+section.instrFetchMisses;
            
            float Instrn                = section.instrFetchTotal;
            float Instrn_Misses         = section.instrFetchMisses;
            
            float Data                  = section.readTotal+section.writeTotal;
            float Data_Misses           = section.readMisses+section.writeMisses;
            
            float Read                  = section.readTotal;
            float Read_Misses           = section.readMisses;
            
            float Write                 = section.writeTotal;
            float Write_Misses          = section.writeMisses;
            
            float Misc                  = 0.0;
            float Misc_Misses           = 0.0;

            printf(    " Demand Fetches        %12.0f  %12.0f  %12.0f  %12.0f  %12.0f  %12.0f\n",
                   Total,
                   Instrn,
                   Data,
                   Read,
                   Write,
                   Misc
                   );
            
            printf(    "    Fraction of total    %10.4f    %10.4f    %10.4f    %10.4f    %10.4f    %10.4f\n",
                   Total    / Total,
                   Instrn   / Total,
                   Data     / Total,
                   Read     / Total,
                   Write    / Total,
                   Misc     / Total
                   );
            
            printf(    "  Demand Misses        %12.0f  %12.0f  %12.0f  %12.0f  %12.0f  %12.0f\n",
                   Total_Misses,
                   Instrn_Misses,
                   Data_Misses,
                   Read_Misses,
                   Write_Misses,
                   Misc_Misses
                   );
            
            printf(    "     Demand miss rate    %10.4f    %10.4f    %10.4f    %10.4f    %10.4f    %10.4f\n",
                   Total            ? Total_Misses      / Total     : 0,
                   Instrn_Misses    ? Instrn_Misses     / Instrn    : 0,
                   Data_Misses      ? Data_Misses       / Data      : 0,
                   Read_Misses      ? Read_Misses       / Read      : 0,
                   Write_Misses     ? Write_Misses      / Write     : 0,
                   Misc_Misses      ? Misc_Misses       / Misc      : 0
                   );
            
            cout<<endl;
            cout<<endl;
        }
    }
}

bool CacheManager::RegressionTest(int testID)
{
    bool regressionTestSucceeded = true;
    
    switch (testID)
    {
            
        case 0:
        {
            auto sectionStats               = mvpCacheStats[0][0];
            float expected_demand_misses    = 491;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            break;
        }
        case 2:
        {
            auto sectionStats               = mvpCacheStats[0][0];
            float expected_demand_misses    = 932;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            break;
        }
        case 10:
        {
            auto sectionStats               = mvpCacheStats[0][0];
            float expected_demand_misses    = 1233;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            break;
        }
        case 3:
        {
            auto  sectionStats              = mvpCacheStats[0][0];
            float expected_demand_misses    = 387;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats                    = mvpCacheStats[0][1];
            expected_demand_misses          = 216;
            empirical_demand_misses         = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            break;
        }
        case 4:
        {
            auto  sectionStats              = mvpCacheStats[0][0];
            float expected_demand_misses    = 1293;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats                    = mvpCacheStats[1][0];
            expected_demand_misses          = 555;
            empirical_demand_misses         = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            break;
        }
            
        case 55:
        {
            auto  sectionStats              = mvpCacheStats[0][1];
            float expected_demand_misses    = 257;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats                    = mvpCacheStats[0][0];
            expected_demand_misses          = 429;
            empirical_demand_misses         = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats                    = mvpCacheStats[1][1];
            expected_demand_misses          = 129;
            empirical_demand_misses         = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats                    = mvpCacheStats[1][0];
            expected_demand_misses          = 160;
            empirical_demand_misses         = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            break;
        }
        case 555:
        {
            auto  sectionStats              = mvpCacheStats[0][1];
            float expected_demand_misses    = 257;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats                    = mvpCacheStats[0][0];
            expected_demand_misses          = 1000;
            empirical_demand_misses         = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats                    = mvpCacheStats[1][1];
            expected_demand_misses          = 129;
            empirical_demand_misses         = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats                    = mvpCacheStats[1][0];
            expected_demand_misses          = 924;
            empirical_demand_misses         = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            break;
        }
        case 100:
        {
            auto sectionStats                   = mvpCacheStats[0][0];
            float expected_demand_misses        = 264414;
            float empirical_demand_misses       = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            float expected_instruction_misses    = 18088;
            float empirical_instruction_misses   = sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            if (expected_instruction_misses != empirical_instruction_misses)
                return false;
            
            break;
        }
    }
            
            
            
    return regressionTestSucceeded;
}
