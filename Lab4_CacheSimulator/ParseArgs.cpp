//
//  ParseArgs.cpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/20/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#include "ParseArgs.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

#define DEFAULT_PARSE_INT -1

using namespace std;

void ParseArgs(CacheConfig& cacheConfig, DineroMatrix& dineroMatrix)
{
    cout<<"Welcome to ECE 550 Group 3's Cache Simulator!" << endl;
    
    // 1)   get # levels: int_parse, {1,2}
    //      for each level,
    // 2)     get mode: string_parse {"u","U","s","S"}
    
    ParseAndPopulateCache(cacheConfig);
    
    ParseTraceFile(dineroMatrix);
    
    cout<<"ParseArgs completed"<<endl;
}

void ParseTraceFile(DineroMatrix& dm)
{
    string defaultFile = "Dinero10000.din";
    
    cout<< endl << "Next, please enter a trace file, or 'd' for default: " << defaultFile <<  endl << endl;
    
    while (true)
    {
        std::string fileName;
        cin>>fileName;
        
        if (fileName=="d" || fileName=="D")
            fileName=defaultFile;

        std::ifstream infile(fileName);
        
        
        if (infile.is_open())
        {
            int code;
            string sAddress;
            
            while (infile >> code >> sAddress)
            {
                cout<<code<<endl;
                cout<<sAddress<<endl;
                
                uint32_t iAddress;
                std::stringstream ss;
                ss << std::hex << sAddress;
                ss >> iAddress;
                
                dm.data.push_back({code,iAddress});
            }
            infile.close();
            return;
        }
        else cout << "\t" << "try again..."<<endl;
    }
    
}

int ParseInt(std::string prompt, int lBound, int uBound)
{
    cout<<prompt;
    cout<<" [";
    cout<<lBound;
    cout<<" ";
    cout<<uBound;
    cout<<"]";
    cout<<endl;

    int parsedInt;
    while (true)
    {
        cin>>parsedInt;
        
        if (parsedInt >= lBound && parsedInt <= uBound)
            return parsedInt;
        
        cout << "try again..."<<endl;
    }
    
    return DEFAULT_PARSE_INT;
}

void ParseAndPopulateCache(CacheConfig& cacheConfig)
{
    cout<<"First, let's configure the cache..." << endl;
    cout<< endl;
    
    // Get # Levels
    std::string numLevelsPrompt = "Please enter the number of cache levels";
    int numLevels = ParseInt(numLevelsPrompt,1,2);
    
    for (int i = 0; i < numLevels; i++)
    {
        Level level;
        cacheConfig.vLevels.push_back(level);
        cout<<"Configuring Level " << i+1 << "..." << endl;
        ParseAndPopulateLevel(level);
    }
    
    // Get DRAM Hit Time
    int lBound=1;
    int uBound=INT_MAX;
    
    std::string prompt = "Please enter the DRAM hit time";
    cacheConfig.iDRAM_hitTime = ParseInt(prompt,lBound,uBound);
    
    cacheConfig.bAllocateOnWriteMiss=ParseAllocateOnWriteMiss();
    
    cacheConfig.replacementAlgorithm = ParseReplacementAlgorithm();
    
    
}

void ParseAndPopulateLevel(Level& level)
{
    cout<<"Please select Unified (U) or Split Mode (S)" << endl;
    level.Mode = ParseMode();
    
    int numSections = level.Mode==eMode::Unified ? 1 : 2;
    
    for (int i = 0; i < numSections; i++)
    {
        Section section;
        level.vSections.push_back(section);
        cout<<"Configuring Section " << i+1 << "..." << endl;
        ParseAndPopulateSection(section);
    }
    
//    // Get Hit Time
//    int lBound=1;
//    int uBound=INT_MAX;
//    
//    std::string prompt = "Please enter this cache level's hit time";
//    level.iHitTime = ParseInt(prompt,lBound,uBound);
}

void ParseAndPopulateSection(Section& section)
{
    int lBound, uBound;
    
    // Get Associativity
    lBound=1;
    uBound=8;
    
    std::string aPrompt = "Please enter the Associativity";
    section.iAssociativity = ParseInt(aPrompt,lBound,uBound);
    
    // Get BlockSize
    lBound=1;
    uBound=INT_MAX;
    
    std::string bPrompt = "Please enter the Block Size";
    section.iBlockSize = ParseInt(bPrompt,lBound,uBound);
    
    // Get Capacity
    lBound=1;
    uBound=INT_MAX;
    
    std::string cPrompt = "Please enter the Capacity";
    section.iCapacity = ParseInt(cPrompt,lBound,uBound);
    
    // Get Hit Time
    lBound=1;
    uBound=INT_MAX;
    
    std::string hitPrompt = "Please enter the cache-section hit time";
    section.iCapacity = ParseInt(hitPrompt,lBound,uBound);
}

eMode ParseMode()
{
    while (true)
    {
        std::string mode;
        cin>>mode;
        
        if (mode=="u")
            return eMode::Unified;
        else if (mode=="U")
            return eMode::Unified;
        else if (mode=="s")
            return eMode::Split;
        else if (mode=="S")
            return eMode::Split;
        
        cout << "try again..."<<endl;
    }
    
    return eMode::Unified;
}

bool ParseAllocateOnWriteMiss()
{
    cout<<"Allocate on write miss? (y/n) "<<endl;
    
    while (true)
    {
        std::string result;
        cin>>result;
        
        if (result=="y")
            return true;
        else if (result=="Y")
            return true;
        else if (result=="n")
            return false;
        else if (result=="N")
            return false;
        
        cout << "try again..."<<endl;
    }
    
    return false;
}

eReplacementAlgorithm ParseReplacementAlgorithm()
{
    cout<<"Please select a replacement algorithm. "<<endl;
    cout<<" (L)RU, (R)ND, (N)MRU, (F)IFO "<<endl;
    
    while (true)
    {
        std::string result;
        cin>>result;
        
        if (result=="l")
            return eReplacementAlgorithm::LRU;
        else if (result=="L")
            return eReplacementAlgorithm::LRU;
        else if (result=="r")
            return eReplacementAlgorithm::RND;
        else if (result=="R")
            return eReplacementAlgorithm::RND;
        else if (result=="n")
            return eReplacementAlgorithm::NMRU;
        else if (result=="N")
            return eReplacementAlgorithm::NMRU;
        else if (result=="f")
            return eReplacementAlgorithm::FIFO;
        else if (result=="F")
            return eReplacementAlgorithm::FIFO;
        
        cout << "try again..."<<endl;
    }
    
    return eReplacementAlgorithm::LRU;
}
