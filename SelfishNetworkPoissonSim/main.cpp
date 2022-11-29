//
//  main.cpp
//  BlockSim
//
//  Created by Harry Kalodner on 6/6/16.
//  Copyright © 2016 Harry Kalodner. All rights reserved.
//

#include "BlockSim/strategy.hpp"
#include "BlockSim/utils.hpp"
#include "BlockSim/miner.hpp"
#include "BlockSim/block.hpp"
#include "BlockSim/blockchain.hpp"
#include "BlockSim/minerStrategies.h"
#include "BlockSim/logging.h"
#include "BlockSim/game.hpp"
#include "BlockSim/minerGroup.hpp"
#include "BlockSim/minerStrategies.h"
#include "BlockSim/game_result.hpp"
#include "BlockSim/miner_result.hpp"
#include "BlockSim/mining_style.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <cmath>
#include <queue>
#include <random>
#include <algorithm>
#include <vector>
#include <map>


#define NOISE_IN_TRANSACTIONS false //miners don't put the max value they can into a block (simulate tx latency)
#define NETWORK_DELAY BlockTime(0)         //network delay in seconds for network to hear about your block
#define EXPECTED_NUMBER_OF_BLOCKS BlockCount(20000)

#define LAMBERT_COEFF 0.13533528323661//coeff for lambert func equil  must be in [0,.2]
//0.13533528323661 = 1/(e^2)

#define B BlockValue(0) // Block reward
//#define TOTAL_BLOCK_VALUE BlockValue(15.625)
#define TOTAL_BLOCK_VALUE BlockValue(Value(25) * SATOSHI_PER_BITCOIN)

#define SEC_PER_BLOCK BlockRate(600)     //mean time in seconds to find a block

//#define B BlockValue(3.125) // Block reward
#define A (TOTAL_BLOCK_VALUE - B)/SEC_PER_BLOCK  //rate transactions come in

#define SELFISH_GAMMA 0.5 //fraction of network favoring your side in a dead tie between honest and selfish blocks. treat it as a coin toss.
//half way and miners have equal hash power

#define HONEST_NETWORK_DELAY BlockTime(0)  // can be changed per each simulation 
#define SELFISH_NETWORK_DELAY BlockTime(0) // can be changed per each simulation 
#define NUM_GAMES 25
#define SATOSHI_VALUE 0.00000001 // value of a single satoshi in terms of bitcoin 

// 
BlockTime GetUniformRandomNetworkDelay(){
    static std::random_device rd;  // Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    static std::uniform_real_distribution<double> dis(0, 10.0); // 
    return BlockTime(dis(gen));
}

BlockTime GetPoissonNetworkDelay(){
    static std::default_random_engine generator;
    static std::poisson_distribution<uint> distribution(890); 
    return BlockTime(distribution(generator)/1000.0);
}

BlockTime GetLinearNetworkDelay(int game_number) {
    return HONEST_NETWORK_DELAY + BlockTime(game_number * .001); // linearly increase by .001 seconds
}

BlockTime GetExponentialNetworkDelay(int game_number){
    return HONEST_NETWORK_DELAY + BlockTime(pow(10, game_number - NUM_GAMES/2)); // make this make sense ! grows too fast.. how to slow down?
}


BlockValue GetLinearCostOfMining(int game_number) {
    return COST_PER_SEC_TO_MINE + BlockValue(game_number * SATOSHI_VALUE * 100);  
}

