//
//  Evictor.hpp
//  Lab4_CacheSimulator
//
//  Created by Benjamin C Flowers on 11/27/18.
//  Copyright © 2018 Benjamin C Flowers. All rights reserved.
//

#ifndef Evictor_hpp
#define Evictor_hpp

#include <stdio.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <set>


class Evictor
{
protected:
    size_t size;
    
public:
    Evictor(size_t size);
    virtual bool Access(uint32_t tag) = 0;
};

class LRU_Evictor : public Evictor
{
private:
    // store keys of cache
    std::list<int> dq;
    
    // store references of key in cache
    std::unordered_map<int, std::list<int>::iterator> ma;
    
public:
    LRU_Evictor(size_t size);

    bool Access(const uint32_t tag);
};

class RND_Evictor : public Evictor
{
private:

    std::set<uint32_t> tagSet; 
    
public:
    RND_Evictor(size_t size);
    
    bool Access(const uint32_t tag);
};



#endif /* Evictor_hpp */
