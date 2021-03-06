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

void ParseArgs(CacheConfig& cacheConfig, DineroMatrix& dineroMatrix, bool debug, int test_id)
{
    cout<<"Welcome to ECE 550 Group 3's Cache Simulator!" << endl;
    
    if (debug) // use pre-made test configuration
        GetTestConfig(cacheConfig, test_id);
    else
        ParseAndPopulateCache(cacheConfig);
    
    ParseTraceFile(dineroMatrix, debug);
    
    cout<<"ParseArgs completed"<<endl;
}

void ParseTraceFile(DineroMatrix& dm, bool debug)
{
    //string defaultFile = "Dinero10k.din";
    string defaultFile = "DineroFull.din";
    
    if (!debug)
        cout<< endl << "Next, please enter a trace file, or 'd' for default: " << defaultFile <<  endl << endl;
    
    while (true)
    {
        
        std::string fileName;
        if (!debug)
            cin>>fileName;
        
        if (fileName=="d" || fileName=="D" || debug)
            fileName=defaultFile;

        std::ifstream infile(fileName);
        
        if (infile.is_open())
        {
            int code;
            string sAddress;
            
            while (infile >> code >> sAddress)
            {
                //cout<<code<<endl;
                //cout<<sAddress<<endl;
                
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
        cout<<"Configuring Level " << i+1 << "..." << endl;
        Level level;
        ParseAndPopulateLevel(level);
        cacheConfig.vLevels.push_back(level);
    }
    
    // Get DRAM Hit Time
    int lBound=1;
    int uBound=INT_MAX;
    
    std::string prompt                  = "Please enter the DRAM hit time";
    cacheConfig.iDRAM_hitTime           = ParseInt(prompt,lBound,uBound);
    cacheConfig.bAllocateOnWriteMiss    = ParseAllocateOnWriteMiss();
    cacheConfig.replacementAlgorithm    = ParseReplacementAlgorithm();
    
    cout<<"here"<<endl; 
    
}

void ParseAndPopulateLevel(Level& level)
{
    cout<<"Please select Unified (U) or Split Mode (S)" << endl;
    level.Mode          = ParseMode();
    int numSections     = level.Mode==eMode::Unified ? 1 : 2;
    
    for (int i = 0; i < numSections; i++)
    {
        std::string sectionString = "Configuring ";
        sectionString += numSections==1 ? "Unified " : (i==0 ? "Data " : "Instruction ");
        sectionString += "Cache...";
        cout<<sectionString<<endl;
        Section section;
        ParseAndPopulateSection(section);
        level.vSections.push_back(section);
    }
}

void ParseAndPopulateSection(Section& section)
{
    int lBound, uBound;
    
    // Get Associativity
    lBound                  = 1;
    uBound                  = 8;
    
    std::string aPrompt     = "Please enter the Associativity";
    section.iAssociativity  = ParseInt(aPrompt,lBound,uBound);
    
    // Get BlockSize
    lBound                  = 1;
    uBound                  = INT_MAX;
    
    std::string bPrompt = "Please enter the Block Size";
    section.iBlockSize = ParseInt(bPrompt,lBound,uBound);
    
    // Get Capacity
    lBound                  = 1;
    uBound                  = INT_MAX;
    
    std::string cPrompt     = "Please enter the Capacity";
    section.iCapacity       = ParseInt(cPrompt,lBound,uBound);
    
    // Get Hit Time
    lBound                  = 1;
    uBound                  = INT_MAX;
    
    std::string hitPrompt   = "Please enter the cache-section hit time";
    section.iHitTime        = ParseInt(hitPrompt,lBound,uBound);
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

// Get pre-made test configuration
void GetTestConfig(CacheConfig& cacheConfig, int testID)
{
    switch (testID) {
        case 0:
        {
            Level l = {
                eMode::Unified,
                {{       1,
                        32,
                        8192,
                        -1
                }}
            };
            cacheConfig.vLevels.push_back(l);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 10:
        {
            Level l = {
                eMode::Unified,
                {{       1,
                    32,
                    8192,
                    -1
                }}
            };
            cacheConfig.vLevels.push_back(l);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = false;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 1:
        {
            Level l = {
                eMode::Unified,
                {{       4,
                    16,
                    8192,
                    -1
                }}
            };
            cacheConfig.vLevels.push_back(l);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 2:
        {
            Level l = {
                eMode::Unified,
                {{       8,
                    16,
                    4096,
                    -1
                }}
            };
            cacheConfig.vLevels.push_back(l);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::RND;
            return;
        }
        case 3:
        {
            Level l = {
                eMode::Split,
                {
                    {                   // D
                        8,
                        16,
                        4096,
                        -1
                    },
                    {                   // I
                        4,
                        32,
                        8192,
                        -1
                    }
                }
                
            };
            cacheConfig.vLevels.push_back(l);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 4:
        {
            Level l1 = {
                eMode::Unified,
                {{  1,
                    16,
                    2048,
                    -1
                }}
            };
            Level l2 = {
                eMode::Unified,
                {{  1,
                    32,
                    8192,
                    -1
                }}
            };
            cacheConfig.vLevels.push_back(l1);
            cacheConfig.vLevels.push_back(l2);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 5:
        {
            Level l1 = {
                eMode::Split,
                {
                    {                   // D
                        1,
                        16,
                        2048,
                        -1
                    },
                    {                   // I
                        1,
                        32,
                        4096,
                        -1
                    }
                }
            };
            Level l2 = {
                eMode::Split,
                {
                    {                   // D
                        1,
                        32,
                        2048,
                        -1
                    },
                    {                   // I
                        1,
                        16,
                        4096,
                        -1
                    }
                }
            };
            cacheConfig.vLevels.push_back(l1);
            cacheConfig.vLevels.push_back(l2);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 55:
        {
            Level l1 = {
                eMode::Split,
                {
                    {                   // D
                        1,
                        16,
                        2048,
                        -1
                    },
                    {                   // I
                        1,
                        32,
                        4096,
                        -1
                    }
                }
            };
            Level l2 = {
                eMode::Split,
                {
                    {                   // D
                        1,
                        64,
                        8192,
                        -1
                    },
                    {                   // I
                        1,
                        64,
                        8192,
                        -1
                    }
                }
            };
            cacheConfig.vLevels.push_back(l1);
            cacheConfig.vLevels.push_back(l2);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 555:
        {
            Level l1 = {
                eMode::Split,
                {
                    {                   // D
                        1,
                        16,
                        2048,
                        -1
                    },
                    {                   // I
                        1,
                        32,
                        4096,
                        -1
                    }
                }
            };
            Level l2 = {
                eMode::Split,
                {
                    {                   // D
                        1,
                        64,
                        8192,
                        -1
                    },
                    {                   // I
                        1,
                        64,
                        8192,
                        -1
                    }
                }
            };
            cacheConfig.vLevels.push_back(l1);
            cacheConfig.vLevels.push_back(l2);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = false;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 201: // 200 with LRU
        {
            Level l1 = {
                eMode::Split,
                {
                    {                   // D
                        2,
                        32,
                        8192,
                        -1
                    },
                    {                   // I
                        1,
                        32,
                        8192,
                        -1
                    }
                }
            };
            Level l2 = {
                eMode::Unified,
                {{  8,
                    32,
                    32768,
                    -1
                }}
            };
            cacheConfig.vLevels.push_back(l1);
            cacheConfig.vLevels.push_back(l2);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 100: // OFFICIAL TA-DETERMINED TEST CASE #1
        {
            Level l = {
                eMode::Unified,
                {{       1,
                    32,
                    8192,
                    -1
                }}
            };
            cacheConfig.vLevels.push_back(l);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = false;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
        case 200: // OFFICIAL TA-DETERMINED TEST CASE #2
        {
            Level l1 = {
                eMode::Split,
                {
                    {                   // D
                        2,
                        32,
                        8192,
                        -1
                    },
                    {                   // I
                        1,
                        32,
                        8192,
                        -1
                    }
                }
            };
            Level l2 = {
                eMode::Unified,
                {{  8,
                    32,
                    32768,
                    -1
                }}
            };
            cacheConfig.vLevels.push_back(l1);
            cacheConfig.vLevels.push_back(l2);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::RND;
            return;
        }
        case 300: // OFFICIAL TA-DETERMINED TEST CASE #3
        {
            Level l1 = {
                eMode::Split,
                {
                    {               // D
                        4,
                        64,
                        16384,
                        -1
                    },
                    {               // I
                        1,
                        64,
                        8192,
                        -1
                    }
                }
            };
            Level l2 = {
                eMode::Split,
                {
                    {               // D
                        16,
                        64,
                        65536,
                        -1
                    },
                    {               // I
                        8,
                        64,
                        32768,
                        -1
                    }
                }
            };
            cacheConfig.vLevels.push_back(l1);
            cacheConfig.vLevels.push_back(l2);
            cacheConfig.iDRAM_hitTime           = -1;
            cacheConfig.bAllocateOnWriteMiss    = true;
            cacheConfig.replacementAlgorithm    = eReplacementAlgorithm::LRU;
            return;
        }
    }
}
