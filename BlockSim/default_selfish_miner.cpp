//
//  default_selfish_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "default_selfish_miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "miner.hpp"
#include "selfish_miner.hpp"
#include "default_miner.hpp"
#include "logging.h"
#include "utils.hpp"
#include "strategy.hpp"

#include <assert.h>
#include <iostream>
#include <random>


using std::placeholders::_1;
using std::placeholders::_2;

Block &blockToMineOn(const Miner &me, const Blockchain &blockchain, double gamma);

std::unique_ptr<Strategy> createDefaultSelfishStrategy(bool noiseInTransactions, double gamma) {
    auto mineFunc = std::bind(blockToMineOn, _1, _2, gamma);
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    return std::make_unique<Strategy>("default-selfish", mineFunc, valueFunc);
}

Block &blockToMineOn(const Miner &me, const Blockchain &chain, double gamma) {
    
    std::vector<Block*> possiblities = chain.oldestBlocks(chain.getMaxHeightPub());
    if (possiblities.size() == 1) { //no forking
        return *possiblities[0];
    }
    else if (possiblities.size() == 2) { //fork between the selfish miner and the rest of the network
        //mineHere should already be set to the side of the fork not the selfish miner
        Block *selfishBlock = nullptr;
        Block *defaultBlock = nullptr;
        if (ownBlock(&me, possiblities[0])) {
            defaultBlock = possiblities[0];
            selfishBlock = possiblities[1];
        } else {
            defaultBlock = possiblities[1];
            selfishBlock = possiblities[0];
        }
        
        //assert(ownBlock(&me, defaultBlock));
        
        //with chance gamma, mine on the selfish miner's block, otherwise not
        if (selectRandomChance() < gamma) {
            return *selfishBlock;
            
        } else {
            COMMENTARY("Having to mine on selfish block due to gamma. ");
            return *defaultBlock;
        }
    } else if (possiblities.size() == 3){ // we must choose between classic selfish, clever selfish, and honest miner
                //mineHere should already be set to the side of the fork not the selfish miner
        Block *cleverSelfishBlock = nullptr;
        Block *classicSelfishBlock = nullptr;
        Block *defaultBlock = nullptr;
        if (ownBlock(&me, possiblities[0])) {
            defaultBlock = possiblities[0];
            classicSelfishBlock = possiblities[1];
            cleverSelfishBlock = possiblities[2];
        } else if (ownBlock(&me, possiblities[1])){
            defaultBlock = possiblities[1];
            classicSelfishBlock = possiblities[0]; // could either be clever instead...
            cleverSelfishBlock = possiblities[2]; // could be classic instead 
        } else {
            defaultBlock = possiblities[2];
            classicSelfishBlock = possiblities[0]; // could either be clever instead...
            cleverSelfishBlock = possiblities[1]; // could be classic instead 
        }
        
        //assert(ownBlock(&me, defaultBlock));
        
        //with chance gamma, mine on the selfish miner's block, otherwise not
        if (selectRandomChance() < gamma) { // mine on selfish block in general

            std::default_random_engine generator;
            std::uniform_int_distribution<int> distribution(0,1);
            int coin_toss = distribution(generator);
            return ( coin_toss )?   *classicSelfishBlock : *cleverSelfishBlock; // coin toss: decide whether we want to mine on CLASSIC OR CLEVER selfish block in particular
            
        } else {
            COMMENTARY("Having to mine on selfish block due to gamma. ");  // what ? (should be mine on honest block...)
            return *defaultBlock;
        }
    } else { //lolwut
        ERROR("\n#####ERROR UNFORSEEN CIRCUMSTANCES IN LOGIC FOR SELFISH MINING SIM###\n\n" << std::endl);
        return *possiblities[0];
    }
}
