//
//  Stubborn_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "head_stubborn.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logging.h"
#include "miner.hpp"
#include "default_miner.hpp"
#include "strategy.hpp"

#include <cmath>
#include <assert.h>
#include <algorithm>

using std::placeholders::_1;
using std::placeholders::_2;

std::unique_ptr<Strategy> createStubbornStrategy(bool noiseInTransactions) {
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    return std::make_unique<Strategy>("Stubborn", StubbornBlockToMineOn, valueFunc, std::make_unique<StubbornPublishingStyle>());
}

Block &StubbornBlockToMineOn(const Miner &me, const Blockchain &chain) {
    Block *newest = me.newestUnpublishedBlock();
    if (newest && chain.getMaxHeightPub() -  newest->height  >= BlockHeight(1) ) { //if higher, equal, or lower by one, continue mining on you chain
        return *newest;
    } else {
        return chain.oldest(chain.getMaxHeightPub(), me);
    }
}

std::vector<std::unique_ptr<Block>> StubbornPublishingStyle::publishBlocks(const Blockchain &chain, const Miner &me, std::vector<std::unique_ptr<Block>> &unpublishedBlocks) {
    assert(!unpublishedBlocks.empty());
    
    auto privateHeight = getPrivateHeadHeight(unpublishedBlocks);
    auto publicHeight = chain.getMaxHeightPub();
    std::vector<std::unique_ptr<Block>> blocks_to_return;
    if (privateHeight == publicHeight + BlockHeight(1)){
        std::copy(chain._blocks.begin(), chain._blocks.end(), back_inserter(blocks_to_return));
        blocks_to_return.push_back(unpublishedBlocks.last())
    }
    return blocks_to_return;
}

BlockHeight StubbornPublishingStyle::getPrivateHeadHeight(std::vector<std::unique_ptr<Block>> &unpublishedBlocks) const {
    return unpublishedBlocks.back()->height;
}
