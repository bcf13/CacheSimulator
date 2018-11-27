//
//  CacheManager.cpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/25/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#define DEBUG 1


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
    
    
//    int A = 2;
//    int B = 16;
//    int C = 32768;
    int A = 1;
    int B = 32;
    int C = 8192;
    int hit = -1;
    Section s = {A,B,C,hit};
    

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
    cout<<instructionID<<","<<address<<endl;
    
    // Delegate to appropriate cache section

    
    CacheSection*   section;
    CacheStats*     cacheStats;
    
    if (mNumLevel1_Sections==1 || instructionID<2) // unified or read/write
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
            
            cout<<section.ID<<" Statistics: "<<endl;
            cout<<endl;
            
            cout<<"Total: "         << section.readTotal+section.writeTotal+section.instrFetchTotal<<endl;
            cout<<"Total Misses: "  << section.readMisses+section.writeMisses+section.instrFetchMisses<<endl;
            cout<<endl;
            
            cout<<"Instrn: "        << section.instrFetchTotal<<endl;
            cout<<"Instrn Misses: " << section.instrFetchMisses<<endl;
            cout<<endl;
            
            cout<<"Data: "          << section.readTotal+section.writeTotal<<endl;
            cout<<"Data Misses: "   << section.readMisses+section.writeMisses<<endl;
            cout<<endl;
            
            cout<<"Read: "          << section.readTotal<<endl;
            cout<<"Read Misses: "   << section.readMisses<<endl;
            cout<<endl;
            
            cout<<"Write: "         << section.writeTotal<<endl;
            cout<<"Write Misses: "  << section.writeMisses<<endl;
            cout<<endl;
            
        }
    }
}