int main(int, const char * argv[]) {
    
    int numberOfGames = atoi(argv[1]);
    int percentageAlpha = atoi(argv[2]);
    int returnZero = atoi(argv[3]);
    char  filename[1024] = {0};
    sprintf(filename, "%s_%s_%s_%s.txt", argv[0], argv[1], argv[2], argv[3]);

    
    //#########################################################################################
    //idea of simulation: 2 miners, only an honest, and a selfish miner. Run many games, with the
    //size of the two changing. Plot the expected profit vs. actual profit. (reproduce fig 2 in selfish paper)
    GAMEINFO("#####\nRunning Selfish Mining Simulation\n#####" << std::endl);
    std::ofstream plot;
    plot.open(filename);
    plot << "Selfish Miner Profit, Selfish Miner Network Delay, Honest Miner Network Delay" << std::endl;
    //start running games
    for (int gameNum = 1; gameNum <= numberOfGames; gameNum++) {
        
        std::vector<std::unique_ptr<Miner>> miners;
        
        // Scale power to reach %50 on the last game
        HashRate selfishPower = HashRate(percentageAlpha/100.0);
//        auto defaultStrat = createDefaultSelfishStrategy(NOISE_IN_TRANSACTIONS, SELFISH_GAMMA);
//        auto selfishStrat = createSelfishStrategy(NOISE_IN_TRANSACTIONS);
        
        using std::placeholders::_1;
        using std::placeholders::_2;
        
        std::function<Value(const Blockchain &, Value)> forkFunc(std::bind(functionForkPercentage, _1, _2, 2));
//        auto selfishStrat = createCleverSelfishStrategy(false, Value(25)*SATOSHI_PER_BITCOIN);
//        auto defaultStrat = createPettyStrategy(NOISE_IN_TRANSACTIONS, SELFISH_GAMMA);
        auto defaultStrat = createDefaultSelfishStrategy(NOISE_IN_TRANSACTIONS, SELFISH_GAMMA);
        auto selfishStrat = createSelfishStrategy(NOISE_IN_TRANSACTIONS);

        auto selfishMinerNetworkDelay = GetPoissonNetworkDelay();
        auto honestMinerNetworkDelay = GetPoissonNetworkDelay();

        MinerParameters selfishMinerParams = {0, std::to_string(0), selfishPower, selfishMinerNetworkDelay, COST_PER_SEC_TO_MINE};
        MinerParameters defaultinerParams = {1, std::to_string(1), HashRate(1.0) - selfishPower, honestMinerNetworkDelay, COST_PER_SEC_TO_MINE};
        
        miners.push_back(std::make_unique<Miner>(selfishMinerParams, *selfishStrat));
        miners.push_back(std::make_unique<Miner>(defaultinerParams, *defaultStrat));
        
        MinerGroup minerGroup(std::move(miners));
        
        GAMEINFO("\n\nGame#: " << gameNum << " The board is set, the pieces are in motion..." << std::endl);
        GAMEINFO("miner ratio:" << selfishPower << " selfish." << std::endl);
        
        BlockchainSettings blockchainSettings = {SEC_PER_BLOCK, A, B, EXPECTED_NUMBER_OF_BLOCKS};
        GameSettings settings = {blockchainSettings};
        
        
        auto blockchain = std::make_unique<Blockchain>(settings.blockchainSettings);
        minerGroup.reset(*blockchain);
        minerGroup.resetOrder();
        
        GAMEINFO("\n\nGame#: " << gameNum << " The board is set, the pieces are in motion..." << std::endl);
        
        auto result = runGame(minerGroup, *blockchain, settings);
        
        auto minerResults = result.minerResults;
        
        GAMEINFO("The game is complete. Calculate the scores:" << std::endl);

        GAMEINFO("Total profit:" << result.moneyInLongestChain << std::endl);
        
        assert(minerResults[0].totalProfit <= result.moneyInLongestChain);
        
        auto fractionOfProfits = valuePercentage(minerResults[0].totalProfit, result.moneyInLongestChain);
        GAMEINFO("Fraction earned by selfish:" << fractionOfProfits << " with " << selfishPower << " fraction of hash power" << std::endl);
        // plot << selfishPower << " " << fractionOfProfits << std::endl;
        plot << fractionOfProfits << ", " << selfishMinerNetworkDelay << ", " << honestMinerNetworkDelay << std::endl;
        
    }
    
    GAMEINFO("Games over." << std::endl);
}
