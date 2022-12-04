CPP := g++
CPPFLAGS := -std=c++14 -Wall -g

LGSL := $(shell gsl-config --libs)
IGSL := $(shell gsl-config --cflags)

LBLAS := -L/home/linuxbrew/.linuxbrew/opt/openblas/lib
IBLAS := -I/home/linuxbrew/.linuxbrew/opt/openblas/include

LIB := -L./
INC := -I./

LDLIBS := -lgsl -lcblas

SRCS := $(wildcard BlockSim/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

STRAT_SRCS := $(wildcard StratSim/*.cpp)
STRAT_OBJS := $(patsubst %.cpp,%.o,$(STRAT_SRCS))

all: strat selfish

strat: $(STRAT_SRCS) $(OBJS)
	$(CPP) $(CPPFLAGS) $(INC) $(IBLAS) $(IGSL) $(LDLIBS) $(LGSL) $(LBLAS) -o $@ $^

selfish-bv: SelfishBlockValueSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-cm: SelfishCostMiningSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-nexp: SelfishNetworkExpSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-nlin: SelfishNetworkLinearSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-npoiss: SelfishNetworkPoissonSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-nuni: SelfishNetworkUniformSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish-comp: SelfishClassicClever/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)

selfish: SelfishSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS)  $(INC) $(IGSL) $(IBLAS)  -o $@ $^ $(LBLAS) $(LGSL)


plot-nuni: 
	python3 plot.py "Profitability of Selfish Mining in Presence of Uni-Dist Network Delay with α = 0.${ALPHA}" "Selfish Network Delay - Honest Network Delay (sec)" selfish-nuni_500_${ALPHA}_0.txt selfish-nuni_500_${ALPHA}_1.txt 

plot-npoiss: 
	python3 plot.py "Profitability of Selfish Mining in Presence of Poisson-Dist Network Delay with α = 0.${ALPHA}" "Selfish Network Delay - Honest Network Delay (sec)" selfish-npoiss_500_${ALPHA}_0.txt selfish-nuni_500_${ALPHA}_1.txt 

plot-nlin: 
	python3 plot.py "Profitability of Selfish Mining in Presence of Linear Network Delay" "Selfish Network Delay (sec)" selfish-nlin_500_12.txt  selfish-nlin_500_25.txt  selfish-nlin_500_38.txt  selfish-nlin_500_40.txt  selfish-nlin_500_50.txt  

plot-nexp: 
	python3 plot.py "Profitability of Selfish Mining in Presence of Exponential Network Delay" "Selfish Network Delay (sec)" selfish-nexp_25_12.txt  selfish-nexp_25_25.txt selfish-nexp_25_38.txt selfish-nexp_25_40.txt selfish-nexp_25_50.txt    

plot-cm: 
	python3 plot.py "Profitability of Selfish Mining in Presence of Mining Cost per Sec" "Mining Cost/Sec (BTC)" selfish-cm_500_12.txt  selfish-cm_500_25.txt selfish-cm_500_38.txt selfish-cm_500_40.txt selfish-cm_500_50.txt    

plot-bv:
	python3 plot.py "Profitability of Selfish Mining vs. Block Value" "Block Value (BTC)" selfish-bv_500_12_${BV}.txt  selfish-bv_500_25_${BV}.txt selfish-bv_500_38_${BV}.txt selfish-bv_500_40_${BV}.txt selfish-bv_500_50_${BV}.txt

plot-comp:
	python3 plot.py "Profitability of Classic and Clever Selfish Miner vs. Respective Share of Hash Power for γ=0.${GAMMA}" "Classic Hash Power α %" selfish-comp_${GAMMA}.txt

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INC) $(IGSL) $(IBLAS) $(LGSL) $(LBLAS) $(LDLIBS) -o $@ -c $<

clean:
	rm -rf BlockSim/*.o *.o strat selfish

.PHONY: all clean

