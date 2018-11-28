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
    mDRAM_hitTime(cacheConfig.iDRAM_hitTime)
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
    
    // Delegate to appropriate cache section

    
    CacheSection*   section;
    CacheStats*     cacheStats;
    
    if (mNumLevel1_Sections==1 || (instructionID<2)) // unified or read/write
    {
        section     = mvpCacheSections[0][0];
        cacheStats  = &mvpCacheStats[0][0];
    }
    else
    {
        section = mvpCacheSections[0][1]; // I-Cache
        cacheStats  = &mvpCacheStats[0][1];
    }
    
    bool alloc = !(instructionID==1 && !mbAllocateOnWriteMiss);
    
    auto partitionedAddress = section->PartitionAddress(address);
    
    auto hit = section->ProcessSet(partitionedAddress,alloc);
    
    switch (instructionID) {
        case 0:
            cacheStats->readMisses += (hit ? 0 : 1);
            cacheStats->readTotal++;
            break;
            
        case 1:
            cacheStats->writeMisses += (hit ? 0 : 1);
            cacheStats->writeTotal++;
            break;
            
        case 2:
            cacheStats->instrFetchMisses += (hit ? 0 : 1);
            cacheStats->instrFetchTotal++;
            break;
    }
    
    mDineroInstructionIndex++;
    return true;
}

void CacheManager::PrintStats()
{
    for (auto level : mvpCacheStats)
    {
        for (auto section : level)
        {
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
            
            
            //float f[6]={1.5,2,3,4,5,6.5};
            
            printf(    " Demand Fetches        %12.0f  %12.0f  %12.0f  %12.0f  %12.0f  %12.0f\n",
                   Total,
                   Instrn,
                   Data,
                   Read,
                   Write,
                   Misc
                   );
            
            printf(    "    Fraction of total    %10.4f    %10.4f    %10.4f    %10.4f    %10.4f    %10.4f\n",
                   Total / Total,
                   Instrn / Total,
                   Data / Total,
                   Read / Total,
                   Write / Total,
                   Misc / Total
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
                   Total / Total_Misses,
                   Instrn_Misses ? Instrn / Instrn_Misses : 0,
                   Data_Misses ? Data / Data_Misses : 0,
                   Read_Misses ? Read / Read_Misses : 0,
                   Write_Misses ? Write / Write_Misses : 0,
                   Misc_Misses ? Misc / Misc_Misses : 0
                   );
            
            cout<<endl;
            cout<<endl;
        }
    }
}
