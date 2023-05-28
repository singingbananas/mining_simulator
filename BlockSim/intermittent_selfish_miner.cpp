//
//  selfish_miner.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 5/25/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "intermittent_selfish_miner.hpp"
#include "block.hpp"
#include "blockchain.hpp"
#include "logging.h"
#include "miner.hpp"
#include "default_miner.hpp"
#include "strategy.hpp"

#include <cmath>
#include <assert.h>
#include <algorithm>

#define EPOCH 2016

using std::placeholders::_1;
using std::placeholders::_2;

std::unique_ptr<Strategy> createIntermittentSelfishStrategy(bool noiseInTransactions) {
    auto valueFunc = std::bind(defaultValueInMinedChild, _1, _2, noiseInTransactions);
    
    return std::make_unique<Strategy>("intermittentSelfish", intermittentSelfishBlockToMineOn, valueFunc, std::make_unique<SelfishPublishingStyle>());
}

Block &intermittentselfishBlockToMineOn(const Miner &me, const Blockchain &chain) {
    Block *newest = me.newestUnpublishedBlock();
    auto publicHeight = chain.getMaxHeightPub();
    if ((publicHeight / EPOCH) % 2  == 1 ){
        return chain.oldest(chain.getMaxHeightPub(), me);
    }
    if (newest && newest->height >= chain.getMaxHeightPub()) {
        return *newest;
    } else {
        return chain.oldest(chain.getMaxHeightPub(), me);
    }
}

std::vector<std::unique_ptr<Block>> IntermittentSelfishPublishingStyle::publishBlocks(const Blockchain &chain, const Miner &me, std::vector<std::unique_ptr<Block>> &unpublishedBlocks) {
    assert(!unpublishedBlocks.empty());
    BlockHeight height = heightToPublish(chain, me, unpublishedBlocks);
    
    std::vector<BlockHeight> heights;
    heights.resize(unpublishedBlocks.size());
    std::transform(begin(unpublishedBlocks), end(unpublishedBlocks), heights.begin(), [&](auto &block) { return block->height; });
    auto splitPoint = std::upper_bound(std::begin(heights), std::end(heights), height, [](auto first, auto second) { return first < second; });
    auto offset = std::distance(std::begin(heights), splitPoint);
    std::vector<std::unique_ptr<Block>> split_lo(std::make_move_iterator(std::begin(unpublishedBlocks)), std::make_move_iterator(std::begin(unpublishedBlocks) + offset));

    unpublishedBlocks.erase(begin(unpublishedBlocks), std::begin(unpublishedBlocks) + offset);
    
    return split_lo;
}

BlockHeight IntermittentSelfishPublishingStyle::getPrivateHeadHeight(std::vector<std::unique_ptr<Block>> &unpublishedBlocks) const {
    return unpublishedBlocks.back()->height;
}

BlockHeight IntermittentSelfishPublishingStyle::heightToPublish(const Blockchain &chain, const Miner &me, std::vector<std::unique_ptr<Block>> &unpublishedBlocks) const {
    assert(!unpublishedBlocks.empty());
    auto publicHeight = chain.getMaxHeightPub();
    auto privateHeight = getPrivateHeadHeight(unpublishedBlocks);
    BlockHeight heightToPublish(publicHeight);
    // If private chain is one block ahead of the public chain and there is a race for the public head then publish
    if (privateHeight == publicHeight + BlockHeight(1) && chain.blocksOfHeight(publicHeight) > BlockCount(1)) {
        heightToPublish = privateHeight;
    }
    return heightToPublish;
}
