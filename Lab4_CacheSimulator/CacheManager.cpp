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
    
    std::pair<int,int> instruction = mDineroMatrix.data[mDineroInstructionIndex];
    
    auto instructionID  =std::get<0>(instruction);
    auto address        =std::get<1>(instruction);
    //cout<<instructionID<<","<<address<<endl;
    
    CacheSection*   section_L1;
    CacheStats*     cacheStats_L1;
    
    if (mNumLevel1_Sections==1 || (instructionID<2)) // unified or read/write
    {
        section_L1     = mvpCacheSections[0][0];
        cacheStats_L1  = &mvpCacheStats[0][0];
    }
    else
    {
        section_L1 = mvpCacheSections[0][1]; // I-Cache
        cacheStats_L1  = &mvpCacheStats[0][1];
    }
    
    bool alloc = !(instructionID==1 && !mbAllocateOnWriteMiss);
    
    auto partitionedAddress = section_L1->PartitionAddress(address);
    
    //cout<<instructionID<< ", set: "<<partitionedAddress.iSet<<" , tag: " << partitionedAddress.iTag <<endl;
    
    
    auto hit = section_L1->ProcessSet(partitionedAddress,alloc);
    
    //cout<< (hit ? "Hit" : "Miss") << endl;
    cout<<endl;
    
    switch (instructionID) {
        case 0:
            cacheStats_L1->readMisses += (hit ? 0 : 1);
            cacheStats_L1->readTotal++;
            break;
            
        case 1:
            cacheStats_L1->writeMisses += (hit ? 0 : 1);
            cacheStats_L1->writeTotal++;
            break;
            
        case 2:
            cacheStats_L1->instrFetchMisses += (hit ? 0 : 1);
            cacheStats_L1->instrFetchTotal++;
            break;
    }
    
    mDineroInstructionIndex++;
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
        }
        case 10:
        {
            auto sectionStats               = mvpCacheStats[0][0];
            float expected_demand_misses    = 1233;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
        }
        case 3:
        {
            auto  sectionStats              = mvpCacheStats[0][0];
            float expected_demand_misses    = 387;
            float empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
            
            sectionStats              = mvpCacheStats[0][1];
            expected_demand_misses    = 216;
            empirical_demand_misses   = sectionStats.readMisses+sectionStats.writeMisses+sectionStats.instrFetchMisses;
            
            if (expected_demand_misses != empirical_demand_misses)
                return false;
        }
    }
            
            
            
    return regressionTestSucceeded;
}
